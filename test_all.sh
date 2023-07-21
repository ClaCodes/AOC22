#!/bin/bash
{
    ./day01 ./example_input/day01;
    ./day02 ./example_input/day02;
    ./day03 ./example_input/day03;
    ./day04 ./example_input/day04;
    ./day05 ./example_input/day05;
    ./day06 ./example_input/day06;
    ./day07 ./example_input/day07;
    ./day08 ./example_input/day08;
    ./day09 ./example_input/day09;
    ./day10 ./example_input/day10;
    ./day11 ./example_input/day11;
} >> example_output

diff example_output expected_example_output && printf "PASS\n"; rm example_output


{
    ./day01 ./real_input/day01;
    ./day02 ./real_input/day02;
    ./day03 ./real_input/day03;
    ./day04 ./real_input/day04;
    ./day05 ./real_input/day05;
    ./day06 ./real_input/day06;
    ./day07 ./real_input/day07;
    ./day08 ./real_input/day08;
    ./day09 ./real_input/day09;
    ./day10 ./real_input/day10;
    ./day11 ./real_input/day11;
} >> real_output

diff real_output expected_real_output && printf "PASS\n"; rm real_output
