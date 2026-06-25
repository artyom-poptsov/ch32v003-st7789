/* st7789.c -- ST7789 driver implementation for CH32V003.
 *
 * Copyright (C) 2026 Artyom V. Poptsov <poptsov.artyom@gmail.com>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.    See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdint.h>

#include "st7789.h"
#include "ch32fun.h"

// ----------------------------------------------------------------------------
// SPI pins control
//

 /**
  * Set the "RESET" pin to HIGH.
  */
void st7789_spi_reset_high(st7789_t* this) {
        GPIOC->BSHR |= (1 << this->spi.pin_reset);
}

/**
 * Set the "RESET" pin to LOW.
 */
void st7789_spi_reset_low(st7789_t* this) {
        GPIOC->BCR |= (1 << this->spi.pin_reset);
}

/**
 * Switch to the "command" mode.
 */
void st7789_spi_command_mode(st7789_t* this) {
        GPIOC->BCR |= (1 << this->spi.pin_dc);
}

/**
 * Switch to the "data" mode.
 */
void st7789_spi_data_mode(st7789_t* this) {
        GPIOC->BSHR |= (1 << this->spi.pin_dc);
}

void st7789_spi_start_write(st7789_t* this) {
        GPIOC->BCR |= (1 << this->spi.pin_cs);
}

void st7789_spi_end_write(st7789_t* this) {
        GPIOC->BSHR |= (1 << this->spi.pin_cs);
}

// ----------------------------------------------------------------------------
// Low-level SPI commands.
//

 /**
  *  Send 8-bit of data over the SPI bus.
  *
  *  @param data Data to send.
  */
static void spi_send(uint8_t data) {
        SPI1->DATAR = data;
        while (!(SPI1->STATR & SPI_STATR_TXE))
                ;
}

/**
 * Send data using SPI direct memory access (DMA.)
 *
 * @param this An ST7789 display instance.
 * @param size Data size.
 * @param repeat Controls how many times data must be send.
 */
void st7789_spi_send_dma(st7789_t* this, uint32_t size, uint16_t repeat) {
        st7789_spi_data_mode(this);
        DMA1_Channel3->MADDR = (uint32_t)this->buffer;
        DMA1_Channel3->CNTR = size;
        DMA1_Channel3->CFGR |= DMA_CFGR1_EN; // Turn on DMA channel

        while (repeat--) {
                DMA1->INTFCR = DMA1_FLAG_TC3;
                while (!(DMA1->INTFR & DMA1_FLAG_TC3))
                        ;
        }

        DMA1_Channel3->CFGR &= ~DMA_CFGR1_EN; // Turn off DMA channel
}

/**
 * Send an 8-bit unsigned data.
 *
 * @param this An ST7789 instance.
 * @param data The data to send.
 */
void st7789_send_data_8(st7789_t* this, uint8_t data) {
        st7789_spi_data_mode(this);
        spi_send(data);
}

/**
 * Send an 16-bit unsigned data.
 *
 * @param this An ST7789 instance.
 * @param data The data to send.
 */
void st7789_send_data_16(st7789_t* this, uint16_t data) {
        st7789_spi_data_mode(this);
        spi_send(data >> 8);
        spi_send(data & 0xFF);
}

/**
 * Send an 32-bit unsigned data.
 *
 * @param this An ST7789 instance.
 * @param data The data to send.
 */
void st7789_send_data_32(st7789_t* this, uint32_t data) {
        st7789_spi_data_mode(this);
        spi_send(data >> 24);
        spi_send((data >> 16) & 0xFF);
        spi_send((data >> 8) & 0xFF);
        spi_send(data & 0xFF);
}

/**
 * Send an 8-bit command through SPI.
 * 
 * @param command A command to send.
 */
void st7789_send_command_8(st7789_t* this, st7789_command_t command) {
        st7789_spi_command_mode(this);
        spi_send(command);
}

// ----------------------------------------------------------------------------
// ST7789 commands.
//

 /**
  * Turn off sleep mode.
  * 
  * See <https://www.buydisplay.com/download/ic/ST7789.pdf>
  * 9.1.12 SLPOUT (11h): Sleep Out
  */
void st7789_command_slpout(st7789_t* this) {
        const uint8_t SLPOUT_DELAY = 120;
        st7789_send_command_8(this, ST7789_COMMAND_SLPOUT);
        Delay_Ms(SLPOUT_DELAY);
}

void st7789_command_slpin(st7789_t* this) {
        st7789_send_command_8(this, ST7789_COMMAND_SLPIN);
}

 /**
  * define the format of RGB picture data, which is to be 
  * transferred via the MCU interface.
  * 
  * See <https://www.buydisplay.com/download/ic/ST7789.pdf>
  * 9.1.12 SLPOUT (11h): Sleep Out
  */
void st7789_command_colmod(st7789_t* this, uint8_t mode) {
        st7789_send_command_8(this, ST7789_COMMAND_COLMOD);
        st7789_send_data_8(this, mode);
}

/**
 * Memory Data Access Control.
 * 
 * See <https://www.buydisplay.com/download/ic/ST7789.pdf>
 * 9.1.28 MADCTL (36h): Memory Data Access Control
 * 
 * @verbatim
 * Bits:
 *   #         Name      Description
 *   --------------------------------------------
 *   D7        MY        Page Address Order
 *   D6        MX        Column Address Order
 *   D5        MV        Page/Column Order
 *   D4        ML        Line Address Order
 *   D3        RGB       RGB/BGR Order
 *   D2        MH        Display Data Latch Order
 *   D1                  Reserved.
 *   D0                  Reserved.
 * @endverbatim
 * 
 * @param value The value to set.
 */
void st7789_command_madctl(st7789_t* this, uint8_t value) {
        st7789_send_command_8(this, ST7789_COMMAND_MADCTL);
        st7789_send_data_8(this, value);
}

void st7789_command_invon(st7789_t* this) {
        st7789_send_command_8(this, ST7789_COMMAND_INVON);
}

void st7789_command_invoff(st7789_t* this) {
        st7789_send_command_8(this, ST7789_COMMAND_INVOFF);
}

void st7789_command_dispon(st7789_t* this) {
        st7789_send_command_8(this, ST7789_COMMAND_DISPON);
}

void st7789_command_dispoff(st7789_t* this) {
        st7789_send_command_8(this, ST7789_COMMAND_DISPOFF);
}

/**
 * Column address set.
 */
void st7789_command_caset(st7789_t* this, uint16_t x0, uint16_t x1) {
        st7789_send_command_8(this, ST7789_COMMAND_CASET);
        st7789_send_data_32(this, ((x0 << 16) | x1));
}

/**
 * Row address set.
 */
void st7789_command_raset(st7789_t* this, uint16_t y0, uint16_t y1) {
        st7789_send_command_8(this, ST7789_COMMAND_RASET);
        st7789_send_data_32(this, ((y0 << 16) | y1));
}

void st7789_command_ramwr(st7789_t* this) {
        st7789_send_command_8(this, ST7789_COMMAND_RAMWR);
}

// ----------------------------------------------------------------------------
// Initialization.
//

static void configure_pins(st7789_t* this) {
        // Enable GPIO Port C and SPI peripheral
        RCC->APB2PCENR |= RCC_APB2Periph_GPIOC | RCC_APB2Periph_SPI1;
        // CS
        if (this->spi.pin_cs != -1) {
                GPIOC->CFGLR &= ~(0xf << (this->spi.pin_cs << 2));
                GPIOC->CFGLR |= (GPIO_CNF_OUT_PP | GPIO_Speed_50MHz)
                        << (this->spi.pin_cs << 2);
        }
        // RESET
        GPIOC->CFGLR &= ~(0xf << (this->spi.pin_reset << 2));
        GPIOC->CFGLR |= (GPIO_CNF_OUT_PP | GPIO_Speed_50MHz)
                << (this->spi.pin_reset << 2);
        // DC
        GPIOC->CFGLR &= ~(0xf << (this->spi.pin_dc << 2));
        GPIOC->CFGLR |= (GPIO_CNF_OUT_PP | GPIO_Speed_50MHz)
                << (this->spi.pin_dc << 2);
        // SCLK
        GPIOC->CFGLR &= ~(0xf << (this->spi.pin_sclk << 2));
        GPIOC->CFGLR |= (GPIO_CNF_OUT_PP_AF | GPIO_Speed_50MHz)
                << (this->spi.pin_sclk << 2);
        // MOSI
        GPIOC->CFGLR &= ~(0xf << (this->spi.pin_mosi << 2));
        GPIOC->CFGLR |= (GPIO_CNF_OUT_PP_AF | GPIO_Speed_50MHz)
                << (this->spi.pin_mosi << 2);
}

/**
 * Initialize serial periphial interface (SPI.)
 */
static void init_spi(st7789_t* this) {
        configure_pins(this);

        SPI1->CTLR1 =
                SPI_CPHA_2Edge  // Bit 0     - Clock PHAse
                | SPI_CPOL_High // Bit 1     - Clock POLarity - idles at the logical
                // high voltage
                | SPI_Mode_Master         // Bit 2     - Master device
                | SPI_BaudRatePrescaler_2 // Bit 3-5   - F_HCLK / 2
                | SPI_FirstBit_MSB        // Bit 7     - MSB transmitted first
                | SPI_NSS_Soft            // Bit 9     - Software slave management
                | SPI_DataSize_8b         // Bit 11    - 8-bit data
                |
                SPI_Direction_1Line_Tx; // Bit 14-15 - 1-line SPI, transmission only
        SPI1->CRCR = 7;             // CRC
        SPI1->CTLR2 |= SPI_I2S_DMAReq_Tx; // Configure SPI DMA Transfer
        SPI1->CTLR1 |= CTLR1_SPE_Set;     // Bit 6     - Enable SPI

        // Enable DMA peripheral
        RCC->AHBPCENR |= RCC_AHBPeriph_DMA1;

        // Config DMA for SPI TX
        DMA1_Channel3->CFGR =
                DMA_DIR_PeripheralDST         // Bit 4     - Read from memory
                | DMA_Mode_Circular           // Bit 5     - Circulation mode
                | DMA_PeripheralInc_Disable   // Bit 6     - Peripheral address no
                // change
                | DMA_MemoryInc_Enable        // Bit 7     - Increase memory address
                | DMA_PeripheralDataSize_Byte // Bit 8-9   - 8-bit data
                | DMA_MemoryDataSize_Byte     // Bit 10-11 - 8-bit data
                | DMA_Priority_VeryHigh       // Bit 12-13 - Very high priority
                | DMA_M2M_Disable; // Bit 14    - Disable memory to memory mode
        DMA1_Channel3->PADDR = (uint32_t)&SPI1->DATAR;
}

/**
 * Reset the display.
 * 
 * See <https://www.buydisplay.com/download/ic/ST7789.pdf>
 * Section 7.4.5 "Reset timing"
 * 
 *  ____  T_RW >= 10us  ________________
 *      \______________/  T_RT <= 120ms
 * 
 */
void st7789_reset(st7789_t* this) {
        const uint8_t T_RW = 15;  // us
        const uint8_t T_RT = 120; // ms
        st7789_spi_reset_high(this);
        Delay_Ms(1);
        st7789_spi_reset_low(this);
        Delay_Us(T_RW);
        st7789_spi_reset_high(this);
        Delay_Ms(T_RT);
}

/**
 * Initialize an ST7789 instance.
 * 
 * @param this An ST7789 intance.
 */
void st7789_init(st7789_t* this) {
        init_spi(this);
        st7789_reset(this);
        st7789_command_slpout(this);
        st7789_command_colmod(this, ST7789_COLOR_MODE_16_BPP);
        st7789_command_madctl(this, ST7789_MADCTL_MX | ST7789_MADCTL_RGB);
        st7789_command_invoff(this);
        st7789_command_dispon(this);
}

// ----------------------------------------------------------------------------
// Display operations.
//

 /**
  * Set the text cursor position.
  */
void st7789_cursor_set(st7789_t* this, uint16_t x, uint16_t y) {
        this->cursor.x = x;
        this->cursor.y = y;
}

/**
 * Get the current text cursor position.
 * 
 * @param this An ST7789 instance.
 * @return A structure that represents the position.
 */
st7789_point_t st7789_cursor_get(st7789_t* this) { 
        return this->cursor; 
}

/**
 * Set the foreground color.
 * 
 * This color is used when drawing a text.
 */
void st7789_foreground_color_set(st7789_t* this, st7789_color_t color) {
        this->foreground_color = color;
}

/**
 * Get the foreground color.
 */
st7789_color_t st7789_foreground_color_get(st7789_t* this) {
        return this->foreground_color;
}

/**
 * Set the background color.
 * 
 * This color is used when drawing a text.
 */
void st7789_background_color_set(st7789_t* this, st7789_color_t color) {
        this->background_color = color;
}

/**
 * Get the background color.
 */
st7789_color_t st7789_background_color_get(st7789_t* this) {
        return this->background_color;
}

void st7789_window_set(st7789_t* this, uint16_t x0, uint16_t y0, uint16_t x1,
        uint16_t y1) {
        st7789_command_caset(this, x0, x1);
        st7789_command_raset(this, y0, y1);
        st7789_command_ramwr(this);
}

/* st7789.c ends here. */