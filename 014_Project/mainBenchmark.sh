#!/bin/bash

echo "Benchmark has started."

# threads and blocksize arguments
threads=("1" "2" "4" "8" "16" "32")
blockSizes=("1" "32" "256" "1024" "2048" "4096")
flags=("i" "s")
largeFileAddress="largeFile.txt"
binFileAddress="binFile.txt"
executableFileName="./main"

# Iterate over flags
for flag in "${flags[@]}"; do
  # If flag is "s", run with a fixed block size of 1
  if [ "$flag" == "s" ]; then
    for thread in "${threads[@]}"; do
      $executableFileName $largeFileAddress $binFileAddress $thread 1 $flag
    done
  else
    # For other flags, iterate over threads and block sizes
    for thread in "${threads[@]}"; do
      for blockSize in "${blockSizes[@]}"; do
        $executableFileName $largeFileAddress $binFileAddress $thread $blockSize $flag
      done
    done
  fi
done

echo "Benchmark has completed."
