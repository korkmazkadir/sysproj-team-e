#!/bin/bash


#Colors

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color
EXECUTABLE="../../build/nachos-step3 -x"
EXECUTABLE_RAND="../../build/nachos-step3 -rs -x"

nbFails=0
Check_Result(){
    if [ "$?" -eq "0" ] 
    then
        printf "${GREEN}OK\n\n"
    else
        printf "${RED}KO\n\n"
        nbFails=$(($nbFails + 1))
    fi
}

printf "\n\n---- Testing ----"
printf "${NC}\n\n### STEP 2 ###\n"


#----------------------------------------------------------------------------
printf "${NC}>> stdio input test\n"
${EXECUTABLE} ../../build/test_stdio_input < ./io/input_test_stdio
Check_Result

#----------------------------------------------------------------------------
printf "${NC}>> stdio output/input test ${RED}\n"
${EXECUTABLE} ../../build/test_stdio_output_input  > ./io/global_output < ./io/global_output
Check_Result


printf "${NC}\n\n### STEP 3 ###\n"

#----------------------------------------------------------------------------
printf "${RED}"
printf "${NC}>> Semaphore test (takes order of 10 sec to execute) ${RED}\n"
${EXECUTABLE} ../../build/testsemaphores -rs 15
Check_Result


#----------------------------------------------------------------------------
printf "${NC}>> manyThreads1 (max nb threads) test ${RED}\n"
${EXECUTABLE_RAND} ../../build/manyThreads1
Check_Result


#----------------------------------------------------------------------------
printf "${NC}>> manyThreads2 (memory limitations) test ${RED}\n"
${EXECUTABLE_RAND} ../../build/manyThreads2
Check_Result


#----------------------------------------------------------------------------
printf "${NC}>> nullFuncP test ${RED}\n"
${EXECUTABLE_RAND} ../../build/nullFuncP
Check_Result


#----------------------------------------------------------------------------
printf "${NC}>> userThreadExit test ${RED}\n"
${EXECUTABLE_RAND} ../../build/userThreadExit
Check_Result


#----------------------------------------------------------------------------
printf "${NC}>> earlyHalt test ${RED}\n"
${EXECUTABLE_RAND} ../../build/earlyHalt >./io/earlyHaltOut < ./io/earlyHaltIn
output=$(head -n -8 ./io/earlyHaltOut)
expected="hellohellohellohelloMachine halting!"
diff  <(echo "$output" ) <(echo "$expected")

if [ "$output" == "hellohellohellohelloMachine halting!" ]
then
    printf "${GREEN}OK\n\n"
else
    printf "output and expected strings do not match\n"
    printf "${RED}KO\n\n"
    nbFails=$(($nbFails + 1))
fi


printf "${NC}\n\n### STEP 4 ###\n"

printf "${NC}\n\n### STEP 5 ###\n"

printf "${NC}\n\n### STEP 6 ###\n"





#############################################
printf "${NC}\nTOTAL:\n"
if [ "$nbFails" -ne "0" ]
then
    printf "${RED}KO $nbFails test(s) failed"
else
    printf "${GREEN}OK all tests passed"
fi
printf "${NC}\n\n----End of Testing----\n\n"

