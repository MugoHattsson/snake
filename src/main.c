#include <SDL2/SDL.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <unistd.h>
#include "main.h"
#include "hamilton.h"

typedef struct pos_t pos_t;

typedef enum shape
{
    HORISONTAL,
    VERTICAL,
    UP_LEFT,
    UP_RIGHT,
    DOWN_LEFT,
    DOWN_RIGHT
} shape;

struct pos_t
{
    pos_t *next;
    pos_t *prev;
    direction dir;
    int row;
    int col;
};

typedef struct snake_t
{
    pos_t *head;
    pos_t *tail;
    direction dir;
    int length;
} snake_t;

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

void draw_shape(shape shape, int row, int col)
{
    SDL_Rect rect;
    SDL_Rect aux;

    int gap = 4;
    int half_gap = gap / 2;

    switch (shape)
    {
    case HORISONTAL:
        rect.w = TILE_SIZE;
        rect.h = TILE_SIZE - gap;
        rect.x = col * TILE_SIZE;
        rect.y = row * TILE_SIZE + half_gap;
        SDL_RenderFillRect(renderer, &rect);
        break;

    case VERTICAL:
        rect.w = TILE_SIZE - gap;
        rect.h = TILE_SIZE;
        rect.x = col * TILE_SIZE + half_gap;
        rect.y = row * TILE_SIZE;
        SDL_RenderFillRect(renderer, &rect);
        break;

    case UP_LEFT:
        rect.w = TILE_SIZE - gap;
        rect.h = TILE_SIZE - half_gap;
        rect.x = col * TILE_SIZE + half_gap;
        rect.y = row * TILE_SIZE;

        aux.w = half_gap;
        aux.h = TILE_SIZE - gap;
        aux.x = col * TILE_SIZE;
        aux.y = row * TILE_SIZE + half_gap;

        SDL_RenderFillRect(renderer, &rect);
        SDL_RenderFillRect(renderer, &aux);
        break;

    case UP_RIGHT:
        rect.w = TILE_SIZE - gap;
        rect.h = TILE_SIZE - half_gap;
        rect.x = col * TILE_SIZE + half_gap;
        rect.y = row * TILE_SIZE;

        aux.w = half_gap;
        aux.h = TILE_SIZE - gap;
        aux.x = col * TILE_SIZE + TILE_SIZE - half_gap;
        aux.y = row * TILE_SIZE + half_gap;
        SDL_RenderFillRect(renderer, &rect);
        SDL_RenderFillRect(renderer, &aux);
        break;

    case DOWN_LEFT:
        rect.w = TILE_SIZE - gap;
        rect.h = TILE_SIZE - half_gap;
        rect.x = col * TILE_SIZE + half_gap;
        rect.y = row * TILE_SIZE + half_gap;

        aux.w = half_gap;
        aux.h = TILE_SIZE - gap;
        aux.x = col * TILE_SIZE;
        aux.y = row * TILE_SIZE + half_gap;

        SDL_RenderFillRect(renderer, &rect);
        SDL_RenderFillRect(renderer, &aux);
        break;

    case DOWN_RIGHT:
        rect.w = TILE_SIZE - gap;
        rect.h = TILE_SIZE - half_gap;
        rect.x = col * TILE_SIZE + half_gap;
        rect.y = row * TILE_SIZE + half_gap;

        aux.w = half_gap;
        aux.h = TILE_SIZE - gap;
        aux.x = col * TILE_SIZE + TILE_SIZE - half_gap;
        aux.y = row * TILE_SIZE + half_gap;
        SDL_RenderFillRect(renderer, &rect);
        SDL_RenderFillRect(renderer, &aux);
        break;

    default:
        break;
    }
}

shape next_shape(pos_t *next, pos_t *cur)
{
    if (next->dir == cur->dir)
        if (next->dir == UP || next->dir == DOWN)
            return VERTICAL;
        else
            return HORISONTAL;

    switch (cur->dir)
    {
    case UP:
        switch (next->dir)
        {
        case UP:
            return VERTICAL;
        case RIGHT:
            return DOWN_RIGHT;
        case LEFT:
            return DOWN_LEFT;
        }
    case RIGHT:
        switch (next->dir)
        {
        case UP:
            return UP_LEFT;
        case RIGHT:
            return HORISONTAL;
        case DOWN:
            return DOWN_LEFT;
        }
    case DOWN:
        switch (next->dir)
        {
        case LEFT:
            return UP_LEFT;
        case RIGHT:
            return UP_RIGHT;
        case DOWN:
            return VERTICAL;
        }
    case LEFT:
        switch (next->dir)
        {
        case UP:
            return UP_RIGHT;
        case LEFT:
            return HORISONTAL;
        case DOWN:
            return DOWN_RIGHT;
        }
    }
}

void draw_apple()
{
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    draw_tile(apple->row, apple->col);
}

void draw_snake(snake_t *snake)
{
    pos_t *current = snake->head;
    pos_t *prev = current;
    shape shape = next_shape(prev, current);

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    draw_shape(shape, current->row, current->col);

    while ((current = current->next) != NULL)
    {
        shape = next_shape(prev, current);
        draw_shape(shape, prev->row, prev->col);
        prev = current;
    }
}

void draw_game(snake_t *snake)
{
    draw_apple();
    draw_snake(snake);
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
                if (snake->dir != DOWN)
                    snake->dir = UP;
                break;

            case SDLK_s:
                if (snake->dir != UP)
                    snake->dir = DOWN;
                break;

            case SDLK_a:
                if (snake->dir != RIGHT)
                    snake->dir = LEFT;
                break;

            case SDLK_d:
                if (snake->dir != LEFT)
                    snake->dir = RIGHT;
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

int main(int argc, char *argv[])
{
    int **tour = hamiltonian();

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

        draw_game(snake);
        present_scene();
        SDL_Delay(80);
    }

    // Close and destroy the window
    SDL_DestroyWindow(window);

    // Clean up
    SDL_Quit();
    printf("Thanks for playing!\n");
    return 0;
}