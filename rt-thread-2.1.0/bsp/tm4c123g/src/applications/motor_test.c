/*
 * motor_test.c
 *
 *  Created on: 2016Äê7ÔÂ23ÈÕ
 *      Author: dbw
 */

#include <board.h>
#include <rtthread.h>
#include <debug.h>

#include <dev_motor.h>
#include <dev_key.h>
#include "motor_test.h"

void motor_thread_entry(void *param)
{
    int v_target = 100;

    while(1)
    {
        debug_printf("wait to run");
        rt_dev_key_wait(KEY_EVENT_SW1_CLICK);

        debug_printf("wait to stop");

        while(1)
        {
            rt_uint32_t event = rt_dev_key_wait2(KEY_EVENT_SW1_CLICK | KEY_EVENT_SW2_CLICK, RT_WAITING_NO);
            if(event == KEY_EVENT_NONE)
            {
            }
            else if(event == KEY_EVENT_SW1_CLICK)
            {
                break;
            }
            else if(event == KEY_EVENT_SW2_CLICK)
            {
                v_target = -v_target;
            }

            rt_dev_motor_set_speed(v_target, v_target);

//            rt_int32_t loc_left, loc_right;
//            rt_int32_t v_left, v_right;
//            rt_dev_motor_get_location(&loc_left, &loc_right);
//            rt_dev_motor_get_velocity(&v_left, &v_right);
//
//            debug_printf("current location:(%04d, %04d), velocity:(%04d,%04d)",
//                    loc_left, loc_right, v_left, v_right);
        }

        rt_dev_motor_set_speed(0, 0);
    }

}

void motor_test_init(void)
{
    rt_thread_t tid;
    tid = rt_thread_create("motor_ts",
            motor_thread_entry, RT_NULL,
            RT_THREAD_STACK_SIZE_NORMAL, RT_THREAD_PRIORITY_NORMAL, RT_THREAD_TICK_NORMAL);
    if (tid != RT_NULL)
        rt_thread_startup(tid);

}
