AOC_BASE=./*/
for EXE in $(find $AOC_BASE -executable -type f | sort)
do
    INPUTA=$(echo $EXE | awk -F '/' '{print "./" $2 "/input"}')
    perf stat $EXE ${INPUTA}>>output
done
diff output expected_output && echo "PASS\n"; rm output



