#include "task_manager.h"
#include "hc06_driver.h"
#include "debug_output.h"
#include <stddef.h>
#include <stdbool.h>
#include <memory.h>

// Defines -------------------------------------------------------------------

#define RESPONSE_MSG_OK "{\"status\":\"OK\"}"
#define RESPONSE_MSG_ERROR "{\"status\":\"ERROR\"}"
#define RESPONSE_MSG_UNCONFIGURED "{\"status\":\"UNCONFIGURED\"}"

enum {
  TASKS_AMOUNT = 5U,
  REQUEST_LIMIT = 2000U
};

// Static variables ----------------------------------------------------------

static task *tasks_list;
static uint8_t tasks_list_size;

static task data_task = NULL; // mode

static task tasks_queue[TASKS_AMOUNT];
static uint8_t top_index = 0;
static uint8_t current_index = 0;
static uint8_t subtask_index = 0;
static bool is_task_in_progress = false;

//static bool skip_remove = false;

// Static functions ----------------------------------------------------------

static task find_task(const char *task_name)
{
  for (uint8_t i = 0; i < tasks_list_size; i++)
  {
    task current_task = *(tasks_list + i);

    if (
      memcmp(current_task->name, task_name, strlen(current_task->name)) == 0
    )
    {
      return current_task;
    }
  }

  return NULL;
}

static void handle_output(const task_output *const output)
{
  char output_data[30];
  switch (output->response)
  {
  case RESPONSE_OK:
    strcpy(output_data, RESPONSE_MSG_OK);
    break;
  case RESPONSE_ERROR:
    strcpy(output_data, RESPONSE_MSG_ERROR);
    break;
  case RESPONSE_UNCONFIGURED:
    strcpy(output_data, RESPONSE_MSG_UNCONFIGURED);
    break;
  default:
    return;
  }

  DEBUG_OUTPUT_WITH_INSERT("handle_output: %s\n", output_data);
  hc06_write((uint8_t*)output_data, strlen(output_data));
}

static void handle_input(
  const task_input *const input,
  const task_output *const output
)
{
  if (output->request_size == 0)
    return;

  uint16_t data_size = (output->request_size > REQUEST_LIMIT) ? 
    REQUEST_LIMIT : output->request_size;
  hc06_read(input->data, data_size);
}

// Implementations -----------------------------------------------------------

void task_manager_add_tasks_list(task *const tasks, uint8_t tasks_amount)
{
  tasks_list = tasks;
  tasks_list_size = tasks_amount;
}

bool task_manager_set(const char *task_name)
{
  if (top_index >= TASKS_AMOUNT)
    return false;
  
  task found_task = find_task(task_name);
  if (found_task == NULL)
    return false;
  
  tasks_queue[top_index++] = found_task;

  return true;
}

bool task_manager_set_data_task(const char *task_name)
{
  task found_task = find_task(task_name);
  if (found_task == NULL)
    return false;

  data_task = found_task;

  return true;
}

task task_manager_get_data_task()
{
  return data_task;
}

// false - error
bool task_manager_run(task_input *const input)
{
  if (task_manager_is_empty())
    return false;
  if (!input->is_input_changed && !is_task_in_progress)
    return true;

  task current_task = tasks_queue[current_index];
  task_output output = task_execute_subtask(current_task, input, subtask_index);

  DEBUG_OUTPUT_WITH_INSERT(
    "execute task: %s\n",
    task_get_name(current_task)
  );
  handle_input(input, &output);
  handle_output(&output);

  if (output.status == EXECUTION_ERROR)
  {
    task_manager_reset();
    return false;
  }
  if (output.status == EXECUTION_IN_PROGRESS)
  {
    is_task_in_progress = true;
    return true;
  }
  if (output.status == EXECUTION_RESTART)
  {
    is_task_in_progress = false;
    return true;
  }
  if (output.start_next_task_immediately)
  {
    task_manager_skip_task(true);
    return true;
  }
  if (task_get_subtasks_amount(current_task) > ++subtask_index)
  {
    is_task_in_progress = false;
    return true;
  }

  task_manager_skip_task(false);

  return true;
}

bool task_manager_is_empty()
{
  return current_index >= top_index;
}

void task_manager_reset()
{
  current_index = 0;
  subtask_index = 0;
  top_index = 0;
  is_task_in_progress = false;
}

void task_manager_skip_task(bool start_immediately)
{
  current_index++;
  subtask_index = 0;
  is_task_in_progress = start_immediately;

  if (task_manager_is_empty())
    task_manager_reset();
}
