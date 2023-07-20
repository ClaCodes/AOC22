#!/bin/bash
{
    perf stat --repeat=10 -o ./temp_stat --append ./day01 ./real_input/day01;
    perf stat --repeat=10 -o ./temp_stat --append ./day02 ./real_input/day02;
} >> /dev/nul

# Assumes performance stats were collected in order
awk '
BEGIN {printf("| day | task-clock |\n|----:|-----------:|\n");i=1}
/.*task/{printf("| %d | %8.2f ms |\n",i,$1);++i}
' >> perf_stat_res < temp_stat

cat perf_stat_res
rm perf_stat_res
rm temp_stat
