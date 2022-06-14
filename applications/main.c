/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-06-14     luhuadong    first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <string.h>

/* defined the LED0 pin: PB1 */
#define LED0_PIN    GET_PIN(A, 5)

/* defined for trigger signal */
#define INPUT1_PIN     GET_PIN(A, 13)
#define INPUT2_PIN     GET_PIN(A, 14)
#define OUTPUT1_PIN    GET_PIN(C, 10)
#define OUTPUT2_PIN    GET_PIN(C, 12)

/* defined the UART to MEMS */
#define SERIAL_NAME       "uart1"

static rt_device_t serial;
static struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;

#define TEST_MSG          "Hello"

static void serial_send(int argc, char**argv)
{
    char *data;

    if (argc < 2) {
        data = TEST_MSG;
    } else {
        data = argv[1];
    }

    rt_device_write(serial, 0, data, strlen(data));
}
MSH_CMD_EXPORT(serial_send, send ASCII data to serial);

static void trigger_thread_entry(void *parameter)
{
    int trigger = 0;

    while (1) {
        /* do your job */
        rt_thread_mdelay(1000);
    }
}

int main(void)
{
    /* init GPIO device */
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(INPUT1_PIN, PIN_MODE_INPUT);
    rt_pin_mode(INPUT2_PIN, PIN_MODE_INPUT);
    rt_pin_mode(OUTPUT1_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(OUTPUT2_PIN, PIN_MODE_OUTPUT);

    /* init serial device */
    serial = rt_device_find(SERIAL_NAME);

    config.baud_rate = BAUD_RATE_9600;        //修改波特率为 9600
    config.data_bits = DATA_BITS_8;           //数据位 8
    config.stop_bits = STOP_BITS_1;           //停止位 1
    config.bufsz     = 128;                   //修改缓冲区 buff size 为 128
    config.parity    = PARITY_NONE;           //无奇偶校验位

    rt_device_control(serial, RT_DEVICE_CTRL_CONFIG, &config);
    rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);

    /* init thread for detail trigger signal */
    rt_thread_t tid = RT_NULL;

    tid = rt_thread_create("trigger", trigger_thread_entry, RT_NULL, 1024, 10, 5);
    if (tid != RT_NULL) {
        rt_thread_startup(tid);
    }

    while (1)
    {
        rt_pin_write(LED0_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
        rt_pin_write(LED0_PIN, PIN_LOW);
        rt_thread_mdelay(500);
    }

    return RT_EOK;
}
