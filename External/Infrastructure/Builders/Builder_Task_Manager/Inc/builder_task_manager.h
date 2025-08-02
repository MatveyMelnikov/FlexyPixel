#ifndef BUILDER_TASK_MANAGER_H
#define BUILDER_TASK_MANAGER_H

#include "task_manager.h"
#include "stm32f1xx_hal.h"

task_manager_status builder_task_manager_build(void);
void builder_task_manager_destroy(void);


#endif
