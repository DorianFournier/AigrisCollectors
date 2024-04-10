#include <constants.h>
#include <game_engine.h>
#include <unity.h>

T_game_data game_data[NUMBER_OF_GAME_DATA] = {{{{1, 10000, 20000, -1, 0},
                                                {2, 10000, 20000, -1, 0},
                                                {3, 10000, 20000, -1, 0},
                                                {4, 10000, 20000, -1, 0},
                                                {5, 10000, 20000, -1, 0},
                                                {6, 10000, 20000, -1, 0},
                                                {7, 10000, 20000, -1, 0},
                                                {8, 10000, 1200, -1, 0}},
                                               {{0, 1, 0, 0, 0},
                                                {0, 2, 0, 0, 0},
                                                {0, 3, 0, 0, 0},
                                                {0, 4, 0, 0, 0},
                                                {0, 5, 0, 0, 0},
                                                {0, 6, 0, 0, 0},
                                                {0, 7, 0, 0, 0},
                                                {0, 8, 10000, 10000, 0},
                                                {0, 9, 10000, 1000, 0}},
                                               {10000, 0}}};

void setUp(void) {}
void tearDown(void) {}

void test_get_angle_between_two_points(void) {
  T_point point_A;
  point_A.pos_X = 5000;
  point_A.pos_Y = 11000;

  T_point point_B;
  point_B.pos_X = 10000;
  point_B.pos_Y = 10000;

  TEST_ASSERT_EQUAL_INT(get_angle_between_two_points(point_A, point_B), 349);
}

void test_get_distance_between_two_points(void) {
  T_point point_A;
  point_A.pos_X = 5000;
  point_A.pos_Y = 11000;

  T_point point_B;
  point_B.pos_X = 17000;
  point_B.pos_Y = 1000;

  TEST_ASSERT_EQUAL_INT(get_distance_between_two_points(point_A, point_B),
                        15620);
}

void test_planet_parsing(void) {
  const char *input = "P 495 10000 20000 -1 0,P 25 3040 40 -1 1,P 1445 340 1 1 "
                      "1,S 0 4 13000 0 0,B 10000 10000";

  T_planet expected_planets[] = {
      {495, 10000, 20000, -1, 0}, {25, 3040, 40, -1, 1}, {1445, 340, 1, 1, 1}};

  T_game_data game_data[NUMBER_OF_GAME_DATA];
  uint8_t number_of_planets;

  parse_planets(input, game_data, &number_of_planets);

  TEST_ASSERT_EQUAL_INT_MESSAGE(
      sizeof(expected_planets) / sizeof(expected_planets[0]), number_of_planets,
      "Number of expected_planets invalid !");

  TEST_ASSERT_EQUAL_INT(3, number_of_planets);

  for (int i = 0; i < number_of_planets; i++) {
    TEST_ASSERT_EQUAL_INT(expected_planets[i].planet_ID,
                          game_data->planets[i].planet_ID);
    TEST_ASSERT_EQUAL_INT(expected_planets[i].pos_X,
                          game_data->planets[i].pos_X);
    TEST_ASSERT_EQUAL_INT(expected_planets[i].pos_Y,
                          game_data->planets[i].pos_Y);
    TEST_ASSERT_EQUAL_INT(expected_planets[i].ship_ID,
                          game_data->planets[i].ship_ID);
    TEST_ASSERT_EQUAL_INT(expected_planets[i].planet_saved,
                          game_data->planets[i].planet_saved);
  }
}

void test_ship_parsing(void) {
  const char *input = "P 495 10000 20000 -1 0,P 95 1000 200 -1 0,S 0 1 10000 "
                      "253 1,S 0 2 12000 53 "
                      "0,S 0 3 10 13 0,S 4 4 4 4 4,B 11111 11111";

  T_ship expected_ships[] = {{0, 1, 10000, 253, 1},
                             {0, 2, 12000, 53, 0},
                             {0, 3, 10, 13, 0},
                             {4, 4, 4, 4, 4}};
  T_game_data game_data[NUMBER_OF_GAME_DATA];
  uint8_t number_of_ships = 4;

  parse_ships(input, game_data);

  for (int i = 0; i < number_of_ships; i++) {
    TEST_ASSERT_EQUAL_INT(expected_ships[i].team_ID,
                          game_data->ships[i].team_ID);
    TEST_ASSERT_EQUAL_INT(expected_ships[i].ship_ID,
                          game_data->ships[i].ship_ID);
    TEST_ASSERT_EQUAL_INT(expected_ships[i].pos_X, game_data->ships[i].pos_X);
    TEST_ASSERT_EQUAL_INT(expected_ships[i].pos_Y, game_data->ships[i].pos_Y);
    TEST_ASSERT_EQUAL_INT(expected_ships[i].broken, game_data->ships[i].broken);
  }
}

void test_get_ship_position(void) {
  T_ship ship = {0, 1, 1520, 11325, 0};
  T_point ship_pos = get_ship_position(ship);
  T_point exepected_ship_pos = {1520, 11325};

  TEST_ASSERT_EQUAL_INT(exepected_ship_pos.pos_X, ship_pos.pos_X);
  TEST_ASSERT_EQUAL_INT(exepected_ship_pos.pos_Y, ship_pos.pos_Y);
}

void test_get_planet_position(void) {
  T_planet planet = {495, 10000, 20000, -1, 0};
  T_point planet_pos = get_planet_position(planet);
  T_point exepected_planet_pos = {10000, 20000};

  TEST_ASSERT_EQUAL_INT(exepected_planet_pos.pos_X, planet_pos.pos_X);
  TEST_ASSERT_EQUAL_INT(exepected_planet_pos.pos_Y, planet_pos.pos_Y);
}

void test_get_base_position(void) {
  T_base base = {10000, 0};
  T_point base_pos = get_base_position(base);
  T_point exepected_base_pos = {10000, 0};

  TEST_ASSERT_EQUAL_INT(exepected_base_pos.pos_X, base_pos.pos_X);
  TEST_ASSERT_EQUAL_INT(exepected_base_pos.pos_Y, base_pos.pos_Y);
}

void test_base_parsing(void) {
  const char *input =
      "P 495 10000 20000 -1 0,P 25 3040 40 -1 1,P 11113 15410 6100 7 0,S 0 1 "
      "10000 253 1,S 0 2 11500 0 0,S 0 3 8500 1546 0, S 0 4 13000 0 0, S 0 5 "
      "7000 19999 1, S 0 6 14500 0 0,S 0 7 5500 21 0, S 0 8 16000 0 0, S 0 9 "
      "4000 0 1,B 10000 0";

  T_base expected_base = {10000, 0};
  T_base parsed_base[BASE_NUMBER];

  T_game_data game_data[NUMBER_OF_GAME_DATA];

  parse_base(input, game_data);

  TEST_ASSERT_EQUAL_INT(expected_base.pos_X, game_data->base.pos_X);
  TEST_ASSERT_EQUAL_INT(expected_base.pos_Y, game_data->base.pos_Y);
}

void test_get_nearest_planet(void) {
  uint16_t expected_nearest_planet_id = 7;
  uint16_t nearest_planet_id = get_nearest_planet(8, game_data);
  TEST_ASSERT_EQUAL_INT(expected_nearest_planet_id, nearest_planet_id);
}

void test_check_desired_ship_speed(void) {
  uint16_t expected_speed = 1000;
  uint16_t speed = check_desired_ship_speed(COLLECTOR_1, 1200);
  TEST_ASSERT_EQUAL_INT(expected_speed, speed);
}

void test_set_planet_collection_status(void) {
  uint8_t ship_ID = 1;
  uint8_t planet_ID = 2;
  set_planet_collection_status(ship_ID, planet_ID, COLLECTED, game_data);
  TEST_ASSERT_EQUAL_INT(game_data->planets[planet_ID].busy_ship_ID, ship_ID);
  TEST_ASSERT_EQUAL_INT(game_data->planets[planet_ID].planet_status, COLLECTED);
}

void test_update_planet_collection_status(void) {
  T_game_data test_game_data[NUMBER_OF_GAME_DATA] = {
      {{{1, 10000, 20000, -1, 0, -1, FREE},
        {2, 10000, 20000, -1, 0, -1, FREE},
        {3, 10000, 20000, -1, 0, -1, FREE},
        {4, 10000, 20000, COLLECTOR_1, 0, -1,
         FREE}, // Case where ship grabbed a planet, but planet status &
                // busy_ship_ID aren't updated yet
        {5, 10000, 20000, -1, 0, COLLECTOR_2,
         COLLECTING}, // Case where ship is destroyed while collecting a planet
        {6, 10000, 20000, -1, 0, COLLECTOR_2,
         COLLECTING_INCOMING}, // Case where ship is destroyed while going to
                               // collect a planet
        {7, 10000, 20000, COLLECTOR_1, 1, COLLECTOR_1,
         COLLECTING_INCOMING}, // Case where collector 1 collected a planet but
                               // planet status isn't updated yet
        {8, 10000, 12000, -1, 0, -1, FREE}},
       {{0, ATTACKER_1, 0, 0, 0},
        {0, ATTACKER_2, 0, 0, 0},
        {0, ATTACKER_3, 0, 0, 0},
        {0, ATTACKER_4, 0, 0, 0},
        {0, ATTACKER_5, 0, 0, 0},
        {0, EXPLORER_1, 0, 0, 0},
        {0, EXPLORER_2, 0, 0, 0},
        {0, COLLECTOR_1, 10000, 10000, 0},
        {0, COLLECTOR_2, 10000, 1000, 1}},
       {10000, 0}}};
  update_planet_collection_status_2(test_game_data);
  TEST_ASSERT_EQUAL_INT(COLLECTOR_1,
                        test_game_data->planets[3].busy_ship_ID); // PLANET 4
  TEST_ASSERT_EQUAL_INT(COLLECTING,
                        test_game_data->planets[3].planet_status); // PLANET 4

  TEST_ASSERT_EQUAL_INT(FREE,
                        test_game_data->planets[4].planet_status); // PLANET 5
  TEST_ASSERT_EQUAL_INT(-1,
                        test_game_data->planets[4].busy_ship_ID); // PLANET 5

  TEST_ASSERT_EQUAL_INT(FREE,
                        test_game_data->planets[5].planet_status); // PLANET 6
  TEST_ASSERT_EQUAL_INT(-1,
                        test_game_data->planets[5].busy_ship_ID); // PLANET 6

  // TEST_ASSERT_EQUAL_INT(COLLECTED,
  //                       test_game_data->planets[6].planet_status); // PLANET
  //                       7

  TEST_ASSERT_EQUAL_INT(test_game_data->planets[6].ship_ID,
                        test_game_data->planets[6].busy_ship_ID); // PLANET 7
}

void test_get_nearest_planet_available(void) {
  T_game_data test_game_data[NUMBER_OF_GAME_DATA] = {
      {{{1, 10000, 10000, -1, 0, -1, FREE},
        {2, 10000, 20000, -1, 0, -1, FREE},
        {3, 15000, 15000, -1, 0, -1, FREE},
        {4, 10000, 20000, -1, 0, -1, FREE},
        {5, 10000, 10000, -1, 0, -1, FREE},
        {6, 10000, 20000, -1, 0, -1, FREE},
        {7, 16500, 17500, -1, 0, -1, FREE},
        {8, 10000, 12000, -1, 0, -1, FREE}},
       {{0, ATTACKER_1, 0, 0, 0},
        {0, ATTACKER_2, 0, 0, 0},
        {0, ATTACKER_3, 0, 0, 0},
        {0, ATTACKER_4, 0, 0, 0},
        {0, ATTACKER_5, 0, 0, 0},
        {0, EXPLORER_1, 0, 0, 0},
        {0, EXPLORER_2, 0, 0, 0},
        {0, COLLECTOR_1, 16000, 17000, 0},
        {0, COLLECTOR_2, 10000, 1000, 1}},
       {10000, 0}}};

  update_planet_collection_status_2(test_game_data);
  TEST_ASSERT_EQUAL_INT(
      6, get_nearest_planet_available(COLLECTOR_1, test_game_data));
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_get_angle_between_two_points);
  RUN_TEST(test_get_distance_between_two_points);
  RUN_TEST(test_planet_parsing);
  RUN_TEST(test_ship_parsing);
  RUN_TEST(test_get_ship_position);
  RUN_TEST(test_get_planet_position);
  RUN_TEST(test_get_base_position);
  RUN_TEST(test_base_parsing);
  RUN_TEST(test_get_nearest_planet);
  RUN_TEST(test_check_desired_ship_speed);
  RUN_TEST(test_set_planet_collection_status);
  RUN_TEST(test_update_planet_collection_status);
  RUN_TEST(test_get_nearest_planet_available);
  UNITY_END(); // stop unit testing
}