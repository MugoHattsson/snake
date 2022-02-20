#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>

#define LOG_TO_FILE
// #undef LOG_TO_FILE

enum
{
    ARENA_ROWS = 30,
    ARENA_COLS = 30,
    ARENA_SIZE = ARENA_ROWS * ARENA_COLS
};

enum direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

typedef struct
{
    int visited;
    int canGoRight;
    int canGoDown;
} node_t;

node_t nodes[ARENA_ROWS / 2][ARENA_COLS / 2];
unsigned int tour[ARENA_ROWS][ARENA_COLS];

void explore(int fromRow, int fromCol, int row, int col)
{
    if (row < 0 || col < 0 || row >= ARENA_ROWS / 2 || col >= ARENA_COLS / 2)
        return;
    if (nodes[row][col].visited == 1)
        return;

    nodes[row][col].visited = 1;

    // Create edges between nodes
    if (fromRow != -1)
    {
        if (fromRow < row)
            nodes[fromRow][fromCol].canGoDown = 1;
        else if (fromRow > row)
            nodes[row][col].canGoDown = 1;
        else if (fromCol < col)
            nodes[fromRow][fromCol].canGoRight = 1;
        else if (fromCol > col)
            nodes[row][col].canGoRight = 1;
    }

    for (int i = 0; i < 1; i++)
    {
        int r = rand() % 4;
        switch (r)
        {
        case 0:
            explore(row, col, row - 1, col);
            break;
        case 1:
            explore(row, col, row + 1, col);
            break;
        case 2:
            explore(row, col, row, col - 1);
            break;
        case 3:
            explore(row, col, row, col + 1);
            break;

        default:
            break;
        }
    }
    explore(row, col, row - 1, col);
    explore(row, col, row + 1, col);
    explore(row, col, row, col - 1);
    explore(row, col, row, col + 1);
}

int canGoUp(int row, int col)
{
    return row == 0 ? 0 : nodes[row - 1][col].canGoDown;
}

int canGoDown(int row, int col)
{
    return nodes[row][col].canGoDown;
}

int canGoLeft(int row, int col)
{
    return col == 0 ? 0 : nodes[row][col - 1].canGoRight;
}

int canGoRight(int row, int col)
{
    return nodes[row][col].canGoRight;
}

enum direction findNextDir(int row, int col, enum direction dir)
{
    switch (dir)
    {
    case UP:
        if (canGoLeft(row, col))
            return LEFT;
        if (canGoUp(row, col))
            return UP;
        if (canGoRight(row, col))
            return RIGHT;
        return DOWN;
        break;
    case DOWN:
        if (canGoRight(row, col))
            return RIGHT;
        if (canGoDown(row, col))
            return DOWN;
        if (canGoLeft(row, col))
            return LEFT;
        return UP;
        break;
    case LEFT:
        if (canGoDown(row, col))
            return DOWN;
        if (canGoLeft(row, col))
            return LEFT;
        if (canGoUp(row, col))
            return UP;
        return RIGHT;
        break;
    case RIGHT:
        if (canGoUp(row, col))
            return UP;
        if (canGoRight(row, col))
            return RIGHT;
        if (canGoDown(row, col))
            return DOWN;
        return LEFT;
        break;
    }
}

void setTourNumber(int row, int col, int number)
{
    if (tour[row][col] != 0)
        return;
    tour[row][col] = number;
}

void createTour()
{
    int row = 0;
    int col = 0;
    int number = 1;
    enum direction dir = nodes[row][col].canGoDown ? UP : LEFT;
    enum direction nextDir;
    do
    {

        printf("%i, %i, %i, %i, %i\n", row, col, number, dir, nextDir);
        nextDir = findNextDir(row, col, dir);
        switch (dir)
        {
        case UP:
            setTourNumber(row * 2 + 1, col * 2, number++);
            if (nextDir == dir || nextDir == RIGHT || nextDir == DOWN)
                setTourNumber(row * 2, col * 2, number++);
            if (nextDir == RIGHT || nextDir == DOWN)
                setTourNumber(row * 2, col * 2 + 1, number++);
            if (nextDir == DOWN)
                setTourNumber(row * 2 + 1, col * 2 + 1, number++);
            break;
        case DOWN:
            setTourNumber(row * 2, col * 2 + 1, number++);
            if (nextDir == dir || nextDir == LEFT || nextDir == UP)
                setTourNumber(row * 2 + 1, col * 2 + 1, number++);
            if (nextDir == LEFT || nextDir == UP)
                setTourNumber(row * 2 + 1, col * 2, number++);
            if (nextDir == UP)
                setTourNumber(row * 2, col * 2, number++);
            break;
        case LEFT:
            setTourNumber(row * 2 + 1, col * 2 + 1, number++);
            if (nextDir == dir || nextDir == UP || nextDir == RIGHT)
                setTourNumber(row * 2 + 1, col * 2, number++);
            if (nextDir == UP || nextDir == RIGHT)
                setTourNumber(row * 2, col * 2, number++);
            if (nextDir == RIGHT)
                setTourNumber(row * 2, col * 2 + 1, number++);
            break;
        case RIGHT:
            setTourNumber(row * 2, col * 2, number++);
            if (nextDir == dir || nextDir == DOWN || nextDir == LEFT)
                setTourNumber(row * 2, col * 2 + 1, number++);
            if (nextDir == DOWN || nextDir == LEFT)
                setTourNumber(row * 2 + 1, col * 2 + 1, number++);
            if (nextDir == LEFT)
                setTourNumber(row * 2 + 1, col * 2, number++);
            break;
        }
        dir = nextDir;

        switch (nextDir)
        {
        case UP:
            --row;
            break;
        case DOWN:
            ++row;
            break;
        case LEFT:
            --col;
            break;
        case RIGHT:
            ++col;
            break;
        }

        printf("%i, %i, %i\n", row, col, number);

    } while (number <= ARENA_SIZE);
}

#ifdef LOG_TO_FILE
void writeMazeToFile()
{
    FILE *f = fopen("maze.txt", "w+");
    for (int r = 0; r < ARENA_ROWS / 2; r++)
    {
        fprintf(f, "#");
        for (int c = 0; c < ARENA_COLS / 2; c++)
        {
            if (nodes[r][c].canGoRight && nodes[r][c].canGoDown)
                fprintf(f, "+");
            else if (nodes[r][c].canGoRight)
                fprintf(f, "-");
            else if (nodes[r][c].canGoDown)
                fprintf(f, "|");
            else
                fprintf(f, " ");
        }
        fprintf(f, "#\n");
    }
    fclose(f);
}

void writeTourToFile()
{
    FILE *f = fopen("tour.txt", "w+");
    for (int row = 0; row < ARENA_ROWS; row++)
    {
        for (int col = 0; col < ARENA_COLS; col++)
        {
            fprintf(f, "%4d", tour[row][col]);
        }
        fprintf(f, "\n");
    }
    fclose(f);
}
#endif

int main(void)
{
    srand(time(0));
    memset(&nodes, 0, sizeof(nodes));
    memset(&tour, 0, sizeof(tour));
    explore(-1, -1, 0, 0);
    createTour();

#ifdef LOG_TO_FILE
    writeMazeToFile();
    writeTourToFile();
#endif
}