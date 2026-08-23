#include "waveshare_24777.h"
#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_conf.h"
#include "stm32f4xx_hal_def.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_spi.h"
#include <stdint.h>
#include <string.h>

extern SPI_HandleTypeDef hspi2;

#define DISPLAY_ON_COMMAND 0xAF
#define DISPLAY_OFF_COMMAND 0xAE
#define SET_COLUMN_ADDR_COMMAND 0x15
#define SET_ROW_ADDR_COMMAND 0x75
#define SET_CONTRAST_COMMAND 0x81
#define SET_REMAP_COMMAND 0xA0
#define SET_START_LINE_COMMAND 0xA1
#define SET_DISPLAY_OFFSET_COMMAND 0xA2
#define SET_NORMAL_DISPLAY_MODE_COMMAND 0xA4
#define SET_PHASE_LENGTH_COMMAND 0xB1
#define SET_ENABLE_INTERNAL_REGULATOR_COMMAND 0xAB
#define SET_SECOND_PRECHARGE_PERIOD_COMMAND 0xB6
#define SET_COM_VOLTAGE_COMMAND 0xBE
#define SET_PRECHARGE_VOLTAGE_COMMAND 0xBC
#define FUNCTION_SELECTION_B_COMMAND 0xD5
#define SET_LOCK_COMMAND 0xFD

void oled_reset();
OledStatus write_data(uint8_t *data, size_t data_size);
OledStatus write_command(uint8_t *data, size_t data_size);
OledStatus reset_cursor();
OledStatus oled_init(uint8_t contrast);

OledStatus oled_enable(uint8_t contrast) {
    OledStatus status;
    oled_reset();
    status = oled_init(contrast);
    if (status == OLED_SPI_ERROR) {
        return status;
    }
    HAL_Delay(200);
    uint8_t cmd = DISPLAY_ON_COMMAND;
    status = write_command(&cmd, 1);
    return status;
}

void oled_draw(uint8_t *data) {
    reset_cursor();

    for (int row = 0; row < DISPLAY_HEIGHT; row++) {
        for (int col = 0; col < DISPLAY_WIDTH; col += 2) {
            uint8_t byte = (data[row * DISPLAY_WIDTH + col] << 4) | data[row * DISPLAY_WIDTH + col + 1];
            write_data(&byte, 1);
        }
    }
}

OledStatus oled_init(uint8_t contrast) {
    OledStatus status;
    uint8_t cmd[3];

    cmd[0] = DISPLAY_OFF_COMMAND;
    status = write_command(cmd, 1);
    if (status == OLED_SPI_ERROR) {
        return status;
    }

    reset_cursor();

    cmd[0] = SET_CONTRAST_COMMAND;
    cmd[1] = contrast;
    write_command(cmd, 2);

    cmd[0] = SET_REMAP_COMMAND;
    cmd[1] = 0x51;
    write_command(cmd, 2);

    cmd[0] = SET_REMAP_COMMAND;
    cmd[1] = 0x51;
    write_command(cmd, 2);

    cmd[0] = SET_START_LINE_COMMAND;
    cmd[1] = 0x0;
    write_command(cmd, 2);

    cmd[0] = SET_DISPLAY_OFFSET_COMMAND;
    cmd[1] = 0x0;
    write_command(cmd, 2);

    cmd[0] = SET_NORMAL_DISPLAY_MODE_COMMAND;
    write_command(cmd, 1);

    cmd[0] = SET_PHASE_LENGTH_COMMAND;
    cmd[1] = 0xf1;
    write_command(cmd, 2);

    cmd[0] = SET_ENABLE_INTERNAL_REGULATOR_COMMAND;
    cmd[1] = 0x01;
    write_command(cmd, 2);

    cmd[0] = SET_SECOND_PRECHARGE_PERIOD_COMMAND;
    cmd[1] = 0x0f;
    write_command(cmd, 2);

    cmd[0] = SET_COM_VOLTAGE_COMMAND;
    cmd[1] = 0x0f;
    write_command(cmd, 2);

    cmd[0] = SET_PRECHARGE_VOLTAGE_COMMAND;
    cmd[1] = 0x08;
    write_command(cmd, 2);

    cmd[0] = SET_PRECHARGE_VOLTAGE_COMMAND;
    cmd[1] = 0x08;
    write_command(cmd, 2);

    cmd[0] = FUNCTION_SELECTION_B_COMMAND;
    cmd[1] = 0x62;
    write_command(cmd, 2);
    
    cmd[0] = SET_LOCK_COMMAND;
    cmd[1] = 0x12;
    write_command(cmd, 2);
    return OLED_OK;
}


void oled_reset() {
    HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_RESET);
    // datasheet says that 100us should be wait, but waveshare code uses 100ms
    // verify if it works 100 micro
    HAL_Delay(100);
    HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_SET);
    HAL_Delay(100);
}

OledStatus reset_cursor() {
    OledStatus status;
    uint8_t cmd[3];

    cmd[0] = SET_COLUMN_ADDR_COMMAND;
    cmd[1] = 0;
    cmd[2] = DISPLAY_WIDTH / 2 - 1;
    status = write_command(cmd, 3);
    if (status == OLED_SPI_ERROR) {
        return status;
    }

    cmd[0] = SET_ROW_ADDR_COMMAND;
    cmd[1] = 0;
    cmd[2] = DISPLAY_HEIGHT - 1;
    status = write_command(cmd, 3);
    if (status == OLED_SPI_ERROR) {
        return status;
    }
    return OLED_OK;
}

OledStatus write_command(uint8_t *data, size_t data_size) {
    HAL_StatusTypeDef status;
    HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_RESET);
    status = HAL_SPI_Transmit(&hspi2, data, data_size, HAL_MAX_DELAY);
    if (status != HAL_OK) {
        return OLED_SPI_ERROR;
    }
    return OLED_OK;
}

OledStatus write_data(uint8_t *data, size_t data_size) {
    HAL_StatusTypeDef status;
    HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);
    status = HAL_SPI_Transmit(&hspi2, data, data_size, HAL_MAX_DELAY);
    if (status != HAL_OK) {
        return OLED_SPI_ERROR;
    }
    return OLED_OK;
}
