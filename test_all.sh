#!/bin/bash
./day01 ./example_input/day01 >> example_output
./day02 ./example_input/day02 >> example_output

diff example_output expected_example_output && printf "PASS\n"; rm example_output


./day01 ./real_input/day01 >> real_output
./day02 ./real_input/day02 >> real_output

diff real_output expected_real_output && printf "PASS\n"; rm real_output
