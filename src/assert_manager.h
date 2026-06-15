#ifndef _ASSERT_MANAGER_H_
#define _ASSERT_MANAGER_H_

typedef struct {
  /* The manager name. */
  const char *manager_name;
} AssertManager;

/* Creates a new AssertManager pointer with a default name. Uses
 * 'ASSERT_MANAGER_DEFAULT_NAME' with name is null or zero length string. */
AssertManager *assert_manager_new(const char *name);

/* Frees the mem of AssertManager. */
void assert_manager_destroy(AssertManager *self);

/* Set a new name to the 'self' assert manager pointer + return itself.
 *
 * If 'self' is null, nothing is changed and null is returned.
 *
 * Is 'name' is null or zero length string, nothing is changed. */
AssertManager *assert_manager_set_name(AssertManager *self, char *name);

#endif // !_ASSERT_MANAGER_H_
