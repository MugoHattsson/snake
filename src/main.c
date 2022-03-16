#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include "main.h"
#include "hamilton.h"
#include "graphics.h"

pos_t *apple;

int board[ARENA_ROWS][ARENA_COLS];

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

snake_t *init_snake()
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
    snake->dir = RIGHT;

    board[head->row][head->col] = 1;

    for (int i = 0; i < 5; i++)
        grow_snake(snake);

    return snake;
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
    }
    if (board[head->row][head->col] == 1)
    {
        return 0;
    }
    board[head->row][head->col] = 1;

    return 1;
}

direction next_snake_direction(snake_t *snake, int **tour)
{
    int row = snake->head->row;
    int col = snake->head->col;
    int next_tour_num = tour[row][col] == ARENA_ROWS * ARENA_COLS ? 1 : tour[row][col] + 1;

    if (col != ARENA_COLS - 1 && tour[row][col + 1] == next_tour_num)
        return RIGHT;
    if (col != 0 && tour[row][col - 1] == next_tour_num)
        return LEFT;
    if (row != ARENA_ROWS - 1 && tour[row + 1][col] == next_tour_num)
        return DOWN;
    if (row != 0 && tour[row - 1][col] == next_tour_num)
        return UP;
}

void guide_snake(snake_t *snake, int **tour)
{
    pos_t *head = snake->head;
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

void print_board()
{
    for (int i = 0; i < ARENA_ROWS; i++)
    {
        for (int j = 0; j < ARENA_COLS; j++)
        {
            printf("%i ", board[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int has_won(snake_t *snake)
{
    if (snake->length == ARENA_COLS * ARENA_ROWS)
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

    snake_t *snake = init_snake();
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
        wait(30);
    }

    cleanup(snake);

    terminate_graphics();

    printf("Thanks for playing!\n");
    return 0;
}