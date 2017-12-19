#!/bin/bash


#Colors

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color
EXECUTABLE="../../build/nachos-step3 -x"

Check_Result(){
    if [ "$?" -ne "123" ] 
    then
        printf "${GREEN}OK \n"
    fi
}

printf "\n\n### Testing ###\n\n"

#----------------------------------------------------------------------------
printf "${NC}>> stdio input test ${RED}\t"
${EXECUTABLE} ../../build/test_stdio_input < ./io/input_test_stdio
Check_Result

#----------------------------------------------------------------------------
printf "${NC}>> stdio output/input test ${RED}\t"
${EXECUTABLE} ../../build/test_stdio_output_input  > ./io/global_output < ./io/global_output
Check_Result

#----------------------------------------------------------------------------
printf "${NC}>> Semaphore test (takes time to execute) ${RED}\t"
${EXECUTABLE} ../../build/testsemaphores -rs 15
Check_Result


#############################################
printf "${NC}\n\n----End of Testing----\n\n"

