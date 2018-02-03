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
 * various project-specific testing utilities.
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
 *
 * Class for #GtuTestCase objects.
 */
struct _GtuTestCaseClass {
  /*< private >*/
  GtuTestObjectClass parent_class;
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
 * @type:                subtype of %GTU_TYPE_TEST_CASE.
 * @name:                the name of the new test case.
 * @func:                function implementing a test.
 * @func_target:         (allow-none) (transfer full) (closure):
 *                       pointer to user data, passed to @func.
 * @func_target_destroy: (allow-none) (destroy): frees @func_target.
 *
 * Base constructor for types deriving from #GtuTestCase. For creating a plain
 * new test case, see gtu_test_case_new() instead.
 *
 * Returns: a floating reference to the new instance of @type.
 */
GtuTestCase* gtu_test_case_construct (GType type,
                                      const char* name,
                                      GtuTestCaseFunc func,
                                      void* func_target,
                                      GDestroyNotify func_target_destroy);

G_END_DECLS

#endif
