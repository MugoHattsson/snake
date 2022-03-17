#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include "main.h"
#include "hamilton.h"
#include "graphics.h"

pos_t *apple;

int board[ARENA_ROWS][ARENA_COLS];
int buffer = 3;

void print_board()
{
    printf("\n");
    for (int i = 0; i < ARENA_ROWS; i++)
    {
        for (int j = 0; j < ARENA_COLS; j++)
        {
            printf("%i ", board[i][j]);
        }
        printf("\n");
    }
}

void grow_snake(snake_t *snake)
{
    if (snake->tail != NULL)
    {
        pos_t *tail = snake->tail;
        pos_t *new = calloc(1, sizeof(pos_t));
        new->row = tail->row;
        new->col = tail->col;
        new->next = NULL;
        new->prev = tail;
        new->dir = tail->dir;
        tail->next = new;
        snake->tail = new;
        snake->length++;
    }
}

void spawn_apple()
{
    if (apple == NULL)
        apple = calloc(1, sizeof(pos_t));
    int row;
    int col;
    do
    {
        row = rand() % ARENA_ROWS;
        col = rand() % ARENA_COLS;
    } while (board[row][col] != 0);

    apple->row = row;
    apple->col = col;
    board[row][col] = 2;
}

int find_apple()
{
    for (int i = 0; i < ARENA_ROWS; i++)
        for (int j = 0; j < ARENA_COLS; j++)
            if (board[i][j] == 2)
                return 1;

    return 0;
}

int move_snake(snake_t *snake)
{

    pos_t *head = snake->head;
    pos_t *tail = snake->tail;

    if ((head->row == 0 && snake->dir == UP) ||
        (head->col == 0 && snake->dir == LEFT) ||
        (head->row == ARENA_ROWS - 1 && snake->dir == DOWN) ||
        (head->col == ARENA_COLS - 1 && snake->dir == RIGHT))
    {
        return 0;
    }

    board[tail->row][tail->col] = 0;
    tail->row = tail->prev->row;
    tail->col = tail->prev->col;
    tail->dir = tail->prev->dir;

    pos_t *current = tail;
    while ((current = current->prev) != NULL)
    {
        if (current->prev != NULL)
        {
            current->row = current->prev->row;
            current->col = current->prev->col;
            current->dir = current->prev->dir;
        }
    }

    board[tail->row][tail->col] = 1;

    int row_dir = 0;
    int col_dir = 0;

    switch (snake->dir)
    {
    case UP:
        row_dir = -1;
        break;
    case DOWN:
        row_dir = 1;
        break;
    case LEFT:
        col_dir = -1;
        break;
    case RIGHT:
        col_dir = 1;
        break;
    }

    head->row += row_dir;
    head->col += col_dir;
    if (board[head->row][head->col] == 2)
    {
        grow_snake(snake);
        spawn_apple();
        int progress = (float)snake->length / (float)ARENA_SIZE * 100;
        printf("%i%%\n", progress);
    }
    if (board[head->row][head->col] == 1)
    {
        return 0;
    }
    board[head->row][head->col] = 1;

    return 1;
}

direction init_direction(snake_t *snake, int **tour)
{
    int row = snake->head->row;
    int col = snake->head->col;

    int next_tour_num = tour[row][col] == ARENA_ROWS * ARENA_COLS ? 1 : tour[row][col] + 1;

    if (col < ARENA_COLS - 1 && tour[row][col + 1] == next_tour_num)
        return RIGHT;
    if (col > 0 && tour[row][col - 1] == next_tour_num)
        return LEFT;
    if (row < ARENA_ROWS - 1 && tour[row + 1][col] == next_tour_num)
        return DOWN;
    if (row > 0 && tour[row - 1][col] == next_tour_num)
        return UP;
}

snake_t *init_snake(int **tour)
{
    snake_t *snake = calloc(1, sizeof(snake_t));
    pos_t *head = calloc(1, sizeof(pos_t));
    head->row = ARENA_ROWS / 2;
    head->col = ARENA_COLS / 2;
    head->next = NULL;
    head->prev = NULL;
    head->dir = RIGHT;

    snake->head = head;
    snake->tail = head;
    snake->length = 1;

    snake->dir = init_direction(snake, tour);

    board[head->row][head->col] = 1;

    int init_length = 2;
    for (int i = 0; i < init_length; i++)
        grow_snake(snake);

    move_snake(snake);

    return snake;
}

int tour_distance(int a, int b)
{
    if (a < b)
        return b - a - 1;
    return b - a - 1 + ARENA_SIZE;
}

int distance_to_apple(snake_t *snake, int **tour)
{
    int apple_pos = tour[apple->row][apple->col];
    int head_pos = tour[snake->head->row][snake->head->col];

    return tour_distance(head_pos, apple_pos);
}

bool collision(int row, int col)
{
    if (row < 0 || row >= ARENA_ROWS || col < 0 || col >= ARENA_COLS)
        return true;

    return board[row][col] == 1 ? true : false;
}

bool valid_cut(snake_t *snake, int **tour, int row, int col)
{
    if (collision(row, col))
        return false;

    return true;

    int head = tour[snake->head->row][snake->head->col];
    int tail = tour[snake->tail->row][snake->tail->col];
    int next = tour[row][col];

    if (head > tail)
    {
        if (next > head && next > tail)
            return true;
        if (next < head && next < tail)
            return true;
    }
    if (head < tail && next > head && next < tail)
        return true;

    return false;
}

char *direction_to_string(direction dir)
{
    switch (dir)
    {
    case UP:
        return "UP";
    case RIGHT:
        return "RI";
    case DOWN:
        return "DO";
    case LEFT:
        return "LE";
    }
}

void debug(direction dir, int food_dist, int tail_dist, int cut, int avail)
{
    char *label = direction_to_string(dir);

    printf("Dir: %s, Fd: %i, Td: %i, Cd: %i, Ad: %i\n", label, food_dist, tail_dist, cut, avail);
}

direction next_snake_direction(snake_t *snake, int **tour)
{
    int row = snake->head->row;
    int col = snake->head->col;
    const int tour_num = tour[snake->head->row][snake->head->col];
    const int apple_dist = tour_distance(tour_num, tour[apple->row][apple->col]);
    const int tail_dist = tour_distance(tour_num, tour[snake->tail->row][snake->tail->col]);
    const int num_empty_tiles = ARENA_SIZE - snake->length - 1 - 1; // Minus -1 * 2 for snake growth and apple pos

    int available_cut_dist = tail_dist - 1 - buffer; // -1 for snake growth, -3 for buffer.

    if (num_empty_tiles < ARENA_SIZE / 2)
        buffer = 10;
    if (apple_dist < tail_dist)
    {
        available_cut_dist--;
        if ((tail_dist - apple_dist) * 4 > num_empty_tiles)
            available_cut_dist -= 10;
    }

    if (apple_dist < available_cut_dist)
        available_cut_dist = apple_dist;
    if (available_cut_dist < 0)
        available_cut_dist = 0;

    bool can_go_up = valid_cut(snake, tour, row - 1, col);
    bool can_go_right = valid_cut(snake, tour, row, col + 1);
    bool can_go_down = valid_cut(snake, tour, row + 1, col);
    bool can_go_left = valid_cut(snake, tour, row, col - 1);

    direction best_dir;
    int best_cut = -1;

    if (can_go_left)
    {
        int cut = tour_distance(tour_num, tour[row][col - 1]);
        // debug(LEFT, apple_dist, tail_dist, cut, available_cut_dist);
        if (cut <= available_cut_dist && cut > best_cut)
        {
            best_cut = cut;
            best_dir = LEFT;
        }
    }
    if (can_go_up)
    {
        int cut = tour_distance(tour_num, tour[row - 1][col]);
        // debug(UP, apple_dist, tail_dist, cut, available_cut_dist);
        if (cut <= available_cut_dist && cut > best_cut)
        {
            best_cut = cut;
            best_dir = UP;
        }
    }
    if (can_go_right)
    {
        int cut = tour_distance(tour_num, tour[row][col + 1]);
        // debug(RIGHT, apple_dist, tail_dist, cut, available_cut_dist);
        if (cut <= available_cut_dist && cut > best_cut)
        {
            best_cut = cut;
            best_dir = RIGHT;
        }
    }
    if (can_go_down)
    {
        int cut = tour_distance(tour_num, tour[row + 1][col]);
        // debug(DOWN, apple_dist, tail_dist, cut, available_cut_dist);
        if (cut <= available_cut_dist && cut > best_cut)
        {
            best_cut = cut;
            best_dir = DOWN;
        }
    }

    // printf("Best dir: %s, cut: %i\n", direction_to_string(best_dir), best_cut);
    return best_dir;
}

void guide_snake(snake_t *snake, int **tour)
{
    direction next_dir = next_snake_direction(snake, tour);

    switch (next_dir)
    {
    case UP:
        snake->dir = UP;
        snake->head->dir = UP;
        break;
    case DOWN:
        snake->dir = DOWN;
        snake->head->dir = DOWN;
        break;
    case RIGHT:
        snake->dir = RIGHT;
        snake->head->dir = RIGHT;
        break;
    case LEFT:
        snake->dir = LEFT;
        snake->head->dir = LEFT;
        break;
    }
}

int has_won(snake_t *snake)
{
    if (snake->length == ARENA_SIZE)
    {
        printf("You Win!\n");
        return 1;
    }

    return 0;
}

void free_snake(snake_t *snake)
{
    pos_t *current = snake->head;

    while ((current = current->next) != NULL)
    {
        free(current->prev);
    }
    free(snake->tail);
    free(snake);
}

void cleanup(snake_t *snake)
{
    free(apple);
    free_snake(snake);
}

int main(int argc, char *argv[])
{
    if (!init_graphics())
        return 1;
    int **tour = hamiltonian();

    snake_t *snake = init_snake(tour);
    srand(time(0));
    spawn_apple();

    int running = 1;
    while (running && !has_won(snake))
    {
        prepare_scene();
        running = handle_input();
        guide_snake(snake, tour);
        running &= move_snake(snake);

        draw_game(snake, apple);
        present_scene();
        wait(16);
    }

    cleanup(snake);

    terminate_graphics();

    printf("Thanks for playing!\n");
    return 0;
}