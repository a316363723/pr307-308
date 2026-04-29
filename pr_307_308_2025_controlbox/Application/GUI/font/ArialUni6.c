/*******************************************************************************
 * Size: 6 px
 * Bpp: 4
 * Opts: 
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef ARIALUNI6
#define ARIALUNI6 1
#endif

#if ARIALUNI6

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */

    /* U+0021 "!" */
    0x72, 0x3, 0x21, 0x18,

    /* U+0022 "\"" */
    0x99, 0x5d, 0x80,

    /* U+0023 "#" */
    0x6, 0x70, 0x8e, 0xd1, 0x17, 0x80, 0x39, 0x24,
    0x40,

    /* U+0024 "$" */
    0x4, 0x0, 0x41, 0xb8, 0x76, 0xa8, 0x27, 0xc0,
    0x27, 0x98, 0x34, 0x48, 0x0,

    /* U+0025 "%" */
    0x57, 0x26, 0x6, 0x16, 0x50, 0x36, 0x26, 0x50,
    0x57, 0x22, 0x0,

    /* U+0026 "&" */
    0x19, 0x80, 0x12, 0xd0, 0x88, 0x7a, 0xf2, 0x11,
    0x19, 0xe1, 0xc0,

    /* U+0027 "'" */
    0x9b,

    /* U+0028 "(" */
    0x5, 0x74, 0xf1, 0xf1, 0x77, 0x0,

    /* U+0029 ")" */
    0x60, 0x77, 0xf, 0xf, 0x23, 0x80,

    /* U+002A "*" */
    0x49, 0x7, 0xd0,

    /* U+002B "+" */
    0x5, 0x0, 0x2f, 0x38, 0xa9, 0xb8, 0x80,

    /* U+002C "," */
    0x41, 0x71,

    /* U+002D "-" */
    0x66, 0x0,

    /* U+002E "." */
    0x41,

    /* U+002F "/" */
    0x6, 0x35, 0x51, 0x80, 0x0,

    /* U+0030 "0" */
    0x47, 0x70, 0x63, 0xf8, 0x7, 0x63, 0xf8, 0x0,

    /* U+0031 "1" */
    0x9, 0x42, 0xf0, 0x22, 0x0, 0x60,

    /* U+0032 "2" */
    0x67, 0x80, 0x77, 0x8, 0xc, 0x58, 0x48, 0xa8,
    0x0,

    /* U+0033 "3" */
    0x46, 0x70, 0x42, 0xf0, 0x24, 0xf0, 0x77, 0x70,
    0x0,

    /* U+0034 "4" */
    0x5, 0x70, 0x41, 0x0, 0x61, 0xe0, 0x43, 0xe0,
    0x0,

    /* U+0035 "5" */
    0x58, 0x60, 0xdf, 0x0, 0x53, 0xf0, 0x23, 0x80,
    0x0,

    /* U+0036 "6" */
    0x47, 0x80, 0xd1, 0xd0, 0x36, 0xd0, 0xf7, 0x0,
    0x0,

    /* U+0037 "7" */
    0x57, 0xc0, 0x52, 0x80, 0x6, 0x20, 0x0, 0x40,
    0x0,

    /* U+0038 "8" */
    0x56, 0x70, 0x7, 0xf0, 0x64, 0xf0, 0x63, 0xf0,
    0x0,

    /* U+0039 "9" */
    0x57, 0x70, 0x63, 0xf0, 0x53, 0x98, 0x30, 0xe0,
    0x0,

    /* U+003A ":" */
    0x41, 0x41, 0x41,

    /* U+003B ";" */
    0x41, 0x41, 0x41, 0x71,

    /* U+003C "<" */
    0x0, 0x8, 0x1b, 0xa0, 0x4, 0x60, 0x6e, 0xe0,

    /* U+003D "=" */
    0x57, 0x70, 0x80, 0x60,

    /* U+003E ">" */
    0x20, 0xc, 0xec, 0x1, 0xe7, 0x60,

    /* U+003F "?" */
    0x67, 0x90, 0x77, 0x0, 0x5, 0xe0, 0x0, 0x42,
    0x0,

    /* U+0040 "@" */
    0x6, 0x76, 0x71, 0x52, 0xfe, 0x16, 0x33, 0x4f,
    0x20, 0x0, 0xa7, 0x69, 0x58, 0xfb, 0x7d, 0x80,

    /* U+0041 "A" */
    0x7, 0x70, 0x5, 0xfe, 0x0, 0x37, 0x82, 0x87,
    0xbc, 0x78, 0x0,

    /* U+0042 "B" */
    0x87, 0x84, 0x0, 0x10, 0x1, 0xe8, 0xc1, 0xe0,
    0x80,

    /* U+0043 "C" */
    0x28, 0x77, 0x5, 0x43, 0xa8, 0x6, 0x10, 0xa8,
    0x74, 0x0,

    /* U+0044 "D" */
    0x88, 0x98, 0x1, 0x10, 0xf0, 0xf, 0xa1, 0xfc,
    0x0,

    /* U+0045 "E" */
    0x88, 0x3a, 0x7, 0x83, 0x83, 0xb8, 0xc2, 0x1d,
    0x40,

    /* U+0046 "F" */
    0x88, 0x39, 0x7, 0x81, 0x3, 0xb8, 0x3, 0x80,

    /* U+0047 "G" */
    0x18, 0x79, 0x9, 0x87, 0xb0, 0x10, 0x3a, 0x38,
    0x94, 0x1,

    /* U+0048 "H" */
    0x80, 0x4, 0x83, 0xb8, 0x81, 0xdc, 0x40, 0x18,

    /* U+0049 "I" */
    0x72, 0x0, 0xf0,

    /* U+004A "J" */
    0x0, 0x40, 0x4, 0x40, 0xa, 0x9c,

    /* U+004B "K" */
    0x90, 0x65, 0x7, 0x5, 0x3, 0xf0, 0x2, 0x5c,
    0x0,

    /* U+004C "L" */
    0x80, 0xf, 0xfe, 0x3, 0xb8, 0x40,

    /* U+004D "M" */
    0x87, 0x5, 0x80, 0xf0, 0x20, 0x6, 0x29, 0x80,
    0x12, 0x50, 0x0,

    /* U+004E "N" */
    0x86, 0x8, 0x1, 0x20, 0x3, 0xd8, 0x80, 0x2d,
    0x40,

    /* U+004F "O" */
    0x28, 0x78, 0xb, 0x87, 0xd4, 0x0, 0x84, 0x2e,
    0x1f, 0x10,

    /* U+0050 "P" */
    0x88, 0x98, 0x22, 0x1e, 0xe, 0xea, 0x7, 0x71,
    0x80,

    /* U+0051 "Q" */
    0x28, 0x78, 0xb, 0x87, 0xd4, 0x0, 0x84, 0x2e,
    0x2b, 0xd4, 0xa2, 0xac, 0x0,

    /* U+0052 "R" */
    0x87, 0x74, 0x80, 0x3f, 0x70, 0x1, 0x1c, 0x60,
    0x12, 0x60, 0x80,

    /* U+0053 "S" */
    0x48, 0x94, 0x1f, 0xa5, 0x67, 0x73, 0x32, 0x30,
    0x80,

    /* U+0054 "T" */
    0x6b, 0x84, 0x6c, 0x94, 0x0, 0xfc,

    /* U+0055 "U" */
    0x80, 0x4, 0x80, 0x70, 0x80, 0x2e, 0x21, 0xe0,

    /* U+0056 "V" */
    0x90, 0x4, 0xe1, 0x9b, 0x16, 0xec, 0xa1, 0xfe,
    0x0,

    /* U+0057 "W" */
    0x80, 0x95, 0x17, 0xf0, 0xed, 0x45, 0x48, 0x4f,
    0xd2, 0x34, 0x33, 0x0, 0x0,

    /* U+0058 "X" */
    0x64, 0x36, 0x66, 0xdb, 0x0, 0x4, 0x22, 0xed,
    0x0,

    /* U+0059 "Y" */
    0x82, 0x18, 0x85, 0xcc, 0x6, 0x60, 0x3, 0x80,

    /* U+005A "Z" */
    0x47, 0xb5, 0x44, 0x62, 0x8d, 0xb8, 0x68, 0x39,
    0x80,

    /* U+005B "[" */
    0x83, 0x3, 0x0, 0xe3,

    /* U+005C "\\" */
    0x60, 0x9, 0x4c, 0xca,

    /* U+005D "]" */
    0x84, 0xc0, 0xe, 0xc0,

    /* U+005E "^" */
    0x1b, 0x6, 0x96,

    /* U+005F "_" */
    0x4, 0x48, 0x0,

    /* U+0060 "`" */
    0x51,

    /* U+0061 "a" */
    0x56, 0x80, 0x10, 0x20, 0x33, 0x8, 0x0,

    /* U+0062 "b" */
    0x80, 0x8, 0x5e, 0x0, 0x5c, 0x4, 0x18, 0x4,

    /* U+0063 "c" */
    0x56, 0x7c, 0x67, 0xf6, 0x70,

    /* U+0064 "d" */
    0x0, 0x43, 0x30, 0xf9, 0x8f, 0x58, 0xc0,

    /* U+0065 "e" */
    0x57, 0x70, 0x78, 0xf8, 0x78, 0xf8, 0x0,

    /* U+0066 "f" */
    0x66, 0x61, 0x6a, 0x0, 0x0,

    /* U+0067 "g" */
    0x66, 0x5f, 0x31, 0xf3, 0x99, 0x87, 0x0,

    /* U+0068 "h" */
    0x80, 0x0, 0xbc, 0x8b, 0x88, 0x4,

    /* U+0069 "i" */
    0x4c, 0x0, 0x0,

    /* U+006A "j" */
    0x4, 0xc, 0x0, 0xc2, 0x20,

    /* U+006B "k" */
    0x80, 0x9, 0x54, 0x3e, 0xa3, 0x70,

    /* U+006C "l" */
    0x80, 0x8,

    /* U+006D "m" */
    0x86, 0xa6, 0x60, 0x31, 0x37, 0x0, 0x70,

    /* U+006E "n" */
    0x86, 0x90, 0x61, 0x0, 0x80,

    /* U+006F "o" */
    0x56, 0x80, 0xd6, 0xf1, 0xd6, 0xf1,

    /* U+0070 "p" */
    0x86, 0x80, 0x3, 0x78, 0x89, 0xbc, 0x44, 0xd0,
    0x0,

    /* U+0071 "q" */
    0x66, 0x5f, 0x31, 0xeb, 0x9a, 0xb9, 0x80,

    /* U+0072 "r" */
    0x87, 0x0, 0x38, 0x6,

    /* U+0073 "s" */
    0x76, 0x43, 0xe1, 0x2e, 0x20,

    /* U+0074 "t" */
    0x0, 0x38, 0x69, 0x91, 0xf2, 0x0,

    /* U+0075 "u" */
    0x80, 0x80, 0xb, 0x58, 0x80,

    /* U+0076 "v" */
    0x80, 0x8d, 0x5d, 0x59, 0x50,

    /* U+0077 "w" */
    0x83, 0x95, 0x2f, 0x41, 0x22, 0x2e, 0xe8, 0x0,

    /* U+0078 "x" */
    0x75, 0x77, 0x43, 0xc7, 0x40,

    /* U+0079 "y" */
    0x80, 0x8c, 0x6d, 0x4d, 0x55, 0x68, 0x0,

    /* U+007A "z" */
    0x48, 0x95, 0x9, 0xaf, 0x50,

    /* U+007B "{" */
    0x18, 0x2b, 0xa3, 0xa3, 0x2b,

    /* U+007C "|" */
    0x70, 0xf, 0xe0,

    /* U+007D "}" */
    0x82, 0xb6, 0x3d, 0x3d, 0xb5,

    /* U+007E "~" */
    0x68, 0x81, 0x0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 27, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 0, .adv_w = 27, .box_w = 2, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 4, .adv_w = 34, .box_w = 2, .box_h = 2, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 7, .adv_w = 53, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 16, .adv_w = 53, .box_w = 4, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 29, .adv_w = 85, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 40, .adv_w = 64, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 51, .adv_w = 18, .box_w = 1, .box_h = 2, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 52, .adv_w = 32, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 58, .adv_w = 32, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 64, .adv_w = 37, .box_w = 3, .box_h = 2, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 67, .adv_w = 56, .box_w = 4, .box_h = 3, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 74, .adv_w = 27, .box_w = 2, .box_h = 2, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 76, .adv_w = 32, .box_w = 2, .box_h = 1, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 78, .adv_w = 27, .box_w = 2, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 79, .adv_w = 27, .box_w = 2, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 84, .adv_w = 53, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 92, .adv_w = 53, .box_w = 3, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 98, .adv_w = 53, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 107, .adv_w = 53, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 116, .adv_w = 53, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 125, .adv_w = 53, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 134, .adv_w = 53, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 143, .adv_w = 53, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 152, .adv_w = 53, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 161, .adv_w = 53, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 170, .adv_w = 27, .box_w = 2, .box_h = 3, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 173, .adv_w = 27, .box_w = 2, .box_h = 4, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 177, .adv_w = 56, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 185, .adv_w = 56, .box_w = 4, .box_h = 2, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 189, .adv_w = 56, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 195, .adv_w = 53, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 204, .adv_w = 97, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 220, .adv_w = 64, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 231, .adv_w = 64, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 240, .adv_w = 69, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 250, .adv_w = 69, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 259, .adv_w = 64, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 268, .adv_w = 59, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 276, .adv_w = 75, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 286, .adv_w = 69, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 294, .adv_w = 27, .box_w = 2, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 297, .adv_w = 48, .box_w = 3, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 303, .adv_w = 64, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 312, .adv_w = 53, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 318, .adv_w = 80, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 329, .adv_w = 69, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 338, .adv_w = 75, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 348, .adv_w = 64, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 357, .adv_w = 75, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 370, .adv_w = 69, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 381, .adv_w = 64, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 390, .adv_w = 59, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 396, .adv_w = 69, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 404, .adv_w = 64, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 413, .adv_w = 91, .box_w = 6, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 426, .adv_w = 64, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 435, .adv_w = 64, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 443, .adv_w = 59, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 452, .adv_w = 27, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 456, .adv_w = 27, .box_w = 2, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 460, .adv_w = 27, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 464, .adv_w = 45, .box_w = 3, .box_h = 2, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 467, .adv_w = 48, .box_w = 5, .box_h = 1, .ofs_x = -1, .ofs_y = -1},
    {.bitmap_index = 470, .adv_w = 32, .box_w = 2, .box_h = 1, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 471, .adv_w = 53, .box_w = 4, .box_h = 3, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 478, .adv_w = 53, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 486, .adv_w = 48, .box_w = 3, .box_h = 3, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 491, .adv_w = 53, .box_w = 3, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 498, .adv_w = 53, .box_w = 4, .box_h = 3, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 505, .adv_w = 27, .box_w = 2, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 510, .adv_w = 53, .box_w = 3, .box_h = 4, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 517, .adv_w = 53, .box_w = 3, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 523, .adv_w = 21, .box_w = 1, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 526, .adv_w = 21, .box_w = 2, .box_h = 5, .ofs_x = -1, .ofs_y = -1},
    {.bitmap_index = 531, .adv_w = 48, .box_w = 3, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 537, .adv_w = 21, .box_w = 1, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 539, .adv_w = 80, .box_w = 5, .box_h = 3, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 546, .adv_w = 53, .box_w = 3, .box_h = 3, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 551, .adv_w = 53, .box_w = 4, .box_h = 3, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 557, .adv_w = 53, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 566, .adv_w = 53, .box_w = 3, .box_h = 4, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 573, .adv_w = 32, .box_w = 3, .box_h = 3, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 577, .adv_w = 48, .box_w = 3, .box_h = 3, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 582, .adv_w = 27, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 588, .adv_w = 53, .box_w = 3, .box_h = 3, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 593, .adv_w = 48, .box_w = 3, .box_h = 3, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 598, .adv_w = 69, .box_w = 5, .box_h = 3, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 606, .adv_w = 48, .box_w = 3, .box_h = 3, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 611, .adv_w = 48, .box_w = 3, .box_h = 4, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 618, .adv_w = 48, .box_w = 3, .box_h = 3, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 623, .adv_w = 32, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 628, .adv_w = 25, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 631, .adv_w = 32, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 636, .adv_w = 56, .box_w = 4, .box_h = 1, .ofs_x = 0, .ofs_y = 1}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LV_VERSION_CHECK(8, 0, 0)
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 1,
    .bpp = 4,
    .kern_classes = 0,
    .bitmap_format = 1,
#if LV_VERSION_CHECK(8, 0, 0)
    .cache = &cache
#endif
};


/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LV_VERSION_CHECK(8, 0, 0)
const lv_font_t ArialUni6 = {
#else
lv_font_t ArialUni6 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 6,          /*The maximum line height required by the font*/
    .base_line = 1,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0)
    .underline_position = -1,
    .underline_thickness = 0,
#endif
    .dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};



#endif /*#if ARIALUNI6*/

