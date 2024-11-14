#ifndef __TOUCH_H__
#define __TOUCH_H__

#include "main.h"

/******************************************************************************************/
/* 电阻触摸屏驱动IC T_PEN/T_CS/T_MISO/T_MOSI/T_SCK 引脚 定义 */
#define T_PEN_GPIO_PORT                 GPIOC
#define T_PEN_GPIO_PIN                  GPIO_PIN_5

#define T_CS_GPIO_PORT                  GPIOB
#define T_CS_GPIO_PIN                   GPIO_PIN_12

#define T_MISO_GPIO_PORT                GPIOB
#define T_MISO_GPIO_PIN                 GPIO_PIN_14

#define T_MOSI_GPIO_PORT                GPIOB
#define T_MOSI_GPIO_PIN                 GPIO_PIN_15

#define T_CLK_GPIO_PORT                 GPIOB
#define T_CLK_GPIO_PIN                  GPIO_PIN_13


/******************************************************************************************/

/* 电阻触摸屏控制引脚 */
#define T_PEN           HAL_GPIO_ReadPin(T_PEN_GPIO_PORT, T_PEN_GPIO_PIN)             /* T_PEN */
#define T_MISO          HAL_GPIO_ReadPin(T_MISO_GPIO_PORT, T_MISO_GPIO_PIN)           /* T_MISO */

#define T_MOSI(x)     do{ x ? \
                          HAL_GPIO_WritePin(T_MOSI_GPIO_PORT, T_MOSI_GPIO_PIN, GPIO_PIN_SET) : \
                          HAL_GPIO_WritePin(T_MOSI_GPIO_PORT, T_MOSI_GPIO_PIN, GPIO_PIN_RESET); \
                      }while(0)     /* T_MOSI */

#define T_CLK(x)      do{ x ? \
                          HAL_GPIO_WritePin(T_CLK_GPIO_PORT, T_CLK_GPIO_PIN, GPIO_PIN_SET) : \
                          HAL_GPIO_WritePin(T_CLK_GPIO_PORT, T_CLK_GPIO_PIN, GPIO_PIN_RESET); \
                      }while(0)     /* T_CLK */

#define T_CS(x)       do{ x ? \
                          HAL_GPIO_WritePin(T_CS_GPIO_PORT, T_CS_GPIO_PIN, GPIO_PIN_SET) : \
                          HAL_GPIO_WritePin(T_CS_GPIO_PORT, T_CS_GPIO_PIN, GPIO_PIN_RESET); \
                      }while(0)     /* T_CS */

#define TP_PRES_DOWN    0x8000  /* 触屏被按下 */
#define TP_CATH_PRES    0x4000  /* 有按键按下了 */

/* 触摸屏控制器 */
typedef struct {
	uint16_t x[2];
	uint16_t y[2];
	uint16_t sta; /*
	 * b15:按下1/松开0;
	 * b14:按下1/松开0;
	 */
} _m_tp_dev;
extern _m_tp_dev tp_dev; /* 触屏控制器在touch.c里面定义 */
/* 电阻屏函数 */
void tp_write_byte(uint8_t data); /* 向控制芯片写入一个数据 */
uint16_t tp_read_ad(uint8_t cmd); /* 读取AD转换值 */
uint16_t tp_read_xoy(uint8_t cmd); /* 带滤波的坐标读取(X/Y) */
void tp_read_xy(uint16_t *x, uint16_t *y); /* 双方向读取(X+Y) */
void tp_scan(uint8_t mode); /* 扫描 */

#endif
