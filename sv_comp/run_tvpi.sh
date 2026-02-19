#!/usr/bin/env bash

#virtual memory limit
ulimit -v $((128 * 1024))

benchmark_dir="../../../Dev/Benchmarks"

goto_analyzer="../src/goto-analyzer/goto-analyzer"
options="--show --tvpi --verbosity 10 --recursive-interprocedural"

output_dir="./out"

mkdir -p $output_dir

#pick your list of files to be verified
#c_files=$(find "$benchmark_dir" -type f -name "*.c")

#array_files="./array_memsafety.txt"
array_files="./array_no_overflow.txt"
#array_files="./array_unreach_call.txt"

#first command line argument
RUN=$1
counter=999
file_name="test"

for benchmark_file in $(cat "$array_files"); do
    full="./$benchmark_file"
    echo $full
    echo $RUN
    #OUTPUTFILE="$output_dir/$benchmark_file-results-run-$RUN"
    #OUTPUTFILE="$output_dir/$file_name"
    OUTPUTFILE="$output_dir/run-${counter}-results.txt"
    echo $OUTPUTFILE
    date | tee $OUTPUTFILE
    gtimeout 25 /usr/bin/time -l -p $goto_analyzer $options $full 2>&1 | tee -a $OUTPUTFILE
    ((counter++))
done