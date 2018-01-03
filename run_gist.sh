#!/bin/bash
make
rm *.o

dPath=./data/Gist/Gist
oPath=./results/heuristic/Gist

./qdafn -alg 0 -n 999000 -qn 1000 -d 960 -ds ${dPath}.ds -qs ${dPath}.q -ts ${dPath}ID.fn2.0

for proj in 60 80 100
do
	for ((j=1; j<=10; j=j+1))
	do
		./qdafn -alg 1 -n 999000 -d 960 -B 16384 -l ${proj} -m 21 -c 2.0 -ds ${dPath}.ds -df ${dPath}/ -of ${oPath}/${proj}/${j}/		
		./qdafn -alg 2 -qn 1000 -d 960 -qs ${dPath}.q -ts ${dPath}ID.fn2.0 -df ${dPath}/ -of ${oPath}/${proj}/${j}/
	done
done
