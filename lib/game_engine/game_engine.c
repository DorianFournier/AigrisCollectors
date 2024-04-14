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
    if (ship_id == EXPLORER_1 || ship_id == EXPLORER_2) {
      snprintf(command_buffer, BUFFER_SIZE, "RADAR %d\n", ship_id + 1);
    }
    break;
  }

  return command_buffer;
}

void explorer_manager(uint8_t explorer_id) {

  static char answer_buffer[RX_COMMAND_BUFFER_SIZE] = {0};

  while (1) {
    aquire_game_data_mutex();

    send_command_radar(generate_command(RADAR_CMD, explorer_id, 0, 0),
                       answer_buffer);

    parse_planets(answer_buffer, game_data, &nb_planets);
    parse_ships(answer_buffer, game_data);
    parse_base(answer_buffer, game_data);

    if (explorer_id == EXPLORER_1) {
      if (can_ship_be_BROKEN(explorer_id, game_data)) {
        go_to_base(explorer_id, game_data->base, EXPLORER_SPEED);
      } else {
        // follow_ship(explorer_id, COLLECTOR_1, EXPLORER_SPEED, BOTTOM_RIGHT);
        set_ship(explorer_id, game_data);
      }

    } else if (explorer_id == EXPLORER_2) {
      if (can_ship_be_BROKEN(explorer_id, game_data)) {
        go_to_base(explorer_id, game_data->base, EXPLORER_SPEED);
      } else {
        // follow_ship(explorer_id, COLLECTOR_2, EXPLORER_SPEED, BOTTOM_RIGHT);
        set_ship(explorer_id, game_data);
      }
    }
    release_game_data_mutex();
    // memset(answer_buffer, 0, sizeof(answer_buffer));
    os_delay(OS_DELAY);
  }
}

void collector_manager(uint8_t collector_id) {
  while (1) {
    aquire_game_data_mutex();
    auto_collect_planet(collector_id, game_data);

    release_game_data_mutex();
    os_delay(OS_DELAY);
  }
}

void attacker_manager(uint8_t attacker_id) {
  while (1) {
    aquire_game_data_mutex();

    if (attacker_id == ATTACKER_1) {
      if (can_ship_be_BROKEN(attacker_id, game_data)) {
        go_to_base(attacker_id, game_data->base, ATTACKER_SPEED);
      } else {
        // follow_ship(attacker_id, COLLECTOR_1, ATTACKER_SPEED, TOP_LEFT);
        set_ship(attacker_id, game_data);
      }
    } else if (attacker_id == ATTACKER_2) {
      if (can_ship_be_BROKEN(attacker_id, game_data)) {
        go_to_base(attacker_id, game_data->base, ATTACKER_SPEED);
      } else {
        // follow_ship(attacker_id, COLLECTOR_1, ATTACKER_SPEED, TOP_RIGHT);
        set_ship(attacker_id, game_data);
      }
    } else if (attacker_id == ATTACKER_3) {
      if (can_ship_be_BROKEN(attacker_id, game_data)) {
        go_to_base(attacker_id, game_data->base, ATTACKER_SPEED);
      } else {
        // follow_ship(attacker_id, COLLECTOR_2, ATTACKER_SPEED, TOP_LEFT);
        set_ship(attacker_id, game_data);
      }
    } else if (attacker_id == ATTACKER_4) {
      if (can_ship_be_BROKEN(attacker_id, game_data)) {
        go_to_base(attacker_id, game_data->base, ATTACKER_SPEED);
      } else {
        // follow_ship(attacker_id, COLLECTOR_2, ATTACKER_SPEED, TOP_RIGHT);
        set_ship(attacker_id, game_data);
      }
    } else if (attacker_id == ATTACKER_5) {
      if (can_ship_be_BROKEN(attacker_id, game_data)) {
        go_to_base(attacker_id, game_data->base, ATTACKER_SPEED);
      } else {
        // follow_ship(attacker_id, COLLECTOR_2, ATTACKER_SPEED, BOTTOM_LEFT);
        set_ship(attacker_id, game_data);
      }
    }

    if (attacker_id == ATTACKER_1 || attacker_id == ATTACKER_2 ||
        attacker_id == ATTACKER_3 || attacker_id == ATTACKER_4 ||
        attacker_id == ATTACKER_5) {
      if (get_ship_FSM(attacker_id, game_data) == READY &&
          can_ship_be_BROKEN(attacker_id, game_data) == false) {

        fire_on_enemy_ship(attacker_id,
                           find_nearest_enemy_ship_id(attacker_id, game_data),
                           game_data);
      }
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

void go_to_point(uint8_t ship_id, T_point point) {
  T_ship ship = game_data->ships[ship_id];
  T_point ship_pos = get_ship_position(ship);
  if (MOVE_CMD_ANGLE_THRESHOLD <
      abs(get_angle_between_two_points(ship_pos, point) -
          game_data->ships[ship_id].angle)) {
    game_data->ships[ship_id].angle =
        get_angle_between_two_points(ship_pos, ship_pos);
    send_command(generate_command(
        MOVE_CMD, ship_id, game_data->ships[ship_id].angle, ATTACKER_SPEED));
  }
}

void go_to_planet(uint8_t ship_id, uint8_t planet_id) {
  T_ship ship = game_data->ships[ship_id];
  T_point ship_pos = get_ship_position(ship);
  T_planet planet = game_data->planets[planet_id];
  T_point planet_pos = get_planet_position(planet);

  if (MOVE_CMD_ANGLE_THRESHOLD <
      abs(get_angle_between_two_points(ship_pos, planet_pos) -
          game_data->ships[ship_id].angle)) {
    game_data->ships[ship_id].angle =
        get_angle_between_two_points(ship_pos, planet_pos);
    game_data->ships[ship_id].angle =
        get_angle_between_two_points(ship_pos, planet_pos);
    send_command(generate_command(
        MOVE_CMD, ship_id, game_data->ships[ship_id].angle, COLLECTOR_SPEED));
  }
}

void go_to_base(uint8_t ship_id, T_base base, T_ships_speed ship_speed) {
  T_ship ship = game_data->ships[ship_id];
  T_point ship_pos = get_ship_position(ship);
  T_point base_pos = get_base_position(base);
  if (MOVE_CMD_ANGLE_THRESHOLD <
      abs(get_angle_between_two_points(ship_pos, base_pos) -
          game_data->ships[ship_id].angle)) {
    game_data->ships[ship_id].angle =
        get_angle_between_two_points(ship_pos, base_pos);
    send_command(generate_command(MOVE_CMD, ship_id,
                                  game_data->ships[ship_id].angle, ship_speed));
  }
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
  reset_undetected_enemy_ships(num_ships, game_data);
}

void reset_undetected_enemy_ships(uint8_t num_ships, T_game_data *game_data) {
  for (uint8_t ship = num_ships; ship <= SHIPS_NUMBER * NUMBER_OF_TEAM;
       ship++) {
    game_data->ships[num_ships].ship_ID = 0;
    game_data->ships[num_ships].pos_X = 0;
    game_data->ships[num_ships].pos_Y = 0;
    game_data->ships[num_ships].broken = 0;
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
  }

  for (int i = 0; i < SHIPS_NUMBER * NUMBER_OF_TEAM; i++) {
    game_data->ships[i].team_ID = 0;
    game_data->ships[i].ship_ID = 0;
    game_data->ships[i].pos_X = 0;
    game_data->ships[i].pos_Y = 0;
    game_data->ships[i].broken = 0;
    game_data->ships[i].FSM = READY;
    game_data->ships[i].target_planet_ID = -1;
    game_data->ships[i].angle = 0;
  }

  game_data->base.pos_X = 0;
  game_data->base.pos_Y = 0;
}

void follow_ship(uint8_t follower_ship_id, uint8_t ship_to_follow_id,
                 uint16_t follower_ship_speed,
                 T_follower_ship_direction relative_position) {

  uint16_t distance_follower_ship = 100;
  // if (follower_ship_id >= EXPLORER_1 && follower_ship_id <= EXPLORER_2) {
  //   distance_follower_ship = EXPLORER_DISTANCE_FOLLOWER_SHIP;
  // } else {
  //   distance_follower_ship = ATTACKER_DISTANCE_FOLLOWER_SHIP;
  // }

  T_ship follower_ship = game_data->ships[follower_ship_id];
  T_point follower_ship_pos = get_ship_position(follower_ship);

  T_point go_to_pos = polar_to_cartesian_coordinates(
      ship_to_follow_id, distance_follower_ship, relative_position, game_data);

  uint16_t distance =
      get_distance_between_two_points(follower_ship_pos, go_to_pos);

  if (distance <= FOLLOW_SHIP_SPEED_DISTANCE_THRESHOLD) {
    follower_ship_speed = check_desired_ship_speed(
        ship_to_follow_id, PLACE_HOLDER_FOLLOWING_MAX_SPEED);
  } else {
    follower_ship_speed =
        check_desired_ship_speed(follower_ship_id,
                                 PLACE_HOLDER_FOLLOWING_MAX_SPEED) -
        250;
  }

  send_command(generate_command(
      MOVE_CMD, follower_ship_id,
      get_angle_between_two_points(follower_ship_pos, go_to_pos),
      follower_ship_speed));
}

void set_ship(uint8_t ship_id, T_game_data *game_data) {
  if (game_data->ships[COLLECTOR_1].pos_Y <
      game_data->ships[COLLECTOR_2].pos_Y) {
    for (uint8_t s_p = ATTACKER_1; s_p <= EXPLORER_2; s_p++) {
      follow_ship(s_p, COLLECTOR_2, COLLECTOR_SPEED, TOP);
    }
  }
  if (game_data->ships[COLLECTOR_1].pos_Y >
      game_data->ships[COLLECTOR_2].pos_Y) {
    for (uint8_t s_p = ATTACKER_1; s_p <= EXPLORER_2; s_p++) {
      follow_ship(s_p, COLLECTOR_1, COLLECTOR_SPEED, TOP);
    }
  }
}

T_ship_type get_ship_type(uint8_t ship_id) {
  if (ship_id >= 0 && ship_id <= 4)
    return ATTACKER;
  else if (ship_id == 5 || ship_id == 6)
    return EXPLORER;
  else if (ship_id == 7 || ship_id == 8)
    return COLLECTOR;
  return UNKNOWN_SHIP;
}

void ship_following_collector(uint8_t ship_id, uint8_t collector_id,
                              T_follower_ship_direction relative_position) {
  // TODO manage speed with get distance between two points

  follow_ship(ship_id, collector_id, 1500, relative_position);
}

uint16_t degres_to_radian(uint16_t degres) { return degres * M_PI / 180.0; }

T_point polar_to_cartesian_coordinates(uint8_t ship_id_to_follow,
                                       uint16_t distance, uint16_t angle,
                                       T_game_data *game_data) {
  T_point cartesian_point;

  uint16_t angle_rad = degres_to_radian(
      get_angle_for_follower_ship(ship_id_to_follow, angle, game_data));

  cartesian_point.pos_X =
      game_data->ships[ship_id_to_follow].pos_X + (distance * cos(angle_rad));
  cartesian_point.pos_Y =
      game_data->ships[ship_id_to_follow].pos_Y + (distance * sin(angle_rad));

  return cartesian_point;
}

uint16_t get_angle_for_follower_ship(uint8_t ship_id, uint16_t angle,
                                     T_game_data *game_data) {
  return ((angle + game_data->ships[ship_id].angle) % 360);
}

uint8_t get_nearest_planet(uint8_t ship_id, T_game_data *game_data) {
  uint16_t distance = 0;
  uint16_t distance_min = MAX_DISTANCE_BETWEEN_POINT;
  uint8_t planet_id_to_collect = 0;

  for (uint8_t planet_num = 0; planet_num < MAX_PLANETS_NUMBER; planet_num++) {
    if (game_data->planets[planet_num].planet_saved != 1 &&
        game_data->planets[planet_num].planet_ID != 0) {
      if (ship_id == COLLECTOR_1) {
        if (game_data->ships[COLLECTOR_2].target_planet_ID != planet_num) {
          distance = get_distance_between_two_points(
              get_ship_position(game_data->ships[ship_id]),
              get_planet_position(game_data->planets[planet_num]));

          if (distance < distance_min) {
            distance_min = distance;
            planet_id_to_collect = planet_num;
          }
        }
      } else {
        if (game_data->ships[COLLECTOR_1].target_planet_ID != planet_num) {
          distance = get_distance_between_two_points(
              get_ship_position(game_data->ships[ship_id]),
              get_planet_position(game_data->planets[planet_num]));

          if (distance < distance_min) {
            distance_min = distance;
            planet_id_to_collect = planet_num;
          }
        }
      }
    }
  }

  return planet_id_to_collect;
}

int8_t get_planet_ID_from_ship(uint8_t ship_id, T_game_data *game_data) {
  for (uint8_t planet_num = 0; planet_num < MAX_PLANETS_NUMBER; planet_num++) {
    if (game_data->planets[planet_num].ship_ID == ship_id + 1) {
      return planet_num;
    }
  }
  return -1;
}

void auto_collect_planet(uint8_t ship_id, T_game_data *game_data) {
  static int8_t collector_1_desired_target_planet_id;
  static int8_t collector_2_desired_target_planet_id;
  int8_t desired_target_planet_id;
  if (ship_id == COLLECTOR_1) {
    desired_target_planet_id = collector_1_desired_target_planet_id;
  } else {
    desired_target_planet_id = collector_2_desired_target_planet_id;
  }

  if (get_ship_FSM(ship_id, game_data) == READY) {
    desired_target_planet_id = get_nearest_planet(ship_id, game_data);

    if (can_ship_be_GOING_TO_PLANET(ship_id, desired_target_planet_id,
                                    game_data)) {
      set_ship_FSM(ship_id, GOING_TO_PLANET, game_data);
      set_ship_target_planet_ID(ship_id, desired_target_planet_id, game_data);
    }

    else if (can_ship_be_BROKEN(ship_id, game_data)) {
      set_ship_FSM(ship_id, BROKEN, game_data);
    }
  }

  else if (get_ship_FSM(ship_id, game_data) == GOING_TO_PLANET) {

    if (can_ship_be_COLLECTING(ship_id, game_data)) {
      set_ship_FSM(ship_id, COLLECTING, game_data);
    }

    else if (can_ship_be_COLLECTING_WRONG_PLANET(ship_id, game_data)) {
      set_ship_FSM(ship_id, COLLECTING_WRONG_PLANET, game_data);
    }

    else if (can_ship_be_PLANET_STOLEN(ship_id, game_data)) {
      set_ship_FSM(ship_id, PLANET_STOLEN, game_data);
    }

    else if (can_ship_be_BROKEN(ship_id, game_data)) {
      set_ship_FSM(ship_id, BROKEN, game_data);

    } else {
      set_ship_target_planet_ID(ship_id, desired_target_planet_id, game_data);
      go_to_planet(ship_id, desired_target_planet_id);
    }
  }

  else if (get_ship_FSM(ship_id, game_data) == COLLECTING) {
    go_to_base(ship_id, game_data->base, COLLECTOR_SPEED);
    if (can_ship_be_COLLECTED(ship_id, game_data)) {
      set_ship_target_planet_ID(ship_id, -1, game_data);
      set_ship_FSM(ship_id, READY, game_data);
    }

    else if (can_ship_be_BROKEN(ship_id, game_data)) {
      set_ship_FSM(ship_id, BROKEN, game_data);
    }
  }

  else if (get_ship_FSM(ship_id, game_data) == COLLECTING_WRONG_PLANET) {

    set_ship_target_planet_ID(
        ship_id, get_planet_ID_from_ship(ship_id, game_data), game_data);
    if (can_ship_be_COLLECTED(ship_id, game_data)) {
      set_ship_FSM(ship_id, COLLECTED, game_data);
    }

    else if (can_ship_be_COLLECTING(ship_id, game_data)) {
      set_ship_FSM(ship_id, COLLECTING, game_data);
    }

    else if (can_ship_be_BROKEN(ship_id, game_data)) {
      set_ship_FSM(ship_id, BROKEN, game_data);
    } else {
      go_to_base(ship_id, game_data->base, COLLECTOR_SPEED);
    }
  }

  else if (get_ship_FSM(ship_id, game_data) == PLANET_STOLEN) {
    set_ship_target_planet_ID(ship_id, -1, game_data);

    if (can_ship_be_READY(ship_id, game_data)) {
      set_ship_FSM(ship_id, READY, game_data);
    }

    else if (can_ship_be_BROKEN(ship_id, game_data)) {
      set_ship_FSM(ship_id, BROKEN, game_data);
    }

    else {
      while (1) {
        puts("FSM : CANNOT TRANSITION TO READY STATE\n");
        os_delay(1000);
      }
    }
  }

  else if (get_ship_FSM(ship_id, game_data) == PLANET_STOLEN) {
    set_ship_target_planet_ID(ship_id, -1, game_data);
    if (can_ship_be_READY(ship_id, game_data)) {
      set_ship_FSM(ship_id, READY, game_data);
    } else if (can_ship_be_BROKEN(ship_id, game_data)) {
      set_ship_FSM(ship_id, BROKEN, game_data);

    } else {
      go_to_base(ship_id, game_data->base, COLLECTOR_SPEED);
    }
  }

  else if (get_ship_FSM(ship_id, game_data) == BROKEN) {
    set_ship_target_planet_ID(ship_id, -1, game_data);
    if (can_ship_be_READY(ship_id, game_data)) {
      set_ship_FSM(ship_id, READY, game_data);
    } else {
      go_to_base(ship_id, game_data->base, COLLECTOR_SPEED);
    }
    if (can_ship_be_BROKEN(ship_id, game_data) == false) {
      set_ship_FSM(ship_id, READY, game_data);
    }
  }

  else {
    set_ship_FSM(ship_id, UNKNOWN, game_data);
    while (1) {
      puts("FSM : UNKNOWN STATE \n");
      os_delay(1000);
    }
  }
  if (ship_id == COLLECTOR_1) {
    collector_1_desired_target_planet_id = desired_target_planet_id;
  } else {
    collector_2_desired_target_planet_id = desired_target_planet_id;
  }
}

void set_ship_FSM(uint8_t ship_id, T_ship_FSM FSM_state,
                  T_game_data *game_data) {
  game_data->ships[ship_id].FSM = FSM_state;
}

T_ship_FSM get_ship_FSM(const uint8_t ship_id, const T_game_data *game_data) {
  return game_data->ships[ship_id].FSM;
}

void set_ship_target_planet_ID(uint8_t ship_id, int8_t target_planet_id,
                               T_game_data *game_data) {
  game_data->ships[ship_id].target_planet_ID = target_planet_id;
}

int8_t find_nearest_enemy_ship_id(uint8_t attacker_id, T_game_data *game_data) {
  int8_t nearest_attacker_id = -1;
  uint16_t min_distance = MAX_DISTANCE_BETWEEN_POINT;

  for (uint8_t enemy_ship_id = 9; enemy_ship_id <= 35; enemy_ship_id++) {
    // if (can_ship_be_BROKEN(enemy_ship_id, game_data) == false) {
    if (game_data->ships[enemy_ship_id].pos_X != 0 &&
        game_data->ships[enemy_ship_id].pos_Y != 0) {
      uint16_t distance = get_distance_between_two_points(
          get_ship_position(game_data->ships[attacker_id]),
          get_ship_position(game_data->ships[enemy_ship_id]));

      if (distance < min_distance) {
        min_distance = distance;
        nearest_attacker_id = enemy_ship_id;
      }
    }
  }
  return nearest_attacker_id;
}

T_fire_result fire_on_enemy_ship(uint8_t attacker_id, uint8_t enemy_ship_id,
                                 T_game_data *game_data) {
  uint16_t angle = get_angle_between_two_points(
      get_ship_position(game_data->ships[attacker_id]),
      get_ship_position(game_data->ships[enemy_ship_id]));

  uint16_t distance = get_distance_between_two_points(
      get_ship_position(game_data->ships[attacker_id]),
      get_ship_position(game_data->ships[enemy_ship_id]));

  // if (distance <= FIRE_DISTANCE) {
  if (game_data->ships[enemy_ship_id].ship_ID != 0 &&
      game_data->ships[enemy_ship_id].broken != 1) {
    if (get_ship_FSM(attacker_id, game_data) == READY) {
      send_command(generate_command(FIRE_CMD, attacker_id, angle, 0));
    }
  }

  return OUT_OF_RANGE;
}

bool can_ship_be_READY(uint8_t ship_id, T_game_data *game_data) {
  if (game_data->ships[ship_id].target_planet_ID == -1 &&
      can_ship_be_BROKEN(ship_id, game_data) == false &&
      get_planet_ID_from_ship(ship_id, game_data) == -1) {
    return true;
  } else {
    return false;
  }
}

bool can_ship_be_GOING_TO_PLANET(uint8_t ship_id, int8_t desired_target_ID,
                                 T_game_data *game_data) {
  if (desired_target_ID != -1 &&
      can_ship_be_BROKEN(ship_id, game_data) == false &&
      get_planet_ID_from_ship(ship_id, game_data) == -1) {
    return true;
  } else {
    return false;
  }
}

bool can_ship_be_COLLECTING(uint8_t ship_id, T_game_data *game_data) {
  if (game_data->ships[ship_id].target_planet_ID != -1 &&
      get_planet_ID_from_ship(ship_id, game_data) ==
          game_data->ships[ship_id].target_planet_ID &&
      can_ship_be_BROKEN(ship_id, game_data) == false) {
    return true;
  } else {
    return false;
  }
}

bool can_ship_be_COLLECTED(uint8_t ship_id, T_game_data *game_data) {
  if (game_data->ships[ship_id].target_planet_ID != -1) {
    uint8_t target_planet_id_copy = game_data->ships[ship_id].target_planet_ID;

    if (game_data->planets[target_planet_id_copy].planet_saved == 1 &&
        get_planet_ID_from_ship(ship_id, game_data) == -1 &&
        can_ship_be_BROKEN(ship_id, game_data) == false) {
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

bool can_ship_be_COLLECTING_WRONG_PLANET(uint8_t ship_id,
                                         T_game_data *game_data) {
  if (game_data->ships[ship_id].target_planet_ID != -1 &&
      get_planet_ID_from_ship(ship_id, game_data) !=
          game_data->ships[ship_id].target_planet_ID &&
      get_planet_ID_from_ship(ship_id, game_data) != -1 &&
      can_ship_be_BROKEN(ship_id, game_data) == false) {
    return true;
  } else {
    return false;
  }
}

bool can_ship_be_PLANET_STOLEN(uint8_t ship_id, T_game_data *game_data) {
  if (game_data->ships[ship_id].target_planet_ID != -1) {
    uint8_t target_planet_id_copy = game_data->ships[ship_id].target_planet_ID;
    if (game_data->planets[target_planet_id_copy].ship_ID != (ship_id + 1) &&
        game_data->planets[target_planet_id_copy].ship_ID != -1 &&
        can_ship_be_BROKEN(ship_id, game_data) == false) {
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

bool can_ship_be_BROKEN(uint8_t ship_id, T_game_data *game_data) {
  return game_data->ships[ship_id].broken == 1;
}

// void os_firing_timer_callback(uint8_t ship_id, T_game_data *game_data) {
//   if (can_ship_be_BROKEN(ship_id, game_data)) {
//     set_ship_FSM(ship_id, BROKEN, game_data);
//   } else {
//     set_ship_FSM(ship_id, READY, game_data);
//   }
// }