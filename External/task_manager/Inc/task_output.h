#ifndef TASK_OUTPUT_H
#define TASK_OUTPUT_H

#include <stdint.h>

/*
 * EXECUTION_COMPLETED - the subtask completes successfully
 * (the next subtask or task is executed);
 * EXECUTION_IN_PROGRESS - the subtask continues to execute;
 * EXECUTION_RESTART - in the task manager the subtask execution flag
 * is reset and it will continue to be executed the next time
 * new data is entered;
 * EXECUTION_ERROR - The subtask is executed with an error and the entire
 * task queue is reset
*/
typedef enum
{
  EXECUTION_COMPLETED,
  EXECUTION_IN_PROGRESS,
  EXECUTION_RESTART,
  EXECUTION_ERROR
} execution_status;

typedef enum
{
  RESPONSE_OK,
  RESPONSE_ERROR,
  RESPONSE_UNCONFIGURED,
  RESPONSE_NONE // nothing will be output
} response_type;

typedef struct
{
  execution_status status;
  response_type response;
  uint16_t request_size; // number of bytes expected to be received
  bool start_next_task_immediately;
} task_output;

#endif
