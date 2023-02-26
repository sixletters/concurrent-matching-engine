#!/bin/bash

for f in tests/*; do
	echo "$f"
	./grader ./engine < "$f"
done
