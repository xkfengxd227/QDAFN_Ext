SRCS=util.cc pri_queue.cc block_file.cc b_node.cc b_tree.cc \
      qdafn.cc afn.cc main.cc
OBJS=${SRCS:.cc=.o}

CXX=g++ -std=c++11
CPPFLAGS=-w -O3

.PHONY: clean

all: ${OBJS}
	${CXX} ${CPPFLAGS} -o qdafn ${OBJS}

util.o: util.h

pri_queue.o: pri_queue.h

block_file.o: block_file.h

b_node.o: b_node.h

b_tree.o: b_tree.h

qdafn.o: qdafn.h

afn.o: afn.h

main.o:

clean:
	-rm ${OBJS} qdafn
