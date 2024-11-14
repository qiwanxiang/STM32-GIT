#include "touch.h"

_m_tp_dev tp_dev = { { 0 }, { 0 }, 0 };
/* 电阻触摸驱动芯片 数据采集 滤波用参数 */
#define TP_READ_TIMES   5       /* 读取次数 */
#define TP_LOST_VAL     1       /* 丢弃值 */

//微秒级延时函数
#define CPU_FREQUENCY_MHZ    168		// STM32时钟主频
void delay_us(__IO uint32_t delay) {
	int last, curr, val;
	int temp;

	while (delay != 0) {
		temp = delay > 900 ? 900 : delay;
		last = SysTick->VAL;
		curr = last - CPU_FREQUENCY_MHZ * temp;
		if (curr >= 0) {
			do {
				val = SysTick->VAL;
			} while ((val < last) && (val >= curr));
		} else {
			curr += CPU_FREQUENCY_MHZ * 1000;
			do {
				val = SysTick->VAL;
			} while ((val <= last) || (val > curr));
		}
		delay -= temp;
	}
}
/******************************************************************************************/
/**
 * @brief       SPI写数据
 *   @note      向触摸屏IC写入1 byte数据
 * @param       data: 要写入的数据
 * @retval      无
 */
void tp_write_byte(uint8_t data) {
	uint8_t count = 0;
	for (count = 0; count < 8; count++) {
		if (data & 0x80) /* 发送1 */
		{
			T_MOSI(1);
		} else /* 发送0 */
		{
			T_MOSI(0);
		}
		data <<= 1;
		T_CLK(0);
		delay_us(1);
		T_CLK(1); /* 上升沿有效 */
	}
}
/******************************************************************************************/
/**
 * @brief       SPI读数据
 *   @note      从触摸屏IC读取adc值
 * @param       cmd: 指令
 * @retval      读取到的数据,ADC值(12bit)
 */
uint16_t tp_read_ad(uint8_t cmd) {
	uint8_t count = 0;
	uint16_t num = 0;

	T_CLK(0); /* 先拉低时钟 */
	T_MOSI(0); /* 拉低数据线 */
	T_CS(0); /* 选中触摸屏IC */
	tp_write_byte(cmd); /* 发送命令字 */
	delay_us(6); /* ADS7846的转换时间最长为6us */
	T_CLK(0);
	delay_us(1);
	T_CLK(1); /* 给1个时钟，清除BUSY */
	delay_us(1);
	T_CLK(0);

	for (count = 0; count < 16; count++) /* 读出16位数据,只有高12位有效 */
	{
		num <<= 1;
		T_CLK(0); /* 下降沿有效 */
		delay_us(1);
		T_CLK(1);

		if (T_MISO)
			num++;
	}

	num >>= 4; /* 只有高12位有效. */
	T_CS(1); /* 释放片选 */
	return num;
}
/******************************************************************************************/

/**
 * @brief       读取一个坐标值(x或者y)
 *   @note      连续读取TP_READ_TIMES次数据,对这些数据升序排列,
 *              然后去掉最低和最高TP_LOST_VAL个数, 取平均值
 *              设置时需满足: TP_READ_TIMES > 2*TP_LOST_VAL 的条件
 *
 * @param       cmd : 指令
 *   @arg       0XD0: 读取X轴坐标(@竖屏状态,横屏状态和Y对调.)
 *   @arg       0X90: 读取Y轴坐标(@竖屏状态,横屏状态和X对调.)
 *
 * @retval      读取到的数据(滤波后的), ADC值(12bit)
 */
uint16_t tp_read_xoy(uint8_t cmd) {
	uint16_t i, j;
	uint16_t buf[TP_READ_TIMES];
	uint16_t sum = 0;
	uint16_t temp;

	for (i = 0; i < TP_READ_TIMES; i++) /* 先读取TP_READ_TIMES次数据 */
	{
		buf[i] = tp_read_ad(cmd);
	}

	for (i = 0; i < TP_READ_TIMES - 1; i++) /* 对数据进行排序 */
	{
		for (j = i + 1; j < TP_READ_TIMES; j++) {
			if (buf[i] > buf[j]) /* 升序排列 */
			{
				temp = buf[i];
				buf[i] = buf[j];
				buf[j] = temp;
			}
		}
	}

	sum = 0;

	for (i = TP_LOST_VAL; i < TP_READ_TIMES - TP_LOST_VAL; i++) /* 去掉两端的丢弃值 */
	{
		sum += buf[i]; /* 累加去掉丢弃值以后的数据. */
	}

	temp = sum / (TP_READ_TIMES - 2 * TP_LOST_VAL); /* 取平均值 */
	return temp;
}
/******************************************************************************************/
void tp_read_xy(uint16_t *x, uint16_t *y) {
	uint16_t xval, yval;
	xval = tp_read_xoy(0X90); /* 读取X轴坐标AD值, 并进行方向变换 */
	yval = tp_read_xoy(0XD0); /* 读取Y轴坐标AD值 */
	*x = xval;
	*y = yval;
}
/******************************************************************************************/
void tp_scan(uint8_t mode) {
	if (T_PEN == 0) /* 有按键按下 */
	{
		if (mode == 1) /* 读取物理坐标, 无需转换 */
		{
			tp_read_xy(&tp_dev.x[0], &tp_dev.y[0]);
		} else /* mode=0 读取屏幕坐标, 需要转换 */
		{
			tp_read_xy(&tp_dev.x[0], &tp_dev.y[0]);
			tp_dev.x[1] = 320 - (tp_dev.x[0] / (4095 / 320)) + 5; //5：补偿值
			tp_dev.y[1] = (tp_dev.y[0] / (4095 / 240));
		}
		static int i = 0;
		lcd_show_num(0, 144, i++, 5, 24, RED);

		if ((tp_dev.sta & TP_PRES_DOWN) == 0) /* 之前没有被按下 */
		{
			tp_dev.sta = TP_PRES_DOWN; /* 按键按下 */
		}
	} else {
		if (tp_dev.sta & TP_PRES_DOWN) /* 之前是被按下的 */
		{
			tp_dev.sta &= ~TP_PRES_DOWN; /* 标记按键松开 */
		} else /* 之前就没有被按下 */
		{
			tp_dev.x[0] = 0x00;
			tp_dev.y[0] = 0x00;
			tp_dev.x[1] = 0x00;
			tp_dev.y[1] = 0x00;
		}
	}
	HAL_Delay(100);
}
