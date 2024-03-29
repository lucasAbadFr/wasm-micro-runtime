/*
 * Copyright (c) 2020 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>

#include <autoconf.h>
#include <version.h>

#if KERNEL_VERSION_NUMBER < 0x030200 /* version 3.2.0 */
#include <zephyr.h>
#include <drivers/uart.h>
#include <device.h>
#else /* else of KERNEL_VERSION_NUMBER < 0x030200 */
#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/device.h>

#include <sample_usbd.h>

#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/usbd.h>


#endif /* end of KERNEL_VERSION_NUMBER < 0x030200 */

#include "bh_platform.h"
#include "wasm_export.h"
#include "app_manager_export.h"
#include "runtime_lib.h"
#include "board_config.h"

extern void init_sensor_framework(void);
extern void exit_sensor_framework(void);
extern int aee_host_msg_callback(void *msg, uint16_t msg_len);

static int uart_char_cnt;

const struct device *uart_dev;

uint32_t dtr = 0;

// needed to map uart to usb 
#if defined(CONFIG_USB_DEVICE_STACK_NEXT)
static struct usbd_contex *sample_usbd;

static int enable_usb_device_next(void)
{
	int err;

	sample_usbd = sample_usbd_init_device(NULL);
	if (sample_usbd == NULL) {
		return -ENODEV;
	}

	err = usbd_enable(sample_usbd);
	if (err) {
		return err;
	}

	return 0;
}
#endif /* IS_ENABLED(CONFIG_USB_DEVICE_STACK_NEXT) */

static void uart_irq_callback(const struct device *dev,
							  void *user_data)
{
	unsigned char ch;

	while (uart_poll_in(dev, &ch) == 0) {
		uart_char_cnt++;
		aee_host_msg_callback(&ch, 1);
	}
	(void)user_data;
}

static bool host_init(void)
{
	//uart_dev = device_get_binding(HOST_DEVICE_COMM_UART_NAME);
	uart_dev =  DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
	if (!uart_dev) {
		printf("UART: Device driver not found.\n");
		return false;
	}
#if defined(CONFIG_USB_DEVICE_STACK_NEXT)
	if (enable_usb_device_next()) {
		return 0;
	}
#else
	if (usb_enable(NULL)) {
		return 0;
	}
#endif
	uart_irq_rx_enable(uart_dev);
	uart_irq_callback_set(uart_dev, uart_irq_callback);
	return true;
}

int host_send(void *ctx, const char *buf, int size)
{
	if (!uart_dev)
		return 0;

	for (int i = 0; i < size; i++)
		uart_poll_out(uart_dev, buf[i]);

	return size;
}

void host_destroy(void)
{
}

static host_interface interface = {
	.init = host_init,
	.send = host_send,
	.destroy = host_destroy
};

static char global_heap_buf[220 * 1024] = { 0 };

static int iwasm_main(void)
{
	RuntimeInitArgs init_args;

	host_init();

	memset(&init_args, 0, sizeof(RuntimeInitArgs));
	init_args.mem_alloc_type = Alloc_With_Pool;
	init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
	init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);

	/* initialize runtime environment */
	if (!wasm_runtime_full_init(&init_args)) {
		printf("Init runtime environment failed.\n");
		return -1;
	}

	init_wasm_timer();

	app_manager_startup(&interface);

	wasm_runtime_destroy();
	return -1;
}

void main(void)
{
	iwasm_main();

	while (!dtr) {
		uart_line_ctrl_get(uart_dev, UART_LINE_CTRL_DTR, &dtr);
		/* Give CPU resources to low priority threads. */
		k_sleep(K_MSEC(100));
	}
	for (;;) {
		k_sleep(Z_TIMEOUT_MS(1000));
	}
}
