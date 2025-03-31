#!/bin/bash
find ./main.c | entr -crs 'gcc main.c -Werror -Wall -Wextra -Wwrite-strings -Wunused-macros -O3 -o main && ./main'
