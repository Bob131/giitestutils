#include <string.h>
#include "gtu-priv.h"

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

G_GNUC_INTERNAL GtuDebugFlags _gtu_debug_flags_get (void) {
  return _debug_flags;
}

static bool _has_initialized = false;

bool gtu_has_initialized (void) {
  return _has_initialized;
}

void gtu_init (char** args, int args_length) {
  if (!g_test_initialized ()) {
    char** temp_args = alloca (args_length * sizeof (char*));
    memcpy (temp_args, args, args_length * sizeof (char*));
    g_test_init (&args_length, &temp_args, NULL);
  }

  if (!_has_initialized) {
    _debug_flags = g_parse_debug_string (
        getenv (GTU_DEBUG),
        _debug_keys,
        G_N_ELEMENTS (_debug_keys)
    );

    if (!(_debug_flags & GTU_DEBUG_FLAGS_FATAL_ASSERTS))
      g_test_set_nonfatal_assertions ();

    _has_initialized = true;
  }
}
