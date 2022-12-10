AOC_BASE=./*/
rm perf_stat.md
for EXE in $(find $AOC_BASE -executable -type f | sort)
do
    INPUTA=$(echo $EXE | awk -F '/' '{print "./" $2 "/input"}')
    DIR=$(echo $EXE | awk -F '/' '{print $2 }')
    FILE=$(echo $EXE | awk -F '/' '{print $3 }')
    echo -n "| " $DIR " | " $FILE " | " >> perf_stat.md
    #PER=$(echo $EXE | awk -F '[/]' '{print "./" $2 $3 "_perf"}')
    perf stat --repeat=10 -o temp_perf $EXE ${INPUTA}>>/dev/nul #>>output
    #echo "asdf">>perf_stat.md
    cat temp_perf | awk '/.*task/{printf("%8.2f ms",$1)}' >> perf_stat.md
    echo " |" >> perf_stat.md
    rm temp_perf
done
cat perf_stat.md
#diff output expected_output && echo "PASS\n"; rm output



