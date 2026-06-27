/* st7789.h -- ST7789 driver implementation header.
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

#ifndef __ST7789_H__
#define __ST7789_H__

#include <stdint.h>
#include "st7789-commands.h"

#define RGB565(r, g, b) ((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3))
#define BGR565(r, g, b) ((((b) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((r) >> 3))
#define RGB RGB565

/**
 * Colors.
 */
enum {
        BLACK = RGB(0, 0, 0),
        NAVY = RGB(0, 0, 123),
        DARKGREEN = RGB(0, 125, 0),
        DARKCYAN = RGB(0, 125, 123),
        MAROON = RGB(123, 0, 0),
        PURPLE = RGB(123, 0, 123),
        OLIVE = RGB(123, 125, 0),
        LIGHTGREY = RGB(198, 195, 198),
        DARKGREY = RGB(123, 125, 123),
        BLUE = RGB(0, 0, 255),
        GREEN = RGB(0, 255, 0),
        CYAN = RGB(0, 255, 255),
        RED = RGB(255, 0, 0),
        MAGENTA = RGB(255, 0, 255),
        YELLOW = RGB(255, 255, 0),
        WHITE = RGB(255, 255, 255),
        ORANGE = RGB(255, 165, 0),
        GREENYELLOW = RGB(173, 255, 41),
        PINK = RGB(255, 130, 198)
};

/**
 * MADCTL Parameters
 */
typedef enum {
        ST7789_MADCTL_MH  = 0x04, // Bit 2 - Refresh Left to Right
        ST7789_MADCTL_RGB = 0x00, // Bit 3 - RGB Order
        ST7789_MADCTL_BGR = 0x08, // Bit 3 - BGR Order
        ST7789_MADCTL_ML  = 0x10, // Bit 4 - Scan Address Increase
        ST7789_MADCTL_MV  = 0x20, // Bit 5 - X-Y Exchange
        ST7789_MADCTL_MX  = 0x40, // Bit 6 - X-Mirror
        ST7789_MADCTL_MY  = 0x80, // Bit 7 - Y-Mirror
} st7789_madctl_t;

enum {
        ST7789_COLOR_MODE_16_BPP = 0x55 // 01010101 (16-bit/pixel)
};

typedef uint16_t st7789_color_t;

typedef struct {
        uint16_t width;
        uint16_t height;
} st7789_dimension_t;

typedef struct {
        uint16_t x;
        uint16_t y;
} st7789_point_t;

typedef struct {
        /**
         * Chip select.
         */
        int8_t pin_cs;

        int8_t pin_reset;

        /**
         * Data/command pin.
         */
        int8_t pin_dc;

        /**
         * Clock pin.
         */
        int8_t pin_sclk;

        /**
         * Master out, slave in (Controller out, periphery in)
         */
        int8_t pin_mosi;
} st7789_spi_t;

/**
 * ST7789 device.
 */
typedef struct {
        st7789_spi_t spi;
        st7789_dimension_t size;
        st7789_point_t cursor;
        st7789_color_t foreground_color;
        st7789_color_t background_color;
        uint8_t* buffer;
        uint32_t buffer_size;
} st7789_t;

void st7789_reset(st7789_t* this);
void st7789_spi_reset_high(st7789_t* this);
void st7789_spi_reset_low(st7789_t* this);
void st7789_spi_command_mode(st7789_t* this);
void st7789_spi_data_mode(st7789_t* this);
void st7789_spi_start_write(st7789_t* this);
void st7789_spi_end_write(st7789_t* this);
void st7789_spi_send_dma(st7789_t* this, uint32_t size, uint16_t repeat);

void st7789_send_data_8(st7789_t* this, uint8_t data);
void st7789_send_data_16(st7789_t* this, uint16_t data);
void st7789_send_command_8(st7789_t* this, st7789_command_t command);

void st7789_command_slpout(st7789_t* this);
void st7789_command_slpin(st7789_t* this);
void st7789_command_colmod(st7789_t* this, uint8_t mode);
void st7789_command_madctl(st7789_t* this, uint8_t value);
void st7789_command_invon(st7789_t* this);
void st7789_command_invoff(st7789_t* this);
void st7789_command_dispon(st7789_t* this);
void st7789_command_dispoff(st7789_t* this);
void st7789_command_ramwr(st7789_t* this);
void st7789_command_caset(st7789_t* this, uint16_t x0, uint16_t x1);
void st7789_command_raset(st7789_t* this, uint16_t y0, uint16_t y1);

void st7789_init(st7789_t* this);
void st7789_cursor_set(st7789_t* this, uint16_t x, uint16_t y);
st7789_point_t st7789_cursor_get(st7789_t* this);
void st7789_foreground_color_set(st7789_t* this, st7789_color_t color);
st7789_color_t st7789_foreground_color_get(st7789_t* this);
void st7789_background_color_set(st7789_t* this, st7789_color_t color);
st7789_color_t st7789_background_color_get(st7789_t* this);

void st7789_window_set(st7789_t* this, uint16_t x0, uint16_t y0, uint16_t x1,
                       uint16_t y1);

#endif /* ifndef __ST7789_H__ */

/* st7789.h ends here. */