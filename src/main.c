#include <SDL2/SDL.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <unistd.h>
#include "main.h"
#include "hamilton.h"

typedef struct pos_t pos_t;
typedef struct snake_t snake_t;

struct pos_t
{
    pos_t *next;
    pos_t *prev;
    int row;
    int col;
};

struct snake_t
{
    pos_t *head;
    pos_t *tail;
    int length;
    int row_dir;
    int col_dir;
};

SDL_Window *window;
SDL_Renderer *renderer;
pos_t *apple;

int board[ARENA_ROWS][ARENA_COLS];

void prepare_scene()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void draw_tile(int row, int col)
{
    assert(row < ARENA_ROWS && col < ARENA_COLS);
    SDL_Rect rect;
    rect.h = TILE_SIZE - 2;
    rect.w = TILE_SIZE - 2;
    rect.x = col * TILE_SIZE + 1;
    rect.y = row * TILE_SIZE + 1;
    SDL_RenderFillRect(renderer, &rect);
}

void draw_snake()
{
    for (int i = 0; i < ARENA_ROWS; i++)
    {
        for (int j = 0; j < ARENA_COLS; j++)
        {
            if (board[i][j] == 1)
            {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 1);
                draw_tile(i, j);
            }
            else if (board[i][j] == 2)
            {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 1);
                draw_tile(i, j);
            }
        }
    }
}

void present_scene()
{
    SDL_RenderPresent(renderer);
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
    printf("Next apple: %i, %i\n", row, col);
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
    snake->head = head;
    snake->tail = head;
    snake->length = 1;
    snake->row_dir = 0;
    snake->col_dir = 1;

    board[head->row][head->col] = 1;

    for (int i = 0; i < 5; i++)
        grow_snake(snake);

    return snake;
}

int move_snake(snake_t *snake)
{

    pos_t *head = snake->head;
    pos_t *tail = snake->tail;

    if ((head->row == 0 && snake->row_dir == -1) ||
        (head->col == 0 && snake->col_dir == -1) ||
        (head->row == ARENA_ROWS - 1 && snake->row_dir == 1) ||
        (head->col == ARENA_COLS - 1 && snake->col_dir == 1))
    {
        return 0;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    draw_tile(tail->row, tail->col);
    board[tail->row][tail->col] = 0;
    tail->row = tail->prev->row;
    tail->col = tail->prev->col;

    pos_t *current = tail;
    while ((current = current->prev) != NULL)
    {
        if (current->prev != NULL)
        {
            current->row = current->prev->row;
            current->col = current->prev->col;
        }
    }

    board[tail->row][tail->col] = 1;

    head->row += snake->row_dir;
    head->col += snake->col_dir;
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

int handle_input(snake_t *snake)
{

    float time = 0.1;
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            return 0;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                return 0;
                break;

            case SDLK_w:
                if (snake->row_dir != 1)
                {
                    snake->row_dir = -1;
                    snake->col_dir = 0;
                }
                break;

            case SDLK_s:
                if (snake->row_dir != -1)
                {
                    snake->row_dir = 1;
                    snake->col_dir = 0;
                }
                break;

            case SDLK_a:
                if (snake->col_dir != 1)
                {
                    snake->row_dir = 0;
                    snake->col_dir = -1;
                }
                break;

            case SDLK_d:
                if (snake->col_dir != -1)
                {
                    snake->row_dir = 0;
                    snake->col_dir = 1;
                }
                break;

            default:
                break;
            }
            break;

        default:
            break;
        }
    }
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
        snake->row_dir = -1;
        snake->col_dir = 0;
        break;
    case DOWN:
        snake->row_dir = 1;
        snake->col_dir = 0;
        break;
    case RIGHT:
        snake->row_dir = 0;
        snake->col_dir = 1;
        break;
    case LEFT:
        snake->row_dir = 0;
        snake->col_dir = -1;
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

int main(int argc, char *argv[])
{
    int **tour = hamiltonian();
    // printf("%i", tour[0][0]);

    SDL_Init(SDL_INIT_VIDEO); // Initialize SDL2

    // Create an application window with the following settings:
    window = SDL_CreateWindow(
        "Snaek",                 // window title
        SDL_WINDOWPOS_UNDEFINED, // initial x position
        SDL_WINDOWPOS_UNDEFINED, // initial y position
        SCREEN_WIDTH,            // width, in pixels
        SCREEN_HEIGHT,           // height, in pixels
        SDL_WINDOW_OPENGL        // flags - see below
    );

    // Check that the window was successfully created
    if (window == NULL)
    {
        // In the case that the window could not be made...
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    // The window is open: could enter program loop here (see SDL_PollEvent())

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer)
    {
        printf("Failed to create renderer: %s\n", SDL_GetError());
        return 1;
    }

    memset(&board, 0, sizeof(board));

    snake_t *snake = init_snake();
    srand(time(0));
    spawn_apple();

    int running = 1;
    while (running && !has_won(snake))
    {
        prepare_scene();
        running = handle_input(snake);
        guide_snake(snake, tour);
        running &= move_snake(snake);
        print_board();

        draw_snake();
        present_scene();
        SDL_Delay(200);
    }

    // Close and destroy the window
    SDL_DestroyWindow(window);

    // Clean up
    SDL_Quit();
    printf("Thanks for playing!\n");
    return 0;
}