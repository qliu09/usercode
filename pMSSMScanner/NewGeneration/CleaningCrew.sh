#!/bin/bash


echo "About to start cleaning"

for delfile in pointlog.txt RecentPointStorage.txt genPoint.dat LesHin LesHout micrOMEGAs_Log.txt res_t.tmp fout.root slhaspectrum.in output.flha Constraints.txt susyhit.in susyhit_slha.out LastPoint.cfg; do
  rm -f $delfile
done

echo "Done cleaning"