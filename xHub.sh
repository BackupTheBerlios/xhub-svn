#!/bin/sh
#export LD_LIBRARY_PATH=.:LD_LIBRARY_PATH
while (true) do
./server $*
echo Restartuji za 120 s ...
sleep 120
done

