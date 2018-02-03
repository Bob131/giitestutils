#include <string.h>
#include "gtu.h"

struct _GtuPath {
  GPtrArray* elements;
  char*      cached_string;
};

GtuPath* gtu_path_new (void) {
  GtuPath* self = g_malloc (sizeof (GtuPath));
  self->elements = g_ptr_array_new_with_free_func (g_free);
  self->cached_string = NULL;
  return self;
}

GtuPath* gtu_path_new_parse (const char* path, char** endptr) {
  GtuPath* self = NULL;
  int index = 0;
  GString* element_buf = NULL;

  if (path[index++] != '/')
    goto error;

  self = gtu_path_new ();
  element_buf = g_string_new (NULL);

  for (; path[index] != '\0'; index++) {
    if (path[index] == '/') {
      if (element_buf->len == 0)
        goto error;

      g_ptr_array_add (self->elements, g_string_free (element_buf, false));
      element_buf = g_string_new (NULL);

      continue;
    }

    if (g_ascii_isspace (path[index]))
      goto error;

    g_string_append_c (element_buf, path[index]);
  }

  /* there should be one final element since trailing '/' is invalid */
  if (element_buf->len == 0)
    goto error;

  g_ptr_array_add (self->elements, g_string_free (element_buf, false));

  goto ret;

error:
  if (self)
    gtu_path_free (self);

  if (element_buf)
    g_string_free (element_buf, true);

  self = NULL;

ret:
  if (endptr != NULL)
    *endptr = (char*) path + index;

  return self;
}

GtuPath* gtu_path_copy (GtuPath* path) {
  unsigned i;
  GtuPath* ret = gtu_path_new ();

  g_ptr_array_set_size (ret->elements, path->elements->len);

  for (i = 0; i < ret->elements->len; i++)
    ret->elements->pdata[i] = g_strdup (path->elements->pdata[i]);

  return ret;
}

void gtu_path_free (GtuPath* path) {
  g_ptr_array_free (path->elements, true);
  path->elements = NULL;

  if (path->cached_string) {
    g_free (path->cached_string);
    path->cached_string = NULL;
  }

  g_free (path);
}

/* There should be only one way to construct an invalid GtuPath instance, and
   that's by making an empty path with gtu_path_new(). So just testing the
   length of `elements' should be enough. */
bool gtu_path_is_valid (GtuPath* path) {
  return path->elements->len > 0;
}

const char* gtu_path_to_string (GtuPath* path) {
  unsigned i;
  GString* ret_buffer;

  g_return_val_if_fail (gtu_path_is_valid (path), NULL);

  if (path->cached_string == NULL) {
    for (i = 0, ret_buffer = g_string_new (NULL);
         i < path->elements->len;
         i++)
    {
      g_string_append_c (ret_buffer, '/');
      g_string_append (ret_buffer, path->elements->pdata[i]);
    }

    path->cached_string = g_string_free (ret_buffer, false);
  }

  return path->cached_string;
}

static bool element_is_valid (const char* element) {
  int i;
  for (i = 0; element[i] != '\0'; i++)
    if (element[i] == '/' || g_ascii_isspace (element[i]))
      return false;

  return i > 0;
}

void gtu_path_prepend_element (GtuPath* path, const char* element) {
  g_return_if_fail (element_is_valid (element));
  g_ptr_array_insert (path->elements, 0, g_strdup (element));
}

void gtu_path_append_element (GtuPath* path, const char* element) {
  g_return_if_fail (element_is_valid (element));
  g_ptr_array_add (path->elements, g_strdup (element));
}

void gtu_path_prepend_path (GtuPath* path, GtuPath* to_prepend) {
  unsigned i;

  for (i = 0; i < to_prepend->elements->len; i++)
    g_ptr_array_insert (path->elements,
                        (int) i,
                        g_strdup (to_prepend->elements->pdata[i]));
}

void gtu_path_append_path (GtuPath* path, GtuPath* to_append) {
  unsigned i;

  for (i = 0; i < to_append->elements->len; i++)
    g_ptr_array_add (path->elements, g_strdup (to_append->elements->pdata[i]));
}

bool gtu_path_has_prefix (GtuPath* path, GtuPath* prefix) {
  unsigned i;

  if (path->elements->len < prefix->elements->len)
    return false;

  for (i = 0; i < prefix->elements->len; i++)
    if (strcmp (path->elements->pdata[i], prefix->elements->pdata[i]) != 0)
      return false;

  return true;
}
