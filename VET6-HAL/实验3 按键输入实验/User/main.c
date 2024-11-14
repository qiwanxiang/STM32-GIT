#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"


int main(void)
{
    uint8_t key;

    HAL_Init();                             /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7);     /* 设置时钟,168Mhz */
    delay_init(168);                        /* 延时初始化 */
    led_init();                             /* 初始化LED */
    key_init();                             /* 初始化按键 */
	
    LED0(0);                                
	LED1(0); 

    while (1)
    {
        key = key_scan(0);                  /* 得到键值 */

        if (key)
        {
            switch (key)
            {
                case WKUP_PRES:
                    LED0_TOGGLE();
                    LED1_TOGGLE();
                    break;

                case KEY0_PRES:             /* 控制LED0(RED)翻转 */
                    LED0_TOGGLE();          /* LED0状态取反 */
                    break;

                case KEY1_PRES:             /* 控制LED1(GREEN)翻转 */
                    LED1_TOGGLE();          /* LED1状态取反 */
                    break;

                default :
                    break;
            }
        }
        else
        {
            delay_ms(10);
        }
    }
}

