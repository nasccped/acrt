#ifndef _ASSERT_MANAGER_H_
#define _ASSERT_MANAGER_H_

/* 'AssertManager' default naming. */
#define ASSERT_MANAGER_DEFAULT_NAME "<GENERIC ASSERTION>"

typedef struct {
  /* The manager name. */
  char *manager_name;
} AssertManager;

/* Creates a new assert Manager.
 *
 * 'manager_name' field is set to the default value
 * ('ASSERT_MANAGER_DEFAULT_NAME'). */
AssertManager assert_manager_new();

/* Set a new name to the 'self' assert manager pointer + return itself.
 *
 * If 'self' is null, nothing is changed and null is returned.
 *
 * Is 'name' is null or zero length string, nothing is changed. */
AssertManager *assert_manager_set_name(AssertManager *self, char *name);

#endif // !_ASSERT_MANAGER_H_
