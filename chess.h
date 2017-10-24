#include <pthread.h>
#include <iostream>
#include <vector>
#include <tuple>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <algorithm>

#define ERROR -1
#define SUCCESS 0

typedef std::tuple<int, int, int> possiblePlay;
typedef struct {
    int i, j, turno, **board;
} st, *state;

int create_state(state *memAddress, int i, int j);
state branch_state(state *memAddress, int i, int j);
void delete_state(state *memAddress);
void *thread_main(void *arg);
int move_knight(state move, int i, int j);

int valid_pos(int i, int j);
int valid_move(int **board, int i, int j);
int find_moves(state move, std::vector<possiblePlay>*);
void print_game(int **matrix);
