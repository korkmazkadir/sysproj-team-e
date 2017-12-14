#! /bin/bash

#run from build:
#../test/bashTestStep2.sh ../test/step2ActualOutput.txt ../test/step2ExpectedOutput.txt
#prints OK if test passes
#prints KO if test fails, and writes the result of diff command to difflog.txt

../build/nachos-step2 -x testStep2>$1
nb=`wc -l <$2`
nblines=`expr $nb - 1`
output=`diff <(head -$nblines $1) <(head -$nblines $2)`
if([ "$output" = "" ])
then
    echo OK
else
    echo KO
    echo $output>difflog.txt
fi
