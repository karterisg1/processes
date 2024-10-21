#!/bin/bash

while read word _; # reads first word of each line 
do 
	jobCommander stop  $word
done < <(jobCommander poll running)

while read word _; # reads first word of each line 
do 
	jobCommander stop  $word
done < <(jobCommander poll queued)
