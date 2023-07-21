#!/bin/bash
{
    perf stat --repeat=10 -o ./temp_stat --append ./day01 ./real_input/day01;
    perf stat --repeat=10 -o ./temp_stat --append ./day02 ./real_input/day02;
    perf stat --repeat=10 -o ./temp_stat --append ./day03 ./real_input/day03;
    perf stat --repeat=10 -o ./temp_stat --append ./day04 ./real_input/day04;
    perf stat --repeat=10 -o ./temp_stat --append ./day05 ./real_input/day05;
    perf stat --repeat=10 -o ./temp_stat --append ./day06 ./real_input/day06;
    perf stat --repeat=10 -o ./temp_stat --append ./day07 ./real_input/day07;
    perf stat --repeat=10 -o ./temp_stat --append ./day08 ./real_input/day08;
    perf stat --repeat=10 -o ./temp_stat --append ./day09 ./real_input/day09;
    perf stat --repeat=10 -o ./temp_stat --append ./day10 ./real_input/day10;
} >> /dev/nul

# Assumes performance stats were collected in order
awk '
BEGIN {printf("| day | task-clock |\n|----:|-----------:|\n");i=1}
/.*task/{printf("| %d | %8.2f ms |\n",i,$1);++i}
' >> perf_stat_res < temp_stat

cat perf_stat_res
rm perf_stat_res
rm temp_stat
