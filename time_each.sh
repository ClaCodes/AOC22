AOC_BASE=./*/
for EXE in $(find $AOC_BASE -executable -type f | sort)
do
    INPUTA=$(echo $EXE | awk -F '/' '{print "./" $2 "/input"}')
    PER=$(echo $EXE | awk -F '[/]' '{print "./" $2 $3 "_perf"}')
    echo $PER
    perf stat -o $PER $EXE ${INPUTA}>>output
    cat $PER >> perf_stat
    rm $PER
done
diff output expected_output && echo "PASS\n"; rm output



