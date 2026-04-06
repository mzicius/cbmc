 #!/usr/bin/env bash

#virtual memory limit
#ulimit -v $((100 * 1024 * 1024))

#location of benchmarks
benchmark_dir="./current"

#location of the goto-analyzer
goto_analyzer="../src/goto-analyzer/goto-analyzer"
#options of the tool
options="--verify --tvpi --verbosity 10 --recursive-interprocedural --loop-unwind 3 --one-domain-per-history"
ops="--show --tvpi --verbosity 10 --recursive-interprocedural --loop-unwind 3 --one-domain-per-history"

output_dir="./out"

mkdir -p $output_dir

#pick your list of files to be verified
array_files=$(find "$benchmark_dir" -type f -name "*.c")

#array_files="./array_memsafety.txt"
#array_files="./array_no_overflow.txt"
#array_files="./array_unreach_call.txt"

#first command line argument
RUN=$1
file_name="test"

for benchmark_file in $array_files; do
    file_name="./$benchmark_file"
    echo $RUN
    OUTPUTFILE="$output_dir/$(basename $benchmark_file)-results-run-$RUN"
    GOTOFILE="$output_dir/$(basename $benchmark_file)-goto-program-$RUN"
    SHOWFILE="$output_dir/$(basename $benchmark_file)-show-program-$RUN"
    #OUTPUTFILE="$output_dir/$file_name"
    mkdir -p "$(dirname "$OUTPUTFILE")"
    echo $OUTPUTFILE
    date | tee $OUTPUTFILE
    gtimeout 20 /usr/bin/time -l -p $goto_analyzer $options $file_name 2>&1 | tee -a $OUTPUTFILE
    $goto_analyzer $ops $file_name > $SHOWFILE 2>&1
    $goto_analyzer --show-goto-functions $file_name > $GOTOFILE 2>&1
done