#include "logio.h"

#include "priv.h"
#include "log-glib.h"
#include "log-color.h"

#include "gtu.h"

static GtuLogColor level_color (GLogLevelFlags level) {
  GtuLogColor color = GTU_LOG_COLOR_FLAG_BOLD;

  switch (level & G_LOG_LEVEL_MASK) {
    case G_LOG_LEVEL_ERROR:     color |= GTU_LOG_COLOR_FG_RED;     break;
    case G_LOG_LEVEL_CRITICAL:  color |= GTU_LOG_COLOR_FG_MAGENTA; break;
    case G_LOG_LEVEL_WARNING:   color |= GTU_LOG_COLOR_FG_YELLOW;  break;
    case G_LOG_LEVEL_MESSAGE:   color |= GTU_LOG_COLOR_FG_GREEN;   break;
    case G_LOG_LEVEL_INFO:      color |= GTU_LOG_COLOR_FG_BLUE;    break;
    case G_LOG_LEVEL_DEBUG:     color |= GTU_LOG_COLOR_FG_CYAN;    break;

    default:
      g_assert_not_reached ();
  }

  return color;
}

static const char* level_to_string (GLogLevelFlags log_level) {
  switch (log_level & G_LOG_LEVEL_MASK) {
    case G_LOG_LEVEL_ERROR:     return "ERROR";
    case G_LOG_LEVEL_CRITICAL:  return "CRITICAL";
    case G_LOG_LEVEL_WARNING:   return "WARNING";
    case G_LOG_LEVEL_MESSAGE:   return "MESSAGE";
    case G_LOG_LEVEL_INFO:      return "INFO";
    case G_LOG_LEVEL_DEBUG:     return "DEBUG";

    default:
      g_assert_not_reached ();
  }
}

void gtu_log_g_format_message_append (GString* string,
                                      const char* domain,
                                      GLogLevelFlags level,
                                      const char* message)
{
  const char *fatal_color_begin = "",
             *fatal_level = "",
             *fatal_color_end = "",
             *fatal_sep = "",
             *domain_head = "",
             *domain_str  = "",
             *domain_tail = "";

  if (level & G_LOG_FLAG_FATAL) {
    fatal_color_begin = gtu_log_lookup_color (level_color (G_LOG_LEVEL_ERROR));
    fatal_level = "FATAL";
    fatal_color_end = gtu_log_lookup_color (GTU_LOG_COLOR_DISABLE);
    fatal_sep = "-";
  }

  if (domain != NULL && domain[0] != '\0') {
    domain_head = "(**";
    domain_str = domain;
    domain_tail = ") ";
  }

  g_string_append_printf (
    string,
    "%s%s%s%s%s%s%s: %s%s%s%s",
    fatal_color_begin,
    fatal_level,
    fatal_color_end,
    fatal_sep,

    gtu_log_lookup_color (level_color (level)),
    level_to_string (level),
    gtu_log_lookup_color (GTU_LOG_COLOR_DISABLE),

    domain_head,
    domain_str,
    domain_tail,

    message
  );
}

char* gtu_log_g_format_message (const char* domain,
                                GLogLevelFlags level,
                                const char* message)
{
  GString* ret = g_string_new (NULL);
  gtu_log_g_format_message_append (ret, domain, level, message);
  return g_string_free (ret, false);
}
