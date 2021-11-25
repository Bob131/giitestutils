#include "priv-complex.h"
#include "priv-setjmp.h"
#include "log/logio.h"

typedef struct {
  GEnumClass* subunit_enum_class;
} GtuComplexCasePrivate;

#define PRIVATE(obj) \
  ((GtuComplexCasePrivate*) \
   gtu_complex_case_get_instance_private ((GtuComplexCase*) (obj)))

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (GtuComplexCase,
                                     gtu_complex_case,
                                     GTU_TYPE_TEST_CASE)

typedef struct {
  GtuComplexCase* self;
  int enum_value;
} ComplexRunContext;

static void gtu_complex_case_init (GtuComplexCase* self) {
  (void) self;
}

static void gtu_complex_case_finalize (GtuTestObject* self) {
  GtuComplexCasePrivate* priv = PRIVATE (self);
  g_type_class_unref (priv->subunit_enum_class);
  priv->subunit_enum_class = NULL;

  GTU_TEST_OBJECT_CLASS (gtu_complex_case_parent_class)->finalize (self);
}

static void gtu_complex_case_class_init (GtuComplexCaseClass* klass) {
  GTU_TEST_OBJECT_CLASS (klass)->finalize = &gtu_complex_case_finalize;
}

static bool check_enum_names (GtuComplexCase* self) {
  unsigned i;
  GEnumClass* enum_class = PRIVATE (self)->subunit_enum_class;

  for (i = 0; i < enum_class->n_values; i++)
    if (!_gtu_path_element_is_valid (enum_class->values[i].value_nick)) {
      g_critical ("Enumeration nick is not a valid path element: %s",
                  enum_class->values[i].value_nick);
      return false;
    }

  return true;
}

GtuComplexCase* gtu_complex_case_construct (GType type,
                                            const char* name,
                                            GType subunit_enum_type)
{
  GtuComplexCase* self;

  g_return_val_if_fail (g_type_is_a (type, GTU_TYPE_COMPLEX_CASE), NULL);
  g_return_val_if_fail (G_TYPE_IS_ENUM (subunit_enum_type), NULL);

  self = GTU_COMPLEX_CASE (gtu_test_case_construct (type, name));
  PRIVATE (self)->subunit_enum_class =
    G_ENUM_CLASS (g_type_class_ref (subunit_enum_type));

  if (!check_enum_names (self)) {
    gtu_test_object_unref (self);
    return NULL;
  }

  return self;
}

GtuComplexCase* gtu_complex_case_construct_vala (GType type,
                                                 GType subunit_enum_type,
                                                 GBoxedCopyFunc dummy1,
                                                 GDestroyNotify dummy2,
                                                 const char* name)
{
  g_return_val_if_fail (dummy1 == NULL, NULL);
  g_return_val_if_fail (dummy2 == NULL, NULL);
  return gtu_complex_case_construct (type, name, subunit_enum_type);
}

void run_inner (void* data) {
  ComplexRunContext* context = data;
  GTU_COMPLEX_CASE_GET_CLASS (context->self)->test_impl (
    context->self,
    GINT_TO_POINTER (context->enum_value)
  );
}

/* TODO: communicate number of failed subunits to the GtuTestSuite runner */
GtuTestResult _gtu_complex_case_run (GtuComplexCase* self, char** out_message) {
  unsigned i;
  GEnumClass* enum_class;
  ComplexRunContext context = { NULL, 0 };

  GtuTestResult result = GTU_TEST_RESULT_INVALID;
  unsigned n_skipped = 0;

  const GtuPath* our_path;

  g_assert (GTU_IS_COMPLEX_CASE (self));
  enum_class = PRIVATE (self)->subunit_enum_class;
  g_assert (G_IS_ENUM_CLASS (enum_class));

  context.self = self;
  our_path = gtu_test_object_get_path (GTU_TEST_OBJECT (self));

  for (i = 0; i < enum_class->n_values; i++) {
    GtuPath* temp_path;
    char* message = NULL;
    GEnumValue* value = &enum_class->values[i];

    context.enum_value = value->value;

    temp_path = gtu_path_copy (our_path);
    gtu_path_append_element (temp_path, value->value_nick);

    if (!_gtu_path_should_run (temp_path)) {
      g_message ("Ignoring skip arguments: %s",
                 "cannot skip individual subunits of a complex test case");
    }

    if (result != GTU_TEST_RESULT_FAIL)
      result = _gtu_test_case_exec_inner (&run_inner, &context, &message);

    switch (result) {
      case GTU_TEST_RESULT_PASS:
        gtu_log_test_success (gtu_path_to_string (temp_path), message);
        break;

      case GTU_TEST_RESULT_SKIP:
        gtu_log_test_skipped (gtu_path_to_string (temp_path), message);
        n_skipped++;
        break;

      case GTU_TEST_RESULT_FAIL:
        /* message may be NULL if we're skipping the last tests */
        gtu_log_test_failed (gtu_path_to_string (temp_path),
                             message != NULL ?
                               message :
                               "Previous subunit failed");
        break;

      default:
        g_assert_not_reached ();
    }

    gtu_path_free (temp_path);

    if (message != NULL)
      g_free (message);
  }

  switch (result) {
    case GTU_TEST_RESULT_SKIP:
      if (n_skipped != enum_class->n_values)
        break;

      *out_message = g_strdup ("All subunits were skipped");
      return GTU_TEST_RESULT_SKIP;

    case GTU_TEST_RESULT_FAIL:
      *out_message = g_strdup ("Subunits failed");
      return GTU_TEST_RESULT_FAIL;

    default:
      break;
  }

  if (gtu_test_mode_flags_get_flags () & GTU_TEST_MODE_FLAGS_VERBOSE)
    *out_message = g_strdup ("All subunits passed successfully");

  return GTU_TEST_RESULT_PASS;
}

unsigned _gtu_complex_case_get_length (GtuComplexCase* self) {
  return PRIVATE (self)->subunit_enum_class->n_values;
}
