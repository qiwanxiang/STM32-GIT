#ifndef __DS18B20_H
#define __DS18B20_H

#include "main.h"

/******************************************************************************************/
/* DS18B20引脚 定义 */

#define DS18B20_DQ_GPIO_PORT                GPIOB
#define DS18B20_DQ_GPIO_PIN                 GPIO_PIN_11
#define DS18B20_DQ_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PG口时钟使能 */

/******************************************************************************************/

/* IO操作函数 */
#define DS18B20_DQ_OUT(x)   do{ x ? \
                                HAL_GPIO_WritePin(DS18B20_DQ_GPIO_PORT, DS18B20_DQ_GPIO_PIN, GPIO_PIN_SET) : \
                                HAL_GPIO_WritePin(DS18B20_DQ_GPIO_PORT, DS18B20_DQ_GPIO_PIN, GPIO_PIN_RESET); \
                            }while(0)                                                       /* 数据端口输出 */
#define DS18B20_DQ_IN       HAL_GPIO_ReadPin(DS18B20_DQ_GPIO_PORT, DS18B20_DQ_GPIO_PIN)     /* 数据端口输入 */


uint8_t ds18b20_init(void);         /* 初始化DS18B20 */
uint8_t ds18b20_check(void);        /* 检测是否存在DS18B20 */
short ds18b20_get_temperature(void);/* 获取温度 */

#endif















