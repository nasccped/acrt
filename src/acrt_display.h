#ifndef _ACRT_DISPLAY_
#define _ACRT_DISPLAY_

#include "acrt.h"
#include "acrt_result.h"

typedef struct __acrt_result result_t;

// Display the assertion result in a pretty way.
void display_acrt_result(result_t *res);

// Display the counting struct inner data in a pretty way.
void display_counting_data(const char *ctx_name,
                           struct __acrt_counting *counting, FILE *f);

#endif
