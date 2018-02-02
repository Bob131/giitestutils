#include <string.h>
#include <setjmp.h>
#include "test-case/priv.h"

/* lazy sanity checking */
static const uint32_t TR_MAGIC = 0x7357CA5E;

typedef struct {
  uint32_t magic;
  jmp_buf caller_context;
  char* message;
  GtuTestResult result;
} TestRunContext;

static void _gtu_tr_context_check (const TestRunContext* tr_context) {
  g_assert (gtu_has_initialized ()        &&
            tr_context->magic == TR_MAGIC &&
            tr_context->message == NULL);
}

static TestRunContext* _current_tr_context = NULL;

#define CURRENT_CONTEXT_CHECK() _gtu_tr_context_check (_current_tr_context)
#define CURRENT_CONTEXT (CURRENT_CONTEXT_CHECK (), _current_tr_context)

#define PREEMPT_TEST() G_STMT_START {               \
  longjmp (_current_tr_context->caller_context, 1); \
  g_assert_not_reached ();                          \
} G_STMT_END


void _gtu_assertion_message (const char* file,
                             const char* line,
                             const char* function,
                             const char* message)
{
  char* location_message;
  CURRENT_CONTEXT_CHECK ();

  g_assert (file != NULL && line != NULL && function != NULL);
  if (message == NULL)
    message = "assert failed";

  location_message = g_strdup_printf ("%s:%s:%s: %s",
                                      file, line, function, message);

  if (_gtu_debug_flags_get () & GTU_DEBUG_FLAGS_FATAL_ASSERTS) {
    g_printerr ("**\nERROR:%s\n", location_message);
    abort ();
  }

  _current_tr_context->message = location_message;
  _current_tr_context->result = GTU_TEST_RESULT_FAIL;

  PREEMPT_TEST ();
}

void _gtu_skip_if_reached_message (const char* file,
                                   const char* line,
                                   const char* function,
                                   const char* message)
{
  CURRENT_CONTEXT_CHECK ();
  g_assert ((file != NULL && line != NULL && function != NULL) ||
            message != NULL);

  _current_tr_context->message = message != NULL ?
    g_strdup (message) :
    g_strdup_printf ("Check failed at %s:%s:%s", file, line, function);

  _current_tr_context->result = GTU_TEST_RESULT_SKIP;

  PREEMPT_TEST ();
}

GtuTestResult _gtu_test_case_exec_inner (GtuTestCaseFunc func,
                                         void* func_target,
                                         char** message)
{
  GtuTestResult ret;

  g_assert (func != NULL && message != NULL);
  g_assert (_current_tr_context == NULL);

  _current_tr_context = calloc (1, sizeof (TestRunContext));
  _current_tr_context->magic = TR_MAGIC;
  _current_tr_context->result = GTU_TEST_RESULT_PASS;

  if (!setjmp (_current_tr_context->caller_context))
    func (func_target);

  *message = _current_tr_context->message;
  ret = _current_tr_context->result;

  memset (_current_tr_context, 0, sizeof (TestRunContext));
  free (_current_tr_context);
  _current_tr_context = NULL;

  return ret;
}
