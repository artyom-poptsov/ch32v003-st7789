/* st7789-graphics.h -- ST7789 graphics header.
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

#ifndef __ST7789_GRAPHICS_H__
#define __ST7789_GRAPHICS_H__

#include "st7789.h"
#include "st7789-fonts.h"

#include <stdint.h>

/**
 * Rectangle shape.
 */
typedef struct {
        /**
         * Position of the upper left corner of the rectangle.
         */
        st7789_point_t position;

        /**
         * Rectangle size (in pixels.)
         */
        st7789_dimension_t size;

        /**
         * 16-bit color.
         */
        uint16_t color;
} st7789_rectangle_t;

/**
 * Line shape.
 */
typedef struct {
        st7789_point_t start;
        st7789_point_t end;
        uint16_t color;
} st7789_line_t;

/**
 * This structure describes an 1-bit image (bitmap.)
 */
typedef struct {
        /**
         * Position of the image.
         */
        st7789_point_t position;
        
        /**
         * Size of the image.
         */
        st7789_dimension_t size;

        /**
         * A color to draw pixels with.
         */
        uint16_t color;

        /**
         * A pointer to the image data.
         */
        uint8_t* data;
} st7789_bitmap_t;

/* Low-level graphic primitives. */

void st7789_draw_pixel(st7789_t* this, uint16_t x, uint16_t y, uint16_t color);
void st7789_draw_vertical_line(st7789_t* this, uint16_t x, uint16_t y,
                               uint16_t height, uint16_t color);
void st7789_draw_horizontal_line(st7789_t* this, uint16_t x, uint16_t y,
                                 uint16_t width, uint16_t color);

/* High-level primitives. */

void st7789_draw_screen(st7789_t* this, uint16_t color);
void st7789_draw_line(st7789_t* this, st7789_line_t line);
void st7789_draw_rectangle(st7789_t* this, st7789_rectangle_t rect);
void st7789_draw_rectangle_ptr(st7789_t* this, const st7789_rectangle_t* rect);
void st7789_draw_filled_rectangle(st7789_t* this, st7789_rectangle_t rect);
void st7789_draw_filled_rectangle_ptr(st7789_t* this,
                                      const st7789_rectangle_t* rect);
void st7789_draw_bitmap(st7789_t* this, st7789_bitmap_t* bitmap);

#ifdef ST7789_FONTS_ENABLED
void st7789_draw_char(st7789_t* this, char c,
        const st7789_font_t* font);
void st7789_draw_text(st7789_t* this, const char *str,
        const st7789_font_t* font);
#endif

#endif /* ifndef __ST7789_GRAPHICS_H__ */

/* st7789-graphics.h ends here. */