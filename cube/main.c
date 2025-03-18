#include "stdlib.h"
#include "stdio.h"
#include "stdbool.h"

#define SIDE 3
#define FIELD_ROWS 6
#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[234]))

char input[] =
"        ...#"
"        .#.."
"        #..."
"        ...."
"...#.......#"
"........#..."
"..#....#...."
"..........#."
"        ...#...."
"        .....#.."
"        .#......"
"        ......#.";

enum tile {
    OPEN,
    WALL,
};

struct row {
    enum tile v[SIDE];
};

struct side {
    struct row rows[SIDE];
    bool in_use;
};

struct field_row {
    struct side fields[FIELD_ROWS];
};

struct field {
    struct field_row field_rows[FIELD_ROWS];
};

void parse_field(struct field *field, const char* in)
{
}

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;
    struct field field = {0};
    parse_field(&field, input);
    return EXIT_SUCCESS;
}
