#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"


int main(void)
{
    uint8_t key;

    HAL_Init();                             /* ��ʼ��HAL�� */
    sys_stm32_clock_init(336, 8, 2, 7);     /* ����ʱ��,168Mhz */
    delay_init(168);                        /* ��ʱ��ʼ�� */
    led_init();                             /* ��ʼ��LED */
    key_init();                             /* ��ʼ������ */
	
    LED0(0);                                
	LED1(0); 

    while (1)
    {
        key = key_scan(0);                  /* �õ���ֵ */

        if (key)
        {
            switch (key)
            {
                case WKUP_PRES:
                    LED0_TOGGLE();
                    LED1_TOGGLE();
                    break;

                case KEY0_PRES:             /* ����LED0(RED)��ת */
                    LED0_TOGGLE();          /* LED0״̬ȡ�� */
                    break;

                case KEY1_PRES:             /* ����LED1(GREEN)��ת */
                    LED1_TOGGLE();          /* LED1״̬ȡ�� */
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

