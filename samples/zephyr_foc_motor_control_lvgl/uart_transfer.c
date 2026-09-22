#include "zephyr/kernel.h"
#include "zephyr/kernel/thread_stack.h"
#include <stdlib.h>
#include <zephyr/drivers/uart.h>

#define BUF_LEN 10
#define TX_THREAD_STACK_SIZE 128

K_THREAD_STACK_DEFINE(tx_thread_stack, TX_THREAD_STACK_SIZE);

K_SEM_DEFINE(buf_ready_sem, 0, 1);

#ifdef CONFIG_UART_INTERRUPT_DRIVEN
K_SEM_DEFINE(interrupt_tx_complete_sem, 0, 1);
#endif

struct uart_cb_data {
    char *buf;
    size_t len;
    size_t sent;
};

extern uint16_t idx;
extern uint8_t buffer[BUF_LEN];

static const struct device *const uart_dev = DEVICE_DT_GET(DT_NODELABEL(unicomm0_uart));
bool tx_complete = true;
static struct k_thread tx_thread;

static struct uart_cb_data user_data = {
    .buf = NULL,
    .len = 0,
    .sent = 0
};

#ifdef CONFIG_UART_INTERRUPT_DRIVEN
static void uart_cb(const struct device *dev, void *user_data) {
    // Update the interrupt status
    uart_irq_update(dev);

    size_t *tx_sent = &((struct uart_cb_data *)user_data)->sent;
    size_t tx_len = ((struct uart_cb_data *)user_data)->len;
    uint8_t *tx_buf = ((struct uart_cb_data *)user_data)->buf;

    // 2. Check if TX FIFO can accept more data
    if (uart_irq_tx_ready(dev)) {
        if (*tx_sent < tx_len) {
            int sent = uart_fifo_fill(dev, &tx_buf[*tx_sent], tx_len - *tx_sent);
            *tx_sent += sent;
        } else {
            // 3. Disable TX interrupt when all data is sent
            uart_irq_tx_disable(dev);

            k_sem_give(&interrupt_tx_complete_sem);
        }
    }
}

void tx_interrupt(char *buf, size_t len) {
    user_data.buf = buf;
    user_data.len = len;
    user_data.sent = 0;

    uart_irq_callback_user_data_set(uart_dev, uart_cb, (void *)&user_data);
    uart_irq_tx_enable(uart_dev);

    k_sem_take(&interrupt_tx_complete_sem, K_FOREVER);
}
#else
void tx_polling(char *buf, size_t len) {
    for (int idx = 0; idx < len; idx++) {
        uart_poll_out(uart_dev, buf[idx]);
    }
}
#endif

void transfer_buffer(void *arg1, void *arg2, void *arg3) {
    while (1) {
        k_sem_take(&buf_ready_sem, K_FOREVER);

#if CONFIG_UART_INTERRUPT_DRIVEN
        tx_interrupt((char *)buffer, BUF_LEN);
#else
        tx_polling((char *)buffer, BUF_LEN);
#endif

        tx_complete = true;
    }
}

void uart_transfer_init(void) {
    if (!device_is_ready(uart_dev)) abort();

	k_thread_create(&tx_thread,
			tx_thread_stack,
			TX_THREAD_STACK_SIZE,
			transfer_buffer,
			NULL, NULL, NULL,
			1,
			0,
			K_NO_WAIT);
}