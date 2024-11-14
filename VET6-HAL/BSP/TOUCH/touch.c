#include "stdio.h"
#include "stdlib.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/TOUCH/touch.h"
#include "./SYSTEM/delay/delay.h"


_m_tp_dev tp_dev =
{
    tp_init,
    tp_scan,
    tp_adjust,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};

/**
 * @brief       SPI写数据
 *   @note      向触摸屏IC写入1 byte数据
 * @param       data: 要写入的数据
 * @retval      无
 */
static void tp_write_byte(uint8_t data)
{
    uint8_t count = 0;

    for (count = 0; count < 8; count++)
    {
        if (data & 0x80)    /* 发送1 */
        {
            T_MOSI(1);
        }
        else                /* 发送0 */
        {
            T_MOSI(0);
        }

        data <<= 1;
        T_CLK(0);
        delay_us(1);
        T_CLK(1);           /* 上升沿有效 */
    }
}

/**
 * @brief       SPI读数据
 *   @note      从触摸屏IC读取adc值
 * @param       cmd: 指令
 * @retval      读取到的数据,ADC值(12bit)
 */
static uint16_t tp_read_ad(uint8_t cmd)
{
    uint8_t count = 0;
    uint16_t num = 0;

    T_CLK(0);           /* 先拉低时钟 */
    T_MOSI(0);          /* 拉低数据线 */
    T_CS(0);            /* 选中触摸屏IC */
    tp_write_byte(cmd); /* 发送命令字 */
    delay_us(6);        /* ADS7846的转换时间最长为6us */
    T_CLK(0);
    delay_us(1);
    T_CLK(1);           /* 给1个时钟，清除BUSY */
    delay_us(1);
    T_CLK(0);

    for (count = 0; count < 16; count++)    /* 读出16位数据,只有高12位有效 */
    {
        num <<= 1;
        T_CLK(0);       /* 下降沿有效 */
        delay_us(1);
        T_CLK(1);

        if (T_MISO) num++;
    }

    num >>= 4;          /* 只有高12位有效. */
    T_CS(1);            /* 释放片选 */
    return num;
}

/* 电阻触摸驱动芯片 数据采集 滤波用参数 */
#define TP_READ_TIMES   5       /* 读取次数 */
#define TP_LOST_VAL     1       /* 丢弃值 */

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
static uint16_t tp_read_xoy(uint8_t cmd)
{
    uint16_t i, j;
    uint16_t buf[TP_READ_TIMES];
    uint16_t sum = 0;
    uint16_t temp;

    for (i = 0; i < TP_READ_TIMES; i++)     /* 先读取TP_READ_TIMES次数据 */
    {
        buf[i] = tp_read_ad(cmd);
    }

    for (i = 0; i < TP_READ_TIMES - 1; i++) /* 对数据进行排序 */
    {
        for (j = i + 1; j < TP_READ_TIMES; j++)
        {
            if (buf[i] > buf[j])   /* 升序排列 */
            {
                temp = buf[i];
                buf[i] = buf[j];
                buf[j] = temp;
            }
        }
    }

    sum = 0;

    for (i = TP_LOST_VAL; i < TP_READ_TIMES - TP_LOST_VAL; i++)   /* 去掉两端的丢弃值 */
    {
        sum += buf[i];  /* 累加去掉丢弃值以后的数据. */
    }

    temp = sum / (TP_READ_TIMES - 2 * TP_LOST_VAL); /* 取平均值 */
    return temp;
}

/**
 * @brief       读取x, y坐标
 * @param       x,y: 读取到的坐标值
 * @retval      无
 */
static void tp_read_xy(uint16_t *x, uint16_t *y)
{
    uint16_t xval, yval;

    if (tp_dev.touchtype & 0X01)    /* X,Y方向与屏幕相反 */
    {
        xval = tp_read_xoy(0X90);   /* 读取X轴坐标AD值, 并进行方向变换 */
        yval = tp_read_xoy(0XD0);   /* 读取Y轴坐标AD值 */
    }
    else                            /* X,Y方向与屏幕相同 */
    {
        xval = tp_read_xoy(0XD0);   /* 读取X轴坐标AD值 */
        yval = tp_read_xoy(0X90);   /* 读取Y轴坐标AD值 */
    }

    *x = xval;
    *y = yval;
}

/* 连续两次读取X,Y坐标的数据误差最大允许值 */
#define TP_ERR_RANGE    50      /* 误差范围 */

/**
 * @brief       连续读取2次触摸IC数据, 并滤波
 *   @note      连续2次读取触摸屏IC,且这两次的偏差不能超过ERR_RANGE,满足
 *              条件,则认为读数正确,否则读数错误.该函数能大大提高准确度.
 *
 * @param       x,y: 读取到的坐标值
 * @retval      0, 失败; 1, 成功;
 */
static uint8_t tp_read_xy2(uint16_t *x, uint16_t *y)
{
    uint16_t x1, y1;
    uint16_t x2, y2;

    tp_read_xy(&x1, &y1);   /* 读取第一次数据 */
    tp_read_xy(&x2, &y2);   /* 读取第二次数据 */

    /* 前后两次采样在+-TP_ERR_RANGE内 */
    if (((x2 <= x1 && x1 < x2 + TP_ERR_RANGE) || (x1 <= x2 && x2 < x1 + TP_ERR_RANGE)) &&
            ((y2 <= y1 && y1 < y2 + TP_ERR_RANGE) || (y1 <= y2 && y2 < y1 + TP_ERR_RANGE)))
    {
        *x = (x1 + x2) / 2;
        *y = (y1 + y2) / 2;
        return 1;
    }

    return 0;
}

/******************************************************************************************/
/* 与LCD部分有关的函数, 用来校准用的 */

/**
 * @brief       画一个大点(2*2的点)
 * @param       x,y   : 坐标
 * @param       color : 颜色
 * @retval      无
 */
void tp_draw_big_point(uint16_t x, uint16_t y, uint16_t color)
{
    lcd_draw_point(x, y, color);       /* 中心点 */
    lcd_draw_point(x + 1, y, color);
    lcd_draw_point(x, y + 1, color);
    lcd_draw_point(x + 1, y + 1, color);
}

/******************************************************************************************/

/**
 * @brief       触摸按键扫描
 * @param       mode: 坐标模式
 *   @arg       0, 屏幕坐标;
 *   @arg       1, 物理坐标(校准等特殊场合用)
 *
 * @retval      0, 触屏无触摸; 1, 触屏有触摸;
 */
static uint8_t tp_scan(uint8_t mode)
{
    if (T_PEN == 0)     /* 有按键按下 */
    {
        if (mode)       /* 读取物理坐标, 无需转换 */
        {
            tp_read_xy2(&tp_dev.x[0], &tp_dev.y[0]);
        }
        else if (tp_read_xy2(&tp_dev.x[0], &tp_dev.y[0]))     /* 读取屏幕坐标, 需要转换 */
        {
            /* 将X轴 物理坐标转换成逻辑坐标(即对应LCD屏幕上面的X坐标值) */
            tp_dev.x[0] = (signed short)(tp_dev.x[0] - tp_dev.xc) / tp_dev.xfac + lcddev.width / 2;

            /* 将Y轴 物理坐标转换成逻辑坐标(即对应LCD屏幕上面的Y坐标值) */
            tp_dev.y[0] = (signed short)(tp_dev.y[0] - tp_dev.yc) / tp_dev.yfac + lcddev.height / 2;
        }

        if ((tp_dev.sta & TP_PRES_DOWN) == 0)   /* 之前没有被按下 */
        {
            tp_dev.sta = TP_PRES_DOWN | TP_CATH_PRES;   /* 按键按下 */
            tp_dev.x[CT_MAX_TOUCH - 1] = tp_dev.x[0];   /* 记录第一次按下时的坐标 */
            tp_dev.y[CT_MAX_TOUCH - 1] = tp_dev.y[0];
        }
    }
    else
    {
        if (tp_dev.sta & TP_PRES_DOWN)      /* 之前是被按下的 */
        {
            tp_dev.sta &= ~TP_PRES_DOWN;    /* 标记按键松开 */
        }
        else     /* 之前就没有被按下 */
        {
            tp_dev.x[CT_MAX_TOUCH - 1] = 0;
            tp_dev.y[CT_MAX_TOUCH - 1] = 0;
            tp_dev.x[0] = 0xFFFF;
            tp_dev.y[0] = 0xFFFF;
        }
    }

    return tp_dev.sta & TP_PRES_DOWN; /* 返回当前的触屏状态 */
}

/* TP_SAVE_ADDR_BASE定义触摸屏校准参数保存在EEPROM里面的位置(起始地址)
 * 占用空间 : 13字节.
 */
#define TP_SAVE_ADDR_BASE   40


/* 提示字符串 */
char *const TP_REMIND_MSG_TBL = "Please use the stylus click the cross on the screen.The cross will always move until the screen adjustment is completed.";



/**
 * @brief       触摸屏校准代码
 *   @note      使用五点校准法(具体原理请百度)
 *              本函数得到x轴/y轴比例因子xfac/yfac及物理中心坐标值(xc,yc)等4个参数
 *              我们规定: 物理坐标即AD采集到的坐标值,范围是0~4095.
 *                        逻辑坐标即LCD屏幕的坐标, 范围为LCD屏幕的分辨率.
 *
 * @param       无
 * @retval      无
 */
void tp_adjust(void)
{
    if (lcddev.dir==0) //竖屏
    {
        tp_dev.xfac =-14.98;
        tp_dev.yfac =-10.85;
        tp_dev.xc =0x0828;
        tp_dev.yc =0x0875;
        tp_dev.x[0]=230;
        tp_dev.y[0]=310;
    }
    else
    {
        tp_dev.xfac =10.98;
        tp_dev.yfac =-14.74;
        tp_dev.xc =0x0851;
        tp_dev.yc =0x0815;
        tp_dev.x[0]=310;
        tp_dev.y[0]=230;
    }

    lcd_clear(WHITE);   /* 清屏 */
    lcd_show_string(35, 110, lcddev.width, lcddev.height, 16, "Touch Screen Adjust OK!", BLUE); /* 校正完成 */
    lcd_clear(WHITE);   /* 清屏 */
    return; /* 校正完成 */
}






/**
 * @brief       触摸屏初始化
 * @param       无
 * @retval      0,没有进行校准
 *              1,进行过校准
 */
uint8_t tp_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;

    tp_dev.touchtype = 0;                   /* 默认设置(电阻屏 & 竖屏) */
    tp_dev.touchtype |= lcddev.dir & 0X01;  /* 根据LCD判定是横屏还是竖屏 */

    if (lcddev.id == 0x7796)    /* 3.5寸屏有两种，一种屏幕ID为0x5510带电阻触摸屏，一种屏幕ID为0x7796带GT型号的电容触摸屏 */
    {
        if (gt9xxx_init() == 0) /* 初始化GT系列触摸屏成功,即当前3.5寸屏为电容触摸屏 */
        {
            tp_dev.scan = gt9xxx_scan;  /* 扫描函数指向GT9147触摸屏扫描 */
            tp_dev.touchtype |= 0X80;   /* 电容屏 */
            return 0;
        }
    }

    if (lcddev.id == 0X5510 || lcddev.id == 0X9806 || lcddev.id == 0X4342 || lcddev.id == 0X4384 || lcddev.id == 0X1018)  /* 电容触摸屏,4.3寸/10.1寸屏 */
    {
        gt9xxx_init();
        tp_dev.scan = gt9xxx_scan;  /* 扫描函数指向GT9147触摸屏扫描 */
        tp_dev.touchtype |= 0X80;   /* 电容屏 */
        return 0;
    }
    else if (lcddev.id == 0X1963 || lcddev.id == 0X7084 || lcddev.id == 0X7016)     /* SSD1963 7寸屏或者 7寸800*480/1024*600 RGB屏 */
    {
        if (!ft5206_init())             /* 触摸IC是FT系列的就执行ft5206_init函数以及使用ft5206_scan扫描函数 */
        {
            tp_dev.scan = ft5206_scan;  /* 扫描函数指向FT5206触摸屏扫描 */
        }
        else                            /* 触摸IC是GT系列的就执行gt9xxx_init函数以及使用gt9xxx_scan扫描函数 */
        {
            gt9xxx_init();
            tp_dev.scan = gt9xxx_scan;  /* 扫描函数指向GT9147触摸屏扫描 */
        }

        tp_dev.touchtype |= 0X80;       /* 电容屏 */
        return 0;
    }
    else
    {
        T_PEN_GPIO_CLK_ENABLE();    /* T_PEN脚时钟使能 */
        T_CS_GPIO_CLK_ENABLE();     /* T_CS脚时钟使能 */
        T_MISO_GPIO_CLK_ENABLE();   /* T_MISO脚时钟使能 */
        T_MOSI_GPIO_CLK_ENABLE();   /* T_MOSI脚时钟使能 */
        T_CLK_GPIO_CLK_ENABLE();    /* T_CLK脚时钟使能 */

        gpio_init_struct.Pin = T_PEN_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_INPUT;                 /* 输入 */
        gpio_init_struct.Pull = GPIO_PULLUP;                     /* 上拉 */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;      /* 高速 */
        HAL_GPIO_Init(T_PEN_GPIO_PORT, &gpio_init_struct);       /* 初始化T_PEN引脚 */

        gpio_init_struct.Pin = T_MISO_GPIO_PIN;
        HAL_GPIO_Init(T_MISO_GPIO_PORT, &gpio_init_struct);      /* 初始化T_MISO引脚 */

        gpio_init_struct.Pin = T_MOSI_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;             /* 推挽输出 */
        gpio_init_struct.Pull = GPIO_PULLUP;                     /* 上拉 */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;      /* 高速 */
        HAL_GPIO_Init(T_MOSI_GPIO_PORT, &gpio_init_struct);      /* 初始化T_MOSI引脚 */

        gpio_init_struct.Pin = T_CLK_GPIO_PIN;
        HAL_GPIO_Init(T_CLK_GPIO_PORT, &gpio_init_struct);       /* 初始化T_CLK引脚 */

        gpio_init_struct.Pin = T_CS_GPIO_PIN;
        HAL_GPIO_Init(T_CS_GPIO_PORT, &gpio_init_struct);        /* 初始化T_CS引脚 */



        tp_read_xy(&tp_dev.x[0], &tp_dev.y[0]); /* 第一次读取初始化 */
        lcd_clear(WHITE);   /* 清屏 */
        tp_adjust();        /* 屏幕校准 */
    }

    return 1;
}









