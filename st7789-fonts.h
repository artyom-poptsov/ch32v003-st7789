/* st7789-fonts.h -- ST7789 fonts header.
 *
 * This font implementation is based on the work of Tilen Majerle.
 *
 * Copyright (C) 2014 Tilen Majerle
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

#ifndef __ST7789_FONTS_H__
#define __ST7789_FONTS_H__

#include <stdint.h>

typedef struct {
	uint8_t width;    /*!< Font width in pixels */
	uint8_t height;   /*!< Font height in pixels */
	const uint16_t* data; /*!< Pointer to data font data array */
} st7789_font_t;

extern const st7789_font_t ST7789_FONT_7X10;
extern const st7789_font_t ST7789_FONT_11X18;
extern const st7789_font_t ST7789_FONT_16X26;

#endif /* ifndef __ST7789_FONTS_H__ */