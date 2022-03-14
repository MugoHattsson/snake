#define SCREEN_WIDTH 720
#define SCREEN_HEIGHT 720
#define ARENA_ROWS 6 // Has to be divisible by 2
#define ARENA_COLS 6
#define TILE_SIZE 120

// #define SCREEN_WIDTH 840
// #define SCREEN_HEIGHT 840
// #define ARENA_ROWS 14 // Has to be divisible by 2
// #define ARENA_COLS 14
// #define TILE_SIZE 60

// #define SCREEN_WIDTH 900
// #define SCREEN_HEIGHT 900
// #define ARENA_ROWS 30 // Has to be divisible by 2
// #define ARENA_COLS 30
// #define TILE_SIZE 30

typedef enum
{
    UP,
    DOWN,
    LEFT,
    RIGHT
} direction;