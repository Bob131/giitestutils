#include <string.h>
#include "gtu-priv.h"

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

static bool path_is_valid_pointer (const GtuPath* path) {
  return path != NULL && path->elements != NULL;
}

GtuPath* gtu_path_copy (const GtuPath* path) {
  unsigned i;
  GtuPath* ret;

  g_return_val_if_fail (path_is_valid_pointer (path), NULL);

  ret = gtu_path_new ();
  g_ptr_array_set_size (ret->elements, path->elements->len);

  for (i = 0; i < ret->elements->len; i++)
    ret->elements->pdata[i] = g_strdup (path->elements->pdata[i]);

  return ret;
}

void gtu_path_free (GtuPath* path) {
  g_return_if_fail (path_is_valid_pointer (path));

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
bool gtu_path_is_valid (const GtuPath* path) {
  g_return_val_if_fail (path_is_valid_pointer (path), false);
  return path->elements->len > 0;
}

/* we lie about `path' being const just to make the API simpler */
const char* gtu_path_to_string (const GtuPath* _path) {
  unsigned i;
  GString* ret_buffer;
  GtuPath* path;

  g_return_val_if_fail (gtu_path_is_valid (_path), NULL);
  path = (GtuPath*) _path;

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

bool _gtu_path_element_is_valid (const char* element) {
  int i;

  if (element == NULL)
    return false;

  for (i = 0; element[i] != '\0'; i++)
    if (element[i] == '/' || g_ascii_isspace (element[i]))
      return false;

  return i > 0;
}

void gtu_path_prepend_element (GtuPath* path, const char* element) {
  g_return_if_fail (path_is_valid_pointer (path));
  g_return_if_fail (_gtu_path_element_is_valid (element));
  g_ptr_array_insert (path->elements, 0, g_strdup (element));
}

void gtu_path_append_element (GtuPath* path, const char* element) {
  g_return_if_fail (path_is_valid_pointer (path));
  g_return_if_fail (_gtu_path_element_is_valid (element));
  g_ptr_array_add (path->elements, g_strdup (element));
}

void gtu_path_prepend_path (GtuPath* path, const GtuPath* to_prepend) {
  unsigned i;

  g_return_if_fail (path_is_valid_pointer (path));
  g_return_if_fail (path_is_valid_pointer (to_prepend));

  for (i = 0; i < to_prepend->elements->len; i++)
    g_ptr_array_insert (path->elements,
                        (int) i,
                        g_strdup (to_prepend->elements->pdata[i]));
}

void gtu_path_append_path (GtuPath* path, const GtuPath* to_append) {
  unsigned i;

  g_return_if_fail (path_is_valid_pointer (path));
  g_return_if_fail (path_is_valid_pointer (to_append));

  for (i = 0; i < to_append->elements->len; i++)
    g_ptr_array_add (path->elements, g_strdup (to_append->elements->pdata[i]));
}

bool gtu_path_has_prefix (const GtuPath* path, const GtuPath* prefix) {
  unsigned i;

  g_return_val_if_fail (path_is_valid_pointer (path),   false);
  g_return_val_if_fail (path_is_valid_pointer (prefix), false);

  if (path->elements->len < prefix->elements->len)
    return false;

  for (i = 0; i < prefix->elements->len; i++)
    if (strcmp (path->elements->pdata[i], prefix->elements->pdata[i]) != 0)
      return false;

  return true;
}

bool _gtu_path_should_run (const GtuPath* path) {
  bool should_run = true;
  GtuTestMode* test_mode = _gtu_get_test_mode ();
  GList* cursor;

  for (cursor = test_mode->path_selectors;
       cursor != NULL && should_run;
       cursor = cursor->next)
  {
    should_run = gtu_path_has_prefix (path, cursor->data);
  }

  for (cursor = test_mode->path_skippers;
       cursor != NULL && should_run;
       cursor = cursor->next)
  {
    should_run = !gtu_path_has_prefix (path, cursor->data);
  }

  return should_run;
}
