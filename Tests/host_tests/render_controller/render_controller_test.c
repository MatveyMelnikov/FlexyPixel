#include "unity_fixture.h"
#include "render_controller.h"
#include "mock_render_controller_io.h"
#include "mock_hc06_io.h"
#include "pixel_mode_handler.h"
#include <string.h>

// Defines -------------------------------------------------------------------

// Static variables ----------------------------------------------------------

static handler modes[1];
static char *ok_output = "{\"status\":\"OK\"}";
static char *unconfigured_output = "{\"status\":\"UNCONFIGURED\"}";
static char *error_output = "{\"status\":\"ERROR\"}";

// Static functions ----------------------------------------------------------

static void start_render_controller(char *next_input)
{
  static char *set_baudrate_data = "AT+UART=115200,0,0\r\n";
  static char *set_baudrate_status = "OK115200\r\n";

  mock_hc06_io_expect_write(
    (uint8_t*)set_baudrate_data,
    strlen(set_baudrate_data)
  );
  mock_hc06_io_expect_read_then_return(
    (uint8_t*)set_baudrate_status,
    strlen(set_baudrate_status)
  );
  mock_hc06_io_expect_baudrate_change();
  mock_hc06_io_expect_read_external_then_return(
    (uint8_t*)next_input,
    strlen(next_input)
  );

  render_controller_create(modes, 1);
  render_controller_io_receive_complete();
}

static void send_cmd(char *arg, char *next_input)
{
  // set cmd handler
  mock_hc06_io_expect_write(
    (uint8_t*)ok_output,
    strlen(ok_output)
  );
  mock_hc06_io_expect_read_external_then_return(
    (uint8_t*)arg,
    strlen(arg)
  );
  mock_render_controller_io_expect_start_timer();

  // cmd handler
  mock_render_controller_io_expect_stop_timer();
  mock_hc06_io_expect_write(
    (uint8_t*)ok_output,
    strlen(ok_output)
  );

  // getting the next command
  mock_hc06_io_expect_read_external_then_return(
    (uint8_t*)next_input,
    strlen(next_input)
  );

  render_controller_process();

  render_controller_io_receive_complete();
  render_controller_process();
}

// Tests ---------------------------------------------------------------------

TEST_GROUP(render_controller);

TEST_SETUP(render_controller)
{
  mock_render_controller_io_create(10);
  mock_hc06_io_create(20);

  modes[0] = pixel_mode_handler_create();
}

TEST_TEAR_DOWN(render_controller)
{
  handler_destroy(modes[0]);

  mock_render_controller_io_verify_complete();
  mock_render_controller_io_destroy();
  mock_hc06_io_verify_complete();
  mock_hc06_io_destroy();

  render_controller_destroy();
}

TEST(render_controller, render_controller_start)
{
  start_render_controller("{\"type\":\"data\"}");
}

TEST(render_controller, data_cmd_unconfigured)
{
  char *input_data = "{\"type\":\"data\"}";

  start_render_controller(input_data);

  mock_hc06_io_expect_write(
    (uint8_t*)unconfigured_output,
    strlen(unconfigured_output)
  );
  mock_hc06_io_expect_read_external_then_return(
    (uint8_t*)input_data,
    strlen(input_data)
  );

  render_controller_process();
}

TEST(render_controller, mode_cmd_success)
{
  char *cmd_input = "{\"type\":\"mode\"}";
  char *mode_input = "{\"mode\":\"PIX\"}";

  start_render_controller(cmd_input);
  send_cmd(mode_input, cmd_input);
}

TEST(render_controller, multiple_mode_cmd_success)
{
  char *cmd_input = "{\"type\":\"mode\"}";
  char *mode_input = "{\"mode\":\"PIX\"}";

  start_render_controller(cmd_input);
  send_cmd(mode_input, cmd_input);
  send_cmd(mode_input, cmd_input);
}

TEST(render_controller, mode_cmd_and_data_error)
{
  char *mode_cmd_input = "{\"type\":\"mode\"}";
  char *mode_input = "{\"mode\":\"PIX\"}";
  char *data_cmd_input = "{\"type\":\"data\"}";

  start_render_controller(mode_cmd_input);
  send_cmd(mode_input, data_cmd_input);

  mock_hc06_io_expect_write(
    (uint8_t*)unconfigured_output,
    strlen(unconfigured_output)
  );
  mock_hc06_io_expect_read_external_then_return(
    (uint8_t*)mode_cmd_input,
    strlen(mode_cmd_input)
  );

  render_controller_process();
}

TEST(render_controller, conf_cmd_success)
{
  char *cmd_input = "{\"type\":\"conf\"}";
  char *conf_input = "{\"configuration\":["
    "\"256\",\"064\",\"256\",\"064\",\"000\","
    "\"256\",\"256\",\"256\",\"256\"]}";

  start_render_controller(cmd_input);

  render_controller_io_destroy();
  send_cmd(conf_input, cmd_input);
}

TEST(render_controller, multiple_conf_cmd_success)
{
  char *cmd_input = "{\"type\":\"conf\"}";
  char *conf_input_0 = "{\"configuration\":["
    "\"256\",\"064\",\"256\",\"064\",\"000\","
    "\"256\",\"256\",\"256\",\"256\"]}";
  char *conf_input_1 = "{\"configuration\":["
    "\"064\",\"000\",\"256\",\"064\",\"000\","
    "\"256\",\"256\",\"256\",\"256\"]}";

  start_render_controller(cmd_input);

  render_controller_io_destroy();
  send_cmd(conf_input_0, cmd_input);
  send_cmd(conf_input_1, cmd_input);
}

TEST(render_controller, conf_cmd_and_data_error)
{
  char *conf_cmd_input = "{\"type\":\"conf\"}";
  char *conf_input = "{\"configuration\":["
    "\"256\",\"064\",\"256\",\"064\",\"000\","
    "\"256\",\"256\",\"256\",\"256\"]}";
  char *data_cmd_input = "{\"type\":\"data\"}";

  start_render_controller(conf_cmd_input);
  send_cmd(conf_input, data_cmd_input);

  mock_hc06_io_expect_write(
    (uint8_t*)unconfigured_output,
    strlen(unconfigured_output)
  );
  mock_hc06_io_expect_read_external_then_return(
    (uint8_t*)conf_cmd_input,
    strlen(conf_cmd_input)
  );

  render_controller_process();
}

TEST(render_controller, conf_plus_mode_cmd_and_data_success)
{
  char *conf_cmd_input = "{\"type\":\"conf\"}";
  char *mode_cmd_input = "{\"type\":\"mode\"}";
  char *mode_input = "{\"mode\":\"PIX\"}";
  char *conf_input = "{\"configuration\":["
    "\"256\",\"064\",\"256\",\"064\",\"000\","
    "\"256\",\"256\",\"256\",\"256\"]}";
  char *data_cmd_input = "{\"type\":\"data\"}";
  char *data_input = "{\"panelPosition\":0,\"pixelPosition\":"
    "\"000\",\"pixelColor\":\"967\"}";

  start_render_controller(conf_cmd_input);
  send_cmd(conf_input, mode_cmd_input);
  send_cmd(mode_input, data_cmd_input);

  mock_hc06_io_expect_write(
    (uint8_t*)ok_output,
    strlen(ok_output)
  );
  mock_hc06_io_expect_read_external_then_return(
    (uint8_t*)data_input,
    strlen(data_input)
  );
  mock_render_controller_io_expect_start_timer();

  mock_render_controller_io_expect_stop_timer();
  mock_hc06_io_expect_write(
    (uint8_t*)ok_output,
    strlen(ok_output)
  );
  mock_hc06_io_expect_read_external_then_return(
    (uint8_t*)conf_cmd_input,
    strlen(conf_cmd_input)
  );

  render_controller_process();
  render_controller_io_receive_complete();
  render_controller_process();
}

TEST(render_controller, conf_plus_mode_cmd_and_data_panel_pos_error)
{
  char *conf_cmd_input = "{\"type\":\"conf\"}";
  char *mode_cmd_input = "{\"type\":\"mode\"}";
  char *mode_input = "{\"mode\":\"PIX\"}";
  char *conf_input = "{\"configuration\":["
    "\"064\",\"000\",\"256\",\"064\",\"000\","
    "\"256\",\"256\",\"256\",\"256\"]}";
  char *data_cmd_input = "{\"type\":\"data\"}";
  char *data_input = "{\"panelPosition\":1,\"pixelPosition\":"
    "\"000\",\"pixelColor\":\"967\"}";

  start_render_controller(conf_cmd_input);
  send_cmd(conf_input, mode_cmd_input);
  send_cmd(mode_input, data_cmd_input);

  mock_hc06_io_expect_write(
    (uint8_t*)ok_output,
    strlen(ok_output)
  );
  mock_hc06_io_expect_read_external_then_return(
    (uint8_t*)data_input,
    strlen(data_input)
  );
  mock_render_controller_io_expect_start_timer();

  mock_render_controller_io_expect_stop_timer();
  mock_hc06_io_expect_write(
    (uint8_t*)error_output,
    strlen(error_output)
  );
  mock_hc06_io_expect_read_external_then_return(
    (uint8_t*)conf_cmd_input,
    strlen(conf_cmd_input)
  );

  render_controller_process();
  render_controller_io_receive_complete();
  render_controller_process();
}

TEST(render_controller, conf_plus_mode_cmd_and_data_pixel_pos_error)
{
  char *conf_cmd_input = "{\"type\":\"conf\"}";
  char *mode_cmd_input = "{\"type\":\"mode\"}";
  char *mode_input = "{\"mode\":\"PIX\"}";
  char *conf_input = "{\"configuration\":["
    "\"064\",\"000\",\"256\",\"064\",\"000\","
    "\"256\",\"256\",\"256\",\"256\"]}";
  char *data_cmd_input = "{\"type\":\"data\"}";
  char *data_input = "{\"panelPosition\":0,\"pixelPosition\":"
    "\"125\",\"pixelColor\":\"967\"}";

  start_render_controller(conf_cmd_input);
  send_cmd(conf_input, mode_cmd_input);
  send_cmd(mode_input, data_cmd_input);

  mock_hc06_io_expect_write(
    (uint8_t*)ok_output,
    strlen(ok_output)
  );
  mock_hc06_io_expect_read_external_then_return(
    (uint8_t*)data_input,
    strlen(data_input)
  );
  mock_render_controller_io_expect_start_timer();

  mock_render_controller_io_expect_stop_timer();
  mock_hc06_io_expect_write(
    (uint8_t*)error_output,
    strlen(error_output)
  );
  mock_hc06_io_expect_read_external_then_return(
    (uint8_t*)conf_cmd_input,
    strlen(conf_cmd_input)
  );

  render_controller_process();
  render_controller_io_receive_complete();
  render_controller_process();
}

