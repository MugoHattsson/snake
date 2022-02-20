#include <SDL2/SDL.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <unistd.h>

#define SCREEN_WIDTH 900
#define SCREEN_HEIGHT 900
#define TILE_NUMBER 30
#define TILE_SIZE 30

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

int board[TILE_NUMBER][TILE_NUMBER];

void prepareScene()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void drawTile(int row, int col)
{
    assert(row < TILE_NUMBER && col < TILE_NUMBER);
    SDL_Rect rect;
    rect.h = TILE_SIZE;
    rect.w = TILE_SIZE;
    rect.x = col * TILE_SIZE;
    rect.y = row * TILE_SIZE;
    SDL_RenderFillRect(renderer, &rect);
}

void drawSnake()
{
    for (int i = 0; i < TILE_NUMBER; i++)
    {
        for (int j = 0; j < TILE_NUMBER; j++)
        {
            if (board[i][j] == 1)
            {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 1);
                drawTile(i, j);
            }
            else if (board[i][j] == 2)
            {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 1);
                drawTile(i, j);
            }
        }
    }
}

void presentScene()
{
    SDL_RenderPresent(renderer);
}

void growSnake(snake_t *snake)
{
    if (snake->head != NULL)
    {
        pos_t *new = calloc(1, sizeof(pos_t));
        new->row = snake->head->row + snake->row_dir;
        new->col = snake->head->col + snake->col_dir;
        new->prev = NULL;
        new->next = snake->head;
        snake->head->prev = new;
        snake->head = new;
        snake->length++;

        board[new->row][new->col] = 1;
        printf("Length: %i\n", snake->length);
    }
}

void spawnApple()
{
    int row;
    int col;
    do
    {
        row = rand() % TILE_NUMBER;
        col = rand() % TILE_NUMBER;
    } while (board[row][col] == 1);

    board[row][col] = 2;
}

snake_t *initSnake()
{
    snake_t *snake = calloc(1, sizeof(snake_t));
    pos_t *head = calloc(1, sizeof(pos_t));
    head->row = 15;
    head->col = 10;
    head->next = NULL;
    head->prev = NULL;
    snake->head = head;
    snake->tail = head;
    snake->length = 1;
    snake->row_dir = 0;
    snake->col_dir = 1;

    board[head->row][head->col] = 1;

    growSnake(snake);
    growSnake(snake);

    return snake;
}

int moveSnake(snake_t *snake)
{

    pos_t *head = snake->head;
    pos_t *tail = snake->tail;

    if ((head->row == 0 && snake->row_dir == -1) ||
        (head->col == 0 && snake->col_dir == -1) ||
        (head->row == TILE_NUMBER - 1 && snake->row_dir == 1) ||
        (head->col == TILE_NUMBER - 1 && snake->col_dir == 1))
    {
        return 0;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    drawTile(tail->row, tail->col);
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

    head->row += snake->row_dir;
    head->col += snake->col_dir;
    if (board[head->row][head->col] == 2)
    {
        growSnake(snake);
        spawnApple();
    }
    else if (board[head->row][head->col] == 1)
    {
        return 0;
    }
    board[head->row][head->col] = 1;

    return 1;
}

int handleInput(snake_t *snake)
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

int main(int argc, char *argv[])
{

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

    snake_t *snake = initSnake();
    srand(time(0));
    spawnApple();

    prepareScene();
    int running = 1;
    while (running)
    {
        running = handleInput(snake);
        running &= moveSnake(snake);

        drawSnake();
        presentScene();
        SDL_Delay(80);
    }

    // Close and destroy the window
    SDL_DestroyWindow(window);

    // Clean up
    SDL_Quit();
    printf("Thanks for playing!\n");
    return 0;
}