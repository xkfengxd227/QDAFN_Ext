#!/bin/bash
make
rm *.o

# ------------------------------------------------------------------------------
#  Parameters
# ------------------------------------------------------------------------------
qn=1000
c=2.0

# ------------------------------------------------------------------------------
#  Mnist
# ------------------------------------------------------------------------------
dname=Mnist
n=59000
d=50
B=4096
dPath=./data/${dname}/${dname}
dFolder=./data/${dname}/

proj=10
for ((i=2; i<=10; i=i+1))
do
  proj=$(($proj + 10))
  cand=$((273 - $proj))	
  for ((j=1; j<=5; j=j+1))
  do
	oFolder=./results${c}/${dname}/qdafn/heuristic/${proj}/${j}/

	./qdafn -alg 1 -n ${n} -d ${d} -B ${B} -l ${proj} -m ${cand} -c ${c} -ds ${dPath}.ds -df ${dFolder} -of ${oFolder}
	./qdafn -alg 2 -qn ${qn} -d ${d} -qs ${dPath}.q -ts ${dPath}.fn2.0 -df ${dFolder} -of ${oFolder}
  done
done

# ------------------------------------------------------------------------------
#  Sift
# ------------------------------------------------------------------------------
dname=Sift
n=999000
d=128
B=4096
dPath=./data/${dname}/${dname}
dFolder=./data/${dname}/

cand=122
for proj in 50 60 70 80 90 100
do
  for ((j=1; j<=5; j=j+1))
  do
    oFolder=./results${c}/${dname}/qdafn/heuristic/${proj}/${j}/

	./qdafn -alg 1 -n ${n} -d ${d} -B ${B} -l ${proj} -m ${cand} -c ${c} -ds ${dPath}.ds -df ${dFolder} -of ${oFolder}
	./qdafn -alg 2 -qn ${qn} -d ${d} -qs ${dPath}.q -ts ${dPath}.fn2.0 -df ${dFolder} -of ${oFolder}
  done
done

# ------------------------------------------------------------------------------
#  Gist
# ------------------------------------------------------------------------------
dname=Gist
n=999000
d=960
B=16384
dPath=./data/${dname}/${dname}
dFolder=./data/${dname}/

cand=21
for proj in 60 80 100
do
  for ((j=1; j<=10; j=j+1))
  do
	oFolder=./results${c}/${dname}/qdafn/heuristic/${proj}/${j}/

	./qdafn -alg 1 -n ${n} -d ${d} -B ${B} -l ${proj} -m ${cand} -c ${c} -ds ${dPath}.ds -df ${dFolder} -of ${oFolder}
	./qdafn -alg 2 -qn ${qn} -d ${d} -qs ${dPath}.q -ts ${dPath}.fn2.0 -df ${dFolder} -of ${oFolder}
  done
done

# ------------------------------------------------------------------------------
#  Trevi
# ------------------------------------------------------------------------------
dname=Trevi
n=99900
d=4096
B=65536
dPath=./data/${dname}/${dname}
dFolder=./data/${dname}/

proj=10
cand=27
for ((i=2; i<=10; i=i+1))
do
  proj=$(($proj + 10))
  for ((j=1; j<=5; j=j+1))
  do
	oFolder=./results${c}/${dname}/qdafn/heuristic/${proj}/${j}/

	./qdafn -alg 1 -n ${n} -d ${d} -B ${B} -l ${proj} -m ${cand} -c ${c} -ds ${dPath}.ds -df ${dFolder} -of ${oFolder}
	./qdafn -alg 2 -qn ${qn} -d ${d} -qs ${dPath}.q -ts ${dPath}.fn2.0 -df ${dFolder} -of ${oFolder}
  done
done

# ------------------------------------------------------------------------------
#  P53
# ------------------------------------------------------------------------------
dname=P53
n=30159
d=5408
B=65536
dPath=./data/${dname}/${dname}
dFolder=./data/${dname}/

proj=10
cand=21
for ((i=2; i<=3; i=i+1))
do
  proj=$(($proj + 10))
  for ((j=1; j<=5; j=j+1))
  do
	oFolder=./results${c}/${dname}/qdafn/heuristic/${proj}/${j}/

	./qdafn -alg 1 -n ${n} -d ${d} -B ${B} -l ${proj} -m ${cand} -c ${c} -ds ${dPath}.ds -df ${dFolder} -of ${oFolder}
	./qdafn -alg 2 -qn ${qn} -d ${d} -qs ${dPath}.q -ts ${dPath}.fn2.0 -df ${dFolder} -of ${oFolder}
  done
done