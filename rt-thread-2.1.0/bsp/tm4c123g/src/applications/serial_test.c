/*
 * serial_test.c
 *
 *  Created on: 2016Äê7ÔÂ26ÈÕ
 *      Author: dongb
 */


#include <board.h>
#include <rtthread.h>
#include <debug.h>

#include "serial_test.h"
#include "rtdevice.h"

#define SERIAL_NAME "uart1"

#define SERIAL_EVENT_RX		0x01


rt_device_t serial_dev = RT_NULL;
rt_event_t serial_event = RT_NULL;

rt_err_t serial_rx_indicate(rt_device_t dev, rt_size_t size)
{
	rt_event_send(serial_event, SERIAL_EVENT_RX);
	return RT_EOK;
}

rt_err_t serial_init(void)
{
	rt_err_t ret = -RT_ERROR;

	serial_event = rt_event_create("srl_t", RT_IPC_FLAG_FIFO);
	if(!serial_event)
	{
		debug_printf("Can't create serial_event");
		goto FAIL;
	}

	serial_dev = rt_device_find(SERIAL_NAME);
	if(!serial_dev)
	{
		debug_printf("Can't find %s", SERIAL_NAME);
		ret = -RT_ENOSYS;
		goto RELEASE_EVENT;
	}

	ret = rt_device_open(serial_dev, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
	if(ret != RT_EOK)
	{
		debug_printf("Can't open %s, ret=%d", SERIAL_NAME, ret);
		ret = -RT_EIO;
		goto RELEASE_DEV;
	}

	rt_device_set_rx_indicate(serial_dev, serial_rx_indicate);

	return RT_EOK;

RELEASE_DEV:
	serial_dev = RT_NULL;
RELEASE_EVENT:
	rt_event_delete(serial_event);
FAIL:
	return ret;
}

void serial_thread_entry(void *param)
{
	rt_err_t ret;
	rt_uint32_t event;

	static rt_uint8_t data[64];
	rt_size_t read_size;

	ret = serial_init();
	if(ret != RT_EOK)
	{
		return;
	}

	while(1)
	{

		// try to read data
		read_size = rt_device_read(serial_dev, 0, data, sizeof(data));
		// if read nothing, then wait the rx event
		if(read_size == 0)
		{
			// wait rx event
			rt_event_recv(serial_event, SERIAL_EVENT_RX, RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, &event);
			continue;
		}
		// send them back
		rt_device_write(serial_dev, 0, data, read_size);
	}
}

void serial_test_init(void)
{
	rt_thread_t tid;
	tid = rt_thread_create("srl_t",
			serial_thread_entry, RT_NULL,
			RT_THREAD_STACK_SIZE_NORMAL, RT_THREAD_PRIORITY_NORMAL, RT_THREAD_TICK_NORMAL);
	if (tid != RT_NULL)
		rt_thread_startup(tid);
}

