#!/bin/bash
{
    ./day01 ./example_input/day01;
    ./day02 ./example_input/day02;
    ./day03 ./example_input/day03;
    ./day04 ./example_input/day04;
} >> example_output

diff example_output expected_example_output && printf "PASS\n"; rm example_output


{
    ./day01 ./real_input/day01;
    ./day02 ./real_input/day02;
    ./day03 ./real_input/day03;
    ./day04 ./real_input/day04;
} >> real_output

diff real_output expected_real_output && printf "PASS\n"; rm real_output
