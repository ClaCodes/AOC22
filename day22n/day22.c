#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

#define MAX_CMD_CNT 99999
/*#define SIDE_LEN 50*/
#define SIDE_LEN 4
#define MAX_SIDE_CNT 6
#define NO_NEIGHBOUR 234

enum block_type {
    UNDEF = 0,
    FLOOR,
    WALL,
};

char block_char(enum block_type t) {
    switch(t) {
        case UNDEF: return ' ';
        case FLOOR: return '.';
        case WALL: return '#';
    }
    fprintf(stderr, "undef block %d", t);
    exit(EXIT_FAILURE);
}

struct row {
    enum block_type blocks[SIDE_LEN];
};

enum rotation {
    NONE,
    CLOCK90,
    CLOCK180,
    CLOCK270,
};

struct neighbour_id {
    size_t i;
    enum rotation rotation;
};

struct neighbours {
    struct neighbour_id top;
    struct neighbour_id bot;
    struct neighbour_id right;
    struct neighbour_id left;
};

struct side {
    struct row rows[SIDE_LEN];
    size_t i;
    size_t j;
    struct neighbours neighbours;
};

struct cube {
    struct side sides[MAX_SIDE_CNT];
    size_t side_cnt;
};

enum command_type {
    STEPS,
    CLOCK,
    COUNTERCLOCK,
};

struct command {
    enum command_type command_type;
    int steps;
};

struct cmd_array {
    struct command cmds[MAX_CMD_CNT];
    size_t cmd_cnt;
};

struct problem {
    struct cube cube;
    struct cmd_array cmds;
};

void add_block(struct cube *c, size_t ii, size_t jj, enum block_type t)
{
    size_t imod = ii % SIDE_LEN;
    size_t jmod = jj % SIDE_LEN;
    size_t i = ii / SIDE_LEN;
    size_t j = jj / SIDE_LEN;
    struct side *s = NULL;
    for(size_t k=0;k<c->side_cnt;++k) {
        s = &c->sides[k];
        if(s->i == i && s->j == j) {
            break;
        } else {
            s = NULL;
        }
    }

    if(s == NULL) {
        if(c->side_cnt < ARRAY_SIZE(c->sides)) {
            s = &c->sides[c->side_cnt];
            c->side_cnt++;
            s->i = i;
            s->j = j;
        } else {
            fprintf(stderr, "Not enough space for cube side. Increase c->sides array size. Have %zu.\n", ARRAY_SIZE(c->sides));
            exit(EXIT_FAILURE);
        }
    }

    if(imod > ARRAY_SIZE(s->rows)) {
        fprintf(stderr, "Increase array size of s->rows. have %zu.\n", ARRAY_SIZE(s->rows));
        exit(EXIT_FAILURE);
    }

    if(jmod > ARRAY_SIZE(s->rows[imod].blocks)) {
        fprintf(stderr, "Increase array size of s->rows[row].blocks. have %zu.\n", ARRAY_SIZE(s->rows[imod].blocks));
        exit(EXIT_FAILURE);
    }

    s->rows[imod].blocks[jmod] = t;
}

void scan_cube(FILE *in, struct cube *cube)
{
    size_t i=0;
    size_t j=0;
    while (1) {
        char c = fgetc(in);
        fprintf(stderr, "%c", c);
        switch (c) {
            case ' ': j++; break;
            case '\n': {
                           if(j==0) {
                               /*fprintf(stderr, "done with cube at %zu, %zu\n", i,j);*/
                               return;
                           }else {
                               i++;
                               j=0;
                               break;
                           }
                       }
            case '.': add_block(cube, i, j, FLOOR); j++; break;
            case '#': add_block(cube, i, j, WALL); j++; break;
            default:
                      fprintf(stderr, "bad stuff\n");
                      exit(EXIT_FAILURE);

        }
    }
}

void update_with_digit(struct cmd_array *ca, char c) {
    if(ca->cmd_cnt < 0 || ca->cmd_cnt > ARRAY_SIZE(ca->cmds)) {
        fprintf(stderr, "command array size assumption violated.\n");
        exit(EXIT_FAILURE);
    }
    ca->cmds[ca->cmd_cnt].command_type = STEPS;
    ca->cmds[ca->cmd_cnt].steps *= 10;
    ca->cmds[ca->cmd_cnt].steps += c - '0';
}

void update_with_rotation(struct cmd_array *ca, enum command_type c)
{
    ca->cmd_cnt++;
    if(ca->cmd_cnt >= ARRAY_SIZE(ca->cmds)) {
        fprintf(stderr, "increase array size of ca->cmds. is %zu.\n", ARRAY_SIZE(ca->cmds));
        exit(EXIT_FAILURE);
    }
    ca->cmds[ca->cmd_cnt].command_type = c;
    ca->cmd_cnt++;
    if(ca->cmd_cnt >= ARRAY_SIZE(ca->cmds)) {
        fprintf(stderr, "increase array size of ca->cmds. is %zu.\n", ARRAY_SIZE(ca->cmds));
        exit(EXIT_FAILURE);
    }
}

void scan_commands(FILE* in, struct cmd_array *ca)
{
    int in_number = 0;
    while (1) {
        char c = fgetc(in);
        if(c==EOF) {
            break;
        }
        else if (isspace(c)) {
            //ignore
        } else if(isdigit(c)) {
            in_number = 1;
            update_with_digit(ca, c);
        } else if(c=='R') {
            in_number = 0;
            update_with_rotation(ca, CLOCK);
        } else if (c=='L') {
            in_number = 0;
            update_with_rotation(ca, COUNTERCLOCK);
        } else {
            fprintf(stderr, "unknown input '%c'\n", c);
            exit(EXIT_FAILURE);
        }
    }
    if(in_number) {
        ca->cmd_cnt ++;
    }
}

void scan_problem(FILE *in, struct problem *p)
{
    scan_cube(in, &p->cube);
    scan_commands(in, &p->cmds);
}

enum neighbour_dir {
    LEFT, RIGHT, TOP, BOT,
};

int find_side(struct cube *cube, int i, int j, size_t *found_index)
{
    for(size_t ii = 0; ii<ARRAY_SIZE(cube->sides);++ii) {
        if(cube->sides[ii].i==i && cube->sides[ii].j==j) {
            *found_index = ii;
            return 1;
        }
    }
    return 0;
}

size_t index_of_neighbour(struct cube *cube, size_t ii, enum neighbour_dir dir)
{
    int found = 0;
    size_t found_index = 0;
    size_t i = cube->sides[ii].i;
    size_t j = cube->sides[ii].j;
    while(!found) {
        switch(dir) {
            case LEFT: i-=1;break;
            case RIGHT: i+=1;break;
            case TOP: j-=1;break;
            case BOT: j+=1;break;
        }
        i%=MAX_SIDE_CNT;
        j%=MAX_SIDE_CNT;
        found = find_side(cube, i,j, &found_index);
    }
    return found_index;
}

size_t index_of_neighbourB(struct cube *cube, size_t ii, enum neighbour_dir dir)
{
    int found = 0;
    size_t found_index = 0;
    size_t i = cube->sides[ii].i;
    size_t j = cube->sides[ii].j;
        switch(dir) {
            case LEFT: i-=1;break;
            case RIGHT: i+=1;break;
            case TOP: j-=1;break;
            case BOT: j+=1;break;
        }
        i%=MAX_SIDE_CNT;
        j%=MAX_SIDE_CNT;
        found = find_side(cube, i,j, &found_index);
    if(found) {
        return found_index;
    } else {
        return NO_NEIGHBOUR;
    }
}

enum neighbour_dir clock(enum neighbour_dir dir) {
    switch (dir) {
    case LEFT:return TOP;
    case RIGHT:return BOT;
    case TOP:return RIGHT;
    case BOT: return LEFT;
    }
}

enum neighbour_dir counterclock(enum neighbour_dir dir) {
    switch (dir) {
    case LEFT:return BOT;
    case RIGHT:return TOP;
    case TOP:return LEFT;
    case BOT: return RIGHT;
    }
}

const char* neighbout_dir_txt(enum neighbour_dir dir) {
    switch (dir) {
    case LEFT: return "LEFT";
    case RIGHT: return "RIGHT";
    case TOP: return "TOP";
    case BOT: return "BOT";
    }
}

struct cursor {
    size_t side_index;
    int i;
    int j;
    enum neighbour_dir facing;
};

void rotate_cursor_clock(struct cursor *cursor)
{
    struct cursor new_cursor = *cursor;
    new_cursor.facing = clock(cursor->facing);
    new_cursor.i = cursor->j;
    new_cursor.j = SIDE_LEN - 1 - cursor->i;
    *cursor = new_cursor;
}

void rotate(struct cursor *cursor, enum rotation r)
{
    switch (r) {
        default:
        case NONE:
            break;
        case CLOCK90:
            rotate_cursor_clock(cursor);
            break;;
        case CLOCK180:
            rotate_cursor_clock(cursor);
            rotate_cursor_clock(cursor);
            break;
        case CLOCK270:
            rotate_cursor_clock(cursor);
            rotate_cursor_clock(cursor);
            rotate_cursor_clock(cursor);
            break;
    }
}

void wrap_cursor(struct cube *cube, struct cursor *cursor)
{
    struct side *s = &cube->sides[cursor->side_index];
    if(cursor->i<0) {
        cursor->side_index = s->neighbours.left.i;
        cursor->i+=SIDE_LEN;
        rotate(cursor, s->neighbours.left.rotation);
    }else if (cursor->j<0) {
        cursor->side_index = s->neighbours.top.i;
        cursor->j+=SIDE_LEN;
        rotate(cursor, s->neighbours.top.rotation);
    } else if(cursor->i>=ARRAY_SIZE(s->rows)) {
        cursor->side_index = s->neighbours.right.i;
        cursor->i-=SIDE_LEN;
        rotate(cursor, s->neighbours.right.rotation);
    } else if(cursor->j>=ARRAY_SIZE(s->rows[cursor->i].blocks)) {
        cursor->side_index = s->neighbours.bot.i;
        cursor->j-=SIDE_LEN;
        rotate(cursor, s->neighbours.bot.rotation);
    }
}

enum block_type block_at(struct cube *cube, size_t si, int i, int j) {
    struct cursor cursor = {
        .side_index = si,
        .i=i,
        .j=j
    };
    wrap_cursor(cube, &cursor);
    i=cursor.i;
    j=cursor.j;
    si=cursor.side_index;
    struct side *s = &cube->sides[si];

    if(i<0) {
        return UNDEF;
    }else if (j<0) {
        return UNDEF;
    } else if(i>=ARRAY_SIZE(s->rows)) {
        return UNDEF;
    } else if(j>=ARRAY_SIZE(s->rows[i].blocks)) {
        return UNDEF;
    } else {
        return s->rows[i].blocks[j];
    }
}

void print_cube(struct cube *cube)
{
    for(ssize_t s = 0; s<ARRAY_SIZE(cube->sides); ++s) {
        struct side *side = &cube->sides[s];
        fprintf(stderr, "Cube side of %zu i=%zu, j=%zu:\n", s, side->i, side->j);
        for(int i= -SIDE_LEN; i<2*SIDE_LEN; ++i) {
            for(int j= -SIDE_LEN; j<2*SIDE_LEN; ++j) {
                    fprintf(stderr, "%c", block_char(block_at(cube, s, i, j)));
            }
            fprintf(stderr, "\n");
        }
        fprintf(stderr, "\n");
    }
}


int can_move_there(struct cube *cube, struct cursor *cursor)
{
    if (cursor->side_index > cube->side_cnt) {
        return 0;
    }
    struct side *side = &cube->sides[cursor->side_index];
    if(cursor->i > ARRAY_SIZE(side->rows)) {
        return 0;
    }
    struct row *row = &side->rows[cursor->i];
    if(cursor->j > ARRAY_SIZE(row->blocks)) {
        return 0;
    }
    return row->blocks[cursor->j] == FLOOR;
}

void cursor_step(struct cube *cube, struct cursor *cursor, int steps)
{
    for(int i = 0; i < steps; i++) {
        struct cursor next_cursor = *cursor;
        switch(next_cursor.facing) {
            case LEFT: next_cursor.j--; break;
            case RIGHT: next_cursor.j++; break;
            case TOP: next_cursor.i--; break;
            case BOT: next_cursor.i++; break;
        }
        wrap_cursor(cube, &next_cursor);
        if(can_move_there(cube, &next_cursor)) {
            *cursor = next_cursor;
        } else {
            break;
        }
    }
}

int face_score(enum neighbour_dir facing) {
    switch (facing) {
    case LEFT:return 2;
    case RIGHT:return 0;
    case TOP:return 3;
    case BOT: return 1;
    }
}

int score(struct cube *cube, struct cursor *cursor)
{
    size_t global_i = cube->sides[cursor->side_index].i;
    size_t global_j = cube->sides[cursor->side_index].j;

    size_t row = global_i * SIDE_LEN + cursor->i + 1;
    size_t column = global_j * SIDE_LEN + cursor->j + 1;

    fprintf(stderr, "Scoring position is row %zu, column %zu, facing %s\n", row, column, neighbout_dir_txt(cursor->facing));

    return 1000 * row + 4 * column + face_score(cursor->facing);

}

int solve_a(struct problem *problem)
{
    struct cursor cursor = {.facing = RIGHT};

    for(size_t ii = 0; ii < problem->cmds.cmd_cnt; ++ii) {
        struct command *cmd = &problem->cmds.cmds[ii];
        switch (cmd->command_type) {
            case STEPS:
                fprintf(stderr, "step %d, ", cmd->steps);
                cursor_step(&problem->cube, &cursor, cmd->steps);
                /*score(&problem->cube, &cursor);*/
                break;
            case CLOCK:
                fprintf(stderr, "clock, ");
                cursor.facing = clock(cursor.facing);
                break;
            case COUNTERCLOCK:
                fprintf(stderr, "counterclock, ");
                cursor.facing = counterclock(cursor.facing);
                break;
        }
    }

    fprintf(stderr, "Final position is %d %d %s\n", cursor.i, cursor.j, neighbout_dir_txt(cursor.facing));


    return score(&problem->cube, &cursor);
}

void compute_A(struct cube *cube)
{
    for(ssize_t s = 0; s<ARRAY_SIZE(cube->sides); ++s) {
        struct side *side = &cube->sides[s];
        side->neighbours.left.i = index_of_neighbour(cube, s, LEFT);
        side->neighbours.top.i = index_of_neighbour(cube, s, TOP);
        side->neighbours.right.i = index_of_neighbour(cube, s, RIGHT);
        side->neighbours.bot.i = index_of_neighbour(cube, s, BOT);
    }
}

void compute_B(struct cube *cube)
{
    for(ssize_t s = 0; s<ARRAY_SIZE(cube->sides); ++s) {
        struct side *side = &cube->sides[s];
        side->neighbours.left.i = index_of_neighbourB(cube, s, LEFT);
        side->neighbours.top.i = index_of_neighbourB(cube, s, TOP);
        side->neighbours.right.i = index_of_neighbourB(cube, s, RIGHT);
        side->neighbours.bot.i = index_of_neighbourB(cube, s, BOT);
    }

    print_cube(cube);

    for(ssize_t s = 0; s<ARRAY_SIZE(cube->sides); ++s) {
        struct side *side = &cube->sides[s];
    }

    print_cube(cube);

}

int main(int argc, char **argv)
{
    FILE *in;
    if(argc > 1) {
        in = fopen(argv[1], "r");
    } else {
        in = stdin;
    }

    struct problem problem = {0};
    scan_problem(in, &problem);
    compute_A(&problem.cube);
    /*print_cube(&problem.cube);*/
    int sa = solve_a(&problem);
    fprintf(stdout, "Solution A: %d\n", sa);
    compute_B(&problem.cube);
    print_cube(&problem.cube);
    /*int sb = solve_b(&problem);*/
    /*fprintf(stdout, "Solution B: %d\n", sb);*/
}
