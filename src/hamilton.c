#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>
#include "main.h"

// #define LOG_TO_FILE

int ARENA_SIZE = ARENA_ROWS * ARENA_COLS;

typedef struct node_t
{
    int visited;
    int can_go_right;
    int can_go_down;
} node_t;

node_t nodes[ARENA_ROWS / 2][ARENA_COLS / 2];
int **tour;

#ifdef LOG_TO_FILE
void maze_to_file()
{
    FILE *f = fopen("maze.txt", "w+");
    for (int r = 0; r < ARENA_ROWS / 2; r++)
    {
        fprintf(f, "#");
        for (int c = 0; c < ARENA_COLS / 2; c++)
        {
            if (nodes[r][c].can_go_right && nodes[r][c].can_go_down)
                fprintf(f, "+");
            else if (nodes[r][c].can_go_right)
                fprintf(f, "-");
            else if (nodes[r][c].can_go_down)
                fprintf(f, "|");
            else
                fprintf(f, " ");
        }
        fprintf(f, "#\n");
    }
    fclose(f);
}

void tour_to_file()
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

void generate_maze(int fromRow, int fromCol, int row, int col)
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
            nodes[fromRow][fromCol].can_go_down = 1;
        else if (fromRow > row)
            nodes[row][col].can_go_down = 1;
        else if (fromCol < col)
            nodes[fromRow][fromCol].can_go_right = 1;
        else if (fromCol > col)
            nodes[row][col].can_go_right = 1;
    }

    for (int i = 0; i < 1; i++)
    {
        int r = rand() % 4;
        switch (r)
        {
        case 0:
            generate_maze(row, col, row - 1, col);
            break;
        case 1:
            generate_maze(row, col, row + 1, col);
            break;
        case 2:
            generate_maze(row, col, row, col - 1);
            break;
        case 3:
            generate_maze(row, col, row, col + 1);
            break;

        default:
            break;
        }
    }
    generate_maze(row, col, row - 1, col);
    generate_maze(row, col, row + 1, col);
    generate_maze(row, col, row, col - 1);
    generate_maze(row, col, row, col + 1);
}

int can_go_up(int row, int col)
{
    return row == 0 ? 0 : nodes[row - 1][col].can_go_down;
}

int can_go_down(int row, int col)
{
    return nodes[row][col].can_go_down;
}

int can_go_left(int row, int col)
{
    return col == 0 ? 0 : nodes[row][col - 1].can_go_right;
}

int can_go_right(int row, int col)
{
    return nodes[row][col].can_go_right;
}

direction find_next_dir(int row, int col, direction dir)
{
    switch (dir)
    {
    case UP:
        if (can_go_left(row, col))
            return LEFT;
        if (can_go_up(row, col))
            return UP;
        if (can_go_right(row, col))
            return RIGHT;
        return DOWN;
        break;
    case DOWN:
        if (can_go_right(row, col))
            return RIGHT;
        if (can_go_down(row, col))
            return DOWN;
        if (can_go_left(row, col))
            return LEFT;
        return UP;
        break;
    case LEFT:
        if (can_go_down(row, col))
            return DOWN;
        if (can_go_left(row, col))
            return LEFT;
        if (can_go_up(row, col))
            return UP;
        return RIGHT;
        break;
    case RIGHT:
        if (can_go_up(row, col))
            return UP;
        if (can_go_right(row, col))
            return RIGHT;
        if (can_go_down(row, col))
            return DOWN;
        return LEFT;
        break;
    }
}

void set_tour_number(int row, int col, int number)
{
    if (tour[row][col] != 0)
        return;
    tour[row][col] = number;
}

void create_tour()
{
    int row = 0;
    int col = 0;
    int number = 1;
    direction dir = nodes[row][col].can_go_down ? UP : LEFT;
    direction nextDir;
    do
    {

        // printf("%i, %i, %i, %i, %i\n", row, col, number, dir, nextDir);
        nextDir = find_next_dir(row, col, dir);
        switch (dir)
        {
        case UP:
            set_tour_number(row * 2 + 1, col * 2, number++);
            if (nextDir == dir || nextDir == RIGHT || nextDir == DOWN)
                set_tour_number(row * 2, col * 2, number++);
            if (nextDir == RIGHT || nextDir == DOWN)
                set_tour_number(row * 2, col * 2 + 1, number++);
            if (nextDir == DOWN)
                set_tour_number(row * 2 + 1, col * 2 + 1, number++);
            break;
        case DOWN:
            set_tour_number(row * 2, col * 2 + 1, number++);
            if (nextDir == dir || nextDir == LEFT || nextDir == UP)
                set_tour_number(row * 2 + 1, col * 2 + 1, number++);
            if (nextDir == LEFT || nextDir == UP)
                set_tour_number(row * 2 + 1, col * 2, number++);
            if (nextDir == UP)
                set_tour_number(row * 2, col * 2, number++);
            break;
        case LEFT:
            set_tour_number(row * 2 + 1, col * 2 + 1, number++);
            if (nextDir == dir || nextDir == UP || nextDir == RIGHT)
                set_tour_number(row * 2 + 1, col * 2, number++);
            if (nextDir == UP || nextDir == RIGHT)
                set_tour_number(row * 2, col * 2, number++);
            if (nextDir == RIGHT)
                set_tour_number(row * 2, col * 2 + 1, number++);
            break;
        case RIGHT:
            set_tour_number(row * 2, col * 2, number++);
            if (nextDir == dir || nextDir == DOWN || nextDir == LEFT)
                set_tour_number(row * 2, col * 2 + 1, number++);
            if (nextDir == DOWN || nextDir == LEFT)
                set_tour_number(row * 2 + 1, col * 2 + 1, number++);
            if (nextDir == LEFT)
                set_tour_number(row * 2 + 1, col * 2, number++);
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

        // printf("%i, %i, %i\n", row, col, number);

    } while (number <= ARENA_SIZE);
}

int **hamiltonian(void)
{
    tour = calloc(1, ARENA_ROWS * sizeof(int *));
    for (int row = 0; row < ARENA_ROWS; row++)
    {
        tour[row] = calloc(1, ARENA_COLS * sizeof(int));
    }
    // printf("%i", tour[0][1]);

    srand(time(0));
    // memset(&nodes, 0, sizeof(nodes));
    generate_maze(-1, -1, 0, 0);
    create_tour();

#ifdef LOG_TO_FILE
    maze_to_file();
    tour_to_file();
#endif

    return tour;
}