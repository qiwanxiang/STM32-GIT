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
 * @brief       SPIд����
 *   @note      ������ICд��1 byte����
 * @param       data: Ҫд�������
 * @retval      ��
 */
static void tp_write_byte(uint8_t data)
{
    uint8_t count = 0;

    for (count = 0; count < 8; count++)
    {
        if (data & 0x80)    /* ����1 */
        {
            T_MOSI(1);
        }
        else                /* ����0 */
        {
            T_MOSI(0);
        }

        data <<= 1;
        T_CLK(0);
        delay_us(1);
        T_CLK(1);           /* ��������Ч */
    }
}

/**
 * @brief       SPI������
 *   @note      �Ӵ�����IC��ȡadcֵ
 * @param       cmd: ָ��
 * @retval      ��ȡ��������,ADCֵ(12bit)
 */
static uint16_t tp_read_ad(uint8_t cmd)
{
    uint8_t count = 0;
    uint16_t num = 0;

    T_CLK(0);           /* ������ʱ�� */
    T_MOSI(0);          /* ���������� */
    T_CS(0);            /* ѡ�д�����IC */
    tp_write_byte(cmd); /* ���������� */
    delay_us(6);        /* ADS7846��ת��ʱ���Ϊ6us */
    T_CLK(0);
    delay_us(1);
    T_CLK(1);           /* ��1��ʱ�ӣ����BUSY */
    delay_us(1);
    T_CLK(0);

    for (count = 0; count < 16; count++)    /* ����16λ����,ֻ�и�12λ��Ч */
    {
        num <<= 1;
        T_CLK(0);       /* �½�����Ч */
        delay_us(1);
        T_CLK(1);

        if (T_MISO) num++;
    }

    num >>= 4;          /* ֻ�и�12λ��Ч. */
    T_CS(1);            /* �ͷ�Ƭѡ */
    return num;
}

/* ���败������оƬ ���ݲɼ� �˲��ò��� */
#define TP_READ_TIMES   5       /* ��ȡ���� */
#define TP_LOST_VAL     1       /* ����ֵ */

/**
 * @brief       ��ȡһ������ֵ(x����y)
 *   @note      ������ȡTP_READ_TIMES������,����Щ������������,
 *              Ȼ��ȥ����ͺ����TP_LOST_VAL����, ȡƽ��ֵ
 *              ����ʱ������: TP_READ_TIMES > 2*TP_LOST_VAL ������
 *
 * @param       cmd : ָ��
 *   @arg       0XD0: ��ȡX������(@����״̬,����״̬��Y�Ե�.)
 *   @arg       0X90: ��ȡY������(@����״̬,����״̬��X�Ե�.)
 *
 * @retval      ��ȡ��������(�˲����), ADCֵ(12bit)
 */
static uint16_t tp_read_xoy(uint8_t cmd)
{
    uint16_t i, j;
    uint16_t buf[TP_READ_TIMES];
    uint16_t sum = 0;
    uint16_t temp;

    for (i = 0; i < TP_READ_TIMES; i++)     /* �ȶ�ȡTP_READ_TIMES������ */
    {
        buf[i] = tp_read_ad(cmd);
    }

    for (i = 0; i < TP_READ_TIMES - 1; i++) /* �����ݽ������� */
    {
        for (j = i + 1; j < TP_READ_TIMES; j++)
        {
            if (buf[i] > buf[j])   /* �������� */
            {
                temp = buf[i];
                buf[i] = buf[j];
                buf[j] = temp;
            }
        }
    }

    sum = 0;

    for (i = TP_LOST_VAL; i < TP_READ_TIMES - TP_LOST_VAL; i++)   /* ȥ�����˵Ķ���ֵ */
    {
        sum += buf[i];  /* �ۼ�ȥ������ֵ�Ժ������. */
    }

    temp = sum / (TP_READ_TIMES - 2 * TP_LOST_VAL); /* ȡƽ��ֵ */
    return temp;
}

/**
 * @brief       ��ȡx, y����
 * @param       x,y: ��ȡ��������ֵ
 * @retval      ��
 */
static void tp_read_xy(uint16_t *x, uint16_t *y)
{
    uint16_t xval, yval;

    if (tp_dev.touchtype & 0X01)    /* X,Y��������Ļ�෴ */
    {
        xval = tp_read_xoy(0X90);   /* ��ȡX������ADֵ, �����з���任 */
        yval = tp_read_xoy(0XD0);   /* ��ȡY������ADֵ */
    }
    else                            /* X,Y��������Ļ��ͬ */
    {
        xval = tp_read_xoy(0XD0);   /* ��ȡX������ADֵ */
        yval = tp_read_xoy(0X90);   /* ��ȡY������ADֵ */
    }

    *x = xval;
    *y = yval;
}

/* �������ζ�ȡX,Y�������������������ֵ */
#define TP_ERR_RANGE    50      /* ��Χ */

/**
 * @brief       ������ȡ2�δ���IC����, ���˲�
 *   @note      ����2�ζ�ȡ������IC,�������ε�ƫ��ܳ���ERR_RANGE,����
 *              ����,����Ϊ������ȷ,�����������.�ú����ܴ�����׼ȷ��.
 *
 * @param       x,y: ��ȡ��������ֵ
 * @retval      0, ʧ��; 1, �ɹ�;
 */
static uint8_t tp_read_xy2(uint16_t *x, uint16_t *y)
{
    uint16_t x1, y1;
    uint16_t x2, y2;

    tp_read_xy(&x1, &y1);   /* ��ȡ��һ������ */
    tp_read_xy(&x2, &y2);   /* ��ȡ�ڶ������� */

    /* ǰ�����β�����+-TP_ERR_RANGE�� */
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
/* ��LCD�����йصĺ���, ����У׼�õ� */

/**
 * @brief       ��һ�����(2*2�ĵ�)
 * @param       x,y   : ����
 * @param       color : ��ɫ
 * @retval      ��
 */
void tp_draw_big_point(uint16_t x, uint16_t y, uint16_t color)
{
    lcd_draw_point(x, y, color);       /* ���ĵ� */
    lcd_draw_point(x + 1, y, color);
    lcd_draw_point(x, y + 1, color);
    lcd_draw_point(x + 1, y + 1, color);
}

/******************************************************************************************/

/**
 * @brief       ��������ɨ��
 * @param       mode: ����ģʽ
 *   @arg       0, ��Ļ����;
 *   @arg       1, ��������(У׼�����ⳡ����)
 *
 * @retval      0, �����޴���; 1, �����д���;
 */
static uint8_t tp_scan(uint8_t mode)
{
    if (T_PEN == 0)     /* �а������� */
    {
        if (mode)       /* ��ȡ��������, ����ת�� */
        {
            tp_read_xy2(&tp_dev.x[0], &tp_dev.y[0]);
        }
        else if (tp_read_xy2(&tp_dev.x[0], &tp_dev.y[0]))     /* ��ȡ��Ļ����, ��Ҫת�� */
        {
            /* ��X�� ��������ת�����߼�����(����ӦLCD��Ļ�����X����ֵ) */
            tp_dev.x[0] = (signed short)(tp_dev.x[0] - tp_dev.xc) / tp_dev.xfac + lcddev.width / 2;

            /* ��Y�� ��������ת�����߼�����(����ӦLCD��Ļ�����Y����ֵ) */
            tp_dev.y[0] = (signed short)(tp_dev.y[0] - tp_dev.yc) / tp_dev.yfac + lcddev.height / 2;
        }

        if ((tp_dev.sta & TP_PRES_DOWN) == 0)   /* ֮ǰû�б����� */
        {
            tp_dev.sta = TP_PRES_DOWN | TP_CATH_PRES;   /* �������� */
            tp_dev.x[CT_MAX_TOUCH - 1] = tp_dev.x[0];   /* ��¼��һ�ΰ���ʱ������ */
            tp_dev.y[CT_MAX_TOUCH - 1] = tp_dev.y[0];
        }
    }
    else
    {
        if (tp_dev.sta & TP_PRES_DOWN)      /* ֮ǰ�Ǳ����µ� */
        {
            tp_dev.sta &= ~TP_PRES_DOWN;    /* ��ǰ����ɿ� */
        }
        else     /* ֮ǰ��û�б����� */
        {
            tp_dev.x[CT_MAX_TOUCH - 1] = 0;
            tp_dev.y[CT_MAX_TOUCH - 1] = 0;
            tp_dev.x[0] = 0xFFFF;
            tp_dev.y[0] = 0xFFFF;
        }
    }

    return tp_dev.sta & TP_PRES_DOWN; /* ���ص�ǰ�Ĵ���״̬ */
}

/* TP_SAVE_ADDR_BASE���崥����У׼����������EEPROM�����λ��(��ʼ��ַ)
 * ռ�ÿռ� : 13�ֽ�.
 */
#define TP_SAVE_ADDR_BASE   40


/* ��ʾ�ַ��� */
char *const TP_REMIND_MSG_TBL = "Please use the stylus click the cross on the screen.The cross will always move until the screen adjustment is completed.";



/**
 * @brief       ������У׼����
 *   @note      ʹ�����У׼��(����ԭ����ٶ�)
 *              �������õ�x��/y���������xfac/yfac��������������ֵ(xc,yc)��4������
 *              ���ǹ涨: �������꼴AD�ɼ���������ֵ,��Χ��0~4095.
 *                        �߼����꼴LCD��Ļ������, ��ΧΪLCD��Ļ�ķֱ���.
 *
 * @param       ��
 * @retval      ��
 */
void tp_adjust(void)
{
    if (lcddev.dir==0) //����
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

    lcd_clear(WHITE);   /* ���� */
    lcd_show_string(35, 110, lcddev.width, lcddev.height, 16, "Touch Screen Adjust OK!", BLUE); /* У����� */
    lcd_clear(WHITE);   /* ���� */
    return; /* У����� */
}






/**
 * @brief       ��������ʼ��
 * @param       ��
 * @retval      0,û�н���У׼
 *              1,���й�У׼
 */
uint8_t tp_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;

    tp_dev.touchtype = 0;                   /* Ĭ������(������ & ����) */
    tp_dev.touchtype |= lcddev.dir & 0X01;  /* ����LCD�ж��Ǻ����������� */

    if (lcddev.id == 0x7796)    /* 3.5���������֣�һ����ĻIDΪ0x5510�����败������һ����ĻIDΪ0x7796��GT�ͺŵĵ��ݴ����� */
    {
        if (gt9xxx_init() == 0) /* ��ʼ��GTϵ�д������ɹ�,����ǰ3.5����Ϊ���ݴ����� */
        {
            tp_dev.scan = gt9xxx_scan;  /* ɨ�躯��ָ��GT9147������ɨ�� */
            tp_dev.touchtype |= 0X80;   /* ������ */
            return 0;
        }
    }

    if (lcddev.id == 0X5510 || lcddev.id == 0X9806 || lcddev.id == 0X4342 || lcddev.id == 0X4384 || lcddev.id == 0X1018)  /* ���ݴ�����,4.3��/10.1���� */
    {
        gt9xxx_init();
        tp_dev.scan = gt9xxx_scan;  /* ɨ�躯��ָ��GT9147������ɨ�� */
        tp_dev.touchtype |= 0X80;   /* ������ */
        return 0;
    }
    else if (lcddev.id == 0X1963 || lcddev.id == 0X7084 || lcddev.id == 0X7016)     /* SSD1963 7�������� 7��800*480/1024*600 RGB�� */
    {
        if (!ft5206_init())             /* ����IC��FTϵ�еľ�ִ��ft5206_init�����Լ�ʹ��ft5206_scanɨ�躯�� */
        {
            tp_dev.scan = ft5206_scan;  /* ɨ�躯��ָ��FT5206������ɨ�� */
        }
        else                            /* ����IC��GTϵ�еľ�ִ��gt9xxx_init�����Լ�ʹ��gt9xxx_scanɨ�躯�� */
        {
            gt9xxx_init();
            tp_dev.scan = gt9xxx_scan;  /* ɨ�躯��ָ��GT9147������ɨ�� */
        }

        tp_dev.touchtype |= 0X80;       /* ������ */
        return 0;
    }
    else
    {
        T_PEN_GPIO_CLK_ENABLE();    /* T_PEN��ʱ��ʹ�� */
        T_CS_GPIO_CLK_ENABLE();     /* T_CS��ʱ��ʹ�� */
        T_MISO_GPIO_CLK_ENABLE();   /* T_MISO��ʱ��ʹ�� */
        T_MOSI_GPIO_CLK_ENABLE();   /* T_MOSI��ʱ��ʹ�� */
        T_CLK_GPIO_CLK_ENABLE();    /* T_CLK��ʱ��ʹ�� */

        gpio_init_struct.Pin = T_PEN_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_INPUT;                 /* ���� */
        gpio_init_struct.Pull = GPIO_PULLUP;                     /* ���� */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;      /* ���� */
        HAL_GPIO_Init(T_PEN_GPIO_PORT, &gpio_init_struct);       /* ��ʼ��T_PEN���� */

        gpio_init_struct.Pin = T_MISO_GPIO_PIN;
        HAL_GPIO_Init(T_MISO_GPIO_PORT, &gpio_init_struct);      /* ��ʼ��T_MISO���� */

        gpio_init_struct.Pin = T_MOSI_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;             /* ������� */
        gpio_init_struct.Pull = GPIO_PULLUP;                     /* ���� */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;      /* ���� */
        HAL_GPIO_Init(T_MOSI_GPIO_PORT, &gpio_init_struct);      /* ��ʼ��T_MOSI���� */

        gpio_init_struct.Pin = T_CLK_GPIO_PIN;
        HAL_GPIO_Init(T_CLK_GPIO_PORT, &gpio_init_struct);       /* ��ʼ��T_CLK���� */

        gpio_init_struct.Pin = T_CS_GPIO_PIN;
        HAL_GPIO_Init(T_CS_GPIO_PORT, &gpio_init_struct);        /* ��ʼ��T_CS���� */



        tp_read_xy(&tp_dev.x[0], &tp_dev.y[0]); /* ��һ�ζ�ȡ��ʼ�� */
        lcd_clear(WHITE);   /* ���� */
        tp_adjust();        /* ��ĻУ׼ */
    }

    return 1;
}









