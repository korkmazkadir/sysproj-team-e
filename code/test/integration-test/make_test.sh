#!/bin/bash


#Colors

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color
EXECUTABLE="../../build/nachos-step6 -x"
EXECUTABLE_RAND="../../build/nachos-step6 -rs -x"
EXECUTABLE_RAND_SEED="../../build/nachos-step6 -rs 18 -x"

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
printf "${NC}>> Semaphore test (takes time to execute) ${RED}\n"
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

printf "${NC}\n\n### STEP 4 ###\n"

#----------------------------------------------------------------------------
printf "${NC}>> Two sub-processes with userthreads - no joins${RED}\n"
${EXECUTABLE_RAND_SEED} ../../build/twoProcessesNoJoins >./io/twoProcessesNoJoinsOut
output=$(head -n -8 ./io/twoProcessesNoJoinsOut)
expected="abcdMachine halting!"
diff  <(echo "$output" ) <(echo "$expected")

if [ "$output" == "abcdMachine halting!" ]
then
    printf "${GREEN}OK\n\n"
else
    printf "output and expected strings do not match\n"
    printf "${RED}KO\n\n"
    nbFails=$(($nbFails + 1))
fi

#----------------------------------------------------------------------------
printf "${NC}>> Two sub-processes with userthreads - first joins${RED}\n"
${EXECUTABLE_RAND_SEED} ../../build/twoProcessesFirstOneJoins >./io/twoProcessesFirstOneJoinsOut
output=$(head -n -8 ./io/twoProcessesFirstOneJoinsOut)
expected="abcdMachine halting!"
diff  <(echo "$output" ) <(echo "$expected")

if [ "$output" == "abcdMachine halting!" ]
then
    printf "${GREEN}OK\n\n"
else
    printf "output and expected strings do not match\n"
    printf "${RED}KO\n\n"
    nbFails=$(($nbFails + 1))
fi

#----------------------------------------------------------------------------
printf "${NC}>> Two sub-processes with userthreads - second joins${RED}\n"
${EXECUTABLE_RAND_SEED} ../../build/twoProcessesSecondOneJoins >./io/twoProcessesSecondOneJoinsOut
output=$(head -n -8 ./io/twoProcessesSecondOneJoinsOut)
expected="abcdMachine halting!"
diff  <(echo "$output" ) <(echo "$expected")

if [ "$output" == "abcdMachine halting!" ]
then
    printf "${GREEN}OK\n\n"
else
    printf "output and expected strings do not match\n"
    printf "${RED}KO\n\n"
    nbFails=$(($nbFails + 1))
fi



#----------------------------------------------------------------------------
printf "${NC}\n\n### STEP 6 ###\n"
python execute.py
Check_Result


printf "${NC}\n\n### STEP 5 ###\n"



#############################################
printf "${NC}\nTOTAL:\n"
if [ "$nbFails" -ne "0" ]
then
    printf "${RED}KO $nbFails test(s) failed"
else
    printf "${GREEN}OK all tests passed"
fi
printf "${NC}\n\n----End of Testing----\n\n"

