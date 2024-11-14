/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2021-10-14
 * @brief       template 实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台：正点原子 探索者 F407开发板
 * 在线视频：www.yuanzige.com
 * 技术论坛：http://www.openedv.com/forum.php
 * 公司网址：www.alientek.com
 * 购买地址：zhengdianyuanzi.tmall.com
 *
 ****************************************************************************************************
 */

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"


void led_init(void);                    /* LED初始化函数声明 */

int main(void)
{
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7); /* 设置时钟,168Mhz */
    delay_init(168);                    /* 延时初始化 */
    led_init();                         /* 初始化LED */
    
    while(1)
    {
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET);     /* LED0 亮 */
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_SET);      /* LED1 灭 */
        delay_ms(1000);
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET);       /* LED0 灭 */
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_RESET);    /* LED1 亮 */
        delay_ms(1000);
    }
}

/**
 * @brief       初始化LED相关IO口, 并使能时钟
 * @param       无
 * @retval      无
 */
void led_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    __HAL_RCC_GPIOA_CLK_ENABLE();                       /* LED0，LED1时钟使能 */

    gpio_init_struct.Pin = GPIO_PIN_6;                  /* LED0引脚 */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;        /* 推挽输出 */
    gpio_init_struct.Pull = GPIO_PULLUP;                /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;      /* 高速 */
    HAL_GPIO_Init(GPIOA, &gpio_init_struct);            /* 初始化LED0引脚 */

    gpio_init_struct.Pin = GPIO_PIN_7;                 /* LED1引脚 */
    HAL_GPIO_Init(GPIOA, &gpio_init_struct);            /* 初始化LED1引脚 */

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET); /* LED0 灭 */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);/* LED1 灭 */
}
