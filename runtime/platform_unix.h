#ifndef _ACRT_RUNTIME_PLATFORM_UNIX_H_
#define _ACRT_RUNTIME_PLATFORM_UNIX_H_

#include "platform.h"

#include <stdlib.h>

/* Data struct referring to an unix required fields. Since both mapped memory and dynamically
 * loaded object is needed for this operation, this struct holds the file name and required fields
 * for symbol looking and de-initialization. */
struct platform_dependent {

  size_t file_size;

  union {
    void *mdata;
  };

  void *dlobj;
};

#endif
