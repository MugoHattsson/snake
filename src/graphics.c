#include <SDL2/SDL.h>
#include <assert.h>
#include "main.h"

typedef enum shape
{
    HORISONTAL,
    VERTICAL,
    UP_LEFT,
    UP_RIGHT,
    DOWN_LEFT,
    DOWN_RIGHT
} shape;

SDL_Window *window;
SDL_Renderer *renderer;

void prepare_scene(void)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void draw_tile(int row, int col)
{
    assert(row < ARENA_ROWS && col < ARENA_COLS);
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

void draw_apple(pos_t *apple)
{
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect rect = {
        .h = TILE_SIZE - 2,
        .w = TILE_SIZE - 2,
        .x = apple->col * TILE_SIZE + 1,
        .y = apple->row * TILE_SIZE + 1};
    SDL_RenderFillRect(renderer, &rect);
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

    shape = next_shape(prev, prev);
    draw_shape(shape, prev->row, prev->col);
}

void draw_game(snake_t *snake, pos_t *apple)
{
    draw_apple(apple);
    draw_snake(snake);
}

void present_scene()
{
    SDL_RenderPresent(renderer);
}

int handle_input(void)
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

                // case SDLK_w:
                //     if (snake->dir != DOWN)
                //         snake->dir = UP;
                //     break;

                // case SDLK_s:
                //     if (snake->dir != UP)
                //         snake->dir = DOWN;
                //     break;

                // case SDLK_a:
                //     if (snake->dir != RIGHT)
                //         snake->dir = LEFT;
                //     break;

                // case SDLK_d:
                //     if (snake->dir != LEFT)
                //         snake->dir = RIGHT;
                //     break;

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

int init_graphics()
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
}

void terminate_graphics()
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void wait(int time)
{
    SDL_Delay(time);
}