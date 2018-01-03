#!/bin/bash
make
rm *.o

dPath=./data/P53/P53
oPath=./results/heuristic/P53

./qdafn -alg 0 -n 30159 -qn 1000 -d 5408 -ds ${dPath}.ds -qs ${dPath}.q -ts ${dPath}ID.fn2.0

proj=10
for ((i=2; i<=5; i=i+1))
do
	proj=$(($proj + 10))

	for ((j=1; j<=5; j=j+1))
	do
		./qdafn -alg 1 -n 30159 -d 5408 -B 65536 -l ${proj} -m 21 -c 2.0 -ds ${dPath}.ds -df ${dPath}/ -of ${oPath}/${proj}/${j}/
		./qdafn -alg 2 -qn 1000 -d 5408 -qs ${dPath}.q -ts ${dPath}ID.fn2.0 -df ${dPath}/ -of ${oPath}/${proj}/${j}/
	done
done


