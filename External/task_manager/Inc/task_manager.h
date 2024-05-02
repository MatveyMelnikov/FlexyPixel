#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include <stdint.h>
#include "task.h"

void task_manager_add_tasks_list(task *const tasks, uint8_t tasks_amount);
bool task_manager_set(const char *task_name);
// mode
bool task_manager_set_data_task(const char *task_name);
task task_manager_get_data_task(void);
bool task_manager_run(task_input *const input);
bool task_manager_is_empty(void);
void task_manager_reset(void);

#endif
