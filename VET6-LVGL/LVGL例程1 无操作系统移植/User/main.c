/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-28
 * @brief       LVGL V8无操作系统移植 实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 探索者 F407开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/SRAM/sram.h"
#include "./BSP/TIMER/btim.h"

/* LVGL */
#include "lvgl.h"
#include "lv_port_indev_template.h"
#include "lv_port_disp_template.h"
#include "lv_demo_stress.h"


int main(void)
{
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7); /* 设置时钟,168Mhz */
    delay_init(168);                    /* 延时初始化 */
    usart_init(115200);                 /* 串口初始化为115200 */
    led_init();                         /* 初始化LED */
    key_init();                         /* 初始化按键 */
    sram_init();                        /* SRAM初始化 */
    btim_timx_int_init(10-1,8400-1);    /* 初始化定时器 */

    lv_init();                          /* lvgl系统初始化 */
    lv_port_disp_init();                /* lvgl显示接口初始化,放在lv_init()的后面 */
    lv_port_indev_init();               /* lvgl输入接口初始化,放在lv_init()的后面 */
    
    lv_demo_stress();                   /* 官方例程测试 */

    while (1)
    {
        lv_task_handler();
        delay_ms(5);
    }
}


