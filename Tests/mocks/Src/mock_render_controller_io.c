#include "mock_render_controller_io.h"
#include "unity_fixture.h"
#include "led_panels_driver.h"
#include "hc06_driver.h"
#include <stddef.h>

// Static variables ----------------------------------------------------------

typedef struct
{
  int kind;
} expectation;

enum
{
  IO_START,
  IO_STOP,
  NO_EXPECTED_VALUE = -1
};

static char *report_not_init = "MockIO not initialized";
static char *report_no_room_for_exp = "No room for expectations in MockIO";
static char *report_verify_error = "Verify error in MockIO. Expected %u "
  "operations, but got %u";
static char *report_kind_error = "Error kind in MockIO."
  " Num of expectation %u ";

static volatile bool is_transmit_timeout = false;
static led_panels_buffer **front_buffer = NULL;
static expectation *expectations = NULL;
static int set_expectation_count;
static int get_expectation_count;
static int max_expectation_count;
static uint32_t current_tick;

// Static functions ----------------------------------------------------------

static void fail_when_no_init()
{
  if (expectations == NULL)
    FAIL(report_not_init);
}

static void fail_when_no_room_for_expectations()
{
  fail_when_no_init();
  if (set_expectation_count >= max_expectation_count)
    FAIL(report_no_room_for_exp);
}


static void record_expectation(const int kind)
{
  expectations[set_expectation_count].kind = kind;
  set_expectation_count++;
}

static void check_kind(const expectation *const current_expectation, int kind)
{
  char *message[sizeof(report_kind_error) + 10];

  if (current_expectation->kind == kind)
    return;

  sprintf((char*)message, report_kind_error, get_expectation_count);
  FAIL((char*)message);
}

// Implementations -----------------------------------------------------------

void mock_render_controller_io_create(const uint8_t max_expectations)
{
  if (expectations != NULL)
    free(expectations);
  expectations = calloc(max_expectations, sizeof(expectation));
  max_expectation_count = max_expectations;
}

void mock_render_controller_io_destroy()
{
  if (expectations)
    free(expectations);
  expectations = NULL;

  set_expectation_count = 0;
  get_expectation_count = 0;
  max_expectation_count = 0;
}

void mock_render_controller_io_expect_start_timer()
{
  fail_when_no_room_for_expectations();
  record_expectation(IO_START);
}

void mock_render_controller_io_expect_stop_timer()
{
  fail_when_no_room_for_expectations();
  record_expectation(IO_STOP);
}

void mock_render_controller_io_set_timeout()
{
  is_transmit_timeout = true;
}

void mock_render_controller_io_verify_complete()
{
  char *message[sizeof(report_verify_error) + 10];

  if (set_expectation_count == get_expectation_count)
    return;
    
  sprintf(
    (char*)message,
    report_verify_error,
    set_expectation_count,
    get_expectation_count
  );
  FAIL((char*)message);
}

// Implementations from render_controller_io ---------------------------------

void render_controller_io_create(led_panels_buffer **buffer)
{
  front_buffer = buffer; // ?
}

void render_controller_io_start_timeout_timer()
{
  expectation current_expectation = expectations[get_expectation_count];

  fail_when_no_init();
  check_kind(&current_expectation, IO_START);

  get_expectation_count++;
}

void render_controller_io_stop_timeout_timer()
{
  expectation current_expectation = expectations[get_expectation_count];

  fail_when_no_init();
  check_kind(&current_expectation, IO_STOP);

  get_expectation_count++;
}

void mock_render_controller_io_set_tick(uint32_t tick)
{
  current_tick = tick;
}

void render_controller_io_timeout_timer_complete()
{
  is_transmit_timeout = true;
}

bool render_controller_io_is_timeout()
{
  return is_transmit_timeout;
}

void render_controller_io_send_frame_complete()
{
  if (*front_buffer)
    led_panels_send_complete(*front_buffer);
}

void render_controller_io_receive_complete()
{
  hc06_receive_complete();
}

uint32_t render_controller_io_get_ticks()
{
  return current_tick;
}

void render_controller_io_destroy()
{
  front_buffer = NULL;
}
