#include "../include/constants.h"
#include <game_engine.h>
#include <unity.h>

void setUp(void) {}
void tearDown(void) {}

void test_1equal1(void) { TEST_ASSERT_EQUAL(1, 1); }

void test_angle(void) { TEST_ASSERT_EQUAL(get_angle(6000, 7810), 50); }

void test_angle_2(void) { TEST_ASSERT_EQUAL(get_angle(2000, 3000), 41); }

void test_explore(void) {
  char *buffer = create_buffer(BUFFER_SIZE);
  TEST_ASSERT_EQUAL_STRING(explore(7, buffer), "RADAR 7\n");
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_angle);
  RUN_TEST(test_angle_2);
  RUN_TEST(test_explore);
  UNITY_END(); // stop unit testing
}
