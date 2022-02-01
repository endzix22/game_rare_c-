// Kosci.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"


#include <string>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <thread>
#include <chrono>
#include <cassert>

using namespace std;

#define WIDTH				81
#define HEIGHT				24

#define CUBES_OFFSET 			16
#define RESULT_NAME_OFFSET		6
#define CHECK_MARK 			'>'
#define CHECK_MARK_OFFSET 		5

#define COMPUTER_PLAYER_NAME		"Komputer"
#define USER_MAX_NAME_LENGTH		21

#define WIDTH_TABLE			25
#define COUNT_OF_RESULTS		9

#define RESULT_ONE_PAIR			0
#define RESULT_TWO_PAIRS		1
#define RESULT_THREE			2
#define RESULT_FOUR			3
#define RESULT_LITTLE_STRAIGHT		4
#define RESULT_HIGH_STRAIGHT		5
#define RESULT_FULL			6
#define RESULT_POKER			7
#define RESULT_CHANCE			8

#define TEXT_ONE_PAIR			"Jedna para"
#define TEXT_TWO_PAIRS			"Dwie pary"
#define TEXT_THREE			"Trojka"
#define TEXT_FOUR			"Kareta"
#define TEXT_LITTLE_STRAIGHT		"Maly strit"
#define TEXT_HIGH_STRAIGHT		"Duzy strit"
#define TEXT_FULL			"Full"
#define TEXT_POKER			"Poker"
#define TEXT_CHANCE			"Szansa"

#define COUNT_OF_CUBES			5
#define MAX_ROLL_NUMBER			3
#define MIN_NUMBER_ON_CUBE		1
#define MAX_NUMBER_ON_CUBE		6

#define COUNT_OF_POINT_LITTLE_STRAIGHT	15
#define COUNT_OF_POINT_HIGH_STRAIGHT	25

#define COUNT_OF_OPTIONS 		6

#define CUBES_OFFSET_Y			15

#define SPECIAL_CHAR_DIAMOND 		167
#define SPECIAL_CHAR_VLINE 		186
#define SPECIAL_CHAR_HLINE 		205
#define SPECIAL_CHAR_ULCORNER 		201
#define SPECIAL_CHAR_URCORNER 		187
#define SPECIAL_CHAR_BRCORNER 		188
#define SPECIAL_CHAR_BLCORNER 		200
#define SPECIAL_CHAR_BTEE 		202
#define SPECIAL_CHAR_LTEE 		204
#define SPECIAL_CHAR_RTEE 		185
#define SPECIAL_CHAR_TTEE 		203

enum BARS_ID {
	BAR_RAND_ID = 0,
	BAR_ENDGAME_ID = 5,
	BAR_SELECT_CUBE_ID = 4,
	BAR_BACK_ID = 1,
	BAR_WRITE_RESULT_ID = 2,
	BAR_NEXT_PLAYER_ID = 3
};


#define BAR_RAND			"[R]zuc kosci"
#define BAR_ENDGAME			"[W]yjdz z gry"
#define BAR_SELECT_CUBE			"[Z]aznacz kosci"
#define BAR_BACK			"[P]owrot"
#define BAR_WRITE_RESULT		"[D]odaj wynik"
#define BAR_NEXT_PLAYER			"[O]drzuc wynik"


/*======================================================================================================================
 *
 *                                      +-------------------------+                          +-------------------------+
 *                                      |                         |                          |                         |
 *                                      | CURR_GAME_MODE_NEW_GAME |                          | CURR_GAME_MODE_END_GAME |
 *                                      |                         |                          |                         |
 *                                      +-----------+-------------+                          +-------------------------+
 *                                                  |
 *                                                  |
 *                                                  |
 *                                                  v
 *                                    +-------------+--------------+
 *                                    |                            |
 *                                    | CURR_GAME_MODE_BEFORE_RAND <-------------------------------------------+
 *                                    |                            |                                           |
 *                                    +-------------+--------------+                                           |
 *                                                  |                                                          |
 *                +-------------------------------->+<---------------------------------+                       |
 *                |                                 |                                  |                       |
 *                |                                 v                                  |                       |
 * +--------------+---------------+      +----------+----------+         +--------------+--------------+       |
 * |                              |      |                     |         |                             |       |
 * | CURR_GAME_MODE_SELECT_RESULT |      | CURR_GAME_MODE_RAND +-------->+ CURR_GAME_MODE_SELECT_CUBES |       |
 * |                              |      |                     |         |                             |       |
 * +--------------+---------------+      +--+---------+--------+         +-----------------------------+       |
 *                |                         ^         |                                                        |
 *                V                         |         |                                                        |
 * +--------------+------------------+      |         |                                                        |
 * |                                 |      |         |                                                        |
 * | CURR_GAME_MODE_SELECT_SUBRESULT +------+         |                                                        |
 * |                                 |                V                                                        |
 * +---------------------------------+   +------------+----------------------+                                 |
 *                                       |                                   |                                 |
 *                                       | CURR_GAME_MODE_BEFORE_NEXT_PLAYER |                                 |
 *                                       |                                   |                                 |
 *                                       +------------+----------------------+                                 |                                                                                              -+
 *                                                    |                                                        |                                                                                               |
 *                                                    V                                                        |                                                                                               |
 *                                      +-------------+--------------+                                         |                                                                                               |
 *                                      |                            |                                         |                                                                                              -+
 *                                      | CURR_GAME_MODE_NEXT_PLAYER +-----------------------------------------+
 *                                      |                            |
 *                                      +----------------------------+
 *======================================================================================================================
 */


enum Mode {
	CURR_GAME_MODE_INVALID,
	CURR_GAME_MODE_NEW_GAME,
	CURR_GAME_MODE_BEFORE_RAND,
	CURR_GAME_MODE_RAND,
	CURR_GAME_MODE_END_RAND,
	CURR_GAME_MODE_SELECT_CUBES,
	CURR_GAME_MODE_SELECT_RESULT,
	CURR_GAME_MODE_SELECT_SUBRESULT,

	CURR_GAME_MODE_BEFORE_NEXT_PLAYER,
	CURR_GAME_MODE_COMPUTER_RAND,
	CURR_GAME_MODE_COMPUTER_SELECT_RESULT,
	CURR_GAME_MODE_END_GAME,

};

enum Message {
	MESSAGE_NONE,
	MESSAGE_BEFORE_RAND,
	MESSAGE_LETS_RAND,
	MESSAGE_END_RAND,
	MESSAGE_SELECT_CUBE,
	MESSAGE_SELECT_RESULT_ERROR,
	MESSAGE_SELECT_RESULT,
	MESSAGE_SELECT_SUBRESULT,
	MESSAGE_NEXT_PLAYER,
	MESSAGE_COMPUTER_RAND
};

enum Draw {
	DRAW_DEFAULT,
	DRAW_INIT_SCREEN,
	DRAW_END_GAME
};

struct Player {
	string name;
	int results[COUNT_OF_RESULTS];
	bool is_active = false;
};


struct Game {
	bool end_game = false;
	bool wait_for_new_input = false;
	bool new_turn = false;
	bool wait_for_player_name = false;
	struct Player player;
	struct Player computer_player;
	struct Player *current_player;
	enum Draw draw;
	unsigned char drawn_values[COUNT_OF_CUBES];
	bool selected_cube[COUNT_OF_CUBES];
	const char * options[COUNT_OF_OPTIONS];
	bool active_options[COUNT_OF_OPTIONS];
	enum Message active_message;
	enum Mode last_game_mode = CURR_GAME_MODE_INVALID;
	enum Mode current_game_mode = CURR_GAME_MODE_NEW_GAME;
	int roll_number;
	char screen_buffer[HEIGHT][WIDTH];
	bool cubes_for_result[COUNT_OF_RESULTS][COUNT_OF_CUBES];
	char input;
	unsigned char values_of_pairs[2];

};

void addchar(char buffer[HEIGHT][WIDTH], int y, int x, char c)
{
	assert(x < WIDTH - 1);
	assert(y < HEIGHT);

	if (c != '\n' && c != '\t' && c != '\r')
		buffer[y][x] = c;
}

void addword(char buffer[HEIGHT][WIDTH], int y, int x, string str)
{

	for (int i = 0; i < str.size(); i++)
		addchar(buffer, y, x + i, str[i]);
}

void clear_screen_buffer(struct Game &game)
{
	memset(game.screen_buffer, ' ', sizeof(game.screen_buffer[0][0]) * WIDTH * HEIGHT);
	for (int i = 0; i < HEIGHT; i++)
		game.screen_buffer[i][WIDTH - 1] = 0;
}

void clear_cubes_with_result(struct Game &game, int id = -1)
{
	assert(id >= -1 && id < COUNT_OF_RESULTS);

	if (id != -1)
		memset(&game.cubes_for_result[id][0], false, sizeof(game.cubes_for_result[id][0]) * COUNT_OF_CUBES);
	else
		memset(game.cubes_for_result, false, sizeof(game.cubes_for_result[0][0]) * COUNT_OF_RESULTS * COUNT_OF_CUBES);
}

void set_cubes_with_result(struct Game &game, int id)
{
	assert(id >= -1 && id < COUNT_OF_RESULTS);

	memset(&game.cubes_for_result[id][0], true, sizeof(game.cubes_for_result[id][0]) * COUNT_OF_CUBES);

}

bool is_cube_should_selected(struct Game &game, int id)
{
	bool is_selected = false;
	for (int i = 0; i < COUNT_OF_RESULTS; i++)
		is_selected |= game.cubes_for_result[i][id];

	return  is_selected;
}

void init_console(struct Game &game)
{

	clear_screen_buffer(game);
	srand(time(NULL));
}

char get_input()
{
	char value;
	do {
		value = cin.get();

	} while (value == ' ' || value == '\t');

	return value;

}

bool rand_bool()
{
	return rand() % 2 == 0;
}


void change_current_game_mode(struct Game &game, enum Mode new_mode)
{
	game.last_game_mode = game.current_game_mode;
	game.current_game_mode = new_mode;
}

void clear_drawn_cubes(struct Game &game)
{
	for (unsigned char i = 0; i < COUNT_OF_CUBES; i++)
	{
		game.drawn_values[i] = 0;
		game.selected_cube[i] = false;
	}
}

void change_player(struct Game &game)
{

	game.roll_number = 0;
	clear_cubes_with_result(game);
	clear_drawn_cubes(game);

	game.computer_player.is_active = !game.computer_player.is_active;
	game.player.is_active = !game.player.is_active;

	if (game.computer_player.is_active) {
		game.current_player = &game.computer_player;
		game.current_game_mode = CURR_GAME_MODE_COMPUTER_RAND;
	}
	else {
		game.current_player = &game.player;
	}

}

bool is_one_pair(struct Game &game)
{
	int count_of_numbers;

	clear_cubes_with_result(game, RESULT_ONE_PAIR);
	for (unsigned char i = MIN_NUMBER_ON_CUBE; i <= MAX_NUMBER_ON_CUBE; i++) {
		count_of_numbers = 0;
		for (unsigned char j = 0; j < COUNT_OF_CUBES; j++) {
			if (game.drawn_values[j] == i) {
				count_of_numbers++;
				game.cubes_for_result[RESULT_ONE_PAIR][j] = true;
				if (count_of_numbers == 2)
					return true;
			}
		}


		clear_cubes_with_result(game, RESULT_ONE_PAIR);
	}
	return false;
}

bool is_three(struct Game &game)
{
	int count_of_numbers;

	clear_cubes_with_result(game, RESULT_THREE);
	for (unsigned char i = MIN_NUMBER_ON_CUBE; i <= MAX_NUMBER_ON_CUBE; i++) {
		count_of_numbers = 0;
		for (unsigned char j = 0; j < COUNT_OF_CUBES; j++)
			if (game.drawn_values[j] == i) {
				count_of_numbers++;
				game.cubes_for_result[RESULT_THREE][j] = true;
				if (count_of_numbers == 3)
					return true;
			}


		clear_cubes_with_result(game, RESULT_THREE);
	}
	return false;
}

bool is_four(struct Game &game)
{
	int count_of_numbers;

	clear_cubes_with_result(game, RESULT_FOUR);
	for (unsigned char i = MIN_NUMBER_ON_CUBE; i <= MAX_NUMBER_ON_CUBE; i++) {
		count_of_numbers = 0;
		for (unsigned char j = 0; j < COUNT_OF_CUBES; j++)
			if (game.drawn_values[j] == i) {
				count_of_numbers++;
				game.cubes_for_result[RESULT_FOUR][j] = true;
				if (count_of_numbers == 4)
					return true;
			}
		clear_cubes_with_result(game, RESULT_FOUR);
	}
	return false;
}

bool is_full(struct Game &game)
{
	bool is_cube_of_value[] = { false, false, false, false, false, false };
	int count_of_true = 0;
	int id = 0;

	for (int i = 0; i < COUNT_OF_CUBES; i++) {
		id = game.drawn_values[i] - 1;
		if (id >= 0)
			is_cube_of_value[id] = true;
		else
			return false;

	}
	for (int i = 0; i < MAX_NUMBER_ON_CUBE; i++)
		if (is_cube_of_value[i])
			count_of_true++;

	if (!is_four(game) && count_of_true == 2) {
		set_cubes_with_result(game, RESULT_FULL);
		return true;

	}
	else {
		return false;
	}

}



bool is_two_pairs(struct Game &game)
{
	int count_of_pairs = 0;
	int count_of_numbers;

	clear_cubes_with_result(game, RESULT_TWO_PAIRS);
	for (unsigned char i = MIN_NUMBER_ON_CUBE; i <= MAX_NUMBER_ON_CUBE; i++) {
		count_of_numbers = 0;
		for (unsigned char j = 0; j < COUNT_OF_CUBES; j++)
			if (game.drawn_values[j] == i) {
				count_of_numbers++;
			}
		if (count_of_numbers == 2) {
			count_of_pairs++;
			for (unsigned char l = 0; l < COUNT_OF_CUBES; l++)
				if (game.drawn_values[l] == i)
					game.cubes_for_result[RESULT_TWO_PAIRS][l] = true;
		}
	}

	if (count_of_pairs == 2)
		return true;
	else if (is_four(game)) {
		for (int i = 0; i < COUNT_OF_CUBES; i++)
			if (game.cubes_for_result[RESULT_FOUR][i])
				game.cubes_for_result[RESULT_TWO_PAIRS][i] = true;
		return true;
	}
	else if (is_full(game)) {
		int values_of_pairs[2] = { 0,0 };
		int count_of_cubes_for_pair[2] = { 0,0 };
		values_of_pairs[0] = game.drawn_values[0];
		for (int i = 1; i < COUNT_OF_CUBES; i++)
			if (values_of_pairs[0] != game.drawn_values[i]) {
				values_of_pairs[1] = game.drawn_values[i];
				break;
			}
		for (int i = 0; i < COUNT_OF_CUBES; i++) {
			if (values_of_pairs[0] == game.drawn_values[i]) {
				if (values_of_pairs[0]++ < 2)
					game.cubes_for_result[RESULT_TWO_PAIRS][i] = true;
			}
			else if (values_of_pairs[1] == game.drawn_values[i]) {
				if (count_of_cubes_for_pair[1]++ < 2)
					game.cubes_for_result[RESULT_TWO_PAIRS][i] = true;
			}
		}
		return true;
	}

	clear_cubes_with_result(game, RESULT_TWO_PAIRS);
	return false;
}


bool is_little_straight(struct Game &game)
{
	bool is_value;

	clear_cubes_with_result(game, RESULT_LITTLE_STRAIGHT);
	for (unsigned char i = MIN_NUMBER_ON_CUBE; i <= MAX_NUMBER_ON_CUBE - 1; i++) {
		is_value = false;
		for (unsigned char j = 0; j < COUNT_OF_CUBES; j++) {
			if (game.drawn_values[j] == i) {
				is_value = true;
				break;
			}
		}
		if (!is_value)
			return false;
	}
	set_cubes_with_result(game, RESULT_LITTLE_STRAIGHT);
	return true;
}

bool is_high_straight(struct Game &game)
{
	bool is_value;

	clear_cubes_with_result(game, RESULT_HIGH_STRAIGHT);
	for (unsigned char i = MIN_NUMBER_ON_CUBE + 1; i <= MAX_NUMBER_ON_CUBE; i++) {
		is_value = false;
		for (unsigned char j = 0; j < COUNT_OF_CUBES; j++) {
			if (game.drawn_values[j] == i) {
				is_value = true;
				break;
			}
		}
		if (!is_value)
			return false;
	}

	set_cubes_with_result(game, RESULT_HIGH_STRAIGHT);
	return true;
}


bool is_poker(struct Game &game)
{
	int count_of_numbers;

	clear_cubes_with_result(game, RESULT_POKER);
	for (unsigned char i = 1; i <= MAX_NUMBER_ON_CUBE; i++) {
		count_of_numbers = 0;
		for (unsigned char j = 0; j < COUNT_OF_CUBES; j++)
			if (game.drawn_values[j] == i)
				count_of_numbers++;
		if (count_of_numbers == 5) {
			set_cubes_with_result(game, RESULT_POKER);
			return true;
		}
	}
	return false;
}

void update_current_game_mode(struct Game &game)
{
	switch (game.current_game_mode) {
	case CURR_GAME_MODE_NEW_GAME:
		game.active_message = MESSAGE_NONE;
		game.draw = DRAW_INIT_SCREEN;
		game.wait_for_player_name = true;

		break;
	case CURR_GAME_MODE_BEFORE_RAND:
		game.draw = DRAW_DEFAULT;
		game.active_message = MESSAGE_BEFORE_RAND;
		game.active_options[BAR_RAND_ID] = true;
		game.active_options[BAR_ENDGAME_ID] = true;
		game.active_options[BAR_SELECT_CUBE_ID] = false;
		game.active_options[BAR_BACK_ID] = false;
		game.active_options[BAR_WRITE_RESULT_ID] = false;
		game.active_options[BAR_NEXT_PLAYER_ID] = false;
		game.wait_for_new_input = true;
		game.new_turn = true;
		break;
	case CURR_GAME_MODE_RAND:
		game.draw = DRAW_DEFAULT;
		game.active_message = MESSAGE_LETS_RAND;
		game.active_options[BAR_RAND_ID] = true;
		game.active_options[BAR_ENDGAME_ID] = true;
		game.active_options[BAR_SELECT_CUBE_ID] = true;
		game.active_options[BAR_BACK_ID] = false;
		game.active_options[BAR_WRITE_RESULT_ID] = true;
		game.active_options[BAR_NEXT_PLAYER_ID] = false;
		break;
	case CURR_GAME_MODE_END_RAND:
		game.draw = DRAW_DEFAULT;
		game.active_message = MESSAGE_END_RAND;
		game.active_options[BAR_RAND_ID] = false;
		game.active_options[BAR_ENDGAME_ID] = true;
		game.active_options[BAR_SELECT_CUBE_ID] = false;
		game.active_options[BAR_BACK_ID] = false;
		game.active_options[BAR_WRITE_RESULT_ID] = true;
		game.active_options[BAR_NEXT_PLAYER_ID] = true;
		break;
	case CURR_GAME_MODE_SELECT_CUBES:
		game.draw = DRAW_DEFAULT;
		game.active_message = MESSAGE_SELECT_CUBE;
		game.active_options[BAR_RAND_ID] = false;
		game.active_options[BAR_ENDGAME_ID] = true;
		game.active_options[BAR_SELECT_CUBE_ID] = false;
		game.active_options[BAR_BACK_ID] = true;
		game.active_options[BAR_WRITE_RESULT_ID] = false;
		break;
	case CURR_GAME_MODE_SELECT_RESULT:
		game.draw = DRAW_DEFAULT;
		if (game.active_message != MESSAGE_SELECT_RESULT_ERROR)
			game.active_message = MESSAGE_SELECT_RESULT;
		game.active_options[BAR_RAND_ID] = false;
		game.active_options[BAR_ENDGAME_ID] = true;
		game.active_options[BAR_SELECT_CUBE_ID] = false;
		game.active_options[BAR_BACK_ID] = true;
		game.active_options[BAR_WRITE_RESULT_ID] = false;
		break;
	case CURR_GAME_MODE_SELECT_SUBRESULT:
		game.draw = DRAW_DEFAULT;
		if (game.active_message != MESSAGE_SELECT_RESULT_ERROR)
			game.active_message = MESSAGE_SELECT_SUBRESULT;
		game.active_options[BAR_RAND_ID] = false;
		game.active_options[BAR_ENDGAME_ID] = true;
		game.active_options[BAR_SELECT_CUBE_ID] = false;
		game.active_options[BAR_BACK_ID] = true;
		game.active_options[BAR_WRITE_RESULT_ID] = false;
		break;
	case CURR_GAME_MODE_BEFORE_NEXT_PLAYER:
		game.draw = DRAW_DEFAULT;
		game.active_message = MESSAGE_NEXT_PLAYER;
		game.active_options[BAR_RAND_ID] = false;
		game.active_options[BAR_ENDGAME_ID] = false;
		game.active_options[BAR_SELECT_CUBE_ID] = false;
		game.active_options[BAR_BACK_ID] = false;
		game.active_options[BAR_WRITE_RESULT_ID] = false;
		game.active_options[BAR_NEXT_PLAYER_ID] = false;
		game.wait_for_new_input = false;
		game.new_turn = true;

		break;
	case CURR_GAME_MODE_COMPUTER_RAND:
		game.draw = DRAW_DEFAULT;
		game.active_message = MESSAGE_COMPUTER_RAND;
		game.active_options[BAR_RAND_ID] = false;
		game.active_options[BAR_ENDGAME_ID] = false;
		game.active_options[BAR_SELECT_CUBE_ID] = false;
		game.active_options[BAR_BACK_ID] = false;
		game.active_options[BAR_WRITE_RESULT_ID] = false;
		game.active_options[BAR_NEXT_PLAYER_ID] = false;
		game.wait_for_new_input = false;
		break;
	case CURR_GAME_MODE_END_GAME:
		game.draw = DRAW_END_GAME;
		game.end_game = true;
		break;
	case CURR_GAME_MODE_INVALID:
		assert(false);

	}
}

void init_new_game(struct Game &game)
{
	game.computer_player.name = COMPUTER_PLAYER_NAME;

	game.player.results[RESULT_ONE_PAIR] = 0;
	game.player.results[RESULT_TWO_PAIRS] = 0;
	game.player.results[RESULT_THREE] = 0;
	game.player.results[RESULT_FOUR] = 0;
	game.player.results[RESULT_LITTLE_STRAIGHT] = 0;
	game.player.results[RESULT_HIGH_STRAIGHT] = 0;
	game.player.results[RESULT_FULL] = 0;
	game.player.results[RESULT_POKER] = 0;
	game.player.results[RESULT_CHANCE] = 0;
	game.player.is_active = true;

	game.computer_player.results[RESULT_ONE_PAIR] = 0;
	game.computer_player.results[RESULT_TWO_PAIRS] = 0;
	game.computer_player.results[RESULT_THREE] = 0;
	game.computer_player.results[RESULT_FOUR] = 0;
	game.computer_player.results[RESULT_LITTLE_STRAIGHT] = 0;
	game.computer_player.results[RESULT_HIGH_STRAIGHT] = 0;
	game.computer_player.results[RESULT_FULL] = 0;
	game.computer_player.results[RESULT_POKER] = 0;
	game.computer_player.results[RESULT_CHANCE] = 0;
	game.computer_player.is_active = false;

	game.current_player = &game.player;

	game.options[BAR_RAND_ID] = BAR_RAND;
	game.options[BAR_ENDGAME_ID] = BAR_ENDGAME;
	game.options[BAR_SELECT_CUBE_ID] = BAR_SELECT_CUBE;
	game.options[BAR_BACK_ID] = BAR_BACK;
	game.options[BAR_WRITE_RESULT_ID] = BAR_WRITE_RESULT;
	game.options[BAR_NEXT_PLAYER_ID] = BAR_NEXT_PLAYER;

	game.roll_number = 0;
	clear_drawn_cubes(game);
}

/*
void rand_cubes(struct Game &game)
{
	clear_cubes_with_result(game);

	for (unsigned char i = 0; i < COUNT_OF_CUBES; ++i) {
		if (!game.selected_cube[i])
			game.drawn_values[i] = rand() % MAX_NUMBER_ON_CUBE + 1;

	}
	game.roll_number++;
}
*/

void rand_cubes(struct Game &game)
{
	clear_cubes_with_result(game);

	game.drawn_values[0] = 3;
	game.drawn_values[1] = 3;
	game.drawn_values[2] = 3;
	game.drawn_values[3] = 6;
	game.drawn_values[4] = 1;

	game.roll_number++;
}


void assign_values_to_pairs(struct Game &game) {
	game.values_of_pairs[0] = 0;
	game.values_of_pairs[1] = 0;

	for (unsigned char i = 0; i < COUNT_OF_CUBES; i++) {
		if (game.cubes_for_result[RESULT_TWO_PAIRS][i]) {
			if (game.values_of_pairs[0] == 0) {
				game.values_of_pairs[0] = game.drawn_values[i];
			}
			else if (game.values_of_pairs[0] != game.drawn_values[i])
				game.values_of_pairs[1] = game.drawn_values[i];
			else
				continue;
		}
	}
}

int get_max_result_one_pair(struct Game &game)
{
	if (game.current_player->results[RESULT_ONE_PAIR] == 0) {
		if (is_two_pairs(game)) {
			assign_values_to_pairs(game);
			int max_id = game.values_of_pairs[0] > game.values_of_pairs[1] ? 0 : 1;
			return game.values_of_pairs[max_id] * 2;

		}
		else if (is_one_pair(game)) {
			for (int i = MIN_NUMBER_ON_CUBE - 1; i < MAX_NUMBER_ON_CUBE; i++)
				if (game.cubes_for_result[RESULT_ONE_PAIR][i]) {
					return game.drawn_values[i] * 2;
				}
		}

	}

	return 0;
}

int get_max_result_two_pairs(struct Game &game)
{
	int value = 0;
	if (game.current_player->results[RESULT_TWO_PAIRS] == 0 && is_two_pairs(game))
		for (int i = MIN_NUMBER_ON_CUBE - 1; i < MAX_NUMBER_ON_CUBE; i++)
			if (game.cubes_for_result[RESULT_TWO_PAIRS][i]) {
				value += game.drawn_values[i];
			}

	return  value;
}

int get_max_result_three(struct Game &game)
{
	int value = 0;

	if (game.current_player->results[RESULT_THREE] == 0 && is_three(game))
		for (int i = MIN_NUMBER_ON_CUBE - 1; i < MAX_NUMBER_ON_CUBE; i++)
			if (game.cubes_for_result[RESULT_THREE][i]) {
				value += game.drawn_values[i];
			}
	return value;
}

int get_max_result_four(struct Game &game)
{
	int value = 0;

	if (game.current_player->results[RESULT_FOUR] == 0 && is_four(game))
		for (int i = MIN_NUMBER_ON_CUBE - 1; i < MAX_NUMBER_ON_CUBE; i++)
			if (game.cubes_for_result[RESULT_FOUR][i]) {
				value += game.drawn_values[i];
			}

	return value;
}

int get_max_result_little_straight(struct Game &game)
{
	if (game.current_player->results[RESULT_LITTLE_STRAIGHT] == 0 && is_little_straight(game))
		return COUNT_OF_POINT_LITTLE_STRAIGHT;

	return 0;
}

int get_max_result_high_straight(struct Game &game)
{

	if (game.current_player->results[RESULT_HIGH_STRAIGHT] == 0 && is_high_straight(game))
		return COUNT_OF_POINT_HIGH_STRAIGHT;

	return 0;

}

int get_max_result_full(struct Game &game)
{
	int value = 0;

	if (game.current_player->results[RESULT_FULL] == 0 && is_full(game))
		for (int i = MIN_NUMBER_ON_CUBE - 1; i < MAX_NUMBER_ON_CUBE; i++)
			if (game.cubes_for_result[RESULT_FULL][i]) {
				value += game.drawn_values[i];
			}

	return value;
}

int get_max_result_poker(struct Game &game)
{
	if (game.current_player->results[RESULT_POKER] == 0 && is_poker(game))
		return game.drawn_values[0] * 5 * 3;
	return 0;
}

int get_max_result_chance(struct Game &game)
{
	int value = 0;

	if (game.current_player->results[RESULT_CHANCE] == 0)
		for (int i = MIN_NUMBER_ON_CUBE - 1; i < MAX_NUMBER_ON_CUBE; i++)
			value += game.drawn_values[i];

	return  value;
}

bool add_result_for_player(struct Game &game, int selected, int selected_pair = -1)
{
	bool ret = false;

	assert(selected >= 0 && selected < COUNT_OF_RESULTS);
	switch (selected) {
	case RESULT_ONE_PAIR:
		if (game.current_player->results[RESULT_ONE_PAIR] == 0 && is_one_pair(game))
			if (selected_pair == -1) {
				for (int i = MIN_NUMBER_ON_CUBE - 1; i < MAX_NUMBER_ON_CUBE; i++)
					if (game.cubes_for_result[RESULT_ONE_PAIR][i]) {
						game.current_player->results[RESULT_ONE_PAIR] =
							game.drawn_values[i] * 2;
						return true;
					}
			}
			else {
				assert(game.values_of_pairs[selected_pair] != 0);
				game.current_player->results[RESULT_ONE_PAIR] =
					game.values_of_pairs[selected_pair] * 2;
			}
		break;
	case RESULT_TWO_PAIRS:
		game.current_player->results[RESULT_TWO_PAIRS] = get_max_result_two_pairs(game);
		return game.current_player->results[RESULT_TWO_PAIRS] != 0;
	case RESULT_THREE:
		game.current_player->results[RESULT_THREE] = get_max_result_three(game);
		return game.current_player->results[RESULT_THREE] != 0;
	case RESULT_FOUR:
		game.current_player->results[RESULT_FOUR] = get_max_result_four(game);
		return game.current_player->results[RESULT_FOUR] != 0;
	case RESULT_LITTLE_STRAIGHT:
		game.current_player->results[RESULT_LITTLE_STRAIGHT] = get_max_result_little_straight(game);
		return game.current_player->results[RESULT_LITTLE_STRAIGHT] != 0;
	case RESULT_HIGH_STRAIGHT:
		game.current_player->results[RESULT_HIGH_STRAIGHT] = get_max_result_high_straight(game);
		return game.current_player->results[RESULT_HIGH_STRAIGHT] != 0;
	case RESULT_FULL:
		game.current_player->results[RESULT_FULL] = get_max_result_full(game);
		return game.current_player->results[RESULT_FULL] != 0;
	case RESULT_POKER:
		game.current_player->results[RESULT_POKER] = get_max_result_poker(game);
		return game.current_player->results[RESULT_POKER] != 0;
	case RESULT_CHANCE:
		game.current_player->results[RESULT_CHANCE] = get_max_result_chance(game);
		return game.current_player->results[RESULT_CHANCE] != 0;
	}

	return ret;
}

void get_best_result(struct Game &game, int &max, int &max_id)
{
	int value;

	max = get_max_result_one_pair(game);
	max_id = RESULT_ONE_PAIR;

	value = get_max_result_two_pairs(game);
	if (max < value) {
		max = value;
		max_id = RESULT_TWO_PAIRS;
	}

	value = get_max_result_three(game);
	if (max < value) {
		max = value;
		max_id = RESULT_THREE;
	}

	value = get_max_result_four(game);
	if (max < value) {
		max = value;
		max_id = RESULT_FOUR;
	}

	value = get_max_result_little_straight(game);
	if (max < value) {
		max = value;
		max_id = RESULT_LITTLE_STRAIGHT;
	}

	value = get_max_result_high_straight(game);
	if (max < value) {
		max = value;
		max_id = RESULT_HIGH_STRAIGHT;
	}

	value = get_max_result_full(game);
	if (max < value) {
		max = value;
		max_id = RESULT_FULL;
	}

	value = get_max_result_poker(game);
	if (max < value) {
		max = value;
		max_id = RESULT_POKER;
	}

	value = get_max_result_chance(game);
	if (max < value) {
		max = value;
		max_id = RESULT_CHANCE;
	}
}

bool computer_write_result(struct Game &game)
{
	int value;
	int id;

	get_best_result(game, value, id);
	if (value != 0) {
		game.computer_player.results[id] = value;
		return true;
	}

	return false;
}

void compuetr_strategy(struct Game &game)
{

	switch (game.roll_number) {
	case 0:
		rand_cubes(game);
		break;
	case 1:
		if (computer_write_result(game)) {
			change_current_game_mode(game, CURR_GAME_MODE_BEFORE_RAND);
			change_player(game);
		}
		else
			rand_cubes(game);
		break;
	case 2:
		if (computer_write_result(game)) {
			change_current_game_mode(game, CURR_GAME_MODE_BEFORE_RAND);
			change_player(game);
		}
		else
			rand_cubes(game);
		break;
	case 3:
		computer_write_result(game);
		change_current_game_mode(game, CURR_GAME_MODE_BEFORE_RAND);
		change_player(game);
		break;
	}

}

bool is_player_end(struct Player &player)
{
	bool is_end = true;

	for (int i = 0; i < COUNT_OF_RESULTS; i++)
		if (player.results[i] == 0)
			return false;
	return true;
}

bool is_end(struct Game &game)
{
	return is_player_end(game.player) || is_player_end(game.computer_player);
}

int sum_of_points(struct Player &player)
{
	int value = 0;

	for (int i = 0; i < COUNT_OF_RESULTS; i++)
		value += player.results[i];

	return value;
}

void update(struct Game &game)
{
	if (game.new_turn)
	{
		for (unsigned char i = 0; i < COUNT_OF_CUBES; i++)
			game.selected_cube[i] = false;
		clear_cubes_with_result(game);
		game.new_turn = false;
	}


	if (game.current_game_mode == CURR_GAME_MODE_COMPUTER_RAND) {
		compuetr_strategy(game);
		this_thread::sleep_for(chrono::seconds(2));
	}

	if (game.wait_for_new_input) {
		switch (game.input) {
		case 'w':
		case 'W':
			if (!game.active_options[BAR_ENDGAME_ID])
				break;
			game.current_game_mode = CURR_GAME_MODE_END_GAME;
			break;
		case 'r':
		case 'R':
			if (!game.active_options[BAR_RAND_ID])
				break;
			change_current_game_mode(game, CURR_GAME_MODE_RAND);

			rand_cubes(game);

			if (game.roll_number == MAX_ROLL_NUMBER)
				change_current_game_mode(game, CURR_GAME_MODE_END_RAND);
			break;
		case 'z':
		case 'Z':
			if (!game.active_options[BAR_SELECT_CUBE_ID])
				break;
			change_current_game_mode(game, CURR_GAME_MODE_SELECT_CUBES);
			break;
		case 'p':
		case 'P':
			if (!game.active_options[BAR_BACK_ID])
				break;
			change_current_game_mode(game, game.last_game_mode);

			break;
		case 'd':
		case 'D':
			if (!game.active_options[BAR_WRITE_RESULT_ID])
				break;
			change_current_game_mode(game, CURR_GAME_MODE_SELECT_RESULT);
			break;
		case 'o':
		case 'O':
			if (!game.active_options[BAR_NEXT_PLAYER_ID])
				break;
			change_current_game_mode(game, CURR_GAME_MODE_BEFORE_NEXT_PLAYER);
			change_player(game);
			break;
		case '1':
			if (game.current_game_mode == CURR_GAME_MODE_SELECT_RESULT) {
				if (is_two_pairs(game) && !is_four(game)) {
					change_current_game_mode(game, CURR_GAME_MODE_SELECT_SUBRESULT);
					assign_values_to_pairs(game);
					break;
				}
			}
			else if (game.current_game_mode == CURR_GAME_MODE_SELECT_SUBRESULT) {
				add_result_for_player(game, RESULT_ONE_PAIR, 0);
				change_current_game_mode(game, CURR_GAME_MODE_BEFORE_NEXT_PLAYER);
				change_player(game);
				break;
			}
		case '2':
			if (game.current_game_mode == CURR_GAME_MODE_SELECT_SUBRESULT) {
				add_result_for_player(game, RESULT_ONE_PAIR, 1);
				change_current_game_mode(game, CURR_GAME_MODE_BEFORE_NEXT_PLAYER);
				change_player(game);
				break;
			}
		case '3':
		case '4':
		case '5':
			if (game.current_game_mode != CURR_GAME_MODE_SELECT_RESULT && game.current_game_mode != CURR_GAME_MODE_SELECT_CUBES && game.current_game_mode != CURR_GAME_MODE_SELECT_SUBRESULT)
				break;
			else if (game.current_game_mode == CURR_GAME_MODE_SELECT_CUBES) {
				game.selected_cube[game.input - '0' - 1] = !game.selected_cube[game.input - '0' - 1];
				break;
			}
		case '6':
		case '7':
		case '8':
		case '9':
			if (game.current_game_mode == CURR_GAME_MODE_SELECT_RESULT) {

				if (add_result_for_player(game, game.input - '0' - 1)) {
					change_current_game_mode(game, CURR_GAME_MODE_BEFORE_NEXT_PLAYER);
					change_player(game);
				}
				else
					game.active_message = MESSAGE_SELECT_RESULT_ERROR;
			}
			break;
		}
	}


	if (game.current_game_mode == CURR_GAME_MODE_NEW_GAME) {
		init_new_game(game);
	}

	if (is_end(game)) {
		game.end_game = true;
	}

	update_current_game_mode(game);

	if (game.end_game)
		game.draw = DRAW_END_GAME;


}

void draw_text_at_center(struct Game &game, const char * text, int offset = 0)
{
	addword(game.screen_buffer, HEIGHT / 2 + offset, (WIDTH - strlen(text)) / 2, text);
}

void draw_text_at_center_of_row(struct Game &game, const char * text, int row)
{
	addword(game.screen_buffer, row, (WIDTH - strlen(text)) / 2, text);
}

void draw_init_screen(struct Game &game)
{
	const char *init_game_message = "Witaj w grze w Kosci";
	const char *new_player_input_message = "Podaj swoje imie:";


	draw_text_at_center(game, init_game_message, -1);
	draw_text_at_center(game, new_player_input_message);

}

void draw_empty_table(struct Game &game, int offset_y = 0, int offset_x = 0)
{
	addchar(game.screen_buffer, offset_y, offset_x, SPECIAL_CHAR_ULCORNER);
	for (int i = 0; i < WIDTH_TABLE - 2; i++)
		addchar(game.screen_buffer, offset_y, offset_x + 1 + i, SPECIAL_CHAR_HLINE);

	addchar(game.screen_buffer, offset_y, offset_x + WIDTH_TABLE - 1, SPECIAL_CHAR_URCORNER);

	addchar(game.screen_buffer, offset_y + 1, offset_x + 4, SPECIAL_CHAR_VLINE);
	addchar(game.screen_buffer, offset_y + 1, offset_x, SPECIAL_CHAR_VLINE);
	addchar(game.screen_buffer, offset_y + 1, offset_x + WIDTH_TABLE - 1, SPECIAL_CHAR_VLINE);

	addchar(game.screen_buffer, offset_y + 2, offset_x, SPECIAL_CHAR_LTEE);
	for (unsigned char i = 0; i < WIDTH_TABLE - 2; i++)
		addchar(game.screen_buffer, offset_y + 2, offset_x + 1 + i, SPECIAL_CHAR_HLINE);
	addchar(game.screen_buffer, offset_y + 2, offset_x + 4, SPECIAL_CHAR_TTEE);
	addchar(game.screen_buffer, offset_y + 2, offset_x + WIDTH_TABLE - 1, SPECIAL_CHAR_RTEE);
	addchar(game.screen_buffer, offset_y + 2, offset_x + WIDTH_TABLE - 7, SPECIAL_CHAR_TTEE);

	for (unsigned char i = 3; i <= COUNT_OF_RESULTS + 2; i++) {
		addchar(game.screen_buffer, offset_y + i, offset_x, SPECIAL_CHAR_VLINE);
		addchar(game.screen_buffer, offset_y + i, offset_x + 4, SPECIAL_CHAR_VLINE);
		addchar(game.screen_buffer, offset_y + i, offset_x + WIDTH_TABLE - 7, SPECIAL_CHAR_VLINE);
		addchar(game.screen_buffer, offset_y + i, offset_x + WIDTH_TABLE - 1, SPECIAL_CHAR_VLINE);
	}

	addchar(game.screen_buffer, offset_y + 12, offset_x, SPECIAL_CHAR_BLCORNER);
	for (int i = 0; i < WIDTH_TABLE - 2; i++)
		addchar(game.screen_buffer, offset_y + 12, offset_x + 1 + i, SPECIAL_CHAR_HLINE);
	addchar(game.screen_buffer, offset_y + 12, offset_x + 4, SPECIAL_CHAR_BTEE);
	addchar(game.screen_buffer, offset_y + 12, offset_x + WIDTH_TABLE - 1, SPECIAL_CHAR_BRCORNER);
	addchar(game.screen_buffer, offset_y + 12, offset_x + WIDTH_TABLE - 7, SPECIAL_CHAR_BTEE);

}

void draw_result_table(struct Game &game, struct Player &player, int offset_y, int offset_x)
{

	draw_empty_table(game, offset_y, offset_x);

	/* Draw Player name */
	addword(game.screen_buffer, offset_y + 1, offset_x + 2, player.name.c_str());

	/* Draw result id */
	for (unsigned char i = 0; i < COUNT_OF_RESULTS; i++)
		addchar(game.screen_buffer, offset_y + 3 + i, offset_x + 2, '1' + i);


	/* Draw results names */
	addword(game.screen_buffer, offset_y + 3, offset_x + RESULT_NAME_OFFSET, TEXT_ONE_PAIR);
	addword(game.screen_buffer, offset_y + 4, offset_x + RESULT_NAME_OFFSET, TEXT_TWO_PAIRS);
	addword(game.screen_buffer, offset_y + 5, offset_x + RESULT_NAME_OFFSET, TEXT_THREE);
	addword(game.screen_buffer, offset_y + 6, offset_x + RESULT_NAME_OFFSET, TEXT_FOUR);
	addword(game.screen_buffer, offset_y + 7, offset_x + RESULT_NAME_OFFSET, TEXT_LITTLE_STRAIGHT);
	addword(game.screen_buffer, offset_y + 8, offset_x + RESULT_NAME_OFFSET, TEXT_HIGH_STRAIGHT);
	addword(game.screen_buffer, offset_y + 9, offset_x + RESULT_NAME_OFFSET, TEXT_FULL);
	addword(game.screen_buffer, offset_y + 10, offset_x + RESULT_NAME_OFFSET, TEXT_POKER);
	addword(game.screen_buffer, offset_y + 11, offset_x + RESULT_NAME_OFFSET, TEXT_CHANCE);


	/* Draw check mark */
	if (player.is_active && player.results[RESULT_ONE_PAIR] == 0 && is_one_pair(game))
		addchar(game.screen_buffer, offset_y + 3, offset_x + CHECK_MARK_OFFSET, CHECK_MARK);
	if (player.is_active && player.results[RESULT_TWO_PAIRS] == 0 && is_two_pairs(game))
		addchar(game.screen_buffer, offset_y + 4, offset_x + CHECK_MARK_OFFSET, CHECK_MARK);
	if (player.is_active && player.results[RESULT_THREE] == 0 && is_three(game))
		addchar(game.screen_buffer, offset_y + 5, offset_x + CHECK_MARK_OFFSET, CHECK_MARK);
	if (player.is_active && player.results[RESULT_FOUR] == 0 && is_four(game))
		addchar(game.screen_buffer, offset_y + 6, offset_x + CHECK_MARK_OFFSET, CHECK_MARK);
	if (player.is_active && player.results[RESULT_LITTLE_STRAIGHT] == 0 && is_little_straight(game))
		addchar(game.screen_buffer, offset_y + 7, offset_x + CHECK_MARK_OFFSET, CHECK_MARK);
	if (player.is_active && player.results[RESULT_HIGH_STRAIGHT] == 0 && is_high_straight(game))
		addchar(game.screen_buffer, offset_y + 8, offset_x + CHECK_MARK_OFFSET, CHECK_MARK);
	if (player.is_active && player.results[RESULT_FULL] == 0 && is_full(game))
		addchar(game.screen_buffer, offset_y + 9, offset_x + CHECK_MARK_OFFSET, CHECK_MARK);
	if (player.is_active && player.results[RESULT_POKER] == 0 && is_poker(game))
		addchar(game.screen_buffer, offset_y + 10, offset_x + CHECK_MARK_OFFSET, CHECK_MARK);
	if (player.is_active && player.results[RESULT_CHANCE] == 0 && game.drawn_values[0] != 0)
		addchar(game.screen_buffer, offset_y + 11, offset_x + CHECK_MARK_OFFSET, CHECK_MARK);


	/* Draw results */
	for (int i = 0; i < COUNT_OF_RESULTS; i++)
		if (player.results[i] != 0)
			addword(game.screen_buffer, offset_y + 3 + i, offset_x + 20, to_string(player.results[i]));

}

void draw_bottom_bar(struct Game &game)
{
	int offset = 1;

	for (int i = 0; i < COUNT_OF_OPTIONS; i++) {
		if (!game.active_options[i])
			continue;

		addword(game.screen_buffer, HEIGHT - 1, offset, game.options[i]);
		offset += strlen(game.options[i]) + 1;
		addchar(game.screen_buffer, HEIGHT - 1, offset, '|');
		offset += 2;
	}
}

void draw_cube(char buffer[HEIGHT][WIDTH], int offset_y, int offset_x, int value, int id, bool selected)
{
	if (selected) {

	}

	addchar(buffer, offset_y + 1, offset_x + 5, '0' + id);

	for (int i = 0; i < 7; i++)
		addchar(buffer, offset_y, offset_x + 1 + i, SPECIAL_CHAR_HLINE);

	for (int i = 0; i < 7; i++)
		addchar(buffer, offset_y + 2, offset_x + 3 + i, SPECIAL_CHAR_HLINE);

	for (int i = 0; i < 7; i++)
		addchar(buffer, offset_y + 6, offset_x + 3 + i, SPECIAL_CHAR_HLINE);

	for (int i = 0; i < 3; i++)
		addchar(buffer, offset_y + i + 3, offset_x + 2, SPECIAL_CHAR_VLINE);

	for (int i = 0; i < 3; i++)
		addchar(buffer, offset_y + i + 3, offset_x + 10, SPECIAL_CHAR_VLINE);

	for (int i = 0; i < 3; i++)
		addchar(buffer, offset_y + i + 1, offset_x + 0, SPECIAL_CHAR_VLINE);

	addchar(buffer, offset_y + 5, offset_x + 1, '\\');
	addchar(buffer, offset_y + 1, offset_x + 1, '\\');
	addchar(buffer, offset_y + 1, offset_x + 9, '\\');


	switch (value)
	{
	case 1:
		addchar(buffer, offset_y + 4, offset_x + 6, SPECIAL_CHAR_DIAMOND);
		break;
	case 2:
		addchar(buffer, offset_y + 3, offset_x + 8, SPECIAL_CHAR_DIAMOND);
		addchar(buffer, offset_y + 5, offset_x + 4, SPECIAL_CHAR_DIAMOND);
		break;
	case 3:
		addchar(buffer, offset_y + 3, offset_x + 8, SPECIAL_CHAR_DIAMOND);
		addchar(buffer, offset_y + 4, offset_x + 6, SPECIAL_CHAR_DIAMOND);
		addchar(buffer, offset_y + 5, offset_x + 4, SPECIAL_CHAR_DIAMOND);
		break;
	case 4:
		addchar(buffer, offset_y + 3, offset_x + 8, SPECIAL_CHAR_DIAMOND);
		addchar(buffer, offset_y + 5, offset_x + 4, SPECIAL_CHAR_DIAMOND);
		addchar(buffer, offset_y + 3, offset_x + 4, SPECIAL_CHAR_DIAMOND);
		addchar(buffer, offset_y + 5, offset_x + 8, SPECIAL_CHAR_DIAMOND);
		break;
	case 5:
		addchar(buffer, offset_y + 3, offset_x + 8, SPECIAL_CHAR_DIAMOND);
		addchar(buffer, offset_y + 4, offset_x + 6, SPECIAL_CHAR_DIAMOND);
		addchar(buffer, offset_y + 5, offset_x + 4, SPECIAL_CHAR_DIAMOND);
		addchar(buffer, offset_y + 3, offset_x + 4, SPECIAL_CHAR_DIAMOND);
		addchar(buffer, offset_y + 5, offset_x + 8, SPECIAL_CHAR_DIAMOND);
		break;
	case 6:
		addchar(buffer, offset_y + 3, offset_x + 8, SPECIAL_CHAR_DIAMOND);
		addchar(buffer, offset_y + 5, offset_x + 4, SPECIAL_CHAR_DIAMOND);
		addchar(buffer, offset_y + 3, offset_x + 4, SPECIAL_CHAR_DIAMOND);
		addchar(buffer, offset_y + 5, offset_x + 8, SPECIAL_CHAR_DIAMOND);
		addchar(buffer, offset_y + 3, offset_x + 6, SPECIAL_CHAR_DIAMOND);
		addchar(buffer, offset_y + 5, offset_x + 6, SPECIAL_CHAR_DIAMOND);
		break;
	default:
		assert(false);
	}

	if (selected)
	{
		int i;
		addchar(buffer, offset_y + 7, offset_x - 2, SPECIAL_CHAR_BLCORNER);
		for (i = 1; i < 14; i++)
			addchar(buffer, offset_y + 7, offset_x - 2 + i, SPECIAL_CHAR_HLINE);

		addchar(buffer, offset_y + 7, offset_x - 2 + i, SPECIAL_CHAR_BRCORNER);

	}
}

void draw_message_to_player(struct Game &game)
{
	char buffer[50];
	switch (game.active_message) {
	case MESSAGE_BEFORE_RAND:
		draw_text_at_center_of_row(game, "Twoj ruch", 1);
		draw_text_at_center_of_row(game, "Rzuc koscmi wciskajac r/R,", 2);
		draw_text_at_center_of_row(game, "a nastepnie ENTER", 3);
		break;
	case MESSAGE_LETS_RAND:
		draw_text_at_center_of_row(game, "Twoj ruch", 1);

		sprintf_s(buffer, "Wykonales(as) %d/%d rzutow", game.roll_number, MAX_ROLL_NUMBER);
		draw_text_at_center_of_row(game, buffer, 3);

		draw_text_at_center_of_row(game, "Mozesz rzucic koscmi", 5);

		draw_text_at_center_of_row(game, "Wcisnij z/Z by zaznaczyc", 7);
		draw_text_at_center_of_row(game, "kosci ktorymi nie chcesz", 8);
		draw_text_at_center_of_row(game, "rzucac,", 9);
		draw_text_at_center_of_row(game, "lub d/D by zapisac wynik.", 10);
		break;
	case MESSAGE_END_RAND:
		draw_text_at_center_of_row(game, "Twoj ruch", 1);

		draw_text_at_center_of_row(game, "Wykorzorzystales wszystkie", 3);
		draw_text_at_center_of_row(game, "rzuty.", 4);
		draw_text_at_center_of_row(game, "Dodaj wynik do tabelki,", 5);
		draw_text_at_center_of_row(game, "lub oddaj kolejke.", 6);
		break;
	case MESSAGE_SELECT_CUBE:
		draw_text_at_center_of_row(game, "Twoj ruch", 1);
		draw_text_at_center_of_row(game, "Zaznacz kosci ktore", 2);
		draw_text_at_center_of_row(game, "chcesz zatrzymac", 3);
		draw_text_at_center_of_row(game, "klikajac na", 4);
		draw_text_at_center_of_row(game, "klawiaturze od 1 do 5", 5);
		break;
	case MESSAGE_SELECT_RESULT:
		draw_text_at_center_of_row(game, "Twoj ruch", 1);
		draw_text_at_center_of_row(game, "Wybierz miejsce w tabelce", 2);
		draw_text_at_center_of_row(game, "w ktore chcesz wpisac", 3);
		draw_text_at_center_of_row(game, "punkty, klikajac na", 4);
		draw_text_at_center_of_row(game, "klawiaturze od 1 do 9", 5);
		break;
	case MESSAGE_SELECT_SUBRESULT:
		draw_text_at_center_of_row(game, "Twoj ruch", 1);

		draw_text_at_center_of_row(game, "Twoj wybor jest", 3);
		draw_text_at_center_of_row(game, "niejednoznaczny.", 4);
		draw_text_at_center_of_row(game, "Ktora pare chcesz", 5);
		draw_text_at_center_of_row(game, "dodac.", 6);
		draw_text_at_center_of_row(game, "Wybierz 1 lub 2.", 7);
		break;
	case MESSAGE_SELECT_RESULT_ERROR:
		draw_text_at_center_of_row(game, "Twoj ruch", 1);
		draw_text_at_center_of_row(game, "Nie mozesz wpisac", 2);
		draw_text_at_center_of_row(game, "punktow w to miejsce", 3);
		draw_text_at_center_of_row(game, "Sprobuj ponownie", 4);
		break;
	case MESSAGE_NEXT_PLAYER:
		draw_text_at_center_of_row(game, "Koniec Twojej kolejki", 1);
		draw_text_at_center_of_row(game, "Tura drugiego gracza", 2);
		break;
	case MESSAGE_COMPUTER_RAND:
		draw_text_at_center_of_row(game, "Tura komputera", 1);

		sprintf_s(buffer, "Rzut %d/%d ", game.roll_number, MAX_ROLL_NUMBER);
		draw_text_at_center_of_row(game, buffer, 3);

		draw_text_at_center_of_row(game, "Czekaj na swoja kolej.", 4);
		break;
	default:
		assert(false);
	}

}

void draw_cubes_for_subresult(struct Game &game, int offset_x) {

	for (unsigned char i = 0; i < 2; i++, offset_x += CUBES_OFFSET) {
		draw_cube(game.screen_buffer, CUBES_OFFSET_Y, offset_x, game.values_of_pairs[i], i + 1, false);
		offset_x += CUBES_OFFSET;
		draw_cube(game.screen_buffer, CUBES_OFFSET_Y, offset_x, game.values_of_pairs[i], i + 1, false);
	}
}

void draw_cubes(struct Game &game)
{
	int offset_x = 3;
	bool is_selected;

	if (game.current_game_mode == CURR_GAME_MODE_SELECT_SUBRESULT) {
		draw_cubes_for_subresult(game, offset_x);
	}
	else {
		for (unsigned char i = 0; i < COUNT_OF_CUBES; i++, offset_x += CUBES_OFFSET) {
			if (game.current_game_mode == CURR_GAME_MODE_END_RAND ||
				game.current_game_mode == CURR_GAME_MODE_SELECT_RESULT)
				is_selected = is_cube_should_selected(game, i);
			else
				is_selected = game.selected_cube[i];

			draw_cube(game.screen_buffer, CUBES_OFFSET_Y, offset_x, game.drawn_values[i], i + 1, is_selected);
		}
	}
}

void draw_default(struct Game &game)
{

	game.player.is_active = true;
	draw_result_table(game, game.player, 1, 1);
	draw_result_table(game, game.computer_player, 1, WIDTH - WIDTH_TABLE - 1);
	draw_message_to_player(game);
	draw_bottom_bar(game);

	if (game.drawn_values[0] > 0)
		draw_cubes(game);

}

void draw_end_screen(struct Game &game)
{
	int sum_player = sum_of_points(game.player);
	int sum_computer = sum_of_points(game.computer_player);


	draw_text_at_center(game, "Koniec Gry", -1);

	if (sum_player > sum_computer)
		draw_text_at_center(game, "Wygrales(as)");
	else if (sum_computer == sum_player)
		draw_text_at_center(game, "Remis");
	else
		draw_text_at_center(game, "Przegrales(as)");
}

void draw(struct Game &game)
{

	switch (game.draw) {
	case DRAW_INIT_SCREEN:
		draw_init_screen(game);
		break;
	case DRAW_END_GAME:
		draw_end_screen(game);
		break;
	case DRAW_DEFAULT:
	default:
		draw_default(game);
	}

	for (int i = 0; i < HEIGHT; i++)
		cout << game.screen_buffer[i] << endl;

	clear_screen_buffer(game);
}

void event(struct Game &game) {
	if (game.wait_for_player_name) {
		string buffer;

		cin >> buffer;
		cin.clear();

		game.player.name = buffer.substr(0, USER_MAX_NAME_LENGTH);
		game.wait_for_player_name = false;
		change_current_game_mode(game, CURR_GAME_MODE_BEFORE_RAND);

	}
	else if (game.wait_for_new_input) {
		game.input = get_input();
	}
}

int main()
{
	struct Game game;
	init_console(game);

	do {
		event(game);
		update(game);
		draw(game);


	} while (!game.end_game);

	cout << "Wpisz x by zakonczyc..." << endl;

	char wait;

	cin >> wait;


	return 0;
}