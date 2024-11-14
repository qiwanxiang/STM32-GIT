#ifndef __DS18B20_H
#define __DS18B20_H

#include "main.h"

/******************************************************************************************/
/* DS18B20���� ���� */

#define DS18B20_DQ_GPIO_PORT                GPIOB
#define DS18B20_DQ_GPIO_PIN                 GPIO_PIN_11
#define DS18B20_DQ_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PG��ʱ��ʹ�� */

/******************************************************************************************/

/* IO�������� */
#define DS18B20_DQ_OUT(x)   do{ x ? \
                                HAL_GPIO_WritePin(DS18B20_DQ_GPIO_PORT, DS18B20_DQ_GPIO_PIN, GPIO_PIN_SET) : \
                                HAL_GPIO_WritePin(DS18B20_DQ_GPIO_PORT, DS18B20_DQ_GPIO_PIN, GPIO_PIN_RESET); \
                            }while(0)                                                       /* ���ݶ˿���� */
#define DS18B20_DQ_IN       HAL_GPIO_ReadPin(DS18B20_DQ_GPIO_PORT, DS18B20_DQ_GPIO_PIN)     /* ���ݶ˿����� */


uint8_t ds18b20_init(void);         /* ��ʼ��DS18B20 */
uint8_t ds18b20_check(void);        /* ����Ƿ����DS18B20 */
short ds18b20_get_temperature(void);/* ��ȡ�¶� */

#endif















