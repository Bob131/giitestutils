#ifndef __GII_TEST_UTILS_PATH_H__
#define __GII_TEST_UTILS_PATH_H__

/**
 * SECTION:gtu-path
 * @short_description: test object identifiers
 * @title: Test Paths
 * @include: gtu.h
 *
 * Test objects in GTU, like in GLib, are identified by a path (similar in
 * appearance to a UNIX file path). To make manipulation of these paths easier,
 * a convenience type is provided: #GtuPath.
 *
 * A #GtuPath internally holds a list of <emphasis>elements</emphasis>, strings
 * that identify individual #GtuTestObject instances. These elements are
 * delimited by a slash/stroke (`/`) to describe hierarchies of #GtuTestSuite
 * and #GtuTestCase.
 *
 * Paths are guaranteed to be absolute, to be without a terminating stroke, and
 * to be free of whitespace.
 *
 * Projects utilising GTU will typically don't need to use any path
 * functionality, or otherwise will only be interested in gtu_path_to_string().
 * Other functions are exposed just in case, but for the most part these are
 * internal to GTU.
 *
 * ## Validity {#Validity}
 *
 * Path elements are invalid if:
 *   - they contain a slash/stroke (`/`), or
 *   - they contain whitespace, or
 *   - they have zero length (`element == ""`).
 *
 * Paths are invalid if:
 *   - they are missing a leading slash/stroke (`/`), or
 *   - they contain a trailing stroke, or
 *   - they contain whitespace, or
 *   - they don't declare any elements (e.g. `path == ""`, `path == "/"`), or
 *   - they contain invalid elements.
 *   - This includes zero length elements (i.e. if the path contains two
 *     consecutive strokes `//`).
 */

#ifndef __GII_TEST_UTILS_H__
#error "Only <gtu.h> can be included directly."
#endif

G_BEGIN_DECLS

/**
 * GtuPath:
 *
 * Type representing a test object identifier.
 */
typedef struct _GtuPath GtuPath;

/**
 * gtu_path_new:
 *
 * Creates an empty #GtuPath. The new instance is considered invalid until
 * elements are added to it.
 *
 * Returns: a new #GtuPath instance.
 */
GtuPath* gtu_path_new (void);

/**
 * gtu_path_new_parse:
 * @path:   a test path string.
 * @endptr: (allow-none) (out): the location in @path at which processing was
 *          stopped.
 *
 * Parses @path into a new #GtuPath instance. Returns %NULL if @path is not
 * valid (see #Validity).
 *
 * @endptr is a return parameter indicating the location in @path where parsing
 * finished. If @path was parsed successfully, this will be equal to the length
 * of @path. Otherwise, it indicates the character which caused the parse to
 * fail.
 *
 * Returns: (allow-none): a new #GtuPath instance filled with elements of
 *          @path, or %NULL if @path is invalid.
 */
GtuPath* gtu_path_new_parse (const char* path, char** endptr);

/**
 * gtu_path_copy:
 * @path: #GtuPath instance to copy.
 *
 * Creates a new #GtuPath instance containing the elements of @path. The result
 * should be freed with gtu_path_free() when it's no longer required.
 *
 * Returns: a new copy of @path.
 */
GtuPath* gtu_path_copy (GtuPath* path);

/**
 * gtu_path_free:
 * @path: #GtuPath instance to free.
 *
 * Destroys an instance of #GtuPath, freeing any resources it holds.
 */
void gtu_path_free (GtuPath* path);

G_DEFINE_AUTOPTR_CLEANUP_FUNC (GtuPath, gtu_path_free)

/**
 * gtu_path_is_valid:
 * @path: #GtuPath instance to check.
 *
 * This function checks whether @path represents a valid test object
 * identifier, as per #Validity.
 *
 * Returns: %TRUE if @self is valid, otherwise %FALSE.
 */
bool gtu_path_is_valid (GtuPath* path);

/**
 * gtu_path_to_string:
 * @path: #GtuPath instance to serialise into a string. Must be valid.
 *
 * Collects elements of @path and concatenates them into a string, guaranteed
 * to be valid as per #Validity. The result is owned by the @path instance and
 * should not be altered or freed.
 *
 * The result of calling this function is undefined if @path is invalid (i.e.
 * if gtu_path_is_valid() would return %FALSE).
 *
 * Returns: (transfer none): string representation of @path.
 */
const char* gtu_path_to_string (GtuPath* path);

/**
 * gtu_path_prepend_element:
 * @path:       #GtuPath instance to which @to_prepend will be added.
 * @to_prepend: element to prepend to @path. Must be valid.
 *
 * Prepends @to_prepend to @path. The @to_prepend string is copied, so the
 * caller may free it once the call has finished.
 */
void gtu_path_prepend_element (GtuPath* path, const char* to_prepend);

/**
 * gtu_path_append_element:
 * @path:       #GtuPath instance to which @to_append will be added.
 * @to_append:  element to append to @path. Must be valid.
 *
 * Appends @to_append to @path. The @to_append string is copied, so the caller
 * may free it once the call has finished.
 */
void gtu_path_append_element (GtuPath* path, const char* to_append);

/**
 * gtu_path_prepend_path:
 * @path:       #GtuPath instance to which the elements of @to_prepend will be
 *              added.
 * @to_prepend: #GtuPath instance whose elements will be prepended to @path.
 *
 * Copies and prepends all elements from @to_prepend to @path.
 */
void gtu_path_prepend_path (GtuPath* path, GtuPath* to_prepend);

/**
 * gtu_path_append_path:
 * @path:       #GtuPath instance to which the elements of @to_append will be
 *              added.
 * @to_append:  #GtuPath instance whose elements will be appended to @path.
 *
 * Copies and appends all elements from @to_append to @path.
 */
void gtu_path_append_path (GtuPath* path, GtuPath* to_append);

/**
 * gtu_path_has_prefix:
 * @path:   #GtuPath instance to check.
 * @prefix: prefix to test against @path.
 *
 * Checks whether @path is a subpath of @prefix.
 *
 * Returns: %TRUE if all elements in @prefix match the first elements in @path,
 *          or if @path and @prefix are equal; otherwise, return %FALSE.
 */
bool gtu_path_has_prefix (GtuPath* path, GtuPath* prefix);

G_END_DECLS

#endif
