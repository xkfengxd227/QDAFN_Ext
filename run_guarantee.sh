#!/bin/bash
make
rm *.o

dPath=./data
oPath=./results


###### Mnist ######
./qdafn -alg 0 -n 59000 -qn 1000 -d 50 -ds ${dPath}/Mnist/Mnist.ds -qs ${dPath}/Mnist/Mnist.q -ts ${dPath}/Mnist/MnistID.fn2.0
./qdafn -alg 1 -n 59000 -d 50 -B 4096 -l 0 -m 0 -c 2.0 -ds ${dPath}/Mnist/Mnist.ds -df ${dPath}/Mnist/ -of ${oPath}/guarantee/Mnist/
./qdafn -alg 2 -qn 1000 -d 50 -qs ${dPath}/Mnist/Mnist.q -ts ${dPath}/Mnist/MnistID.fn2.0 -df ${dPath}/Mnist/ -of ${oPath}/guarantee/Mnist/


# ###### Sift ######
# ./qdafn -alg 0 -n 999000 -qn 1000 -d 128 -ds ${dPath}/Sift/Sift.ds -qs ${dPath}/Sift/Sift.q -ts ${dPath}/Sift/SiftID.fn2.0
# ./qdafn -alg 1 -n 999000 -d 128 -B 4096 -l 0 -m 0 -c 2.0 -ds ${dPath}/Sift/Sift.ds -df ${dPath}/Sift/ -of ${oPath}/guarantee/Sift/
# ./qdafn -alg 2 -qn 1000 -d 128 -qs ${dPath}/Sift/Sift.q -ts ${dPath}/Sift/SiftID.fn2.0 -df ${dPath}/Sift/ -of ${oPath}/guarantee/Sift/


# ###### Gist ######
# ./qdafn -alg 0 -n 999000 -qn 1000 -d 960 -ds ${dPath}/Gist/Gist.ds -qs ${dPath}/Gist/Gist.q -ts ${dPath}/Gist/GistID.fn2.0
# ./qdafn -alg 1 -n 999000 -d 960 -B 16384 -l 0 -m 0 -c 2.0 -ds ${dPath}/Gist/Gist.ds -df ${dPath}/Gist/ -of ${oPath}/guarantee/Gist/
# ./qdafn -alg 2 -qn 1000 -d 960 -qs ${dPath}/Gist/Gist.q -ts ${dPath}/Gist/GistID.fn2.0 -df ${dPath}/Gist/ -of ${oPath}/guarantee/Gist/


# ###### Trevi ######
# ./qdafn -alg 0 -n 99900 -qn 1000 -d 4096 -ds ${dPath}/Trevi/Trevi.ds -qs ${dPath}/Trevi/Trevi.q -ts ${dPath}/Trevi/TreviID.fn2.0
# ./qdafn -alg 1 -n 99900 -d 4096 -B 65536 -l 0 -m 0 -c 2.0 -ds ${dPath}/Trevi/Trevi.ds -df ${dPath}/Trevi/ -of ${oPath}/guarantee/Trevi/
# ./qdafn -alg 2 -qn 1000 -d 4096 -qs ${dPath}/Trevi/Trevi.q -ts ${dPath}/Trevi/TreviID.fn2.0 -df ${dPath}/Trevi/ -of ${oPath}/guarantee/Trevi/


# ###### P53 ######
# ./qdafn -alg 0 -n 30159 -qn 1000 -d 5408 -ds ${dPath}/P53/P53.ds -qs ${dPath}/P53/P53.q -ts ${dPath}/P53/P53ID.fn2.0
# ./qdafn -alg 1 -n 30159 -d 5408 -B 65536 -l 0 -m 0 -c 2.0 -ds ${dPath}/P53/P53.ds -df ${dPath}/P53/ -of ${oPath}/guarantee/P53/
# ./qdafn -alg 2 -qn 1000 -d 5408 -qs ${dPath}/P53/P53.q -ts ${dPath}/P53/P53ID.fn2.0 -df ${dPath}/P53/ -of ${oPath}/guarantee/P53/

