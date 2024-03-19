#ifndef HANDLER_LIST_H
#define HANDLER_LIST_H

#include "handler.h"

enum
{
  HANDLERS_NUM = 5U,
};

void handler_list_add(handler *const new_handlers, uint8_t handlers_num);
void handler_list_set_mode(uint8_t mode_index);
handler handler_list_get_mode(void);
void handler_list_destroy(void);
const char *handler_list_get_name(uint8_t index);
void handler_list_set(uint8_t index, handler_input *const input);

#endif