#ifndef __GII_TEST_UTILS_LOG_COLOR_H__
#define __GII_TEST_UTILS_LOG_COLOR_H__

#include <stdbool.h>

#define GTU_LOG_COLOR_FLAG_SHIFT (0)
#define GTU_LOG_COLOR_FLAG_MASK  (0xFF)

#define GTU_LOG_COLOR_FG_SHIFT   (GTU_LOG_COLOR_FLAG_SHIFT + 8)
#define GTU_LOG_COLOR_FG_MASK    (0xF << GTU_LOG_COLOR_FG_SHIFT)

#define GTU_LOG_COLOR_BG_SHIFT   (GTU_LOG_COLOR_FG_SHIFT + 4)
#define GTU_LOG_COLOR_BG_MASK    (0xF << GTU_LOG_COLOR_BG_SHIFT)

typedef enum {
  GTU_LOG_COLOR_DISABLE         = 0,

  GTU_LOG_COLOR_FLAG_BOLD       = 1 << 0,
  GTU_LOG_COLOR_FLAG_UNDERSCORE = 1 << 1,
  GTU_LOG_COLOR_FLAG_BLINK      = 1 << 2,

  GTU_LOG_COLOR_FG_BLACK   = 1 << GTU_LOG_COLOR_FG_SHIFT,
  GTU_LOG_COLOR_FG_RED     = 2 << GTU_LOG_COLOR_FG_SHIFT,
  GTU_LOG_COLOR_FG_GREEN   = 3 << GTU_LOG_COLOR_FG_SHIFT,
  GTU_LOG_COLOR_FG_YELLOW  = 4 << GTU_LOG_COLOR_FG_SHIFT,
  GTU_LOG_COLOR_FG_BLUE    = 5 << GTU_LOG_COLOR_FG_SHIFT,
  GTU_LOG_COLOR_FG_MAGENTA = 6 << GTU_LOG_COLOR_FG_SHIFT,
  GTU_LOG_COLOR_FG_CYAN    = 7 << GTU_LOG_COLOR_FG_SHIFT,
  GTU_LOG_COLOR_FG_WHITE   = 8 << GTU_LOG_COLOR_FG_SHIFT,

  GTU_LOG_COLOR_BG_BLACK   = 1 << GTU_LOG_COLOR_BG_SHIFT,
  GTU_LOG_COLOR_BG_RED     = 2 << GTU_LOG_COLOR_BG_SHIFT,
  GTU_LOG_COLOR_BG_GREEN   = 3 << GTU_LOG_COLOR_BG_SHIFT,
  GTU_LOG_COLOR_BG_YELLOW  = 4 << GTU_LOG_COLOR_BG_SHIFT,
  GTU_LOG_COLOR_BG_BLUE    = 5 << GTU_LOG_COLOR_BG_SHIFT,
  GTU_LOG_COLOR_BG_MAGENTA = 6 << GTU_LOG_COLOR_BG_SHIFT,
  GTU_LOG_COLOR_BG_CYAN    = 7 << GTU_LOG_COLOR_BG_SHIFT,
  GTU_LOG_COLOR_BG_WHITE   = 8 << GTU_LOG_COLOR_BG_SHIFT
} GtuLogColor;

/**
 * gtu_log_supports_color:
 *
 * Returns %TRUE if the output log supports ANSI colour escapes (e.g., it's a
 * terminal), %FALSE otherwise.
 */
bool gtu_log_supports_color (void);

/**
 * gtu_log_lookup_color:
 * @color: colour value to look up.
 *
 * Gets the ANSI escape sequence to produce @color.
 *
 * Returns: (transfer none): ANSI escape string.
 */
const char* gtu_log_lookup_color (GtuLogColor color);

#endif
