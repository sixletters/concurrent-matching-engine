#!/bin/bash

for f in tests/*; do
	echo TEST "$f"
	./grader ./engine < "$f"
	printf "\n"
done
