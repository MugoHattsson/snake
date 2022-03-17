// The number of rows and columns has to be evenly divisible by 2.
// #define ARENA_ROWS 4
// #define ARENA_COLS 4
// #define TILE_SIZE 120

// #define ARENA_ROWS 6
// #define ARENA_COLS 6
// #define TILE_SIZE 120

#define ARENA_ROWS 14
#define ARENA_COLS 14
#define TILE_SIZE 60

// #define ARENA_ROWS 30
// #define ARENA_COLS 30
// #define TILE_SIZE 30

// #define ARENA_ROWS 34
// #define ARENA_COLS 64
// #define TILE_SIZE 30

#define SCREEN_WIDTH ((ARENA_COLS) * (TILE_SIZE))
#define SCREEN_HEIGHT ((ARENA_ROWS) * (TILE_SIZE))
#define ARENA_SIZE ((ARENA_ROWS) * (ARENA_COLS))

typedef enum
{
    UP,
    DOWN,
    LEFT,
    RIGHT
} direction;

typedef struct pos_t pos_t;

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