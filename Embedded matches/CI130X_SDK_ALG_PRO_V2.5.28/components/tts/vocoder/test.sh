#!/bin/bash

for ((i=1;i<151;i++))
do
        echo "process test_$i.cmp"
	./world.exe ./cmp9/test_$i.cmp
	mv test.wav test_$i.wav
done
echo "synthesis complete"
mkdir wav9
mv *.wav ./wav9/
chmod 777 wav9 -R
