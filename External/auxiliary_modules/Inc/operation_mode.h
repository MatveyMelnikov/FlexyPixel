#ifndef OPERATION_MODE_H
#define OPERATION_MODE_H

#include "handler.h"

void operation_mode_set(handler mode);
void operation_mode_clear(void);
bool operation_mode_is_set(void);
handler operation_mode_get(void);

#endif
