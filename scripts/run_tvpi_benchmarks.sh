#!/usr/bin/env bash

ulimit -v $((512 * 1024))

benchmark_dir="../benchmarks"
goto_analyzer="../src/goto-analyzer/goto-analyzer"
options="--show --tvpi --verbosity 10 --recursive-interprocedural"

c_files=$(find "$benchmark_dir" -type f -name "*.c")
RUN=$1

for benchmark_file in $c_files; do
    echo $benchmark_file
    echo $RUN
    OUTPUTFILE="$benchmark_file-results-run-$RUN"
    echo $OUTPUTFILE
    date | tee $OUTPUTFILE
    /usr/bin/time -l -p $goto_analyzer $options $benchmark_file 2>&1 | tee -a $OUTPUTFILE
done