#!/bin/bash
make
rm *.o

# ------------------------------------------------------------------------------
#  Parameters
# ------------------------------------------------------------------------------
qn=1000
l=0
m=0
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
oFolder=./results${c}/${dname}/qdafn/guarantee/

./qdafn -alg 0 -n ${n} -qn ${qn} -d ${d} -ds ${dPath}.ds -qs ${dPath}.q -ts ${dPath}.fn2.0
./qdafn -alg 1 -n ${n} -d ${d} -B ${B} -l ${l} -m ${m} -c ${c} -ds ${dPath}.ds -df ${dFolder} -of ${oFolder}
./qdafn -alg 2 -qn ${qn} -d ${d} -qs ${dPath}.q -ts ${dPath}.fn2.0 -df ${dFolder} -of ${oFolder}

# ------------------------------------------------------------------------------
#  Sift
# ------------------------------------------------------------------------------
dname=Sift
n=999000
d=128
B=4096
dPath=./data/${dname}/${dname}
dFolder=./data/${dname}/
oFolder=./results${c}/${dname}/qdafn/guarantee/

./qdafn -alg 0 -n ${n} -qn ${qn} -d ${d} -ds ${dPath}.ds -qs ${dPath}.q -ts ${dPath}.fn2.0
./qdafn -alg 1 -n ${n} -d ${d} -B ${B} -l ${l} -m ${m} -c ${c} -ds ${dPath}.ds -df ${dFolder} -of ${oFolder}
./qdafn -alg 2 -qn ${qn} -d ${d} -qs ${dPath}.q -ts ${dPath}.fn2.0 -df ${dFolder} -of ${oFolder}

# ------------------------------------------------------------------------------
#  Gist
# ------------------------------------------------------------------------------
dname=Gist
n=999000
d=960
B=16384
dPath=./data/${dname}/${dname}
dFolder=./data/${dname}/
oFolder=./results${c}/${dname}/qdafn/guarantee/

./qdafn -alg 0 -n ${n} -qn ${qn} -d ${d} -ds ${dPath}.ds -qs ${dPath}.q -ts ${dPath}.fn2.0
./qdafn -alg 1 -n ${n} -d ${d} -B ${B} -l ${l} -m ${m} -c ${c} -ds ${dPath}.ds -df ${dFolder} -of ${oFolder}
./qdafn -alg 2 -qn ${qn} -d ${d} -qs ${dPath}.q -ts ${dPath}.fn2.0 -df ${dFolder} -of ${oFolder}

# ------------------------------------------------------------------------------
#  Trevi
# ------------------------------------------------------------------------------
dname=Trevi
n=99900
d=4096
B=65536
dPath=./data/${dname}/${dname}
dFolder=./data/${dname}/
oFolder=./results${c}/${dname}/qdafn/guarantee/

./qdafn -alg 0 -n ${n} -qn ${qn} -d ${d} -ds ${dPath}.ds -qs ${dPath}.q -ts ${dPath}.fn2.0
./qdafn -alg 1 -n ${n} -d ${d} -B ${B} -l ${l} -m ${m} -c ${c} -ds ${dPath}.ds -df ${dFolder} -of ${oFolder}
./qdafn -alg 2 -qn ${qn} -d ${d} -qs ${dPath}.q -ts ${dPath}.fn2.0 -df ${dFolder} -of ${oFolder}

# ------------------------------------------------------------------------------
#  P53
# ------------------------------------------------------------------------------
dname=P53
n=30159
d=5408
B=65536
dPath=./data/${dname}/${dname}
dFolder=./data/${dname}/
oFolder=./results${c}/${dname}/qdafn/guarantee/

./qdafn -alg 0 -n ${n} -qn ${qn} -d ${d} -ds ${dPath}.ds -qs ${dPath}.q -ts ${dPath}.fn2.0
./qdafn -alg 1 -n ${n} -d ${d} -B ${B} -l ${l} -m ${m} -c ${c} -ds ${dPath}.ds -df ${dFolder} -of ${oFolder}
./qdafn -alg 2 -qn ${qn} -d ${d} -qs ${dPath}.q -ts ${dPath}.fn2.0 -df ${dFolder} -of ${oFolder}
