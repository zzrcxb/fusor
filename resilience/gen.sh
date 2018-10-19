#! /bin/sh

for s in 1 2 3 4 5
do
    for r in 3 4 5 6 7
    do
        echo "python3 ptl.py -i main4.ptl --context \"{\\\"size\\\": 16, \\\"s_level\\\": ${s}, \\\"r_level\\\": ${r}}\" | clang -xc - -o ${s}-${r}.out"
    done
done

