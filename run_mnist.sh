#!/bin/bash
make
rm *.o

dPath=./data/Mnist/Mnist
oPath=./results/heuristic/Mnist

./qdafn -alg 0 -n 59000 -qn 1000 -d 50 -ds ${dPath}.ds -qs ${dPath}.q -ts ${dPath}ID.fn2.0

proj=10
for ((i=2; i<=10; i=i+1))
do
	proj=$(($proj + 10))
	cand=$((273 - $proj))

	for ((j=1; j<=5; j=j+1))
	do
		./qdafn -alg 1 -n 59000 -d 50 -B 4096 -l ${proj} -m ${cand} -c 2.0 -ds ${dPath}.ds -df ${dPath}/ -of ${oPath}/${proj}/${j}/
		./qdafn -alg 2 -qn 1000 -d 50 -qs ${dPath}.q -ts ${dPath}ID.fn2.0 -df ${dPath}/ -of ${oPath}/${proj}/${j}/
	done
done