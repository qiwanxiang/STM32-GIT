#include <sfud.h>
#include <stdarg.h>

#include "main.h"
#include "spi.h"
#include "sfud_cfg.h"
//#include "FreeRTOS.h"
//#include "task.h"
//#include "semphr.h"

static char log_buf[256];
void sfud_log_debug(const char *file, const long line, const char *format, ...);

static void spi_lock(const sfud_spi *spi) {
    /* 裸机中使用开关中断方法 */
    __disable_irq();
    /* OS 中使用临界区或互斥量，推荐互斥量方式 */
    //  taskENTER_CRITICAL();  /* 进入临界区 */
    //  xSemaphoreTake(xMutexSPIFlash, portMAX_DELAY);  /* 互斥信号量 */
}

static void spi_unlock(const sfud_spi *spi) {
    /* 裸机中使用开关中断方法 */
    __enable_irq();
    /* OS 中使用临界区或互斥量，推荐互斥量方式 */
    //  taskEXIT_CRITICAL();  /* 退出临界区 */
    //  xSemaphoreGive(xMutexSPIFlash);  /* 互斥信号量 */
}

/**
 * SPI write data then read data
 */
static sfud_err spi_write_read(const sfud_spi *spi, const uint8_t *write_buf, size_t write_size, uint8_t *read_buf,
                               size_t read_size) {
    sfud_err result = SFUD_SUCCESS;
    //    uint8_t send_data, read_data;
    /**
     * add your spi write and read code
     */
    if (write_size) {
        SFUD_ASSERT(write_buf);
    }
    if (read_size) {
        SFUD_ASSERT(read_buf);
    }

    //  HAL_GPIO_WritePin(W25Q128_CS_GPIO_Port, W25Q128_CS_Pin, GPIO_PIN_RESET);
    //  if( HAL_OK != HAL_SPI_TransmitReceive(&hspi2, (uint8_t *)write_buf, (uint8_t *)read_buf, write_size+read_size, 1000) )
    //      result = SFUD_ERR_TIMEOUT;
    //  HAL_GPIO_WritePin(W25Q128_CS_GPIO_Port, W25Q128_CS_Pin, GPIO_PIN_SET);

    if (read_size != NULL) {
        HAL_GPIO_WritePin(W25Q128_CS_GPIO_Port, W25Q128_CS_Pin, GPIO_PIN_RESET);
        if (HAL_OK != HAL_SPI_Transmit(&hspi2, (uint8_t *)write_buf, write_size, 1000))
            result = SFUD_ERR_TIMEOUT;

        //      HAL_GPIO_WritePin(W25Q128_CS_GPIO_Port, W25Q128_CS_Pin, GPIO_PIN_SET);
        //      HAL_GPIO_WritePin(W25Q128_CS_GPIO_Port, W25Q128_CS_Pin, GPIO_PIN_RESET);
        if (HAL_OK != HAL_SPI_Receive(&hspi2, (uint8_t *)read_buf, read_size, 1000))
            result = SFUD_ERR_TIMEOUT;
        HAL_GPIO_WritePin(W25Q128_CS_GPIO_Port, W25Q128_CS_Pin, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(W25Q128_CS_GPIO_Port, W25Q128_CS_Pin, GPIO_PIN_RESET);
        if (HAL_OK != HAL_SPI_Transmit(&hspi2, (uint8_t *)write_buf, write_size, 1000))
            result = SFUD_ERR_TIMEOUT;
        HAL_GPIO_WritePin(W25Q128_CS_GPIO_Port, W25Q128_CS_Pin, GPIO_PIN_SET);
    }

    return result;
}

#ifdef SFUD_USING_QSPI
/**
 * read flash data by QSPI
 */
static sfud_err qspi_read(const struct __sfud_spi *spi, uint32_t addr, sfud_qspi_read_cmd_format *qspi_read_cmd_format,
                          uint8_t *read_buf, size_t read_size) {
    sfud_err result = SFUD_SUCCESS;
    /**
     * add your qspi read flash data code
     */
    return result;
}

#endif /* SFUD_USING_QSPI */

/* about 100 microsecond delay */
static void retry_delay_100us(void) {
    uint32_t delay = 1700*2;
    while (delay--);
}

sfud_err sfud_spi_port_init(sfud_flash *flash) {
    sfud_err result = SFUD_SUCCESS;
    /**
     * add your port spi bus and device object initialize code like this:
     * 1. rcc initialize
     * 2. gpio initialize
     * 3. spi device initialize
     * 4. flash->spi and flash->retry item initialize
     *    flash->spi.wr = spi_write_read; //Required
     *    flash->spi.qspi_read = qspi_read; //Required when QSPI mode enable
     *    flash->spi.lock = spi_lock;
     *    flash->spi.unlock = spi_unlock;
     *    flash->spi.user_data = &spix;
     *    flash->retry.delay = null;
     *    flash->retry.times = 10000; //Required
     */
    switch (flash->index) {
    case SFUD_W25_DEVICE_INDEX: {
        /* 同步 Flash 移植所需的接口及数据 */
        flash->spi.wr = spi_write_read;
        flash->spi.lock = spi_lock;
        flash->spi.unlock = spi_unlock;
        /* about 100 microsecond delay */
        flash->retry.delay = retry_delay_100us;
        /* adout 60 seconds timeout */
        flash->retry.times = 60 * 10000;
        break;
    }
    }
    return result;
}

/**
 * This function is print debug info.
 *
 * @param file the file which has call this function
 * @param line the line number which has call this function
 * @param format output format
 * @param ... args
 */
void sfud_log_debug(const char *file, const long line, const char *format, ...) {
    va_list args;
    /* args point to the first variable parameter */
    va_start(args, format);
    printf("[SFUD](%s:%ld) ", file, line);
    /* must use vprintf to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    printf("%s\n", log_buf);
    va_end(args);
}

/**
 * This function is print routine info.
 *
 * @param format output format
 * @param ... args
 */
void sfud_log_info(const char *format, ...) {
    va_list args;
    /* args point to the first variable parameter */
    va_start(args, format);
    printf("[SFUD]");
    /* must use vprintf to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    printf("%s\n", log_buf);
    va_end(args);
}
