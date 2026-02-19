#!/usr/bin/env bash

#virtual memory limit
ulimit -v $((128 * 1024))

benchmark_dir="../benchmarks"
goto_analyzer="../src/goto-analyzer/goto-analyzer"
options="--show --tvpi --verbosity 10 --recursive-interprocedural"
#test="../benchmarks/EqBench/airy/airy/Eq/oldV.c"

c_files=$(find "$benchmark_dir" -type f -name "*.c")

#first command line argument
RUN=$1

for benchmark_file in $c_files; do

    echo $benchmark_file
    #if [[ "$benchmark_file" == "$test" ]]; then
        echo $benchmark_file
        echo $RUN
        OUTPUTFILE="$benchmark_file-results-run-$RUN"
        echo $OUTPUTFILE
        date | tee $OUTPUTFILE
        gtimeout 25 /usr/bin/time -l -p $goto_analyzer $options $benchmark_file 2>&1 | tee -a $OUTPUTFILE
    #fi
done