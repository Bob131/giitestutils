#include <string.h>
#include <stdio.h>
#include "gtu-priv.h"
#include "log.h"

#ifdef HAVE_ALLOCA_H
# include <alloca.h>
#elif defined __GNUC__
# define alloca __builtin_alloca
#elif defined _AIX
# define alloca __alloca
#elif defined _MSC_VER
# include <malloc.h>
# define alloca _alloca
#else
# include <stddef.h>
void *alloca (size_t);
#endif

#define GTU_DEBUG "GTU_DEBUG"

static const GDebugKey _debug_keys[] = {
  { "fatal-asserts", GTU_DEBUG_FLAGS_FATAL_ASSERTS }
};

static GtuDebugFlags _debug_flags = GTU_DEBUG_FLAGS_NONE;
static bool _has_initialized = false;

G_GNUC_INTERNAL GtuDebugFlags _gtu_debug_flags_get (void) {
  g_assert (_has_initialized);
  return _debug_flags;
}

bool gtu_has_initialized (void) {
  return _has_initialized;
}

static GtuTestMode _test_mode = {
  NULL, /* path_selectors */
  NULL, /* path_skippers */
  false /* list_only */
};

GtuTestMode* _gtu_get_test_mode (void) {
  g_assert (_has_initialized);
  return &_test_mode;
}

static char* get_arg (char** args, int args_length, int i, const char* arg) {
  if (strncmp (args[i], arg, strlen (arg)) != 0)
    return NULL;

  if (g_str_has_suffix (arg, "=")) {
    char* ret = &args[i][strlen (arg)];
    if (strlen (ret) > 0)
      return ret;
    return NULL;
  }

  if (strlen (args[i]) != strlen (arg) || i == args_length - 1)
    return NULL;

  return args[i + 1];
}

#define GET_ARG(argstr) \
  (get_arg (args, args_length, i, argstr) ? \
   get_arg (args, args_length, i, argstr) : \
   get_arg (args, args_length, i, argstr "="))

#define CHECK_PATH(arg) G_STMT_START {                                       \
  if (arg_path == NULL) {                                                    \
    fprintf (stderr, "Error: invalid path specification:\n");                \
    fprintf (stderr, "    %s\n", GET_ARG (arg));                             \
    fprintf (stderr, "    %*s^\n", (int) (endptr - GET_ARG (arg)) - 1, " "); \
    exit (1);                                                                \
  }                                                                          \
} G_STMT_END

static bool parse_args (char** args, int args_length) {
  int i;
  bool keep_going_set = false;
  bool tap_set = false;

  for (i = 0; i < args_length; i++) {
    if (strcmp (args[i], "--keep-going") == 0 ||
        strcmp (args[i], "-k") == 0)
    {
      keep_going_set = true;

    } else if (strcmp (args[i], "--tap") == 0) {
      tap_set = true;

    } else if (GET_ARG ("-p")) {
      char* endptr;
      GtuPath* arg_path = gtu_path_new_parse (GET_ARG ("-s"), &endptr);

      CHECK_PATH ("-p");

      _test_mode.path_selectors = g_list_prepend (
        _test_mode.path_selectors, arg_path
      );

    } else if (GET_ARG ("-s")) {
      char* endptr;
      GtuPath* arg_path = gtu_path_new_parse (GET_ARG ("-s"), &endptr);

      CHECK_PATH ("-s");

      _test_mode.path_skippers = g_list_prepend (
        _test_mode.path_skippers, arg_path
      );

    } else if (strcmp (args[i], "-l") == 0) {
      _test_mode.list_only = true;

    } else if (GET_ARG ("--GTestLogFD")     ||
               GET_ARG ("--GTestSkipCount") ||
               strcmp  ("--GTestSubprocess", args[i]) == 0)
    {
      g_log (GTU_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
             "unsupported command line argument: %s", args[i]);
    }
  }

  if (!keep_going_set)
    _debug_flags |= GTU_DEBUG_FLAGS_FATAL_ASSERTS;

  /* this shouldn't necessarily be fatal, so just printf TAP-style */
  if (!tap_set && !_test_mode.list_only)
    fprintf (stdout, "# WARNING: non-TAP test logging is unsupported. %s\n",
             "Run with --tap");

  return tap_set;
}

void gtu_init (char** args, int args_length) {
  g_return_if_fail (args != NULL && args_length > 0);

  /* something has gone wrong if either is initialised without the other */
  g_assert (g_test_initialized () == _has_initialized);

  if (!g_test_initialized ()) {
    bool tap_set = parse_args (args, args_length);

    int i;
    int temp_args_length = args_length + (tap_set ? 0 : 1);
    char** temp_args = alloca (temp_args_length * sizeof (char*));

    memcpy (temp_args, args, args_length * sizeof (char*));

    /* GLib seems to think that -p and --tap are mutually exclusive, so we hack
       around it. */
    for (i = 0; i < args_length; i++) {
      if (GET_ARG ("-p")) {
        temp_args[i] = "";
        temp_args[i + 1] = "";
      }
    }

    /* ensure GLib gets --tap */
    if (!tap_set)
      temp_args[temp_args_length - 1] = "--tap";

    g_test_init (&temp_args_length, &temp_args, NULL);
  }

  if (!_has_initialized) {
    /* or-eq so we don't overwrite the -k argument */
    _debug_flags |= g_parse_debug_string (
        getenv (GTU_DEBUG),
        _debug_keys,
        G_N_ELEMENTS (_debug_keys)
    );

    if (!(_debug_flags & GTU_DEBUG_FLAGS_FATAL_ASSERTS))
      g_test_set_nonfatal_assertions ();

    _gtu_install_glib_loggers ();

    _has_initialized = true;
  }
}
