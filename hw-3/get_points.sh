#!/bin/bash

num_threads=$(( $(nproc) * 2))
execution_times=()

for ((i=1; i<=num_threads; i++)); do
    command="/usr/bin/time -f \"%e\" ./make-build/perfect -q -s 1 -e 1000000 -t $i"
    execution_time=$($command 2>&1)

    # removing quotes
    execution_time="${execution_time%\"}"
    execution_time="${execution_time#\"}"

    execution_times+=($execution_time)
done


temp_file="execution_times_temp.txt"
for ((i=0; i<num_threads; i++)); do
    echo "$((i+1)) ${execution_times[i]}" >> "$temp_file"
done

# Replace the original file with the temporary file
output_file="execution_times.txt"
echo "Saving execution times to $output_file"
mv "$temp_file" "$output_file"