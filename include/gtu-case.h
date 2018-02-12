#ifndef __GII_TEST_UTILS_CASE_H__
#define __GII_TEST_UTILS_CASE_H__

/**
 * SECTION:gtu-case
 * @short_description: creating new test cases
 * @title: Test Cases
 * @include: gtu.h
 *
 * #GtuTestCase is the method with which projects making use of GTU can
 * implement test cases. It is an opaque ref-counted object that contains a
 * representation of an individual test case and its state.
 *
 * Projects are free to subclass #GtuTestCase, allowing a neat encapsulation of
 * various project-specific testing utilities. Derivative subclasses should
 * override #GtuTestCaseClass.test_impl.
 */

#ifndef __GII_TEST_UTILS_H__
#error "Only <gtu.h> can be included directly."
#endif

G_BEGIN_DECLS

/**
 * GTU_TYPE_TEST_CASE:
 *
 * #GType for #GtuTestCase objects.
 */
#define GTU_TYPE_TEST_CASE (gtu_test_case_get_type ())
G_DECLARE_DERIVABLE_TYPE (GtuTestCase, gtu_test_case, GTU, TEST_CASE,
                          GtuTestObject)

/**
 * GtuTestCase:
 *
 * A derivable object containing a test case to be executed as part of a
 * project's test suite.
 */

/**
 * GtuTestCaseClass:
 * @test_impl: function implementing the test case. This is only relevant for
 *             types deriving from #GtuTestCase, which should override this
 *             method; otherwise, use gtu_test_case_new().
 *
 * Class for #GtuTestCase objects.
 */
struct _GtuTestCaseClass {
  /*< private >*/
  GtuTestObjectClass parent_class;
  /*< public >*/
  void (*test_impl) (GtuTestCase* self);
};

/**
 * GtuTestCaseFunc:
 * @target: (closure): pointer to user data.
 *
 * A user-supplied function within which a test case is implemented.
 */
typedef void (*GtuTestCaseFunc) (void* target);

/**
 * gtu_test_case_new:
 * @name:                the name of the new test case.
 * @func:                function implementing a test.
 * @func_target:         (allow-none) (transfer full) (closure):
 *                       pointer to user data, passed to @func.
 * @func_target_destroy: (allow-none) (destroy): frees @func_target.
 *
 * Creates a new #GtuTestCase with the given @name and a function implementing
 * the test.
 *
 * @name must not be %NULL and must be a valid #GtuPath element as per
 * #Validity.
 *
 * Returns: a floating reference to the new #GtuTestCase.
 */
GtuTestCase* gtu_test_case_new (const char* name,
                                GtuTestCaseFunc func,
                                void* func_target,
                                GDestroyNotify func_target_destroy);

/**
 * gtu_test_case_construct:
 * @type: subtype of %GTU_TYPE_TEST_CASE.
 * @name: the name of the new test case.
 *
 * Base constructor for types deriving from #GtuTestCase. For creating a plain
 * new test case, see gtu_test_case_new() instead.
 *
 * Returns: a floating reference to the new instance of @type.
 */
GtuTestCase* gtu_test_case_construct (GType type, const char* name);

/**
 * GtuExpectHandle:
 *
 * Handle representing a log match expression.
 */
typedef unsigned GtuExpectHandle;

/**
 * gtu_test_case_expect_message:
 * @self:   #GtuTestCase that's expected to generate a message.
 * @domain: log domain expecting message.
 * @level:  flags to check messages against.
 * @regex:  (transfer full): regular expression we expect the message will
 *          match.
 *
 * Indicates to the test runner that we're expecting a message from g_log() or
 * g_log_structured() matching the provided parameters. If a message is from
 * @domain, has a log level with bits in common with @level and has text
 * matching @regex, the message will be considered a match.
 *
 * Matching messages will be omitted from the test log, unless
 * %GTU_TEST_MODE_FLAGS_VERBOSE is set. If an otherwise fatal message matching
 * the parameters has been logged, the test suite won't be aborted but will
 * continue as normal instead.
 *
 * The @domain parameter mustn't be %NULL or empty; this mechanism does not
 * support matching messages from the default log domain. The result of
 * attempting to match messages from %GTU_LOG_DOMAIN is undefined.
 *
 * @level is treated as a mask. If any common bits between a message and @level
 * are set, the message may be a match.
 *
 * The contents of the message are checked against @regex. If the regex
 * matches, the message may be a match. This function takes the caller's
 * reference to @regex; you should not unref the object after the call
 * completes.
 *
 * Returns: a handle that can be used to query the match state.
 */
GtuExpectHandle gtu_test_case_expect_message (GtuTestCase* self,
                                              const char* domain,
                                              GLogLevelFlags level,
                                              GRegex* regex);

/**
 * gtu_test_case_expect_check:
 * @self:   a #GtuTestCase instance.
 * @handle: a #GtuExpectHandle whose status will be queried.
 *
 * Checks whether any messages have been logged that match @handle.
 *
 * See gtu_test_case_expect_message().
 *
 * Returns: %TRUE if a matching message has been logged, %FALSE otherwise.
 */
bool gtu_test_case_expect_check (GtuTestCase* self, GtuExpectHandle handle);

/**
 * gtu_test_case_expect_count:
 * @self:   a #GtuTestCase instance.
 * @handle: a #GtuExpectHandle whose status will be queried.
 *
 * Gets how many messages matching @handle have been logged and resets the
 * counter.
 *
 * Returns: the number of logged messages matching @handle.
 */
unsigned gtu_test_case_expect_count (GtuTestCase* self, GtuExpectHandle handle);

G_END_DECLS

#endif
