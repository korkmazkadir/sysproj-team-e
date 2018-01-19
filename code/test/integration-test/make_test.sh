#!/bin/bash


#Colors

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color
EXECUTABLE="../../build/nachos-step6 -f -cp"
EXECUTABLE_RAND="../../build/nachos-step6 -rs -f -cp"
#EXECUTABLE_RAND_SEED="../../build/nachos-step6 -rs 18 -x"

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


printf "${NC}\n\n### STEP 3 ###\n"

#----------------------------------------------------------------------------
printf "${RED}"
printf "${NC}>> Semaphore test (takes time to execute) ${RED}\n"
${EXECUTABLE} ../../build/testsemaphores prog -x prog -rs 15
Check_Result


#----------------------------------------------------------------------------
printf "${NC}>> manyThreads1 (max nb threads) test ${RED}\n"
${EXECUTABLE} ../../build/manyThreads1 prog -rs -x prog
Check_Result


#----------------------------------------------------------------------------
printf "${NC}>> manyThreads2 (memory limitations) test ${RED}\n"
${EXECUTABLE} ../../build/manyThreads2 prog -rs -x prog
Check_Result


#----------------------------------------------------------------------------
printf "${NC}>> nullFuncP test ${RED}\n"
${EXECUTABLE} ../../build/nullFuncP prog -rs -x prog
Check_Result


#----------------------------------------------------------------------------
printf "${NC}>> userThreadExit test ${RED}\n"
${EXECUTABLE} ../../build/userThreadExit prog -rs -x prog
Check_Result

printf "${NC}\n\n### STEP 4 ###\n"

#----------------------------------------------------------------------------
printf "${NC}>> Two sub-processes with userthreads - no joins${RED}\n"
${EXECUTABLE} ../../build/twoProcessesNoJoins prog -cp ../../build/userprog0 up0 -cp ../../build/userprog1 up1 -rs 2 -x prog >./io/twoProcessesNoJoinsOut
output=$(head -n -8 ./io/twoProcessesNoJoinsOut)
expected="abcdMachine halting!"
#diff  <(echo "$output" ) <(echo "$expected")

outSize=${#output} 
expSize=${#expected}
if [ $outSize == $expSize ]
then
    printf "${GREEN}OK\n\n"
else
    printf "output and expected strings do not match\n"
    printf "${RED}KO\n\n"
    nbFails=$(($nbFails + 1))
fi


printf "${NC}>> Two sub-processes with userthreads - first joins${RED}\n"
${EXECUTABLE} ../../build/twoProcessesFirstOneJoins tpf -cp ../../build/userprog0join up0j -cp ../../build/userprog1 up1 -rs 1 -x tpf >./io/twoProcessesFirstOneJoinsOut
output=$(head -n -8 ./io/twoProcessesFirstOneJoinsOut)
expected="abcdMachine halting!"
#diff  <(echo "$output" ) <(echo "$expected")

outSize=${#output} 
expSize=${#expected}
if [ $outSize == $expSize ]
then
    printf "${GREEN}OK\n\n"
else
    printf "output and expected strings do not match\n"
    printf "${RED}KO\n\n"
    nbFails=$(($nbFails + 1))
fi

#----------------------------------------------------------------------------
printf "${NC}>> Two sub-processes with userthreads - second joins${RED}\n"
${EXECUTABLE} ../../build/twoProcessesSecondOneJoins prog  -cp ../../build/userprog0 up0 -cp ../../build/userprog1join up1j -rs 1 -x prog >./io/twoProcessesSecondOneJoinsOut
output=$(head -n -8 ./io/twoProcessesSecondOneJoinsOut)
expected="abcdMachine halting!"
#diff  <(echo "$output" ) <(echo "$expected")

outSize=${#output} 
expSize=${#expected}
if [ $outSize == $expSize ]
then
    printf "${GREEN}OK\n\n"
else
    printf "output and expected strings do not match\n"
    printf "${RED}KO\n\n"
    nbFails=$(($nbFails + 1))
fi

printf "${NC}\n\n### STEP 5 ###\n"
printf "${NC}>> File System test is working${RED}\n"
${EXECUTABLE} ../../build/filesys prog -x prog -rs 15
Check_Result


printf "${NC}\n\n### STEP 6 ###\n"

#----------------------------------------------------------------------------
printf "${NC}\n\n### STEP 6 ###\n"
python execute.py
Check_Result



#############################################
printf "${NC}\nTOTAL:\n"
if [ "$nbFails" -ne "0" ]
then
    printf "${RED}KO $nbFails test(s) failed"
else
    printf "${GREEN}OK all tests passed"
fi
printf "${NC}\n\n----End of Testing----\n\n"

