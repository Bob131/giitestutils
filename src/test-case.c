#include <string.h>
#include <setjmp.h>
#include "gtu-priv.h"

/* lazy sanity checking */
static const uint32_t TC_CONTEXT_MAGIC = 0x7357CA5E;

typedef struct {
  uint32_t magic;
  GtuTestFunc func;
  void* func_target;
  GDestroyNotify func_target_destroy;
  jmp_buf fixture_context;
} TestCaseContext;

static void _gtu_tc_context_check (const TestCaseContext* tc_context) {
  g_assert (gtu_has_initialized ()                  &&
            tc_context != NULL                      &&
            tc_context->magic == TC_CONTEXT_MAGIC   &&
            tc_context->func != NULL);
}


static TestCaseContext* _current_tc_context = NULL;

#define CURRENT_CONTEXT_CHECK() _gtu_tc_context_check (_current_tc_context)
#define CURRENT_CONTEXT (CURRENT_CONTEXT_CHECK (), _current_tc_context)

#define PREEMPT_TEST() G_STMT_START {            \
  longjmp (CURRENT_CONTEXT->fixture_context, 1); \
  g_assert_not_reached ();                       \
} G_STMT_END


void _gtu_assertion_message (const char* file,
                             const char* line,
                             const char* function,
                             const char* message)
{
  CURRENT_CONTEXT_CHECK ();

  g_assert (file != NULL && line != NULL && function != NULL);
  if (message == NULL)
    message = "assert failed";

  g_printerr ("**\nERROR:%s:%s:%s: %s\n", file, line, function, message);

  if (_gtu_get_debug_flags () & GTU_DEBUG_FATAL_ASSERTS)
    abort ();

  g_test_fail ();
  PREEMPT_TEST ();
}

void gtu_skip_if_reached (const char* message) {
  CURRENT_CONTEXT_CHECK ();
  g_test_skip (message != NULL ? message : "Test skipped");
  PREEMPT_TEST ();
}


static void _gtu_test_fixture_setup (void* fixture,
                                     const void* context_pointer)
{
  TestCaseContext* context = (TestCaseContext*) context_pointer;
  (void) fixture;

  _gtu_tc_context_check (context);
  g_assert (_current_tc_context == NULL);
  _current_tc_context = context;
}

static void _gtu_test_fixture_run (void* fixture,
                                   const void* context_pointer)
{
  TestCaseContext* context = (TestCaseContext*) context_pointer;
  (void) fixture;

  g_assert (CURRENT_CONTEXT == context);

  if (!setjmp (context->fixture_context))
    context->func (context->func_target);
}

static void _gtu_test_fixture_teardown (void* fixture,
                                        const void* context_pointer)
{
  TestCaseContext* context = (TestCaseContext*) context_pointer;
  (void) fixture;

  g_assert (CURRENT_CONTEXT == context);

  if (context->func_target_destroy != NULL)
    context->func_target_destroy (context->func_target);

  memset (context, 0, sizeof (TestCaseContext));
  free (context);

  _current_tc_context = NULL;
}


GTestCase* gtu_create_test_case (const char* name,
                                 GtuTestFunc func,
                                 void* func_target,
                                 GDestroyNotify func_target_destroy)
{
  TestCaseContext* context;

  g_return_val_if_fail (gtu_has_initialized () && name != NULL && func != NULL,
                        NULL);

  context = calloc (1, sizeof (TestCaseContext));
  context->magic = TC_CONTEXT_MAGIC;
  context->func = func;
  context->func_target = func_target;
  context->func_target_destroy = func_target_destroy;

  return g_test_create_case (name, 0, context,
                             _gtu_test_fixture_setup,
                             _gtu_test_fixture_run,
                             _gtu_test_fixture_teardown);
}
