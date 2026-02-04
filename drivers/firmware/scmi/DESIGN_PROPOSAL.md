# SCMI Transport Design: Support for Multiple Transport Types

## Problem Statement

The current SCMI driver implementation has hardcoded assumptions that tie it to mailbox transport behavior:

1. **Interrupt Control in Core**: The `scmi_interrupt_enable()` function (core.c:90-109) directly accesses mailbox-specific structures (`struct scmi_mbox_channel`) and APIs (`mbox_set_enabled_dt()`), breaking transport abstraction.

2. **Polling Mode Issues**: The `scmi_send_message_polling()` function attempts to disable/enable interrupts for all transports, which is:
   - Unnecessary for SMC transport (no interrupt support)
   - Causes compilation issues when accessing mailbox-specific data

3. **Transport Mode Selection**: Current code uses `#ifdef CONFIG_ARM_SCMI_SMCC_TRANSPORT` to force polling mode, which doesn't scale as more transports are added.

4. **Violation of Abstraction**: Core layer should not know about transport-specific implementation details.

## Requirements

1. Support both mailbox (interrupt-capable) and SMC (polling-only) transports
2. Maintain clean separation between core and transport layers
3. Allow transports to declare their capabilities
4. Enable easy addition of future transports
5. No transport-specific `#ifdef` in core layer code

## Proposed Design: Transport Capabilities and Optional Operations

### 1. Transport Capability Flags

Add capability flags to indicate transport features:

```c
/* include/zephyr/drivers/firmware/scmi/transport.h */

/**
 * @brief SCMI transport capability flags
 */
enum scmi_transport_caps {
	/** Transport supports interrupt-based communication */
	SCMI_TRANSPORT_CAP_INTERRUPT = BIT(0),
	/** Transport supports polling-based communication */
	SCMI_TRANSPORT_CAP_POLLING = BIT(1),
	/** Transport requires polling (cannot use interrupts) */
	SCMI_TRANSPORT_CAP_POLLING_ONLY = BIT(2),
};
```

### 2. Enhanced Transport API

Add optional interrupt control operation to transport API:

```c
/* include/zephyr/drivers/firmware/scmi/transport.h */

struct scmi_transport_api {
	/** Transport capabilities (bitmask of scmi_transport_caps) */
	uint32_t capabilities;

	/* Existing operations */
	int (*init)(const struct device *transport);
	int (*send_message)(const struct device *transport,
			    struct scmi_channel *chan,
			    struct scmi_message *msg);
	int (*setup_chan)(const struct device *transport,
			  struct scmi_channel *chan,
			  bool tx);
	int (*read_message)(const struct device *transport,
			    struct scmi_channel *chan,
			    struct scmi_message *msg);
	bool (*channel_is_free)(const struct device *transport,
				struct scmi_channel *chan);
	struct scmi_channel *(*request_channel)(const struct device *transport,
						uint32_t proto, bool tx);

	/* New optional operation for interrupt control */
	/**
	 * @brief Enable or disable interrupts for a channel
	 *
	 * Optional operation - only implemented by transports that support
	 * interrupt-based communication (SCMI_TRANSPORT_CAP_INTERRUPT).
	 *
	 * @param transport pointer to transport device
	 * @param chan pointer to SCMI channel
	 * @param enable true to enable interrupts, false to disable
	 * @return 0 on success, negative errno on failure
	 */
	int (*interrupt_enable)(const struct device *transport,
				struct scmi_channel *chan,
				bool enable);
};

/**
 * @brief Enable or disable channel interrupts
 *
 * Optional operation - only available for transports with
 * SCMI_TRANSPORT_CAP_INTERRUPT capability.
 *
 * @param transport pointer to transport device
 * @param chan pointer to SCMI channel
 * @param enable true to enable, false to disable
 * @return 0 on success, -ENOTSUP if not supported, other negative errno on failure
 */
static inline int scmi_transport_interrupt_enable(const struct device *transport,
						  struct scmi_channel *chan,
						  bool enable)
{
	const struct scmi_transport_api *api =
		(const struct scmi_transport_api *)transport->api;

	if (!api || !api->interrupt_enable) {
		return -ENOTSUP;
	}

	return api->interrupt_enable(transport, chan, enable);
}

/**
 * @brief Check if transport supports interrupts
 */
static inline bool scmi_transport_has_interrupt_support(const struct device *transport)
{
	const struct scmi_transport_api *api =
		(const struct scmi_transport_api *)transport->api;

	if (!api) {
		return false;
	}

	return (api->capabilities & SCMI_TRANSPORT_CAP_INTERRUPT) != 0;
}

/**
 * @brief Check if transport requires polling only
 */
static inline bool scmi_transport_is_polling_only(const struct device *transport)
{
	const struct scmi_transport_api *api =
		(const struct scmi_transport_api *)transport->api;

	if (!api) {
		return false;
	}

	return (api->capabilities & SCMI_TRANSPORT_CAP_POLLING_ONLY) != 0;
}
```

### 3. Updated Core Layer (core.c)

Remove hardcoded mailbox assumptions:

```c
/* drivers/firmware/scmi/core.c */

static int scmi_send_message_polling(struct scmi_protocol *proto,
					struct scmi_message *msg,
					struct scmi_message *reply)
{
	int ret;
	int status = -ENOTSUP;

	/* wait for channel to be free */
	if (!k_is_pre_kernel() && k_mutex_lock(&proto->tx->lock, K_NO_WAIT)) {
		LOG_ERR("failed to acquire chan lock");
		return -EBUSY;
	}

	/*
	 * For transports that support interrupts: disable them during polling
	 * to avoid unnecessary interrupts and ensure proper polling behavior.
	 * For polling-only transports: this is a no-op.
	 */
	if (scmi_transport_has_interrupt_support(proto->transport)) {
		status = scmi_transport_interrupt_enable(proto->transport, proto->tx, false);
		if (status < 0 && status != -ENOTSUP) {
			LOG_WRN("failed to disable interrupts: %d", status);
		}
	}

	ret = scmi_transport_send_message(proto->transport, proto->tx, msg);
	if (ret < 0) {
		goto cleanup;
	}

	/* Poll for completion */
	while (!scmi_transport_channel_is_free(proto->transport, proto->tx)) {
		/* Busy wait for channel to be free */
	}

	ret = scmi_transport_read_message(proto->transport, proto->tx, reply);
	if (ret < 0) {
		goto cleanup;
	}

cleanup:
	/* Restore interrupt state for interrupt-capable transports */
	if (status >= 0 && scmi_transport_has_interrupt_support(proto->transport)) {
		scmi_transport_interrupt_enable(proto->transport, proto->tx, true);
	}

	if (!k_is_pre_kernel()) {
		k_mutex_unlock(&proto->tx->lock);
	}

	return ret;
}

int scmi_send_message(struct scmi_protocol *proto, struct scmi_message *msg,
		      struct scmi_message *reply, bool use_polling)
{
	if (!proto->tx) {
		return -ENODEV;
	}

	if (!proto->tx->ready) {
		return -EINVAL;
	}

	/*
	 * For polling-only transports: always use polling
	 * For interrupt-capable transports: respect use_polling flag
	 */
	if (scmi_transport_is_polling_only(proto->transport) || use_polling) {
		return scmi_send_message_polling(proto, msg, reply);
	} else {
		return scmi_send_message_interrupt(proto, msg, reply);
	}
}
```

### 4. Mailbox Transport Implementation (mailbox.c)

Implement interrupt control operation:

```c
/* drivers/firmware/scmi/mailbox.c */

static int scmi_mbox_interrupt_enable(const struct device *transport,
				      struct scmi_channel *chan,
				      bool enable)
{
	struct scmi_mbox_channel *mbox_chan = chan->data;
	struct mbox_dt_spec *tx_reply;
	uint32_t comp_int;

	comp_int = enable ? SCMI_SHMEM_CHAN_FLAG_IRQ_BIT : 0;

	if (mbox_chan->tx_reply.dev) {
		tx_reply = &mbox_chan->tx_reply;
	} else {
		tx_reply = &mbox_chan->tx;
	}

	/* Update shared memory flags */
	scmi_shmem_update_flags(mbox_chan->shmem, SCMI_SHMEM_CHAN_FLAG_IRQ_BIT, comp_int);

	/* Enable/disable mailbox interrupt */
	return mbox_set_enabled_dt(tx_reply, enable);
}

static struct scmi_transport_api scmi_mbox_api = {
	.capabilities = SCMI_TRANSPORT_CAP_INTERRUPT | SCMI_TRANSPORT_CAP_POLLING,
	.setup_chan = scmi_mbox_setup_chan,
	.send_message = scmi_mbox_send_message,
	.read_message = scmi_mbox_read_message,
	.channel_is_free = scmi_mbox_channel_is_free,
	.interrupt_enable = scmi_mbox_interrupt_enable,  /* NEW */
};
```

### 5. SMC Transport Implementation (smc.c)

Declare polling-only capability:

```c
/* drivers/firmware/scmi/smc.c */

static int scmi_smc_setup_chan(const struct device *transport,
				struct scmi_channel *chan,
				bool tx)
{
	struct scmi_smc_channel *smc_chan = chan->data;

	if (!tx) {
		return -ENOTSUP;
	}

	/*
	 * SMC transport is polling-only. Set shared memory flags to
	 * indicate no interrupt-based communication.
	 */
	scmi_shmem_update_flags(smc_chan->shmem,
				SCMI_SHMEM_CHAN_FLAG_IRQ_BIT,
				0);

	LOG_DBG("SMC channel setup complete (polling-only mode)");

	return 0;
}

static struct scmi_transport_api scmi_smc_api = {
	.capabilities = SCMI_TRANSPORT_CAP_POLLING_ONLY,  /* Polling only */
	.setup_chan = scmi_smc_setup_chan,
	.send_message = scmi_smc_send_message,
	.read_message = scmi_smc_read_message,
	.channel_is_free = scmi_smc_channel_is_free,
	/* No interrupt_enable - not supported */
};
```

## Benefits of This Design

1. **Clean Abstraction**: Core layer doesn't know about transport-specific implementations
2. **Scalable**: Easy to add new transports with different capabilities
3. **Maintainable**: Each transport declares its capabilities explicitly
4. **No Compile-Time Hacks**: No `#ifdef` based on transport type in core
5. **Backward Compatible**: Existing mailbox transport continues to work with minimal changes
6. **Optional Operations**: Transports only implement operations they support
7. **Runtime Flexibility**: Transport capabilities checked at runtime, not compile-time

## Implementation Steps

1. Add capability flags and API changes to `transport.h`
2. Update `mailbox.c` to:
   - Add `capabilities` field
   - Move interrupt control logic to `interrupt_enable()` operation
3. Update `smc.c` to declare `SCMI_TRANSPORT_CAP_POLLING_ONLY`
4. Update `core.c` to:
   - Remove hardcoded `scmi_interrupt_enable()` function
   - Use transport capability queries
   - Call transport operations via API
5. Test both transports to verify functionality

## Alternative Designs Considered

### Alternative 1: Protocol-Level Polling Flag
Store a `polling_required` flag in `struct scmi_protocol` and set it based on transport.
- **Rejected**: Doesn't address the interrupt control abstraction issue

### Alternative 2: Separate Polling and Interrupt Transport APIs
Create two different API structures for different transport types.
- **Rejected**: Unnecessarily complicates the design and reduces code reuse

### Alternative 3: Callback-Based Interrupt Control
Pass callbacks from transport to core for interrupt management.
- **Rejected**: More complex than needed; operation pointer is cleaner

## Testing Plan

1. Test mailbox transport with interrupt mode
2. Test mailbox transport with polling mode (forced)
3. Test SMC transport with polling mode
4. Verify protocol negotiation works with both transports
5. Test protocol operations (clock, power, etc.) with both transports
