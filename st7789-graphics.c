/* st7789-graphics.c -- ST7789 graphics.
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

#include "st7789-graphics.h"
#include "st7789-fonts.h"

// ----------------------------------------------------------------------------
// Low-level graphics. 
//

/**
 * Draw a pixel on the screen.
 *
 * @param this An ST7789 display instance.
 * @param x X position of the pixel.
 * @param y Y position of the pixel.
 * @param color 16-bit color.
 */
void st7789_draw_pixel(st7789_t* this, uint16_t x, uint16_t y, uint16_t color) {
        st7789_window_set(this, x, y, x, y);
        st7789_send_data_16(this, color);
}

/**
 * Draw a vertical line on the screen.
 *
 * @param this An ST7789 display instance.
 * @param x The X position of the line starting point.
 * @param y The Y position of the line starting point.
 * @param height The height of the line.
 * @param color 16-bit color.
 */
void st7789_draw_vertical_line(st7789_t* this, uint16_t x, uint16_t y,
        uint16_t height, uint16_t color) {
        this->buffer[0] = color >> 8;
        this->buffer[1] = color;
        st7789_window_set(this, x, y, x, y + height - 1);
        st7789_spi_send_dma(this, 2, height);
}

/**
 * Draw a horizontal line on the screen.
 *
 * @param this An ST7789 display instance.
 * @param x The X position of the line starting point.
 * @param y The Y position of the line starting point.
 * @param width The width of the line.
 * @param color 16-bit color.
 */
void st7789_draw_horizontal_line(st7789_t* this, uint16_t x, uint16_t y,
        uint16_t width, uint16_t color) {
        this->buffer[0] = color >> 8;
        this->buffer[1] = color;
        st7789_window_set(this, x, y, x + width - 1, y);
        st7789_spi_send_dma(this, 2, width);
}

// ----------------------------------------------------------------------------
// High-level graphics.
//

/**
 * Draw a rectangle using a pointer.
 *
 * While it is more effective than "st7789_draw_rectangle" (because it does not
 * copy a rectangle instance), it can be less convenient.
 *
 * @param this An ST7789 display instance.
 * @param rect A pointer to a rectangle instance.  The procedure
 *      does not modify the object.
 */
void st7789_draw_rectangle_ptr(st7789_t* this, const st7789_rectangle_t* rect) {
        st7789_draw_horizontal_line(this, rect->position.x, rect->position.y,
                rect->size.width, rect->color);
        st7789_draw_horizontal_line(this, rect->position.x,
                rect->position.y + rect->size.height - 1,
                rect->size.width, rect->color);
        st7789_draw_vertical_line(this, rect->position.x, rect->position.y,
                rect->size.height, rect->color);
        st7789_draw_vertical_line(this, rect->position.x + rect->size.width - 1,
                rect->position.y, rect->size.height,
                rect->color);
}

/**
 * Draw a rectangle using a pointer.
 *
 * @param this An ST7789 display instance.
 * @param rect A rectangle instance.
 */
void st7789_draw_rectangle(st7789_t* this, const st7789_rectangle_t rect) {
        st7789_draw_rectangle_ptr(this, &rect);
}

/**
 * Draw a rectangle using a pointer.
 *
 * While it is more effective than "st7789_draw_filled_rectangle" (because
 * it does not copy a rectangle instance), it can be less convenient.
 *
 * @param this An ST7789 display instance.
 * @param rect A pointer to a rectangle to draw.  The procedure
 *      does not modify the object.
 */
void st7789_draw_filled_rectangle_ptr(st7789_t* this,
        const st7789_rectangle_t* rect) {
        uint16_t x = rect->position.x;
        uint16_t y = rect->position.y;
        uint16_t width = rect->size.width;
        uint16_t height = rect->size.height;

        uint16_t size = 0;
        for (uint16_t x = 0; x < width; x++) {
                this->buffer[size++] = rect->color >> 8;
                this->buffer[size++] = rect->color;
        }

        st7789_window_set(this, x, y, x + width - 1, y + height - 1);
        st7789_spi_send_dma(this, size, height);
}

/**
 * Draw a filled rectangle using a pointer.
 *
 * @param this An ST7789 display instance.
 * @param rect A rectangle instance.
 */
void st7789_draw_filled_rectangle(st7789_t* this, st7789_rectangle_t rect) {
        st7789_draw_filled_rectangle_ptr(this, &rect);
}

void st7789_draw_screen(st7789_t* this, uint16_t color) {
        st7789_rectangle_t full_screen_rect = {
            .position = {.x = 0, .y = 0}, .size = this->size, .color = color };
        st7789_draw_filled_rectangle_ptr(this, &full_screen_rect);
}

/**
 * Calculate the difference between two 16-bit unsigned numbers.
 */
static uint16_t diff_uint16(uint16_t a, uint16_t b) {
        return ((a > b) ? (a - b) : (b - a));
}

/**
 * Swap two 16-bit values.
 */
static void swap_uint16(uint16_t* a, uint16_t* b) {
        uint16_t temp = *a;
        *a = *b;
        *b = temp;
}

/**
 * Draw a filled rectangle using a pointer.
 *
 * This procedure uses Bresenham's algorithm, taken from the
 * Arduino GFX library:
 *   <https://github.com/moononournation/Arduino_GFX>
 *
 * @param this An ST7789 display instance.
 * @param rect A line instance.
 */
static void draw_line_bresenham(st7789_t* this, st7789_line_t line) {
        uint16_t dy = diff_uint16(line.end.y, line.start.y);
        uint16_t dx = diff_uint16(line.end.x, line.start.x);
        uint8_t steep = dy > dx;
        if (steep) {
                swap_uint16(&line.start.x, &line.start.y);
                swap_uint16(&line.end.x, &line.end.y);
        }

        if (line.start.x > line.end.x) {
                swap_uint16(&line.start.x, &line.end.x);
                swap_uint16(&line.start.y, &line.end.y);
        }

        dx = line.end.x - line.start.x;
        dy = diff_uint16(line.end.y, line.start.y);
        int16_t err = dx >> 1;
        int16_t step = (line.start.y < line.end.y) ? 1 : -1;

        for (; line.start.x <= line.end.x; line.start.x++) {
                if (steep) {
                        st7789_draw_pixel(this, line.start.y, line.start.x,
                                line.color);
                }
                else {
                        st7789_draw_pixel(this, line.start.x, line.start.y,
                                line.color);
                }
                err -= dy;
                if (err < 0) {
                        err += dx;
                        line.start.y += step;
                }
        }
}

/**
 * Draw a line shape on a display.
 */
void st7789_draw_line(st7789_t* this, st7789_line_t line) {
        if (line.start.x == line.end.x) {
                if (line.start.y > line.end.y) {
                        swap_uint16(&line.start.y, &line.end.y);
                }
                st7789_draw_vertical_line(this, line.start.x, line.start.y,
                        line.end.y - line.start.y + 1,
                        line.color);
        }
        else if (line.start.y == line.end.y) {
                if (line.start.x > line.end.x) {
                        swap_uint16(&line.start.x, &line.end.x);
                }
                st7789_draw_horizontal_line(this, line.start.x, line.start.y,
                        line.end.x - line.start.x + 1,
                        line.color);
        }
        else {
                draw_line_bresenham(this, line);
        }
}

/**
 * Draw a 1-bit image.
 *
 * This procedure uses the bitmap drawing algorithm taken from the
 * Arduino GFX library, with some slight modifications:
 *   <https://github.com/moononournation/Arduino_GFX>
 *
 * @param this An ST7789 display instance.
 * @param bitmap A bitmap instance.
 */
void st7789_draw_bitmap(st7789_t* this, st7789_bitmap_t* bitmap) {
        uint16_t scanline = (bitmap->size.width + 7) / 8;
        uint16_t x = bitmap->position.x;
        uint16_t y = bitmap->position.y;
        uint8_t byte = 0;
        uint16_t index = 0;
        for (int row = 0; row < bitmap->size.height; row++, y++) {
                for (int column = 0; column < bitmap->size.width; column++) {
                        if (column & 0b111) {
                                byte <<= 1;
                        } else {
                                index = (row * scanline) + (column / 8);
                                byte = bitmap->data[index];
                        }

                        if (byte & (1 << 7)) {
                                st7789_draw_pixel(this, x + column, y,
                                        bitmap->color);
                        }
                }
        }
}

/**
 * Draw a char on the display.
 *
 * Based on "ili9341_putc" procedure from
 *   <https://github.com/AdiHamulic/CH32V003-SPI-DMA---ILI9341>
 *
 * @param this An ST7789 display instance.
 * @param c A character to draw.
 * @param font A font to use.
 *
 * @see ST7789_FONT_7X10
 * @see ST7789_FONT_11X18
 * @see ST7789_FONT_16X26
 */
void st7789_draw_char(st7789_t* this, char c,
        const st7789_font_t* font)
{
        st7789_color_t fg_color = this->foreground_color;
        st7789_color_t bg_color = this->background_color;
        uint32_t data;
        uint16_t y = 0;
        uint16_t x = 0;

	if ((this->cursor.x + font->width) > this->size.width) {
		// If at the end of a line of display, go to new line and
                // set x to 0 position.
		this->cursor.y += font->height;
		this->cursor.x = 0;
	}

	for (uint16_t row = 0; row < font->height; row++) {
		data = font->data[((c - 32) * font->height) + row];
                y = (row * (font->width * 2));
                x = 0;
		for (uint16_t column = 0; column < font->width; column++) {
			if ((data << column) & 0x8000) {
				this->buffer[y + x] = fg_color >> 8;
                                this->buffer[y + x + 1] = fg_color & 0xFF;
			} else {
				this->buffer[y + x] = bg_color;
                                this->buffer[y + x + 1] = bg_color & 0xFF;
			}
                        x += 2;
		}
	}

        this->cursor.x += font->width;

        st7789_window_set(
                this,
                this->cursor.x,
                this->cursor.y,
                this->cursor.x + font->width - 1,
                this->cursor.y + font->height - 1);
        st7789_spi_send_dma(this, (font->width * font->height) * 2, 1);
}

/**
 * Draw a string on the display.
 *
 * Based on "ili9341_puts" procedure from
 *   <https://github.com/AdiHamulic/CH32V003-SPI-DMA---ILI9341>
 *
 * @param this An ST7789 display instance.
 * @param str A character string to draw.
 * @param font A font to use.
 *
 * @see st7789_draw_char
 */
void st7789_draw_text(st7789_t* this, const char *str,
        const st7789_font_t* font)
{
	uint16_t start_x = this->cursor.x;
	for (; *str; str++) {
		//New line
		if (*str == '\n') {
			this->cursor.y += font->height + 1;
			// If after '\n' is also '\r', than go to the left of
                        // the screen.
			if (*(str + 1) == '\r') {
				this->cursor.x = 0;
				str++;
			} else {
				this->cursor.x = start_x;
			}
		} else if (*str == '\r') {
                        continue;
		} else {
                        st7789_draw_char(this, *str, font);
                }
	}
}

/* st7789-graphics.c ends here. */
