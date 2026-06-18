#ifndef LV_CONF_H
#define LV_CONF_H

#define LV_USE_OS   LV_OS_PTHREAD

#define LV_COLOR_DEPTH 16
#define LV_COLOR_16_SWAP 1

#define LV_DPI_DEF 160

#define LV_MEM_SIZE (128 * 1024)

#define LV_DRAW_BUF_STRIDE_ALIGN 1
#define LV_DRAW_BUF_ALIGN 4

#define LV_USE_LOG 1
#define LV_LOG_LEVEL LV_LOG_LEVEL_WARN
#define LV_LOG_PRINTF 1

#define LV_TICK_CUSTOM 1
#define LV_TICK_CUSTOM_INCLUDE <stdint.h>

#define LV_USE_LABEL 1
#define LV_USE_IMG 1
#define LV_USE_LIST 1
#define LV_USE_BAR 1
#define LV_USE_SLIDER 1
#define LV_USE_BTN 1
#define LV_USE_ARC 1
#define LV_USE_DROPDOWN 0
#define LV_USE_ROLLER 0
#define LV_USE_TABLE 0
#define LV_USE_CHART 0
#define LV_USE_CALENDAR 0
#define LV_USE_COLORWHEEL 0
#define LV_USE_KEYBOARD 0
#define LV_USE_TEXTAREA 0
#define LV_USE_SPINBOX 0
#define LV_USE_MENU 0

#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_22 1
#define LV_FONT_MONTSERRAT_28 1
#define LV_FONT_DEFAULT &lv_font_montserrat_16

#define LV_USE_THEME_DEFAULT 1
#define LV_THEME_DEFAULT_DARK 1

#define LV_LABEL_LONG_TXT_HINT 1

#define LV_USE_PNG 1
#define LV_USE_BMP 0
#define LV_USE_GIF 0

#endif
