/*******************************************************************************
 * Size: 8 px
 * Bpp: 4
 * Opts: 
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef ARIALUNI8
#define ARIALUNI8 1
#endif

#if ARIALUNI8

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */

    /* U+0021 "!" */
    0x58, 0x10, 0x7f, 0x11, 0x80,

    /* U+0022 "\"" */
    0xa6, 0x72, 0x52,

    /* U+0023 "#" */
    0x8, 0x9, 0x7, 0x59, 0x53, 0x74, 0xba, 0x39,
    0x29, 0xc3, 0xe9, 0x2a, 0x30,

    /* U+0024 "$" */
    0x8, 0xc3, 0x7, 0xcb, 0xb0, 0x16, 0x84, 0x2,
    0xde, 0x50, 0x31, 0xe8, 0x82, 0xdd, 0x18, 0x1d,
    0x7c, 0x0,

    /* U+0025 "%" */
    0x38, 0x90, 0x62, 0x1, 0xfe, 0x20, 0x20, 0x27,
    0x75, 0x1c, 0x10, 0x1, 0x6f, 0xe6, 0x80, 0x74,
    0xca, 0x2c,

    /* U+0026 "&" */
    0x9, 0x94, 0x0, 0x47, 0x62, 0x0, 0x10, 0xeb,
    0x30, 0x89, 0xdf, 0x40, 0xa8, 0x82, 0xe0, 0x1d,
    0xcc, 0x58, 0x0,

    /* U+0027 "'" */
    0xa2, 0xd2,

    /* U+0028 "(" */
    0x6, 0x12, 0x11, 0x29, 0x80, 0x65, 0xc0, 0x2f,
    0x10,

    /* U+0029 ")" */
    0x53, 0x5, 0x90, 0x1, 0x88, 0x18, 0x4, 0x2b,
    0x0,

    /* U+002A "*" */
    0x39, 0x30, 0x56, 0x3c, 0x40,

    /* U+002B "+" */
    0x0, 0x30, 0x6, 0xc0, 0x2, 0xd3, 0xd1, 0x2d,
    0x3d, 0x10,

    /* U+002C "," */
    0x36, 0x23, 0x15,

    /* U+002D "-" */
    0x7a, 0x40,

    /* U+002E "." */
    0x36,

    /* U+002F "/" */
    0x7, 0x10, 0xe1, 0x17, 0x3, 0xa8, 0x71, 0x6,
    0x0, 0x0,

    /* U+0030 "0" */
    0x1a, 0x96, 0x9, 0x9b, 0x40, 0x48, 0x4, 0x62,
    0x1, 0x14, 0xcd, 0xa0,

    /* U+0031 "1" */
    0x1, 0xd1, 0x91, 0x14, 0x38, 0x7, 0x80,

    /* U+0032 "2" */
    0x2a, 0x98, 0x6, 0xb9, 0x30, 0x41, 0x7d, 0x0,
    0x49, 0xb0, 0x4a, 0xf5, 0x0,

    /* U+0033 "3" */
    0x2a, 0x96, 0x1, 0xb9, 0xc0, 0x30, 0xb2, 0x3,
    0xb, 0x30, 0x5, 0xc9, 0x80,

    /* U+0034 "4" */
    0x0, 0x52, 0x80, 0x1c, 0xc0, 0x8, 0x5e, 0x0,
    0xec, 0xae, 0xa, 0x9a, 0xe0,

    /* U+0035 "5" */
    0x1d, 0xa8, 0x4, 0x60, 0xd0, 0x7a, 0xa4, 0x83,
    0x80, 0x1c, 0x1a, 0xe7, 0x40,

    /* U+0036 "6" */
    0x19, 0x88, 0x3, 0x8, 0xb4, 0x37, 0x27, 0x80,
    0x9c, 0x4, 0x26, 0x6d, 0x0,

    /* U+0037 "7" */
    0x6a, 0xa7, 0x3, 0x56, 0xd0, 0x0, 0x75, 0x0,
    0x13, 0x20, 0x8, 0xcc, 0x0,

    /* U+0038 "8" */
    0x3a, 0x86, 0x1, 0x62, 0xa0, 0xaf, 0xa9, 0x1,
    0x10, 0x70, 0x5c, 0x49, 0x80,

    /* U+0039 "9" */
    0x3a, 0x95, 0xa, 0x89, 0xe0, 0xb8, 0x80, 0xb,
    0x5c, 0x38, 0xb5, 0xd4, 0x80,

    /* U+003A ":" */
    0x36, 0x36, 0x0, 0x1b, 0x0,

    /* U+003B ";" */
    0x36, 0x36, 0x0, 0x1b, 0x11, 0x8a, 0x80,

    /* U+003C "<" */
    0x0, 0x8c, 0x41, 0xa9, 0x5, 0xb6, 0x1c, 0x1a,
    0x96, 0x88, 0x5, 0xe8, 0x80,

    /* U+003D "=" */
    0x59, 0x98, 0x80, 0xce, 0x0,

    /* U+003E ">" */
    0x31, 0x0, 0x86, 0x64, 0x80, 0x51, 0x92, 0x4b,
    0x41, 0xa4, 0xb4, 0xa0, 0x0,

    /* U+003F "?" */
    0x3b, 0x98, 0x7, 0xa9, 0x30, 0x41, 0x9e, 0x0,
    0xf, 0xa8, 0x0, 0xc4, 0x0,

    /* U+0040 "@" */
    0x1, 0x99, 0x44, 0xa8, 0xd, 0xe9, 0x81, 0x1a,
    0x3c, 0x7d, 0x48, 0xb6, 0xf0, 0x4, 0x98, 0x9f,
    0x56, 0x53, 0xfb, 0x8e, 0x54, 0x2, 0x46, 0x4,

    /* U+0041 "A" */
    0x0, 0xf, 0x18, 0x6, 0x68, 0x80, 0x6, 0xb6,
    0x2, 0x0, 0x28, 0x48, 0x40, 0x2, 0x76, 0x6d,
    0x88,

    /* U+0042 "B" */
    0x6c, 0xab, 0x40, 0x9a, 0x8c, 0x9, 0xae, 0xd0,
    0x9a, 0xd9, 0x9, 0xab, 0xa0,

    /* U+0043 "C" */
    0x8, 0x9a, 0x90, 0x5f, 0x9a, 0xc2, 0xd4, 0x0,
    0x29, 0x6a, 0x0, 0x18, 0xd7, 0xe6, 0xbc, 0xc0,

    /* U+0044 "D" */
    0x5c, 0xab, 0x70, 0x5, 0x52, 0xf4, 0x80, 0x36,
    0x38, 0x6, 0xc7, 0xa, 0xab, 0x48,

    /* U+0045 "E" */
    0x5c, 0xaa, 0x40, 0x55, 0x50, 0x15, 0x52, 0x85,
    0x54, 0xa1, 0x55, 0x48,

    /* U+0046 "F" */
    0x5c, 0xaa, 0x28, 0x55, 0x4a, 0x15, 0x50, 0x2,
    0xaa, 0x0, 0xf0,

    /* U+0047 "G" */
    0x8, 0xaa, 0x89, 0x42, 0xa9, 0x63, 0xb8, 0xf,
    0x75, 0xba, 0x7, 0xa2, 0x5e, 0xba, 0xa7, 0x80,

    /* U+0048 "H" */
    0x56, 0x0, 0x51, 0x0, 0x7d, 0x55, 0x38, 0x2,
    0xaa, 0x70, 0xf, 0x80,

    /* U+0049 "I" */
    0x48, 0x0, 0xfc,

    /* U+004A "J" */
    0x0, 0x32, 0x80, 0x7e, 0x60, 0xd, 0x7a, 0x80,

    /* U+004B "K" */
    0x65, 0x6, 0x90, 0xa, 0x6, 0x40, 0x10, 0x68,
    0x1, 0x5d, 0xa0, 0x80, 0x6, 0x13, 0xc2,

    /* U+004C "L" */
    0x65, 0x0, 0xff, 0xe3, 0xcd, 0x50, 0x40,

    /* U+004D "M" */
    0x6e, 0x0, 0x17, 0x88, 0x22, 0x85, 0x20, 0x3,
    0xfc, 0x2, 0x20, 0x8, 0x9b, 0x40, 0x35, 0xd9,
    0x0, 0x0,

    /* U+004E "N" */
    0x5c, 0x0, 0x49, 0x3, 0x40, 0x6, 0xf0, 0x50,
    0xd, 0x1c, 0x40, 0x1a, 0x94, 0x0,

    /* U+004F "O" */
    0x8, 0xa9, 0x90, 0xaf, 0xd4, 0xd4, 0x62, 0x0,
    0xd, 0x7d, 0x0, 0x6, 0xad, 0xf5, 0x35, 0x20,

    /* U+0050 "P" */
    0x6c, 0xaa, 0x40, 0x55, 0x48, 0x15, 0x35, 0x21,
    0x53, 0x4a, 0x1, 0xc0,

    /* U+0051 "Q" */
    0x8, 0xa9, 0x90, 0xb7, 0xd4, 0xd4, 0x7a, 0x0,
    0xd, 0x71, 0x1, 0xd, 0x55, 0xf5, 0x1f, 0xe0,
    0x8a, 0xcc, 0x38,

    /* U+0052 "R" */
    0x5c, 0x99, 0x50, 0x2, 0xa6, 0x40, 0x41, 0x54,
    0xcc, 0x10, 0x55, 0x5c, 0x1, 0x98, 0x94,

    /* U+0053 "S" */
    0x1b, 0xaa, 0x22, 0x32, 0xa8, 0x2a, 0x53, 0x26,
    0x63, 0xcc, 0xbd, 0x26, 0xae, 0x80,

    /* U+0054 "T" */
    0x8a, 0xea, 0x78, 0xa2, 0xa7, 0x0, 0xff, 0xe1,
    0x80,

    /* U+0055 "U" */
    0x56, 0x0, 0x51, 0x0, 0x7f, 0xf0, 0x7, 0x80,
    0xc, 0x68, 0x55, 0x4b, 0x10,

    /* U+0056 "V" */
    0xb1, 0x0, 0x50, 0xfb, 0x1, 0x18, 0xa6, 0x5,
    0x58, 0x3, 0x54, 0x88, 0x0, 0x68, 0xe0, 0x0,

    /* U+0057 "W" */
    0xb0, 0x6, 0x30, 0x3a, 0x61, 0x8a, 0x60, 0x6a,
    0x2a, 0x3d, 0x80, 0x80, 0xb, 0xbc, 0xfe, 0x3c,
    0x1, 0xdc, 0x5, 0x74, 0x0,

    /* U+0058 "X" */
    0x49, 0x2, 0xa0, 0x4e, 0x78, 0xa0, 0x3, 0xcf,
    0x80, 0x55, 0x4e, 0x20, 0x8f, 0x49, 0xe0,

    /* U+0059 "Y" */
    0x95, 0x1, 0xb0, 0x9e, 0x2b, 0x90, 0x5, 0x7f,
    0x88, 0x0, 0x22, 0x50, 0xf, 0xc0,

    /* U+005A "Z" */
    0x5a, 0xa6, 0xc2, 0xd4, 0xbc, 0x1, 0xc5, 0x1,
    0x45, 0x80, 0x38, 0x2a, 0x8c,

    /* U+005B "[" */
    0x7a, 0x0, 0x70, 0x7, 0xfd, 0xc0,

    /* U+005C "\\" */
    0x90, 0x0, 0x80, 0x2d, 0x40, 0xf4, 0x3, 0xac,
    0x40,

    /* U+005D "]" */
    0x7b, 0x70, 0xf, 0x9c, 0x0,

    /* U+005E "^" */
    0x9, 0x60, 0x11, 0x60, 0x4d, 0x62, 0x80,

    /* U+005F "_" */
    0x6, 0x6c, 0x0,

    /* U+0060 "`" */
    0x47, 0x46,

    /* U+0061 "a" */
    0x3a, 0x99, 0x0, 0xce, 0x28, 0x55, 0xb8, 0x3,
    0x85, 0x44, 0x0,

    /* U+0062 "b" */
    0x73, 0x0, 0xd3, 0x10, 0x0, 0x74, 0x10, 0x80,
    0x61, 0xe, 0x9d, 0x0,

    /* U+0063 "c" */
    0x2a, 0x96, 0xbb, 0x4a, 0x80, 0x4d, 0x76, 0x93,

    /* U+0064 "d" */
    0x0, 0xac, 0xea, 0x1e, 0x6e, 0x1c, 0xc0, 0x2b,
    0x87, 0x70,

    /* U+0065 "e" */
    0x29, 0x87, 0xb, 0x0, 0x68, 0x85, 0x46, 0xd,
    0xcc, 0xb8, 0x0,

    /* U+0066 "f" */
    0x3c, 0x49, 0x75, 0xfd, 0x10, 0xf, 0x0,

    /* U+0067 "g" */
    0x2a, 0x8c, 0x8b, 0x80, 0xe, 0x8b, 0x81, 0x62,
    0xa,

    /* U+0068 "h" */
    0x73, 0x0, 0xa6, 0x50, 0x1d, 0x26, 0xe, 0x1,
    0xe0,

    /* U+0069 "i" */
    0x52, 0x20, 0x80, 0x78,

    /* U+006A "j" */
    0x5, 0x20, 0x21, 0x0, 0xfe, 0x2b, 0x20,

    /* U+006B "k" */
    0x73, 0x0, 0xd2, 0x81, 0xfe, 0x40, 0x9d, 0x0,
    0x34, 0x48,

    /* U+006C "l" */
    0x73, 0x0, 0xfc,

    /* U+006D "m" */
    0x79, 0x99, 0xec, 0x1, 0xb2, 0x4e, 0x64, 0xe,
    0x1, 0xff, 0xc0,

    /* U+006E "n" */
    0x79, 0x88, 0x6, 0xc1, 0x80, 0x7e,

    /* U+006F "o" */
    0x2a, 0x97, 0x8, 0xb9, 0xd1, 0x0, 0xe8, 0xb9,
    0xd1,

    /* U+0070 "p" */
    0x7a, 0x88, 0x0, 0x3a, 0x8, 0x40, 0x30, 0x87,
    0x4e, 0x80, 0x3a, 0x5c, 0x0,

    /* U+0071 "q" */
    0x2a, 0x8c, 0xbb, 0x43, 0x81, 0x80, 0x2e, 0x20,
    0xc5, 0x50, 0xc0,

    /* U+0072 "r" */
    0x7a, 0x60, 0xe6, 0x7, 0x0, 0xc0,

    /* U+0073 "s" */
    0x59, 0xa3, 0x20, 0xe2, 0x4b, 0x39, 0x68, 0xdd,
    0x0,

    /* U+0074 "t" */
    0x1, 0x5, 0x40, 0xce, 0xc, 0xe0, 0xc, 0xdc,
    0x20,

    /* U+0075 "u" */
    0x73, 0xb, 0x0, 0xf9, 0xd6, 0x5c, 0x0,

    /* U+0076 "v" */
    0xa0, 0x5, 0x73, 0x33, 0xd3, 0x30, 0xa0, 0x44,
    0x0,

    /* U+0077 "w" */
    0xb0, 0x96, 0x19, 0xc3, 0x36, 0x3d, 0x2c, 0xe8,
    0x61, 0x93, 0xae, 0x98, 0x0,

    /* U+0078 "x" */
    0x76, 0x57, 0x76, 0x7b, 0x80, 0x88, 0x27, 0xfd,
    0x20,

    /* U+0079 "y" */
    0xa1, 0xa, 0x99, 0x37, 0x19, 0x62, 0x6, 0x60,
    0x17, 0x58, 0x0,

    /* U+007A "z" */
    0x68, 0xb9, 0x6a, 0x19, 0x19, 0xa0, 0xa1, 0x97,

    /* U+007B "{" */
    0xb, 0x40, 0x14, 0xae, 0xa, 0xd0, 0x1, 0x80,
    0x48,

    /* U+007C "|" */
    0x45, 0x0, 0xff, 0x0,

    /* U+007D "}" */
    0x87, 0x8, 0xd0, 0x1, 0xa8, 0x1a, 0x80, 0x51,
    0x80,

    /* U+007E "~" */
    0x5b, 0x73, 0x30, 0xd8, 0x41, 0x0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 36, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 0, .adv_w = 36, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 5, .adv_w = 45, .box_w = 3, .box_h = 2, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 8, .adv_w = 71, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 21, .adv_w = 71, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 39, .adv_w = 114, .box_w = 7, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 57, .adv_w = 85, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 76, .adv_w = 24, .box_w = 2, .box_h = 2, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 78, .adv_w = 43, .box_w = 3, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 87, .adv_w = 43, .box_w = 3, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 96, .adv_w = 50, .box_w = 3, .box_h = 3, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 101, .adv_w = 75, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 111, .adv_w = 36, .box_w = 2, .box_h = 3, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 114, .adv_w = 43, .box_w = 3, .box_h = 1, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 116, .adv_w = 36, .box_w = 2, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 117, .adv_w = 36, .box_w = 3, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 127, .adv_w = 71, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 139, .adv_w = 71, .box_w = 3, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 146, .adv_w = 71, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 159, .adv_w = 71, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 172, .adv_w = 71, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 185, .adv_w = 71, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 198, .adv_w = 71, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 211, .adv_w = 71, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 224, .adv_w = 71, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 237, .adv_w = 71, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 250, .adv_w = 36, .box_w = 2, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 255, .adv_w = 36, .box_w = 2, .box_h = 6, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 262, .adv_w = 75, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 275, .adv_w = 75, .box_w = 5, .box_h = 2, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 280, .adv_w = 75, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 293, .adv_w = 71, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 306, .adv_w = 130, .box_w = 8, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 330, .adv_w = 85, .box_w = 7, .box_h = 5, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 347, .adv_w = 85, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 360, .adv_w = 92, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 376, .adv_w = 92, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 390, .adv_w = 85, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 402, .adv_w = 78, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 413, .adv_w = 100, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 429, .adv_w = 92, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 441, .adv_w = 36, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 444, .adv_w = 64, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 452, .adv_w = 85, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 467, .adv_w = 71, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 474, .adv_w = 107, .box_w = 7, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 492, .adv_w = 92, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 506, .adv_w = 100, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 522, .adv_w = 85, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 534, .adv_w = 100, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 553, .adv_w = 92, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 568, .adv_w = 85, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 582, .adv_w = 78, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 591, .adv_w = 92, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 604, .adv_w = 85, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 620, .adv_w = 121, .box_w = 8, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 641, .adv_w = 85, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 656, .adv_w = 85, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 670, .adv_w = 78, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 683, .adv_w = 36, .box_w = 3, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 689, .adv_w = 36, .box_w = 3, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 698, .adv_w = 36, .box_w = 2, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 703, .adv_w = 60, .box_w = 4, .box_h = 3, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 710, .adv_w = 64, .box_w = 6, .box_h = 1, .ofs_x = -1, .ofs_y = -1},
    {.bitmap_index = 713, .adv_w = 43, .box_w = 2, .box_h = 2, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 715, .adv_w = 71, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 726, .adv_w = 71, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 738, .adv_w = 64, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 746, .adv_w = 71, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 756, .adv_w = 71, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 767, .adv_w = 36, .box_w = 3, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 774, .adv_w = 71, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 783, .adv_w = 71, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 792, .adv_w = 28, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 796, .adv_w = 28, .box_w = 3, .box_h = 6, .ofs_x = -1, .ofs_y = -1},
    {.bitmap_index = 803, .adv_w = 64, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 813, .adv_w = 28, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 816, .adv_w = 107, .box_w = 7, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 827, .adv_w = 71, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 833, .adv_w = 71, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 842, .adv_w = 71, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 855, .adv_w = 71, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 866, .adv_w = 43, .box_w = 3, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 872, .adv_w = 64, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 881, .adv_w = 36, .box_w = 3, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 890, .adv_w = 71, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 897, .adv_w = 64, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 906, .adv_w = 92, .box_w = 6, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 919, .adv_w = 64, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 928, .adv_w = 64, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 939, .adv_w = 64, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 947, .adv_w = 43, .box_w = 3, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 956, .adv_w = 33, .box_w = 2, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 960, .adv_w = 43, .box_w = 3, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 969, .adv_w = 75, .box_w = 5, .box_h = 2, .ofs_x = 0, .ofs_y = 2}
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
const lv_font_t ArialUni8 = {
#else
lv_font_t ArialUni8 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 8,          /*The maximum line height required by the font*/
    .base_line = 2,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0)
    .underline_position = -1,
    .underline_thickness = 0,
#endif
    .dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};



#endif /*#if ARIALUNI8*/

