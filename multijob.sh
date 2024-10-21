#!/bin/bash

for i
do
	while read c
	do     
		echo [START]
		$c
		echo [END]
	done < $i
done
exit 0
	