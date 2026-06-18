#ifndef _ASSERTION_WRAPPER_H_
#define _ASSERTION_WRAPPER_H_

/* Kind of the current wrapper (used to easy display with 'data' union). */
typedef enum {
  /* When boolean expression is being used. */
  BOOLEAN_EXPRESSION,
} assertion_wrapper_kind_t;

/* A wrapper for assertion results (used within display stuff). */
typedef struct {
  /* Current kind (tagged enums used). */
  assertion_wrapper_kind_t kind;
  /* File name of the current assertion (equivalent to __FILE__). */
  const char *file_name;
  /* The line number of the current assertion. */
  const int line_number;
  /* Group name of the current assertion being ran. */
  const char *group_name;
  /* Final result of the current assertion. */
  const int result;
  union {
    const char *boolean_expression_representation;
  } data;
} assertion_wrapper_t;

/* Creates a new 'assertion_wrapper_t' of 'BOOLEAN_EXPRESSION' kind. */
assertion_wrapper_t assertion_wrapper_new_bool(const char *, const unsigned int,
                                               const char *, const char *,
                                               const int);

#endif
