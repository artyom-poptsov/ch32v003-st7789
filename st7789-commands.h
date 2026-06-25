/* st7789-commands.h -- ST7789 commands.
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

#ifndef __ST7789_COMMAND_H__
#define __ST7789_COMMAND_H__

/**
 * ST7789 commands.
 * 
 * See https://www.buydisplay.com/download/ic/ST7789.pdf
 * Chapter 9, "COMMAND".
 */ 
typedef enum {
        ST7789_COMMAND_NOP        = 0x00,  // NOP
        ST7789_COMMAND_SWRESET    = 0x01,  // Software Reset
        ST7789_COMMAND_SLPIN      = 0x10,  // Sleep In
        ST7789_COMMAND_SLPOUT     = 0x11,  // Sleep Out
        ST7789_COMMAND_PTLON      = 0x12,  // Partial Display Mode On
        ST7789_COMMAND_NORON      = 0x13,  // Normal Display Mode On
        ST7789_COMMAND_INVOFF     = 0x20,  // Display Inversion Off
        ST7789_COMMAND_INVON      = 0x21,  // Display Inversion On
        ST7789_COMMAND_DISPOFF    = 0x28,  // Display Off
        ST7789_COMMAND_DISPON     = 0x29,  // Display On
        ST7789_COMMAND_CASET      = 0x2A,  // Column Address Set
        ST7789_COMMAND_RASET      = 0x2B,  // Row Address Set
        ST7789_COMMAND_RAMWR      = 0x2C,  // Memory Write
        ST7789_COMMAND_PLTAR      = 0x30,  // Partial Area
        ST7789_COMMAND_MADCTL     = 0x36,  // Memory Data Access Control
        ST7789_COMMAND_COLMOD     = 0x3A,  // Interface Pixel Format
        ST7789_COMMAND_RAMCTRL    = 0xB0,  // RAM Control
        ST7789_COMMAND_PORCTRL    = 0xB2,  // Porch Setting
        ST7789_COMMAND_GCTRL      = 0xB7,  // Gate Control
        ST7789_COMMAND_VCOMS      = 0xBB,  // VCOMS Setting
        ST7789_COMMAND_LCMCTRL    = 0xC0,  // LCM Control
        ST7789_COMMAND_VDVVRHEN   = 0xC2,  // VDV and VRH Command Enable
        ST7789_COMMAND_VRHS       = 0xC3,  // VRH Set
        ST7789_COMMAND_VDVS       = 0xC4,  // VDV Set
        ST7789_COMMAND_FRCTRL2    = 0xC6,  // Frame Rate Control in Normal Mode
        ST7789_COMMAND_PWCTRL1    = 0xD0,  // Power Control 1
        ST7789_COMMAND_PVGAMCTRL  = 0xE0,  // Positive Voltage Gamma Control
        ST7789_COMMAND_NVGAMCTRL  = 0xE1,  // Negative Voltage Gamma Control
} st7789_command_t;

#endif /* ifndef __ST7789_COMMAND_H__ */

/* st7789-commands.h ends here. */