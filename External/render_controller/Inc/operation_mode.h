#ifndef OPERATION_MODE_H
#define OPERATION_MODE_H

#include "mode_handler.h"

void operation_mode_set(mode_handler mode);
void operation_mode_clear(void);
bool operation_mode_is_set(void);
mode_handler operation_mode_get(void);

#endif
