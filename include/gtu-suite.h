#ifndef __GII_TEST_UTILS_SUITE_H__
#define __GII_TEST_UTILS_SUITE_H__

/**
 * SECTION:gtu-suite
 * @short_description: organising collections of test cases
 * @title: Test Suites
 * @include: gtu.h
 *
 * #GtuTestSuite is the method with which projects making use of GTU can
 * organise and execute collections of #GtuTestCase. It is an opaque
 * ref-counted object that contains a tree of test cases exercising elements of
 * project functionality.
 *
 * Projects are free to subclass #GtuTestSuite, allowing a neat encapsulation
 * of various project-specific testing utilities.
 */

#ifndef __GII_TEST_UTILS_H__
#error "Only <gtu.h> can be included directly."
#endif

G_BEGIN_DECLS

/**
 * GTU_TYPE_TEST_SUITE:
 *
 * #GType for #GtuTestSuite objects.
 */
#define GTU_TYPE_TEST_SUITE (gtu_test_suite_get_type ())
G_DECLARE_DERIVABLE_TYPE (GtuTestSuite, gtu_test_suite, GTU, TEST_SUITE,
                          GtuTestObject)

/**
 * GtuTestSuite:
 *
 * A derivable object containing a collection of #GtuTestCase to be executed as
 * part of a project's test suite.
 */

/**
 * GtuTestSuiteClass:
 *
 * Class for #GtuTestSuite objects.
 */
struct _GtuTestSuiteClass {
  /*< private >*/
  GtuTestObjectClass parent_class;
};

/**
 * gtu_test_suite_new:
 * @name: identifier to use as the root name for all children tests of `this`.
 *
 * Creates a new test suite object and returns a floating reference to it. This
 * reference is sunk by the first call to gtu_test_object_ref().
 *
 * @name must not be %NULL and must be a valid #GtuPath element as per
 * #Validity.
 *
 * Returns: a floating reference to the new #GtuTestSuite.
 */
GtuTestSuite* gtu_test_suite_new (const char* name);

/**
 * gtu_test_suite_construct:
 * @type: subtype of %GTU_TYPE_TEST_SUITE.
 * @name: identifier to use as the root name for all children tests of `this`.
 *
 * Base constructor for types deriving from #GtuTestSuite. For creating a plain
 * new test suite, see gtu_test_suite_new() instead.
 *
 * Returns: a floating reference to the new instance of @type.
 */
GtuTestSuite* gtu_test_suite_construct (GType type, const char* name);

/**
 * gtu_test_suite_add:
 * @self:        test suite to which @test_object will be added.
 * @test_object: a test case or suite to become a child of @self.
 *
 * Adds @test_object to @self, such that any tests attached to @test_object
 * will be executed as a part of @self when gtu_test_suite_run() is called. The
 * tests will be given a path resulting from the concatenation of @self's
 * parents' names, @self's name, and the name of the tests themselves.
 *
 * It is invalid to call this function with a @test_object parameter already
 * belonging to a test suite (i.e., if it has been passed to this function
 * already).
 *
 * This function takes a new reference to @test_object, so you needn't worry
 * about holding a reference to @test_object after this function returns.
 */
void gtu_test_suite_add (GtuTestSuite* self, GtuTestObject* test_object);

/**
 * gtu_test_suite_add_obj:
 * @self: test suite instance
 * @obj:  test object instance
 *
 * Convenience macro that casts @obj to #GtuTestObject before calling
 * gtu_test_suite_add().
 */
#define gtu_test_suite_add_obj(self, obj) \
  (gtu_test_suite_add ((self), GTU_TEST_OBJECT ((obj))))

/**
 * gtu_test_suite_run:
 * @self: (transfer full): test suite to be executed.
 *
 * Executes all the tests attached to @self (and #GtuTestSuite children),
 * taking care to communicate the test results and log messages to the test
 * harness.
 *
 * This function may only be called once throughout a program's execution,
 * since many test harnesses require a test plan up-front; as such, any tests
 * that should be run must be added to @self before this function is called.
 *
 * The @self parameter should be an owned reference to the test suite. It's
 * recommended that you drop all references to @self and its children before
 * calling this function, to allow for post-run clean up.
 *
 * Returns: an exit value that should be returned from `main()`, signalling the
 *          overall result to the test runner.
 */
int gtu_test_suite_run (GtuTestSuite* self);

G_END_DECLS

#endif
