#include "unity_fixture.h"
#include "unity_config.h"
#include <stdio.h>

void unity_config_put_c(uint8_t a);

static void run_all_tests()
{
	RUN_TEST_GROUP(hc06_driver);
  RUN_TEST_GROUP(led_panels_driver);
  RUN_TEST_GROUP(flash_driver);
  RUN_TEST_GROUP(render_controller);
}

int main(int argc, char *argv[])
{
	return UnityMain(argc, argv, run_all_tests);
}

void unity_config_put_c(uint8_t a)
{
	(void)putchar(a);
}
