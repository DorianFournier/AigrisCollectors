#include "game_engine.h"
#include "OS_engine.h"

uint8_t nb_planets = 0;
T_game_data game_data[NUMBER_OF_GAME_DATA];

uint16_t check_desired_ship_speed(uint8_t ship_id, uint16_t desired_speed) {
  uint16_t speed = 0;

  if ((ship_id >= ATTACKER_1) && (ship_id <= ATTACKER_5)) {
    if (desired_speed > ATTACKER_SPEED)
      speed = ATTACKER_SPEED;
    else
      speed = desired_speed;
  } else if (ship_id == EXPLORER_1 || ship_id == EXPLORER_2) {
    if (desired_speed > EXPLORER_SPEED)
      speed = EXPLORER_SPEED;
    else
      speed = desired_speed;
  } else if (ship_id == COLLECTOR_1 || ship_id == COLLECTOR_2) {
    if (desired_speed > COLLECTOR_SPEED)
      speed = COLLECTOR_SPEED;
    else
      speed = desired_speed;
  }

  return speed;
}

char *generate_command(T_command_type command_type, uint8_t ship_id,
                       uint16_t angle, uint16_t speed) {
  static char command_buffer[BUFFER_SIZE];

  switch (command_type) {
  case MOVE_CMD:
    speed = check_desired_ship_speed(ship_id, speed);
    snprintf(command_buffer, BUFFER_SIZE, "MOVE %d %d %d\n", ship_id + 1, angle,
             speed);
    break;
  case FIRE_CMD:
    if (ship_id >= ATTACKER_1 && ship_id <= ATTACKER_5) {
      snprintf(command_buffer, BUFFER_SIZE, "FIRE %d %d\n", ship_id + 1, angle);
    }
    break;
  case RADAR_CMD:
    // TODO add EXPLORER_2 treatment
    if (ship_id == EXPLORER_1) {
      snprintf(command_buffer, BUFFER_SIZE, "RADAR %d\n", ship_id + 1);
    }
    break;
  }

  return command_buffer;
}

void explorer_manager(uint8_t explorer_id) {

  static char answer_buffer[RX_COMMAND_BUFFER_SIZE] = {0};
  initialize_game_data(game_data);

  while (1) {
    aquire_game_data_mutex();

    send_command_radar(generate_command(RADAR_CMD, EXPLORER_1, 0, 0),
                       answer_buffer);

    parse_planets(answer_buffer, game_data, &nb_planets);
    parse_ships(answer_buffer, game_data);
    parse_base(answer_buffer, game_data);

    // update_planet_collection_status_2(game_data);

    release_game_data_mutex();

    // memset(answer_buffer, 0, sizeof(answer_buffer));
    os_delay(OS_DELAY);
  }
}

void collector_manager(uint8_t collector_id) {
  while (1) {
    aquire_game_data_mutex();

    if (collector_id == COLLECTOR_1) {
      auto_collect_planet_collector_1(collector_id, game_data);
    } else if (collector_id == COLLECTOR_2) {
      auto_collect_planet_collector_2(collector_id, game_data);
    }
    // auto_collect_planet_2(collector_id, game_data);
    // auto_collect_planet_2(COLLECTOR_2, game_data);
    //  auto_collect_planet_2(collector_id, game_data);
    //   os_delay(OS_DELAY + 20);
    //   auto_collect_planet(COLLECTOR_2, game_data);

    // uint8_t planet_id = get_nearest_planet(COLLECTOR_1, game_data);

    // if (game_data->planets[planet_id].ship_ID != -1) {
    //   go_to_base(game_data->ships[COLLECTOR_1], game_data->base,
    //              COLLECTOR_SPEED);
    // } else {
    //   go_to_planet(game_data->ships[COLLECTOR_1],
    //                game_data->planets[planet_id]);
    // }

    release_game_data_mutex();
    os_delay(OS_DELAY);
  }
}

void attacker_manager(uint8_t id) {
  while (1) {
    aquire_game_data_mutex();

    if (id == ATTACKER_1) {
      follow_ship_new(id, game_data->ships[COLLECTOR_2]);
    }

    if (id == ATTACKER_1 || id == ATTACKER_2 || id == ATTACKER_3 ||
        id == ATTACKER_4 || id == ATTACKER_5) {
      send_command(generate_command(FIRE_CMD, id, 90, 0));
    }

    release_game_data_mutex();
    os_delay(OS_DELAY);
  }
}

uint16_t get_distance_between_two_points(T_point starting_point,
                                         T_point ending_point) {
  return sqrt(pow(ending_point.pos_X - starting_point.pos_X, 2) +
              pow(ending_point.pos_Y - starting_point.pos_Y, 2));
}

uint16_t get_angle_between_two_points(T_point starting_point,
                                      T_point ending_point) {
  double angle_radian = atan2(ending_point.pos_Y - starting_point.pos_Y,
                              ending_point.pos_X - starting_point.pos_X);
  int16_t angle_degree = (int16_t)(angle_radian * (180.0 / M_PI));

  if (angle_degree < 0)
    angle_degree += 360;

  return (uint16_t)angle_degree;
}

T_point get_ship_position(T_ship ship) {
  T_point ship_pos = {ship.pos_X, ship.pos_Y};
  return ship_pos;
}

T_point get_planet_position(T_planet planet) {
  T_point planet_pos = {planet.pos_X, planet.pos_Y};
  return planet_pos;
}

T_point get_base_position(T_base base) {
  T_point base_pos = {base.pos_X, base.pos_Y};
  return base_pos;
}

void go_to_point(T_ship ship, T_point point) {
  T_point ship_pos = get_ship_position(ship);

  send_command(generate_command(MOVE_CMD, ship.ship_ID,
                                get_angle_between_two_points(ship_pos, point),
                                ATTACKER_SPEED));
}

void go_to_planet(T_ship ship, T_planet planet) {
  T_point ship_pos = get_ship_position(ship);
  T_point planet_pos = get_planet_position(planet);

  send_command(generate_command(
      MOVE_CMD, ship.ship_ID,
      get_angle_between_two_points(ship_pos, planet_pos), COLLECTOR_SPEED));
}

void go_to_planet_new(uint8_t ship_id, T_planet planet) {
  T_ship ship = game_data->ships[ship_id];
  T_point ship_pos = get_ship_position(ship);
  T_point planet_pos = get_planet_position(planet);
  if (ship_id == COLLECTOR_1) {
    send_command(generate_command(
        MOVE_CMD, ship_id, get_angle_between_two_points(ship_pos, planet_pos),
        400));
  } else {
    send_command(generate_command(
        MOVE_CMD, ship_id, get_angle_between_two_points(ship_pos, planet_pos),
        COLLECTOR_SPEED));
  }
}

void go_to_base(T_ship ship, T_base base, T_ships_speed ship_speed) {
  T_point ship_pos = get_ship_position(ship);
  T_point base_pos = get_base_position(base);

  send_command(generate_command(
      MOVE_CMD, ship.ship_ID, get_angle_between_two_points(ship_pos, base_pos),
      ship_speed));
}

void go_to_base_new(uint8_t ship_id, T_base base, T_ships_speed ship_speed) {
  T_ship ship = game_data->ships[ship_id];
  T_point ship_pos = get_ship_position(ship);
  T_point base_pos = get_base_position(base);

  send_command(generate_command(
      MOVE_CMD, ship_id, get_angle_between_two_points(ship_pos, base_pos),
      ship_speed));
}

void parse_game_data(char *answer_buffer, T_game_data *game_data) {
  // aquire_game_data_mutex();
  parse_planets(answer_buffer, game_data, &nb_planets);
  parse_ships(answer_buffer, game_data);
  parse_base(answer_buffer, game_data);
  // update_planet_collection_status_2(game_data);
  //  release_game_data_mutex();
}

void parse_planets(const char *server_response, T_game_data *game_data,
                   uint8_t *num_planets) {
  *num_planets = 0;
  const char *str = server_response;
  const char *delimiter = strchr(str, SERVER_RESPONSE_DELIMITER[0]);

  while (delimiter != NULL) {
    if (*str == SERVER_RESPONSE_PLANET_DELIMITER) {
      if (*num_planets >= MAX_PLANETS_NUMBER) {
        break;
      }
      sscanf(str, "P %hu %hu %hu %hu %hu",
             &game_data->planets[*num_planets].planet_ID,
             &game_data->planets[*num_planets].pos_X,
             &game_data->planets[*num_planets].pos_Y,
             &game_data->planets[*num_planets].ship_ID,
             &game_data->planets[*num_planets].planet_saved);
      (*num_planets)++;
    }

    str = delimiter + 1;
    delimiter = strchr(str, SERVER_RESPONSE_DELIMITER[0]);
  }

  // update_planet_collection_status_2(game_data);
}

void parse_ships(const char *server_response, T_game_data *game_data) {
  uint8_t num_ships = 0;
  const char *str = server_response;
  const char *delimiter = strchr(str, SERVER_RESPONSE_DELIMITER[0]);

  while (delimiter != NULL) {
    if (*str == SERVER_RESPONSE_SHIP_DELIMITER) {
      if (num_ships >= SHIPS_NUMBER * NUMBER_OF_TEAM) {
        while (1) {
          ;
        }
      }
      sscanf(str, "S %hu %hu %hu %hu %hu", &game_data->ships[num_ships].team_ID,
             &game_data->ships[num_ships].ship_ID,
             &game_data->ships[num_ships].pos_X,
             &game_data->ships[num_ships].pos_Y,
             &game_data->ships[num_ships].broken);
      num_ships++;
    }

    str = delimiter + 1;
    delimiter = strchr(str, SERVER_RESPONSE_DELIMITER[0]);
  }
}

void parse_base(const char *server_response, T_game_data *game_data) {
  const char *str_token;
  char *save_ptr;
  char server_response_copy[strlen(server_response) + 1];
  strcpy(server_response_copy, server_response);

  str_token =
      strtok_r(server_response_copy, SERVER_RESPONSE_DELIMITER, &save_ptr);
  while (str_token != NULL) {
    if (str_token[0] == 'B') {
      sscanf(str_token, "B %hu %hu", &game_data->base.pos_X,
             &game_data->base.pos_Y);
    }

    str_token = strtok_r(NULL, SERVER_RESPONSE_DELIMITER, &save_ptr);
  }
}

void show_planet(T_planet *planet) {
  char buffer[RX_COMMAND_BUFFER_SIZE];

  for (uint8_t id = 0; id < nb_planets; id++) {
    sprintf(
        buffer,
        "planet_ID: %u, pos_X: %u, pos_Y: %u, ship_ID: %d, planet_saved: %u",
        planet[id].planet_ID, planet[id].pos_X, planet[id].pos_Y,
        planet[id].ship_ID, planet[id].planet_saved);

    puts(buffer);
  }
}

void initialize_game_data(T_game_data *game_data) {
  for (int i = 0; i < MAX_PLANETS_NUMBER; i++) {
    game_data->planets[i].planet_ID = 0;
    game_data->planets[i].pos_X = 0;
    game_data->planets[i].pos_Y = 0;
    game_data->planets[i].ship_ID = -1;
    game_data->planets[i].planet_saved = 0;
    game_data->planets[i].busy_ship_ID = -1;
    game_data->planets[i].planet_status = FREE;
  }

  for (int i = 0; i < SHIPS_NUMBER * NUMBER_OF_TEAM; i++) {
    game_data->ships[i].team_ID = 0;
    game_data->ships[i].ship_ID = 0;
    game_data->ships[i].pos_X = 0;
    game_data->ships[i].pos_Y = 0;
    game_data->ships[i].broken = 0;
  }

  game_data->base.pos_X = 0;
  game_data->base.pos_Y = 0;

  // update_planet_collection_status_2(game_data);
}

void follow_ship(T_ship follower_ship, T_ship ship_to_follow) {
  T_point follower_ship_pos = get_ship_position(follower_ship);
  T_point ship_to_follow_pos = get_ship_position(ship_to_follow);

  send_command(generate_command(
      MOVE_CMD, follower_ship.ship_ID,
      get_angle_between_two_points(follower_ship_pos, ship_to_follow_pos),
      ATTACKER_SPEED));
}

void follow_ship_new(uint8_t follower_ship_id, T_ship ship_to_follow) {
  T_ship follower_ship = game_data->ships[follower_ship_id];
  T_point follower_ship_pos = get_ship_position(follower_ship);
  T_point ship_to_follow_pos = get_ship_position(ship_to_follow);

  send_command(generate_command(
      MOVE_CMD, follower_ship_id,
      get_angle_between_two_points(follower_ship_pos, ship_to_follow_pos),
      ATTACKER_SPEED));
}

uint8_t get_nearest_planet(uint8_t ship_id, T_game_data *game_data) {
  uint16_t distance = 0;
  uint16_t distance_min = MAX_DISTANCE_BETWEEN_POINT;
  uint8_t planet_id_to_collect = 0;

  for (uint8_t planet_num = 0; planet_num < MAX_PLANETS_NUMBER; planet_num++) {
    if (game_data->planets[planet_num].planet_saved != 1 &&
        game_data->planets[planet_num].planet_ID != 0) {

      distance = get_distance_between_two_points(
          get_ship_position(game_data->ships[ship_id]),
          get_planet_position(game_data->planets[planet_num]));

      // printf("Distance / ship_id -> planet_id : %d / %d -> %d\n", distance,
      //        ship_id, planet_num);
      if (distance < distance_min) {
        // printf("Distance / ship_id -> planet_id : %d / %d -> %d\n",
        // distance,
        //        ship_num, planet_num);
        distance_min = distance;
        planet_id_to_collect = planet_num;
      }
    }
  }

  return planet_id_to_collect;
}

uint8_t get_nearest_planet_available(uint8_t ship_id, T_game_data *game_data) {
  uint16_t distance = 0;
  uint16_t distance_min = MAX_DISTANCE_BETWEEN_POINT;
  uint8_t planet_id_to_collect = 0;

  for (uint8_t planet_num = 0; planet_num < MAX_PLANETS_NUMBER; planet_num++) {

    if (game_data->planets[planet_num].busy_ship_ID == -1 &&
        game_data->planets[planet_num].planet_ID != 0) {

      distance = get_distance_between_two_points(
          get_ship_position(game_data->ships[ship_id]),
          get_planet_position(game_data->planets[planet_num]));

      // printf("Distance / ship_id -> planet_id : %d / %d -> %d\n", distance,
      //        ship_id, planet_num);
      if (distance < distance_min) {
        // printf("Distance / ship_id -> planet_id : %d / %d -> %d\n",
        // distance,
        //        ship_num, planet_num);
        distance_min = distance;
        planet_id_to_collect = planet_num;
      }
    }
  }
  return planet_id_to_collect;
}

void set_planet_collection_status(int8_t busy_ship_ID, uint8_t planet_num,
                                  T_planet_status planet_status,
                                  T_game_data *game_data) {
  game_data->planets[planet_num].busy_ship_ID = busy_ship_ID;
  game_data->planets[planet_num].planet_status = planet_status;
}

/*
void update_planet_collection_status(T_game_data *game_data) {
  for (uint8_t planet_num = 0; planet_num < MAX_PLANETS_NUMBER; planet_num++) {
    if (game_data->planets[planet_num].planet_saved ==
        1) // Manages a planet that was collected
    {
      set_planet_collection_status(
          game_data->planets[planet_num].ship_ID, planet_num, COLLECTED,
          game_data); // -1 means no planet is currently on the planet
    }

    else if (game_data->planets[planet_num].ship_ID !=
             -1) // Manages a planet being collected
    {
      set_planet_collection_status(game_data->planets[planet_num].ship_ID,
                                   planet_num, COLLECTING, game_data);
    } else if (game_data->planets[planet_num].busy_ship_ID != -1 ||
               game_data->planets[planet_num].planet_status !=
                   FREE) // Manages a busy planet with a destroyed ship
    {
      int8_t actual_busy_ship = game_data->planets[planet_num].busy_ship_ID;
      if (game_data->ships[actual_busy_ship].broken == 1) {
        set_planet_collection_status(-1, planet_num, FREE, game_data);
      }
    }
  }
}

void auto_collect_planet(uint8_t ship_id, T_game_data *game_data) {
  update_planet_collection_status(game_data);
  uint8_t planet_id = 0;
  bool flag = false;

  for (uint8_t planet_num = 0; planet_num < MAX_PLANETS_NUMBER; planet_num++) {
    if (game_data->planets[planet_num].busy_ship_ID == ship_id) {
      planet_id = planet_num;
      break;
    } else {
      flag = true;
    }
  }
  if (flag)
    planet_id = get_nearest_planet_available(ship_id, game_data);

  // if (game_data->planets[planet_id].planet_status == FREE ||
  //     game_data->planets[planet_id].planet_status == COLLECTING_INCOMING)
  if (game_data->planets[planet_id].planet_status == FREE) {
    go_to_planet(game_data->ships[ship_id], game_data->planets[planet_id]);
    set_planet_collection_status(ship_id, planet_id, COLLECTING_INCOMING,
                                 game_data);
  } else if (game_data->planets[planet_id].planet_status ==
                 COLLECTING_INCOMING &&
             game_data->planets[planet_id].busy_ship_ID == ship_id) {
    go_to_planet(game_data->ships[ship_id], game_data->planets[planet_id]);
  } else if (game_data->planets[planet_id].planet_status == COLLECTING ||
             game_data->ships[ship_id].broken == 1) {
    go_to_base(game_data->ships[ship_id], game_data->base, COLLECTOR_SPEED);
  }
  // else {
  //   putsMutex("ici");
  // }
}
*/

void update_planet_collection_status_2(T_game_data *game_data) {
  for (uint8_t planet_num = 0; planet_num < MAX_PLANETS_NUMBER; planet_num++) {

    if ((game_data->planets[planet_num].ship_ID != -1 &&
         game_data->planets[planet_num].planet_saved !=
             1)) // Manages a planet being collected
    {
      // putsMutex(" SHIP ID : ");
      // putsMutex(game_data->planets[planet_num].ship_ID);
      // TODO test with -1
      set_planet_collection_status(game_data->planets[planet_num].ship_ID,
                                   planet_num, COLLECTING, game_data);
    } else if (game_data->planets[planet_num].ship_ID == -1 &&
               (game_data->planets[planet_num].busy_ship_ID != -1 ||
                game_data->planets[planet_num].planet_status !=
                    FREE)) // Manages a busy planet with a destroyed ship
    {
      // int8_t actual_busy_ship = game_data->planets[planet_num].busy_ship_ID;
      // if (game_data->ships[actual_busy_ship].broken == 1) {
      set_planet_collection_status(-1, planet_num, FREE, game_data);
      // }
    }
  }
}

void update_planet_collection_status_new_2(uint8_t ship_id,
                                           T_game_data *game_data) {
  for (uint8_t planet_num = 0; planet_num < MAX_PLANETS_NUMBER; planet_num++) {

    if ((game_data->planets[planet_num].ship_ID != -1 &&
         game_data->planets[planet_num].planet_saved !=
             1)) // Manages a planet being collected
    {
      // putsMutex(" SHIP ID : ");
      // putsMutex(game_data->planets[planet_num].ship_ID);
      // TODO test with -1
      set_planet_collection_status(ship_id, planet_num, COLLECTING, game_data);
    } else if (game_data->planets[planet_num].ship_ID == -1 &&
               (game_data->planets[planet_num].busy_ship_ID != -1 ||
                game_data->planets[planet_num].planet_status !=
                    FREE)) // Manages a busy planet with a destroyed ship
    {
      // int8_t actual_busy_ship = game_data->planets[planet_num].busy_ship_ID;
      // if (game_data->ships[actual_busy_ship].broken == 1) {
      set_planet_collection_status(-1, planet_num, FREE, game_data);
      // }
    }
  }
}

void auto_collect_planet_collector_1(uint8_t ship_id, T_game_data *game_data) {
  // update_planet_collection_status_2(game_data);
  // update_planet_collection_status_new_2(ship_id, game_data);

  static bool is_ship_available = false;
  uint8_t planet_id = MAX_PLANETS_NUMBER;
  planet_id = is_ship_have_a_planet(ship_id, game_data);

  if (planet_id == MAX_PLANETS_NUMBER) {
    for (uint8_t planet_num = 0; planet_num < MAX_PLANETS_NUMBER;
         planet_num++) {
      if (game_data->planets[planet_num].busy_ship_ID == (ship_id) &&
          game_data->planets[planet_num].ship_ID == -1 &&
          game_data->planets[planet_num].planet_status != COLLECTING) {
        is_ship_available = false;
        // go_to_planet(game_data->ships[ship_id],
        // game_data->planets[planet_num]);

        go_to_planet_new(ship_id, game_data->planets[planet_num]);
        set_planet_collection_status(ship_id, planet_num, COLLECTING_INCOMING,
                                     game_data);

        break;
      } else if (game_data->planets[planet_num].busy_ship_ID == (ship_id) &&
                 game_data->planets[planet_num].ship_ID != -1 &&
                 game_data->planets[planet_num].planet_saved != 1) {
        is_ship_available = false;
        // go_to_base(game_data->ships[ship_id], game_data->base,
        // COLLECTOR_SPEED);

        go_to_base_new(ship_id, game_data->base, COLLECTOR_SPEED);
        set_planet_collection_status(ship_id, planet_num, COLLECTING,
                                     game_data);
        // while (1) {
        //   putsMutex("ON RENTRE JAMAISD LA GROOSS");
        //   os_delay(OS_DELAY);
        // }

        break;
      }
      // else if (game_data->planets[planet_num].ship_ID != -1 &&
      //            game_data->planets[planet_num].busy_ship_ID != ship_id) {
      //   // putsMutex("T MORT");
      //   planet_id = planet_num;

      //   // uint8_t stolen_ship_id =
      //   game_data->planets[planet_num].busy_ship_ID;
      //   // for (uint8_t pln = 0; pln < MAX_PLANETS_NUMBER; pln++) {
      //   //   if (game_data->planets[pln].busy_ship_ID == stolen_ship_id) {
      //   //     set_planet_collection_status(-1, planet_num, FREE, game_data);
      //   //     // go_to_planet_new(stolen_ship_id,
      //   //     // game_data->planets[get_nearest_planet_available(
      //   //     //                      stolen_ship_id, game_data)]);
      //   //   }
      //   // }
      //   // go_to_base_new(ship_id, game_data->base, COLLECTOR_SPEED);
      //   // set_planet_collection_status(ship_id, planet_num, COLLECTING,
      //   //  game_data);
      //   //  break;
      // }
      else if (game_data->planets[planet_num].busy_ship_ID == -1) {
        is_ship_available = true;
      }
    }

    if (is_ship_available == true) {
      planet_id = get_nearest_planet_available(ship_id, game_data);
      // go_to_planet(game_data->ships[ship_id], game_data->planets[planet_id]);
      go_to_planet_new(ship_id, game_data->planets[planet_id]);
      set_planet_collection_status(ship_id, planet_id, COLLECTING_INCOMING,
                                   game_data);
    }
  } else if (planet_id != MAX_PLANETS_NUMBER &&
             game_data->planets[planet_id].ship_ID != -1) {
    // putsMutex("ici");
    //  set_planet_collection_status(ship_id, planet_id, COLLECTING, game_data);
    go_to_base_new(ship_id, game_data->base, COLLECTOR_SPEED);
    // set_planet_collection_status(ship_id, planet_id, COLLECTING, game_data);
  } else {
    // putsMutex("ok");
  }

  // update_planet_collection_status_2(game_data);
}

uint8_t is_ship_have_a_planet(uint8_t ship_id, T_game_data *game_data) {
  for (uint8_t planet_num = 0; planet_num < MAX_PLANETS_NUMBER; planet_num++) {
    if (game_data->planets[planet_num].busy_ship_ID != ship_id &&
        game_data->planets[planet_num].ship_ID == ship_id) {
      return planet_num;
    }
  }
  return MAX_PLANETS_NUMBER;
}

void auto_collect_planet_collector_2(uint8_t ship_id, T_game_data *game_data) {
  // update_planet_collection_status_2(game_data);
  // update_planet_collection_status_new_2(ship_id, game_data);

  static bool is_ship_available = false;

  uint8_t planet_id = MAX_PLANETS_NUMBER;
  planet_id = is_ship_have_a_planet(ship_id, game_data);

  if (planet_id == MAX_PLANETS_NUMBER) {
    for (uint8_t planet_num = 0; planet_num < MAX_PLANETS_NUMBER;
         planet_num++) {
      if (game_data->planets[planet_num].busy_ship_ID == (ship_id) &&
          game_data->planets[planet_num].ship_ID == -1 &&
          game_data->planets[planet_num].planet_status != COLLECTING) {
        is_ship_available = false;
        // go_to_planet(game_data->ships[ship_id],
        // game_data->planets[planet_num]);

        go_to_planet_new(ship_id, game_data->planets[planet_num]);
        set_planet_collection_status(ship_id, planet_num, COLLECTING_INCOMING,
                                     game_data);

        break;
      } else if (game_data->planets[planet_num].busy_ship_ID == (ship_id) &&
                 game_data->planets[planet_num].ship_ID != -1 &&
                 game_data->planets[planet_num].planet_saved != 1) {
        is_ship_available = false;
        // go_to_base(game_data->ships[ship_id], game_data->base,
        // COLLECTOR_SPEED);

        go_to_base_new(ship_id, game_data->base, COLLECTOR_SPEED);
        set_planet_collection_status(ship_id, planet_num, COLLECTING,
                                     game_data);
        // while (1) {
        //   putsMutex("ON RENTRE JAMAISD LA GROOSS");
        //   os_delay(OS_DELAY);
        // }

        break;
      } else if (game_data->planets[planet_num].ship_ID != -1 &&
                 game_data->planets[planet_num].busy_ship_ID != ship_id) {
        // putsMutex("T MORT");
        // planet_id = planet_num;

        uint8_t stolen_ship_id = game_data->planets[planet_num].busy_ship_ID;
        for (uint8_t pln = 0; pln < MAX_PLANETS_NUMBER; pln++) {
          if (game_data->planets[pln].busy_ship_ID == stolen_ship_id) {
            set_planet_collection_status(-1, planet_num, FREE, game_data);
            go_to_planet_new(stolen_ship_id,
                             game_data->planets[get_nearest_planet_available(
                                 stolen_ship_id, game_data)]);
          }
        }
        go_to_base_new(ship_id, game_data->base, COLLECTOR_SPEED);
        set_planet_collection_status(ship_id, planet_num, COLLECTING,
                                     game_data);
        break;
      } else if (game_data->planets[planet_num].busy_ship_ID == -1) {
        is_ship_available = true;
      }
    }

    if (is_ship_available == true) {
      planet_id = get_nearest_planet_available(ship_id, game_data);
      // go_to_planet(game_data->ships[ship_id], game_data->planets[planet_id]);
      go_to_planet_new(ship_id, game_data->planets[planet_id]);
      set_planet_collection_status(ship_id, planet_id, COLLECTING_INCOMING,
                                   game_data);
    }
  } else if (planet_id != MAX_PLANETS_NUMBER &&
             game_data->planets[planet_id].ship_ID != -1) {
    // putsMutex("ici");
    //  set_planet_collection_status(ship_id, planet_id, COLLECTING, game_data);
    // go_to_base_new(ship_id, game_data->base, COLLECTOR_SPEED);
    // set_planet_collection_status(ship_id, planet_id, COLLECTING, game_data);
  } else {
    // putsMutex("ok");
  }

  // update_planet_collection_status_2(game_data);
}