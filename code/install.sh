#!/bin/sh

timeout 1 ./nachos-step6 -f

timeout 1 ./nachos-step6 -cp pico pico

timeout 1 ./nachos-step6 -cp display display

timeout 1 ./nachos-step6 -cp simple_shell shell

#timeout 1 ./nachos-step6 -cp triangulars triangulars

#timeout 1 ./nachos-step6 -cp fsys_rw fsys_rw

#timeout 1 ./nachos-step6 -cp network_receiver net_rec

#timeout 1 ./nachos-step6 -cp network_sender net_sen

timeout 1 ./nachos-step6 -cp demo demo

timeout 1 ./nachos-step6 -cp demorcv demorcv

timeout 1 ./nachos-step6 -D > disk_content.txt


#./nachos-step5 -x ls
