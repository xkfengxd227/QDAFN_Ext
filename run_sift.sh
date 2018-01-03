#!/bin/bash
make
rm *.o

dPath=./data/Sift/Sift
oPath=./results/heuristic/Sift

./qdafn -alg 0 -n 999000 -qn 1000 -d 128 -ds ${dPath}.ds -qs ${dPath}.q -ts ${dPath}ID.fn2.0

cand=122
for proj in 50 60 70 80 90 100
do
	for ((j=1; j<=5; j=j+1))
	do
		./qdafn -alg 1 -n 999000 -d 128 -B 4096 -l ${proj} -m ${cand} -c 2.0 -ds ${dPath}.ds -df ${dPath}/ -of ${oPath}/${proj}/${j}/
		./qdafn -alg 2 -qn 1000 -d 128 -qs ${dPath}.q -ts ${dPath}ID.fn2.0 -df ${dPath}/ -of ${oPath}/${proj}/${j}/
	done
done
