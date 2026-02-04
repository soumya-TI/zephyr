# SCMI (System Control and Management Interface) Subsystem

## Table of Contents
1. [Overview](#overview)
2. [Architecture](#architecture)
3. [Transport Layers](#transport-layers)
4. [Channel Allocation Models](#channel-allocation-models)
5. [Data Structures](#data-structures)
6. [Device Tree Configuration](#device-tree-configuration)
7. [Macro Expansion Hierarchy](#macro-expansion-hierarchy)
8. [Adding New Transports](#adding-new-transports)
9. [Design Guidelines](#design-guidelines)
10. [Examples](#examples)

---

## Overview

The SCMI subsystem provides a standard interface for system control and management operations in Zephyr. It implements the ARM SCMI specification, allowing communication between application processors and System Control Processors (SCP) for operations like:

- **Clock Management** (Protocol 0x14)
- **Power Domain Control** (Protocol 0x11)
- **System Power Management** (Protocol 0x12)
- **Pin Control** (Protocol 0x19)
- **Sensor Management** (Protocol 0x21)
- **Vendor Extensions** (e.g., NXP CPU protocol 0x82)

### Key Features
- Multiple transport layer support (Mailbox, SMC/HVC)
- Static and dynamic channel allocation
- Protocol multiplexing over shared channels
- Interrupt and polling communication modes
- Extensible protocol framework

---

## Architecture

```
┌─────────────────────────────────────────────────────────┐
│                    SCMI Protocols                       │
│  (Clock, Power, Pinctrl, System, Sensor, etc.)         │
│  - Each protocol has a scmi_protocol struct             │
│  - Protocol ID identifies the protocol type             │
│  - Protocols share channels via mutex synchronization   │
└────────────────────┬────────────────────────────────────┘
                     │
                     │ scmi_send_message()
                     ▼
┌─────────────────────────────────────────────────────────┐
│                    SCMI Core                            │
│  (drivers/firmware/scmi/core.c)                         │
│                                                         │
│  - Channel management & locking                         │
│  - Protocol version negotiation                         │
│  - Message routing and synchronization                  │
│  - Polling vs interrupt mode handling                   │
└────────────────────┬────────────────────────────────────┘
                     │
                     │ scmi_transport_* API
                     ▼
┌─────────────────────────────────────────────────────────┐
│              Transport Layer API                        │
│  (include/zephyr/drivers/firmware/scmi/transport.h)     │
│                                                         │
│  struct scmi_transport_api {                            │
│      int (*init)(...);                                  │
│      int (*setup_chan)(...);                            │
│      int (*send_message)(...);                          │
│      int (*read_message)(...);                          │
│      bool (*channel_is_free)(...);                      │
│      struct scmi_channel *(*request_channel)(...);      │
│  }                                                      │
└────────────────────┬────────────────────────────────────┘
                     │
         ┌───────────┴───────────┐
         ▼                       ▼
┌──────────────────┐    ┌──────────────────┐
│  Mailbox         │    │  SMC/HVC         │
│  Transport       │    │  Transport       │
│  (mailbox.c)     │    │  (smc.c)         │
│                  │    │                  │
│  - Doorbells     │    │  - Synchronous   │
│  - Interrupts    │    │  - Polling only  │
│  - Per-protocol  │    │  - Shared        │
│    channels      │    │    channel       │
└────────┬─────────┘    └────────┬─────────┘
         │                       │
         ▼                       ▼
┌──────────────────┐    ┌──────────────────┐
│  Shared Memory   │    │  Shared Memory   │
│  (SHMEM)         │    │  (SHMEM)         │
│  (shmem.c)       │    │  (shmem.c)       │
│                  │    │                  │
│  - Message       │    │  - Message       │
│    buffers       │    │    buffers       │
│  - Status flags  │    │  - Status flags  │
└──────────────────┘    └──────────────────┘
```

---

## Transport Layers

### 1. Mailbox Transport (`CONFIG_ARM_SCMI_MAILBOX_TRANSPORT`)

**Use Case:** Communication via doorbell interrupts and shared memory

**Characteristics:**
- Supports both interrupt and polling modes
- Can have per-protocol dedicated channels
- Uses mailbox (MU - Message Unit) hardware for signaling
- Bidirectional or unidirectional mailbox configurations

**Configuration:**
```kconfig
CONFIG_ARM_SCMI_MAILBOX_TRANSPORT=y
CONFIG_ARM_SCMI_TRANSPORT_HAS_STATIC_CHANNELS=y
CONFIG_ARM_SCMI_SHMEM=y
```

**Hardware Requirements:**
- Mailbox/doorbell peripheral (e.g., NXP MU)
- Shared memory region accessible by both cores
- Optional: Separate TX and TX_REPLY mailbox channels

---

### 2. SMC/HVC Transport (`CONFIG_ARM_SCMI_SMCC_TRANSPORT`)

**Use Case:** Communication via ARM Secure Monitor Call (SMC) or Hypervisor Call (HVC)

**Characteristics:**
- **Synchronous by design** - SMC/HVC blocks until firmware completes
- **Polling mode only** - no interrupt support
- **Single shared channel** - all protocols use the same base channel
- **Static allocation** - one base channel created at compile time, shared by all protocols
- Simple integration with ARM Trusted Firmware (ATF)

**Configuration:**
```kconfig
CONFIG_ARM_SCMI_SMCC_TRANSPORT=y
CONFIG_ARM_SCMI_SHMEM=y
CONFIG_ARM_SCMI_TRANSPORT_HAS_STATIC_CHANNELS=y  # Automatically selected
```

**Hardware Requirements:**
- ARM TrustZone or EL2 hypervisor support
- Shared memory region
- ATF or firmware implementing SCMI server

**Why Polling Only:**
- SMC/HVC instructions trap to EL3/EL2 and execute synchronously
- CPU blocks until the secure/hypervisor handler completes
- No mechanism for asynchronous notification
- Interrupt would be redundant since caller is already blocked

---

## Channel Allocation Models

### Static Channel Allocation

**Enabled by:** `CONFIG_ARM_SCMI_TRANSPORT_HAS_STATIC_CHANNELS=y`

**When to Use:**
- Transport supports per-protocol dedicated channels
- Memory isolation between protocols desired
- Mailbox transport with multiple MU units

**How It Works:**
1. Each protocol can optionally have a `shmem` property in device tree
2. If present, a dedicated channel is created at compile-time
3. If absent, protocol uses the base protocol channel (fallback)
4. Channels are statically allocated via macros

**Device Tree Example:**
```dts
scmi {
    compatible = "arm,scmi";
    shmem = <&scmi_shmem0>;        /* Base channel SHMEM */
    mboxes = <&mu5 0>;
    mbox-names = "tx";

    protocol@14 {  /* Clock protocol */
        compatible = "arm,scmi-clock";
        reg = <0x14>;
        shmem = <&scmi_shmem1>;    /* Dedicated SHMEM - creates new channel */
    };

    protocol@11 {  /* Power protocol */
        compatible = "arm,scmi-power";
        reg = <0x11>;
        /* No shmem - uses base channel */
    };
};
```

**Result:**
- Clock protocol gets dedicated channel with own SHMEM
- Power protocol shares base channel
- Reduced contention for frequently-used protocols

---

### Dynamic Channel Allocation

**Enabled by:** `CONFIG_ARM_SCMI_TRANSPORT_HAS_STATIC_CHANNELS` not set

**When to Use:**
- All protocols share a single channel
- Simple hardware with one communication path
- SMC/HVC transport (always dynamic)

**How It Works:**
1. Only base protocol channel is created
2. During initialization, `scmi_core_protocol_setup()` calls `request_channel()`
3. Transport can return dynamically allocated channel OR NULL
4. If NULL, protocol uses base channel (common case)
5. All protocols get pointer to the same shared channel

**Device Tree Example:**
```dts
scmi {
    compatible = "arm,scmi-smc";   /* SMC transport */
    arm,smc-id = <0x82004000>;     /* Single SMC function ID */
    shmem = <&scmi_shmem0>;        /* Single shared SHMEM */

    protocol@14 { reg = <0x14>; }; /* Clock - shares channel */
    protocol@11 { reg = <0x11>; }; /* Power - shares channel */
    protocol@19 { reg = <0x19>; }; /* Pinctrl - shares channel */
};
```

**Result:**
- All protocols point to `scmi_channel_16_0` (base channel)
- Channel mutex ensures serialized access
- Protocol ID in message header enables firmware demultiplexing

---

## Data Structures

### Core Structures

#### 1. `struct scmi_channel` (transport.h)
The generic channel abstraction used by all transports.

```c
struct scmi_channel {
    struct k_mutex lock;        /* Serializes protocol access to channel */
    struct k_sem sem;           /* Signals reply arrival (interrupt mode) */
    void *data;                 /* Transport-specific private data */
    scmi_channel_cb cb;         /* Callback for reply notification */
    bool ready;                 /* Channel initialization complete? */
};
```

**Usage:**
- `lock`: Acquired by protocol before sending, released after receiving reply
- `sem`: Given by transport callback when reply arrives, taken by protocol
- `data`: Points to `scmi_mbox_channel` or `scmi_smc_channel`
- `cb`: Set by core to `scmi_core_reply_cb()`
- `ready`: Set to `true` after `setup_chan()` succeeds

---

#### 2. `struct scmi_protocol` (protocol.h)
Represents a protocol instance (Clock, Power, etc.).

```c
struct scmi_protocol {
    uint32_t id;                    /* Protocol ID (0x14=Clock, 0x11=Power) */
    struct scmi_channel *tx;        /* Pointer to TX channel */
    const struct device *transport; /* Transport layer device */
    void *data;                     /* Protocol-specific private data */
    uint32_t version;               /* Negotiated protocol version */
};
```

**Linker Section:**
- Placed in `scmi_protocol` section via `STRUCT_SECTION_ITERABLE`
- Core iterates all protocols during initialization
- Each protocol driver defines one instance

---

#### 3. `struct scmi_message` (protocol.h)
Message envelope for SCMI commands and replies.

```c
struct scmi_message {
    uint32_t hdr;      /* Message header: proto_id, msg_id, token */
    uint32_t len;      /* Payload length in bytes */
    void *content;     /* Pointer to payload buffer */
};
```

**Header Format (via `SCMI_MESSAGE_HDR_MAKE`):**
```
Bits [7:0]   - Message ID
Bits [9:8]   - Message type (COMMAND=0, DELAYED_REPLY=2, NOTIFICATION=3)
Bits [17:10] - Protocol ID
Bits [27:18] - Token (for matching requests/replies)
```

---

#### 4. `struct scmi_transport_api` (transport.h)
Function pointers for transport layer operations.

```c
struct scmi_transport_api {
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
};
```

**Optional Functions:**
- `init`: Transport-specific initialization (optional)
- `request_channel`: For dynamic allocation (optional, return NULL for shared)

**Required Functions:**
- `setup_chan`: Prepare channel for use
- `send_message`: Transmit message (must not block)
- `read_message`: Retrieve reply from SHMEM
- `channel_is_free`: Check if firmware has processed request

---

### Transport-Specific Structures

#### 5. `struct scmi_mbox_channel` (mailbox.h)
Private data for mailbox transport.

```c
struct scmi_mbox_channel {
    const struct device *shmem;    /* SHMEM device */
    struct mbox_dt_spec tx;        /* TX doorbell */
    struct mbox_dt_spec tx_reply;  /* TX reply doorbell (optional) */
};
```

**Configurations:**
- **Bidirectional:** `tx` used for both send and receive interrupts
- **Unidirectional:** `tx` for send, `tx_reply` for receive

---

#### 6. `struct scmi_smc_channel` (smc.c)
Private data for SMC transport.

```c
struct scmi_smc_channel {
    const struct device *shmem;  /* SHMEM device */
    uint32_t func_id;            /* SMC function ID (e.g., 0x82004000) */
};
```

**Key Point:**
- Single `func_id` for all protocols
- Firmware demultiplexes based on protocol ID in message header

---

#### 7. `struct scmi_shmem_layout` (shmem.h)
Hardware layout of shared memory region.

```c
struct scmi_shmem_layout {
    volatile uint32_t res0;           /* Reserved */
    volatile uint32_t chan_status;    /* Channel status flags */
    volatile uint32_t res1[2];        /* Reserved */
    volatile uint32_t chan_flags;     /* IRQ enable, polling mode */
    volatile uint32_t len;            /* Message length */
    volatile uint32_t msg_hdr;        /* Message header */
    /* Followed by message payload */
};
```

**Status Flags:**
- `SCMI_SHMEM_CHAN_STATUS_BUSY_BIT`: Firmware processing message
- `SCMI_SHMEM_CHAN_FLAG_IRQ_BIT`: Enable interrupt on completion

---

### Protocol-Specific Structures

#### Clock Protocol (clk.h)

```c
struct scmi_clock_rate_config {
    uint32_t flags;     /* ASYNC, ROUND_UP, ROUND_DOWN */
    uint32_t clk_id;    /* Clock identifier */
    uint32_t rate[2];   /* 64-bit rate (low, high) */
};

struct scmi_clock_config {
    uint32_t clk_id;
    uint32_t attributes;  /* Enable/disable */
    uint32_t extended_cfg_val;
};
```

#### Power Domain Protocol (power.h)

```c
struct scmi_power_state_config {
    uint32_t flags;       /* ASYNC */
    uint32_t domain_id;   /* Power domain ID */
    uint32_t power_state; /* ON=0, OFF=3 */
};
```

#### Pinctrl Protocol (pinctrl.h)

```c
struct scmi_pinctrl_settings {
    uint32_t id;           /* Pin/group identifier */
    uint32_t function;     /* Function selector */
    uint32_t attributes;   /* Type (pin/group) */
    uint32_t config[ARM_SCMI_PINCTRL_MAX_CONFIG_SIZE];
};
```

---

## Device Tree Configuration

### Mailbox Transport Example (NXP i.MX943)

```dts
/ {
    reserved-memory {
        /* Shared memory for SCMI communication */
        scmi_shmem0: memory@44721000 {
            compatible = "arm,scmi-shmem";
            reg = <0x44721000 0x80>;  /* 128 bytes */
        };
    };

    firmware {
        scmi: scmi {
            compatible = "arm,scmi";
            shmem = <&scmi_shmem0>;    /* Base channel SHMEM */
            mboxes = <&mu8 0>;         /* Mailbox channel */
            mbox-names = "tx";         /* Bidirectional mode */

            #address-cells = <1>;
            #size-cells = <0>;

            scmi_devpd: protocol@11 {
                compatible = "arm,scmi-power";
                reg = <0x11>;  /* Protocol ID */
                #power-domain-cells = <1>;
                /* No shmem - shares base channel */
            };

            scmi_system: protocol@12 {
                compatible = "arm,scmi-system";
                reg = <0x12>;
            };

            scmi_clk: protocol@14 {
                compatible = "arm,scmi-clock";
                reg = <0x14>;
                #clock-cells = <1>;
            };

            scmi_iomuxc: protocol@19 {
                compatible = "arm,scmi-pinctrl";
                reg = <0x19>;
                pinctrl: pinctrl {
                    compatible = "nxp,imx943-pinctrl";
                };
            };
        };
    };

    soc {
        mu8: mbox@44720000 {
            compatible = "nxp,mbox-imx-mu";
            reg = <0x44720000 DT_SIZE_K(64)>;
            interrupts = <273 0>;
            #mbox-cells = <1>;
            status = "okay";
        };

        /* Example: Clock consumer */
        lpuart1: serial@44380000 {
            compatible = "nxp,imx-lpuart";
            reg = <0x44380000 DT_SIZE_K(64)>;
            clocks = <&scmi_clk IMX943_CLK_LPUART1>;
            status = "okay";
        };
    };
};
```

---

### SMC Transport Example (TI AM62L)

```dts
/ {
    reserved-memory {
        scmi_shmem: memory@9c000000 {
            compatible = "arm,scmi-shmem";
            reg = <0x00 0x9c000000 0x00 0x100>;  /* 256 bytes */
            no-map;
        };
    };

    firmware {
        scmi: scmi {
            compatible = "arm,scmi-smc";
            arm,smc-id = <0x82000400>;  /* SMC function ID */
            shmem = <&scmi_shmem>;

            #address-cells = <1>;
            #size-cells = <0>;

            scmi_clk: protocol@14 {
                compatible = "arm,scmi-clock";
                reg = <0x14>;
                #clock-cells = <1>;
            };

            scmi_power: protocol@11 {
                compatible = "arm,scmi-power";
                reg = <0x11>;
                #power-domain-cells = <1>;
            };
        };
    };
};
```

**Key Differences:**
- `compatible = "arm,scmi-smc"` instead of `"arm,scmi"`
- `arm,smc-id` property defines SMC function ID
- No `mboxes` property (uses SMC instruction)
- Single SHMEM region shared by all protocols

---

## Macro Expansion Hierarchy

Understanding the macro expansion is crucial for debugging and adding new transports. This section provides a brief overview, followed by a complete detailed walkthrough using the NXP iMX943 design as a concrete example.

### Quick Overview

The mailbox transport uses a complex macro hierarchy to generate channels from device tree:

1. **Top Level:** `DT_INST_SCMI_MAILBOX_DEFINE()` - Entry point
2. **Per-Protocol:** Iterate children, conditionally create channels
3. **Base Channel:** Always create one base protocol channel
4. **Protocol Binding:** Link protocols to their channels
5. **Device Registration:** Create transport device

---

## Complete NXP iMX943 Macro Expansion Walkthrough

This section provides a **complete step-by-step walkthrough** of how the device tree macros expand for the NXP iMX943 mailbox transport, showing exactly what C code gets generated.

### Given Device Tree (NXP iMX943 M33)

```dts
/* From dts/arm/nxp/nxp_imx943_m33.dtsi */
reserved-memory {
    scmi_shmem0: memory@44721000 {
        compatible = "arm,scmi-shmem";
        reg = <0x44721000 0x80>;  /* 128 bytes */
    };
};

/* From dts/arm/nxp/nxp_imx94x.dtsi */
firmware {
    scmi: scmi {
        compatible = "arm,scmi";
        shmem = <&scmi_shmem0>;
        mboxes = <&mu8 0>;
        mbox-names = "tx";

        #address-cells = <1>;
        #size-cells = <0>;

        protocol@11 {  /* Power domain */
            compatible = "arm,scmi-power";
            reg = <0x11>;
            /* NO shmem property */
        };

        protocol@14 {  /* Clock */
            compatible = "arm,scmi-clock";
            reg = <0x14>;
            /* NO shmem property */
        };

        protocol@19 {  /* Pinctrl */
            compatible = "arm,scmi-pinctrl";
            reg = <0x19>;
            /* NO shmem property */
        };
    };
};

/* Board-specific binding */
&scmi {
    shmem = <&scmi_shmem0>;
    mboxes = <&mu8 0>;
};

&mu8 {
    status = "okay";
};
```

**Key Points:**
- Single SHMEM region (`scmi_shmem0`)
- Single mailbox channel (`mu8`, channel 0)
- Three protocols, **none** have `shmem` properties
- All will share the base channel

---

### LEVEL 1: Top-Level Invocation

**File:** `drivers/firmware/scmi/mailbox.c:114`

```c
DT_INST_SCMI_MAILBOX_DEFINE(0, PRE_KERNEL_1,
                            CONFIG_ARM_SCMI_TRANSPORT_INIT_PRIORITY,
                            &scmi_mbox_api);
```

**Macro Definition:** `mailbox.h:103-106`

```c
#define DT_INST_SCMI_MAILBOX_DEFINE(inst, level, prio, api)           \
    DT_INST_FOREACH_CHILD_STATUS_OKAY(inst, SCMI_MBOX_PROTO_CHAN_DEFINE) \
    DT_INST_SCMI_MBOX_BASE_CHAN_DEFINE(inst)                          \
    DT_INST_SCMI_TRANSPORT_DEFINE(inst, NULL, NULL, NULL, level, prio, api)
```

**Expands to:**

```c
// Step 1: For each enabled child protocol node
DT_INST_FOREACH_CHILD_STATUS_OKAY(0, SCMI_MBOX_PROTO_CHAN_DEFINE)

// Step 2: Create base protocol channel
DT_INST_SCMI_MBOX_BASE_CHAN_DEFINE(0)

// Step 3: Register transport device
DT_INST_SCMI_TRANSPORT_DEFINE(0, NULL, NULL, NULL, PRE_KERNEL_1,
                              CONFIG_ARM_SCMI_TRANSPORT_INIT_PRIORITY,
                              &scmi_mbox_api)
```

---

### LEVEL 2: Per-Protocol Channel Iteration

**Step 1 Expansion:**

```c
DT_INST_FOREACH_CHILD_STATUS_OKAY(0, SCMI_MBOX_PROTO_CHAN_DEFINE)
```

Iterates over each child and invokes the macro:

```c
// For protocol@11 (Power)
SCMI_MBOX_PROTO_CHAN_DEFINE(DT_N_S_firmware_S_scmi_S_protocol_11)

// For protocol@14 (Clock)
SCMI_MBOX_PROTO_CHAN_DEFINE(DT_N_S_firmware_S_scmi_S_protocol_14)

// For protocol@19 (Pinctrl)
SCMI_MBOX_PROTO_CHAN_DEFINE(DT_N_S_firmware_S_scmi_S_protocol_19)
```

---

### LEVEL 3: Optional Channel Creation Check

**Macro Definition:** `mailbox.h:68-69`

```c
#define SCMI_MBOX_PROTO_CHAN_DEFINE(node_id) \
    _SCMI_MBOX_CHAN_DEFINE_OPTIONAL(node_id, DT_REG_ADDR(node_id), 0)
```

**For protocol@11 expands to:**

```c
_SCMI_MBOX_CHAN_DEFINE_OPTIONAL(
    DT_N_S_firmware_S_scmi_S_protocol_11,  // node_id
    0x11,                                   // DT_REG_ADDR = 17 decimal
    0                                       // index
)
```

**Macro Definition:** `mailbox.h:58-61`

```c
#define _SCMI_MBOX_CHAN_DEFINE_OPTIONAL(node_id, proto, idx)     \
    COND_CODE_1(DT_PROP_HAS_IDX(node_id, shmem, idx),            \
                (_SCMI_MBOX_CHAN_DEFINE(node_id, proto, idx)),   \
                ())
```

**Conditional Check:**

```c
DT_PROP_HAS_IDX(protocol@11, shmem, 0)  // Does protocol@11 have shmem[0]?
→ FALSE  // No shmem property in protocol@11
```

**Result:** `COND_CODE_1(FALSE, ..., ())` takes the empty branch:

```c
()  // NOTHING GENERATED!
```

**Same result for protocol@14 and protocol@19 - they also have no `shmem` property.**

**Outcome:** **No per-protocol channels created!**

---

### LEVEL 4: Base Protocol Channel Creation

**Step 2 Expansion:**

```c
DT_INST_SCMI_MBOX_BASE_CHAN_DEFINE(0)
```

**Macro Definition:** `mailbox.h:72-89`

```c
#define DT_INST_SCMI_MBOX_BASE_CHAN_DEFINE(inst)                  \
    BUILD_ASSERT(DT_INST_PROP_LEN(inst, mboxes) != 1 ||          \
                 (DT_INST_PROP_HAS_IDX(inst, shmem, 0) &&        \
                  DT_INST_PROP_HAS_NAME(inst, mboxes, tx)),      \
                 "bad bidirectional channel description");        \
                                                                  \
    BUILD_ASSERT(DT_INST_PROP_LEN(inst, mboxes) != 2 ||          \
                 (DT_INST_PROP_HAS_NAME(inst, mboxes, tx) &&     \
                  DT_INST_PROP_HAS_NAME(inst, mboxes, tx_reply)),\
                 "bad unidirectional channel description");       \
                                                                  \
    BUILD_ASSERT(DT_INST_PROP_LEN(inst, shmem) == 1,             \
                 "bad SHMEM count");                              \
                                                                  \
    BUILD_ASSERT(DT_INST_PROP_LEN(inst, mboxes) <= 2,            \
                 "bad mbox count");                               \
                                                                  \
    _SCMI_MBOX_CHAN_DEFINE(DT_INST(inst, DT_DRV_COMPAT),         \
                           SCMI_PROTOCOL_BASE, 0)
```

**Expands to:**

```c
// Compile-time assertions
BUILD_ASSERT(1 != 1 || (true && true), "...");  // Passes
BUILD_ASSERT(1 != 2 || (...), "...");           // Passes (1 != 2 is true)
BUILD_ASSERT(1 == 1, "bad SHMEM count");        // Passes
BUILD_ASSERT(1 <= 2, "bad mbox count");         // Passes

// Create base channel
_SCMI_MBOX_CHAN_DEFINE(
    DT_N_S_firmware_S_scmi,  // The scmi node
    SCMI_PROTOCOL_BASE,      // 16 (0x10) - base protocol ID
    0                        // index
)
```

---

### LEVEL 5: Channel Structure Definition

**Macro Definition:** `mailbox.h:49-52`

```c
#define _SCMI_MBOX_CHAN_DEFINE(node_id, proto, idx)                \
    _SCMI_MBOX_CHAN_DEFINE_PRIV_TX(node_id, proto);               \
    DT_SCMI_TRANSPORT_CHAN_DEFINE(node_id, idx, proto,            \
                                  &(_SCMI_MBOX_CHAN_NAME(proto, idx)));
```

**Expands to:**

```c
// 1. Define private transport-specific data
_SCMI_MBOX_CHAN_DEFINE_PRIV_TX(DT_N_S_firmware_S_scmi, 16);

// 2. Define public channel structure
DT_SCMI_TRANSPORT_CHAN_DEFINE(DT_N_S_firmware_S_scmi, 0, 16,
                              &(_SCMI_MBOX_CHAN_NAME(16, 0)));
```

---

### LEVEL 6: Private Channel Data

**Macro Definition:** `mailbox.h:35-41`

```c
#define _SCMI_MBOX_CHAN_DEFINE_PRIV_TX(node_id, proto)           \
    static struct scmi_mbox_channel _SCMI_MBOX_CHAN_NAME(proto, 0) = \
    {                                                             \
        .shmem = _SCMI_MBOX_SHMEM_BY_IDX(node_id, 0),            \
        .tx = _SCMI_MBOX_CHAN_DBELL(node_id, tx),                \
        .tx_reply = _SCMI_MBOX_CHAN_DBELL(node_id, tx_reply),    \
    }
```

---

### LEVEL 7: Name and Field Resolution

#### 7.1 Channel Name Generation

**Macro Chain:** `mailbox.h:25-26` → `util.h:61`

```c
_SCMI_MBOX_CHAN_NAME(16, 0)
→ CONCAT(SCMI_TRANSPORT_CHAN_NAME(16, 0), _, priv)
→ CONCAT(CONCAT(scmi_channel_, 16, _, 0), _, priv)
→ CONCAT(scmi_channel_16_0, _, priv)
→ scmi_channel_16_0_priv
```

#### 7.2 SHMEM Device Resolution

**Macro:** `mailbox.h:19-22`

```c
_SCMI_MBOX_SHMEM_BY_IDX(DT_N_S_firmware_S_scmi, 0)
→ COND_CODE_1(DT_PROP_HAS_IDX(scmi, shmem, 0),  // TRUE
              (DEVICE_DT_GET(DT_PROP_BY_IDX(scmi, shmem, 0))),
              (NULL))
→ DEVICE_DT_GET(DT_N_S_reserved_memory_S_memory_44721000)
→ &DEVICE_NAME_GET(scmi_shmem0)
```

#### 7.3 TX Doorbell Resolution

**Macro:** `mailbox.h:29-32`

```c
_SCMI_MBOX_CHAN_DBELL(DT_N_S_firmware_S_scmi, tx)
→ COND_CODE_1(DT_PROP_HAS_NAME(scmi, mboxes, tx),  // TRUE
              (MBOX_DT_SPEC_GET(scmi, tx)),
              ({ }))
→ MBOX_DT_SPEC_GET(scmi, tx)
→ {
    .dev = DEVICE_DT_GET(DT_N_S_soc_S_mbox_44720000),  // mu8
    .channel = 0
}
```

#### 7.4 TX Reply Doorbell Resolution

```c
_SCMI_MBOX_CHAN_DBELL(DT_N_S_firmware_S_scmi, tx_reply)
→ COND_CODE_1(DT_PROP_HAS_NAME(scmi, mboxes, tx_reply),  // FALSE
              (MBOX_DT_SPEC_GET(...)),
              ({ }))
→ { }  // Empty initializer - bidirectional mode
```

---

### LEVEL 8: Generated Private Structure

**Complete Expansion:**

```c
static struct scmi_mbox_channel scmi_channel_16_0_priv = {
    .shmem = &DEVICE_NAME_GET(scmi_shmem0),
    .tx = {
        .dev = &DEVICE_NAME_GET(mu8),
        .channel = 0
    },
    .tx_reply = { }  // Empty - bidirectional mailbox
};
```

**Memory Layout:**
```
scmi_channel_16_0_priv:
  +0x00: .shmem     → points to scmi_shmem0 device
  +0x08: .tx.dev    → points to mu8 device
  +0x10: .tx.channel = 0
  +0x14: .tx_reply  → empty (all zeros)
```

---

### LEVEL 9: Public Channel Structure

**Macro:** `util.h:136-140`

```c
#define DT_SCMI_TRANSPORT_CHAN_DEFINE(node_id, idx, proto, pdata)  \
    struct scmi_channel SCMI_TRANSPORT_CHAN_NAME(proto, idx) =     \
    {                                                               \
        .data = pdata,                                              \
    }
```

**Expands to:**

```c
struct scmi_channel scmi_channel_16_0 = {
    .data = &scmi_channel_16_0_priv,
    // Other fields (.lock, .sem, .cb, .ready) zero-initialized
};
```

**At Runtime (after `scmi_core_setup_chan`):**

```c
struct scmi_channel scmi_channel_16_0 = {
    .lock = { /* k_mutex initialized */ },
    .sem = { /* k_sem initialized */ },
    .data = &scmi_channel_16_0_priv,
    .cb = scmi_core_reply_cb,
    .ready = true
};
```

---

### LEVEL 10: Transport Device Registration

**Step 3 Expansion:**

```c
DT_INST_SCMI_TRANSPORT_DEFINE(0, NULL, NULL, NULL, PRE_KERNEL_1,
                              CONFIG_ARM_SCMI_TRANSPORT_INIT_PRIORITY,
                              &scmi_mbox_api)
```

**Macro:** `util.h:192-194`

```c
#define DT_INST_SCMI_TRANSPORT_DEFINE(inst, pm, data, config, level, prio, api) \
    DEVICE_DT_INST_DEFINE(inst, &scmi_core_transport_init,                      \
                          pm, data, config, level, prio, api)
```

**Expands to:**

```c
DEVICE_DT_INST_DEFINE(0,
                      &scmi_core_transport_init,  // IMPORTANT: core init, not driver!
                      NULL,  // pm
                      NULL,  // data
                      NULL,  // config
                      PRE_KERNEL_1,
                      CONFIG_ARM_SCMI_TRANSPORT_INIT_PRIORITY,
                      &scmi_mbox_api)
```

**Final Device Structure:**

```c
static const struct scmi_transport_api scmi_mbox_api = {
    .setup_chan = scmi_mbox_setup_chan,
    .send_message = scmi_mbox_send_message,
    .read_message = scmi_mbox_read_message,
    .channel_is_free = scmi_mbox_channel_is_free,
    // .request_channel = NULL (uses base channel)
};

const struct device DEVICE_DT_NAME_GET(DT_N_S_firmware_S_scmi) = {
    .name = "scmi",
    .config = NULL,
    .api = &scmi_mbox_api,
    .state = &z_devstate_scmi_0,
    .data = NULL,
    .init_fn = &scmi_core_transport_init,  // Core initializes, not driver
    .pm = NULL
};
```

**Why `scmi_core_transport_init` instead of driver init?**
- The core needs to bind channels to protocols during initialization
- Transport layer init happens **inside** core init via `scmi_transport_init()`
- This allows core to set up channels before protocols use them

---

### LEVEL 11: Protocol Binding

Each protocol driver (e.g., `clk.c`, `power.c`) binds to channels:

#### Clock Protocol (drivers/firmware/scmi/clk.c)

**Macro Invocation:**

```c
DT_SCMI_TRANSPORT_CHANNELS_DECLARE(DT_N_S_firmware_S_scmi_S_protocol_14)
```

**Macro:** `util.h:93-94`

```c
#define DT_SCMI_TRANSPORT_CHANNELS_DECLARE(node_id) \
    DT_SCMI_TRANSPORT_TX_CHAN_DECLARE(node_id)
```

**Macro:** `util.h:74-79`

```c
#define DT_SCMI_TRANSPORT_TX_CHAN_DECLARE(node_id)                    \
    COND_CODE_1(DT_SCMI_TRANSPORT_PROTO_HAS_CHAN(node_id, 0),        \
                (extern struct scmi_channel                            \
                 SCMI_TRANSPORT_CHAN_NAME(DT_REG_ADDR_RAW(node_id), 0);), \
                (extern struct scmi_channel                            \
                 SCMI_TRANSPORT_CHAN_NAME(SCMI_PROTOCOL_BASE, 0);))
```

**Check:** `util.h:55-56`

```c
#define DT_SCMI_TRANSPORT_PROTO_HAS_CHAN(node_id, idx) \
    DT_PROP_HAS_IDX(node_id, shmem, idx)
```

**Evaluation:**

```c
DT_PROP_HAS_IDX(protocol@14, shmem, 0)  // Does protocol@14 have shmem?
→ FALSE  // No shmem property
```

**Result:**

```c
extern struct scmi_channel SCMI_TRANSPORT_CHAN_NAME(SCMI_PROTOCOL_BASE, 0);
→ extern struct scmi_channel scmi_channel_16_0;
```

---

#### Protocol Structure Definition

**Macro Invocation:**

```c
DT_SCMI_PROTOCOL_DATA_DEFINE(
    DT_N_S_firmware_S_scmi_S_protocol_14,
    SCMI_PROTOCOL_CLOCK,  // 20 (0x14)
    NULL,                 // no private data
    0x30000               // version 3.0.0
)
```

**Macro:** `util.h:157-164`

```c
#define DT_SCMI_PROTOCOL_DATA_DEFINE(node_id, proto, pdata, version_val) \
    STRUCT_SECTION_ITERABLE(scmi_protocol, SCMI_PROTOCOL_NAME(proto)) =  \
    {                                                                     \
        .id = proto,                                                      \
        .tx = DT_SCMI_TRANSPORT_TX_CHAN(node_id),                        \
        .data = pdata,                                                    \
        .version = version_val                                            \
    }
```

**TX Channel Resolution:** `util.h:119-122`

```c
#define DT_SCMI_TRANSPORT_TX_CHAN(node_id)                         \
    COND_CODE_1(DT_SCMI_TRANSPORT_PROTO_HAS_CHAN(node_id, 0),     \
                (&SCMI_TRANSPORT_CHAN_NAME(DT_REG_ADDR_RAW(node_id), 0)), \
                (&SCMI_TRANSPORT_CHAN_NAME(SCMI_PROTOCOL_BASE, 0)))
```

**Evaluation:**

```c
DT_PROP_HAS_IDX(protocol@14, shmem, 0) → FALSE
→ Take else branch
→ &SCMI_TRANSPORT_CHAN_NAME(SCMI_PROTOCOL_BASE, 0)
→ &scmi_channel_16_0  // USES BASE CHANNEL!
```

**Final Structure:**

```c
STRUCT_SECTION_ITERABLE(scmi_protocol, scmi_protocol_20) = {
    .id = 20,                     // SCMI_PROTOCOL_CLOCK
    .tx = &scmi_channel_16_0,     // ← SHARES BASE CHANNEL!
    .transport = NULL,            // Set by core during init
    .data = NULL,
    .version = 0x30000
};
```

---

### LEVEL 12: Complete Object Hierarchy

**Final Generated Code Summary:**

```c
// ============================================================
// 1. Private mailbox-specific data (transport layer)
// ============================================================
static struct scmi_mbox_channel scmi_channel_16_0_priv = {
    .shmem = &DEVICE_NAME_GET(scmi_shmem0),
    .tx = { .dev = &DEVICE_NAME_GET(mu8), .channel = 0 },
    .tx_reply = { }
};

// ============================================================
// 2. Public SCMI channel (generic, all transports use this)
// ============================================================
struct scmi_channel scmi_channel_16_0 = {
    .lock = { /* initialized by scmi_core_setup_chan */ },
    .sem = { /* initialized by scmi_core_setup_chan */ },
    .data = &scmi_channel_16_0_priv,
    .cb = NULL,  // Set to scmi_core_reply_cb during setup
    .ready = false  // Set to true after setup
};

// ============================================================
// 3. Protocol instances (placed in linker section)
// ============================================================
STRUCT_SECTION_ITERABLE(scmi_protocol, scmi_protocol_17) = {  // Power
    .id = 17,
    .tx = &scmi_channel_16_0,  // ← SHARED
    .transport = NULL,         // Set by core
    .data = NULL,
    .version = 0x30000
};

STRUCT_SECTION_ITERABLE(scmi_protocol, scmi_protocol_20) = {  // Clock
    .id = 20,
    .tx = &scmi_channel_16_0,  // ← SHARED
    .transport = NULL,
    .data = NULL,
    .version = 0x30000
};

STRUCT_SECTION_ITERABLE(scmi_protocol, scmi_protocol_25) = {  // Pinctrl
    .id = 25,
    .tx = &scmi_channel_16_0,  // ← SHARED
    .transport = NULL,
    .data = NULL,
    .version = 0x30000
};

// ============================================================
// 4. Transport device
// ============================================================
static const struct scmi_transport_api scmi_mbox_api = {
    .setup_chan = scmi_mbox_setup_chan,
    .send_message = scmi_mbox_send_message,
    .read_message = scmi_mbox_read_message,
    .channel_is_free = scmi_mbox_channel_is_free,
};

const struct device DEVICE_DT_NAME_GET(DT_N_S_firmware_S_scmi) = {
    .name = "scmi",
    .api = &scmi_mbox_api,
    .init_fn = &scmi_core_transport_init,
    /* ... */
};
```

---

### Visual Object Hierarchy

```
Device Tree Input
═════════════════════════════════════════════════════════
scmi {
    shmem = <&scmi_shmem0>;     /* Single SHMEM */
    mboxes = <&mu8 0>;          /* Single mailbox */

    protocol@11 { reg = <0x11>; /* no shmem */ };
    protocol@14 { reg = <0x14>; /* no shmem */ };
    protocol@19 { reg = <0x19>; /* no shmem */ };
}

Macro Processing
═════════════════════════════════════════════════════════
FOR_EACH_CHILD(protocol@11, @14, @19):
  ├─ protocol@11: No shmem → SKIP channel creation
  ├─ protocol@14: No shmem → SKIP channel creation
  └─ protocol@19: No shmem → SKIP channel creation

BASE_CHAN_DEFINE:
  └─ Creates ONE channel: scmi_channel_16_0

Generated C Structures
═════════════════════════════════════════════════════════

┌──────────────────────────────────────────────────────┐
│ scmi_channel_16_0_priv (static, private)            │
│ ─────────────────────────────────────────────────── │
│ struct scmi_mbox_channel {                          │
│   .shmem = &scmi_shmem0      ◄── SHMEM device       │
│   .tx = { .dev=&mu8, .ch=0 } ◄── Mailbox doorbell   │
│   .tx_reply = { }            ◄── Empty (bidirect.)  │
│ }                                                    │
└──────────────────────────────────────────────────────┘
                      ▲
                      │ .data pointer
                      │
┌──────────────────────────────────────────────────────┐
│ scmi_channel_16_0 (global, BASE CHANNEL)            │
│ ─────────────────────────────────────────────────── │
│ struct scmi_channel {                               │
│   .lock   = { ... }          ◄── Serializes access  │
│   .sem    = { ... }          ◄── Signals replies    │
│   .data   = &chan_16_0_priv  ◄── Transport data     │
│   .cb     = scmi_core_reply_cb                      │
│   .ready  = true (after setup)                      │
│ }                                                    │
└───────▲─────────────────▲─────────────────▲─────────┘
        │                 │                 │
        │ .tx            │ .tx             │ .tx
        │                 │                 │
   ┌────────┐       ┌────────┐       ┌────────┐
   │Power   │       │Clock   │       │Pinctrl │
   │Proto   │       │Proto   │       │Proto   │
   │────────│       │────────│       │────────│
   │id=0x11 │       │id=0x14 │       │id=0x19 │
   │ver=3.0 │       │ver=3.0 │       │ver=3.0 │
   └────────┘       └────────┘       └────────┘

ALL THREE PROTOCOLS SHARE THE SAME CHANNEL!
Synchronized by scmi_channel.lock mutex
```

---

### Initialization Flow

```
System Boot
    │
    ├─> PRE_KERNEL_1
    │   └─> scmi_core_transport_init(&scmi_device)
    │       │
    │       ├─> scmi_transport_init(&scmi_device)
    │       │   └─> /* No .init in scmi_mbox_api, returns 0 */
    │       │
    │       └─> scmi_core_protocol_setup(&scmi_device)
    │           │
    │           └─> STRUCT_SECTION_FOREACH(scmi_protocol, proto) {
    │               │
    │               ├─> proto->transport = &scmi_device
    │               │
    │               ├─> proto->tx already set by DT macros
    │               │   (proto->tx == &scmi_channel_16_0 for all)
    │               │
    │               ├─> scmi_core_setup_chan(proto->tx, true)
    │               │   │
    │               │   ├─> k_mutex_init(&chan->lock)
    │               │   ├─> k_sem_init(&chan->sem, 0, 1)
    │               │   ├─> chan->cb = scmi_core_reply_cb
    │               │   │
    │               │   └─> scmi_transport_setup_chan()
    │               │       └─> scmi_mbox_setup_chan()
    │               │           ├─> Register mailbox callback
    │               │           ├─> Enable mailbox interrupt
    │               │           └─> Enable IRQ flag in SHMEM
    │               │
    │               ├─> chan->ready = true
    │               │
    │               └─> scmi_core_protocol_negotiate(proto)
    │                   ├─> Get platform version
    │                   ├─> Negotiate version if needed
    │                   └─> LOG_INF("Protocol 0x%X ready", proto->id)
    │               }
    │
    └─> Protocols ready for use
```

---

### Message Flow Example: Clock Rate Get

```
1. Driver calls scmi_clock_rate_get(scmi_protocol_20, clk_id, &rate)
   │
   ├─> Prepare message:
   │   msg.hdr = SCMI_MESSAGE_HDR_MAKE(
   │       SCMI_CLK_MSG_CLOCK_RATE_GET,  // 0x6
   │       SCMI_COMMAND,                  // 0x0
   │       20,                            // proto_id = 0x14
   │       0                              // token
   │   )
   │   msg.content = &clk_id
   │   msg.len = 4
   │
   └─> scmi_send_message(proto, &msg, &reply, false)
       │
       ├─> k_mutex_lock(&proto->tx->lock, K_NO_WAIT)
       │   ├─> Acquires scmi_channel_16_0.lock
       │   └─> Blocks other protocols
       │
       ├─> scmi_transport_send_message(proto->transport, proto->tx, msg)
       │   └─> scmi_mbox_send_message()
       │       ├─> scmi_shmem_write_message(chan->data->shmem, msg)
       │       │   └─> Writes to scmi_shmem0:
       │       │       layout->chan_status = BUSY
       │       │       layout->len = 4
       │       │       layout->msg_hdr = 0x00140600
       │       │       layout->payload[0] = clk_id
       │       │
       │       └─> mbox_send_dt(&chan->data->tx, NULL)
       │           └─> Rings MU8 doorbell
       │               └─> ATF/Firmware receives interrupt
       │
       ├─> k_sem_take(&proto->tx->sem, timeout)
       │   └─> Blocks waiting for reply
       │
       ├─> [Firmware processes request]
       │   ├─> Reads protocol_id=20 from msg_hdr
       │   ├─> Routes to clock handler
       │   ├─> Gets clock rate
       │   ├─> Writes reply to SHMEM:
       │   │   layout->chan_status = FREE
       │   │   layout->payload[0] = SCMI_SUCCESS
       │   │   layout->payload[1] = rate_low
       │   │   layout->payload[2] = rate_high
       │   │
       │   └─> Sends mailbox interrupt (MU8 TX_REPLY or TX)
       │
       ├─> Mailbox interrupt fires
       │   └─> scmi_mbox_cb()
       │       └─> chan->cb(chan)
       │           └─> scmi_core_reply_cb()
       │               └─> k_sem_give(&chan->sem)
       │
       ├─> k_sem_take() returns
       │
       ├─> scmi_transport_read_message()
       │   └─> scmi_mbox_read_message()
       │       └─> scmi_shmem_read_message()
       │           └─> Reads reply from SHMEM
       │
       ├─> k_mutex_unlock(&proto->tx->lock)
       │   └─> Releases channel for other protocols
       │
       └─> Returns rate to caller

2. Next protocol (e.g., Power) can now acquire lock and send message
```

---

### Key Insights from NXP Design

1. **Single Channel Created:**
   - Only `scmi_channel_16_0` exists (base protocol)
   - Name comes from: `scmi_channel_<proto_id>_<index>`
   - Proto ID 16 = `SCMI_PROTOCOL_BASE`

2. **All Protocols Share:**
   - Power (17), Clock (20), Pinctrl (25) all point to same channel
   - Decision point: `DT_PROP_HAS_IDX(protocol, shmem, 0)`
   - No `shmem` property → use base channel

3. **Two-Layer Architecture:**
   - **Generic:** `struct scmi_channel` (core uses this)
   - **Specific:** `struct scmi_mbox_channel` (transport uses this)
   - Connected via `.data` pointer

4. **Serialization via Mutex:**
   - `scmi_channel.lock` ensures one protocol at a time
   - Acquired before sending, released after receiving reply
   - Prevents concurrent access to shared SHMEM

5. **Init Magic:**
   - `scmi_core_transport_init` in device init function
   - Core sets up channels before protocol drivers use them
   - Transport driver init happens inside core init

6. **DT Flexibility:**
   - Can add `shmem = <&scmi_shmem1>;` to any protocol node
   - Would create dedicated channel for that protocol
   - Reduces contention for high-frequency protocols

7. **Protocol Demultiplexing:**
   - Firmware reads protocol ID from message header bits [17:10]
   - Single mailbox/SHMEM pair can serve all protocols
   - Simple hardware requirement: one MU + one SHMEM region

---

### What If We Added Dedicated Channel?

**Modified Device Tree:**

```dts
protocol@14 {  /* Clock protocol */
    compatible = "arm,scmi-clock";
    reg = <0x14>;
    shmem = <&scmi_shmem1>;  /* ADD THIS! */
};
```

**Result:**

```c
// New private data
static struct scmi_mbox_channel scmi_channel_20_0_priv = {
    .shmem = &scmi_shmem1,      // Different SHMEM!
    .tx = { .dev = &mu8, .channel = 0 },  // Same mailbox
    .tx_reply = { }
};

// New channel
struct scmi_channel scmi_channel_20_0 = {
    .data = &scmi_channel_20_0_priv,
    /* ... */
};

// Clock protocol now uses dedicated channel
STRUCT_SECTION_ITERABLE(scmi_protocol, scmi_protocol_20) = {
    .id = 20,
    .tx = &scmi_channel_20_0,  // ← DEDICATED!
    /* ... */
};

// Power and Pinctrl still share base
scmi_protocol_17.tx = &scmi_channel_16_0;
scmi_protocol_25.tx = &scmi_channel_16_0;
```

**Benefits:**
- Clock operations don't block power operations
- Separate SHMEM = memory isolation
- Still serialized within each channel

---

This completes the **comprehensive macro expansion walkthrough** for the NXP iMX943 mailbox design!

---

## Adding New Transports

### Step 1: Create Transport Driver

Create `drivers/firmware/scmi/<transport>.c`:

```c
#include <zephyr/drivers/firmware/scmi/transport.h>
#include <zephyr/drivers/firmware/scmi/shmem.h>

/* Transport-specific channel data */
struct scmi_<transport>_channel {
    const struct device *shmem;
    /* Add transport-specific fields */
};

static int scmi_<transport>_setup_chan(const struct device *transport,
                                       struct scmi_channel *chan,
                                       bool tx)
{
    struct scmi_<transport>_channel *tchan = chan->data;

    /* Initialize transport-specific channel */
    /* Set up interrupts, configure hardware, etc. */

    return 0;
}

static int scmi_<transport>_send_message(const struct device *transport,
                                         struct scmi_channel *chan,
                                         struct scmi_message *msg)
{
    struct scmi_<transport>_channel *tchan = chan->data;
    int ret;

    /* Write message to SHMEM */
    ret = scmi_shmem_write_message(tchan->shmem, msg);
    if (ret < 0) {
        return ret;
    }

    /* Trigger transport-specific signaling (doorbell, SMC, etc.) */

    return 0;
}

static int scmi_<transport>_read_message(const struct device *transport,
                                         struct scmi_channel *chan,
                                         struct scmi_message *msg)
{
    struct scmi_<transport>_channel *tchan = chan->data;

    return scmi_shmem_read_message(tchan->shmem, msg);
}

static bool scmi_<transport>_channel_is_free(const struct device *transport,
                                             struct scmi_channel *chan)
{
    struct scmi_<transport>_channel *tchan = chan->data;

    return scmi_shmem_channel_status(tchan->shmem) &
           SCMI_SHMEM_CHAN_STATUS_BUSY_BIT;
}

static struct scmi_transport_api scmi_<transport>_api = {
    .setup_chan = scmi_<transport>_setup_chan,
    .send_message = scmi_<transport>_send_message,
    .read_message = scmi_<transport>_read_message,
    .channel_is_free = scmi_<transport>_channel_is_free,
    /* .request_channel optional */
};

/* Define transport using appropriate macro */
DT_INST_SCMI_<TRANSPORT>_DEFINE(0, PRE_KERNEL_1,
                                CONFIG_ARM_SCMI_TRANSPORT_INIT_PRIORITY,
                                &scmi_<transport>_api);
```

---

### Step 2: Create Header File

Create `drivers/firmware/scmi/<transport>.h` if needed:

```c
#ifndef _ZEPHYR_DRIVERS_FIRMWARE_SCMI_<TRANSPORT>_H_
#define _ZEPHYR_DRIVERS_FIRMWARE_SCMI_<TRANSPORT>_H_

#include <zephyr/drivers/firmware/scmi/transport.h>
#include <zephyr/drivers/firmware/scmi/util.h>

/* Define channel creation macros */
#define DT_INST_SCMI_<TRANSPORT>_BASE_CHAN_DEFINE(inst) \
    /* Create base protocol channel */ \
    static struct scmi_<transport>_channel \
        scmi_channel_##SCMI_PROTOCOL_BASE##_0_priv = { \
        .shmem = DEVICE_DT_GET(DT_INST_PROP_BY_IDX(inst, shmem, 0)), \
        /* ... */ \
    }; \
    DT_SCMI_TRANSPORT_CHAN_DEFINE(DT_INST(inst, DT_DRV_COMPAT), 0, \
                                  SCMI_PROTOCOL_BASE, \
                                  &scmi_channel_##SCMI_PROTOCOL_BASE##_0_priv)

#define DT_INST_SCMI_<TRANSPORT>_DEFINE(inst, level, prio, api) \
    DT_INST_SCMI_<TRANSPORT>_BASE_CHAN_DEFINE(inst) \
    DT_INST_SCMI_TRANSPORT_DEFINE(inst, NULL, NULL, NULL, level, prio, api)

#endif
```

---

### Step 3: Create Device Tree Binding

Create `dts/bindings/firmware/arm,scmi-<transport>.yaml`:

```yaml
description: |
    SCMI transport using <transport_description>

compatible: "arm,scmi-<transport>"

include: [base.yaml]

properties:
  shmem:
    type: phandle
    required: true
    description: |
      Phandle to shared memory region for SCMI messages.

  # Add transport-specific properties
  <transport>-property:
    type: int
    required: true
    description: |
      Description of transport-specific property.

  '#address-cells':
    const: 1

  '#size-cells':
    const: 0
```

---

### Step 4: Add Kconfig Options

Add to `drivers/firmware/scmi/Kconfig`:

```kconfig
config ARM_SCMI_<TRANSPORT>_TRANSPORT
	bool "SCMI transport based on <transport>"
	default y
	depends on DT_HAS_ARM_SCMI_<TRANSPORT>_ENABLED
	depends on ARM_SCMI_SHMEM
	# Add select for STATIC_CHANNELS if applicable
	help
	  Enable support for SCMI transport based on <transport>.
```

---

### Step 5: Update CMakeLists.txt

Add to `drivers/firmware/scmi/CMakeLists.txt`:

```cmake
zephyr_library_sources_ifdef(CONFIG_ARM_SCMI_<TRANSPORT>_TRANSPORT <transport>.c)
```

---

## Design Guidelines

### For SMC/HVC Transport

✅ **DO:**
- Keep single shared channel model
- Use polling mode (SMC is synchronous)
- Allow optional multiple SHMEM for isolation
- Document the single function ID model

❌ **DON'T:**
- Try to add interrupt support (SMC blocks caller)
- Create virtual channels without hardware support
- Add multiple SMC function IDs unless firmware designed for it
- Use `STATIC_CHANNELS` config

---

### For Mailbox Transport

✅ **DO:**
- Support both static and dynamic allocation
- Enable per-protocol channels via DT `shmem` property
- Support both interrupt and polling modes
- Use `STATIC_CHANNELS` config

❌ **DON'T:**
- Force all protocols to dedicated channels
- Skip mutex synchronization
- Assume bidirectional mailbox (support both)

---

### General Best Practices

1. **Channel Serialization:**
   - Always use channel mutex, even for dedicated channels
   - Only one outstanding message per channel at a time
   - Firmware may not support concurrent requests

2. **Error Handling:**
   - Check SCMI status codes from replies
   - Convert to errno via `scmi_status_to_errno()`
   - Handle timeout scenarios properly

3. **Polling vs Interrupts:**
   - Use `k_is_pre_kernel()` to auto-select mode
   - Pre-kernel: Always polling
   - Post-kernel: Interrupt if supported, else polling

4. **Protocol Versioning:**
   - Always negotiate protocol version
   - Handle version mismatches gracefully
   - Downgrade to common version if possible

5. **Memory Barriers:**
   - SHMEM layout uses `volatile` qualifiers
   - Use appropriate memory barriers for DMA/cache coherency
   - Flush cache before write, invalidate before read

---

## Examples

### Example 1: Clock Protocol Usage

```c
#include <zephyr/drivers/firmware/scmi/clk.h>

void configure_uart_clock(void)
{
    struct scmi_protocol *clk_proto = &scmi_protocol_20;  /* Clock protocol */
    struct scmi_clock_rate_config cfg;
    uint32_t actual_rate;
    int ret;

    /* Set clock rate */
    cfg.flags = 0;  /* Synchronous, round to closest */
    cfg.clk_id = IMX943_CLK_LPUART1;
    cfg.rate[0] = 115200 * 16;  /* 64-bit rate, low 32 bits */
    cfg.rate[1] = 0;            /* High 32 bits */

    ret = scmi_clock_rate_set(clk_proto, &cfg);
    if (ret < 0) {
        LOG_ERR("Failed to set clock rate: %d", ret);
        return;
    }

    /* Verify actual rate */
    ret = scmi_clock_rate_get(clk_proto, IMX943_CLK_LPUART1, &actual_rate);
    if (ret == 0) {
        LOG_INF("UART clock set to %u Hz", actual_rate);
    }

    /* Enable clock */
    struct scmi_clock_config clk_cfg = {
        .clk_id = IMX943_CLK_LPUART1,
        .attributes = 1,  /* Enable */
    };

    ret = scmi_clock_config_set(clk_proto, &clk_cfg);
    if (ret < 0) {
        LOG_ERR("Failed to enable clock: %d", ret);
    }
}
```

---

### Example 2: Power Domain Control

```c
#include <zephyr/drivers/firmware/scmi/power.h>

void power_on_peripheral(uint32_t domain_id)
{
    struct scmi_protocol *power_proto = &scmi_protocol_17;  /* Power protocol */
    struct scmi_power_state_config cfg;
    int ret;

    /* Power on domain */
    cfg.flags = 0;  /* Synchronous */
    cfg.domain_id = domain_id;
    cfg.power_state = 0;  /* 0 = ON */

    ret = scmi_power_state_set(power_proto, &cfg);
    if (ret < 0) {
        LOG_ERR("Failed to power on domain %u: %d", domain_id, ret);
        return;
    }

    LOG_INF("Power domain %u enabled", domain_id);
}
```

---

### Example 3: Custom Protocol (Vendor Extension)

```c
/* In drivers/firmware/scmi/vendor/custom.c */

#define SCMI_PROTOCOL_CUSTOM 0x80  /* Vendor-specific protocol ID */

struct custom_cmd {
    uint32_t param1;
    uint32_t param2;
};

struct custom_reply {
    int32_t status;
    uint32_t result;
};

int custom_protocol_operation(struct scmi_protocol *proto,
                              uint32_t p1, uint32_t p2,
                              uint32_t *result)
{
    struct scmi_message msg, reply;
    struct custom_cmd cmd = { .param1 = p1, .param2 = p2 };
    struct custom_reply reply_buf;
    int ret;

    /* Build command message */
    msg.hdr = SCMI_MESSAGE_HDR_MAKE(
        0x3,  /* Custom command ID */
        SCMI_COMMAND,
        proto->id,
        0x0  /* Token */
    );
    msg.len = sizeof(cmd);
    msg.content = &cmd;

    /* Setup reply buffer */
    reply.hdr = msg.hdr;
    reply.len = sizeof(reply_buf);
    reply.content = &reply_buf;

    /* Send message */
    ret = scmi_send_message(proto, &msg, &reply, false);
    if (ret < 0) {
        return ret;
    }

    /* Check status */
    if (reply_buf.status != SCMI_SUCCESS) {
        return scmi_status_to_errno(reply_buf.status);
    }

    *result = reply_buf.result;
    return 0;
}

/* Register protocol */
DT_SCMI_PROTOCOL_DEFINE_NODEV(
    DT_NODE_PATH(/firmware/scmi/protocol@80),
    NULL,  /* No private data */
    0x10000  /* Version 1.0.0 */
);
```

---

## Debugging Tips

### 1. Enable SCMI Logging

```kconfig
CONFIG_LOG=y
CONFIG_ARM_SCMI_LOG_LEVEL_DBG=y
```

### 2. Check Channel Initialization

```c
LOG_DBG("Channel %p ready: %d, data: %p",
        chan, chan->ready, chan->data);
```

### 3. Monitor SHMEM Status

```c
uint32_t status = scmi_shmem_channel_status(shmem);
LOG_DBG("SHMEM status: 0x%x (busy=%d)",
        status, !!(status & SCMI_SHMEM_CHAN_STATUS_BUSY_BIT));
```

### 4. Trace Message Flow

```c
LOG_HEXDUMP_DBG(msg->content, msg->len, "SCMI TX");
/* After reply */
LOG_HEXDUMP_DBG(reply->content, reply->len, "SCMI RX");
```

### 5. Verify Protocol Binding

```c
STRUCT_SECTION_FOREACH(scmi_protocol, proto) {
    LOG_INF("Protocol 0x%02x: tx=%p, transport=%p",
            proto->id, proto->tx, proto->transport);
}
```

---

## Performance Considerations

### Shared Channel Contention

When all protocols share one channel:
- Worst case: Sequential access with mutex contention
- Each protocol waits for previous to complete
- Typical latency: 100-500 µs per message

**Mitigation:**
- Add per-protocol SHMEM to reduce SHMEM access conflicts
- Use dedicated channels for high-frequency protocols (clocks)
- Cache frequently-read values to minimize SCMI calls

### Polling vs Interrupt Overhead

**Polling mode:**
- ✅ Lower latency (no IRQ overhead)
- ✅ Simpler, deterministic
- ❌ CPU busy-wait during message processing
- Best for: SMC, pre-kernel, short transactions

**Interrupt mode:**
- ✅ CPU sleeps during processing
- ✅ Better for power management
- ❌ Interrupt latency overhead
- ❌ Context switch overhead
- Best for: Mailbox, post-kernel, long transactions

### SHMEM Access Optimization

- Align SHMEM buffers to cache line size
- Use uncached memory for SHMEM if possible
- Minimize SHMEM reads (cache status in driver)
- Batch multiple operations when possible

---

## References

- [ARM SCMI Specification v3.2](https://developer.arm.com/documentation/den0056/latest/)
- [ARM Trusted Firmware SCMI Implementation](https://github.com/ARM-software/arm-trusted-firmware)
- Zephyr SCMI Driver: `drivers/firmware/scmi/`
- Zephyr SCMI Bindings: `dts/bindings/firmware/arm,scmi*.yaml`

---

## Contributing

When adding new protocols or transports:

1. Follow existing naming conventions
2. Add comprehensive device tree bindings
3. Include example device tree configurations
4. Document protocol-specific structures
5. Add Kconfig help text with examples
6. Test with both polling and interrupt modes
7. Verify with multiple protocols active

---

## License

Copyright 2024-2026 NXP, Texas Instruments, and contributors

SPDX-License-Identifier: Apache-2.0
