#ifndef __GII_TEST_UTILS_COMPLEX_H__
#define __GII_TEST_UTILS_COMPLEX_H__

/**
 * SECTION:gtu-complex
 * @short_description: test cases with several parts
 * @title: Complex Test Cases
 * @include: gtu.h
 *
 * It is sometimes the case that several test cases are needed to properly
 * exercise some unit of functionality. To serve this use case, GTU has
 * "complex tests": single test cases which contain subunits; on the failure of
 * any individual subunit, the entire case is abandoned.
 *
 * Subunits are declared using #GEnumClass, of which the nicks are used to
 * identify subunits and values used for control flow. Subunits are guaranteed
 * to be executed in the order they're given when registered with GType, and
 * cannot be skipped individually using the command-line arguments.
 *
 * #GtuComplexCase is an abstract class, so it cannot be used directly. You
 * should derive from #GtuComplexCase in order to create new complex test
 * cases.
 */

#ifndef __GII_TEST_UTILS_H__
#error "Only <gtu.h> can be included directly."
#endif

G_BEGIN_DECLS

/**
 * GTU_TYPE_COMPLEX_CASE:
 *
 * #GType for #GtuComplexCase objects.
 */
#define GTU_TYPE_COMPLEX_CASE (gtu_complex_case_get_type ())
G_DECLARE_DERIVABLE_TYPE (GtuComplexCase, gtu_complex_case, GTU, COMPLEX_CASE,
                          GtuTestCase)

/**
 * GtuComplexCase:
 *
 * Abstract class containing test case subunits.
 */

/**
 * GtuComplexCaseClass:
 * @test_impl: implementation of the test. @subunit is a boxed enum value,
 *             which can be recovered with %GPOINTER_TO_INT. Implementations
 *             should switch on this value to ensure the expected subunit is
 *             executed on each call. This function must be implemented by
 *             derivative classes.
 *
 * Abstract class for #GtuComplexCase.
 */
struct _GtuComplexCaseClass {
  /*< private >*/
  GtuTestCaseClass parent_class;
  /*< public >*/
  void (*test_impl) (GtuComplexCase* self, gconstpointer subunit);
};

/**
 * gtu_complex_case_construct:
 * @type:              #GType of the class deriving from %GTU_TYPE_COMPLEX_CASE.
 * @name:              name of the complex test case. Must be valid as per
 *                     #Validity.
 * @subunit_enum_type: #GType of the enum declaring the subunits of the complex
 *                     type. Must derive from #G_TYPE_ENUM.
 *
 * Base constructor for derivative #GtuComplexCase implementations.
 *
 * Returns: new floating #GtuComplexCase instance.
 */
GtuComplexCase* gtu_complex_case_construct (GType type,
                                            const char* name,
                                            GType subunit_enum_type);

/**
 * gtu_complex_case_construct_vala: (skip)
 * @type:              #GType of the class deriving from %GTU_TYPE_COMPLEX_CASE.
 * @subunit_enum_type: #GType of the enum declaring the subunits of the complex
 *                     type. Must derive from #G_TYPE_ENUM.
 * @dummy1:            (allow-none): dummy parameter. Must be %NULL.
 * @dummy2:            (allow-none): dummy parameter. Must be %NULL.
 * @name:              name of the complex test case. Must be valid as per
 *                     #Validity.
 *
 * Wrapper around gtu_complex_case_construct() with a signature compatible with
 * Vala class generics.
 *
 * Returns: new floating #GtuComplexCase instance.
 */
GtuComplexCase* gtu_complex_case_construct_vala (GType type,
                                                 GType subunit_enum_type,
                                                 GBoxedCopyFunc dummy1,
                                                 GDestroyNotify dummy2,
                                                 const char* name);

G_END_DECLS

#endif
