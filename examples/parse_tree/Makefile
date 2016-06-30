all: a.out

run: a.out
	cat test1.txt | ./a.out | dot -Tpng > test1.png
	cat test2.txt | ./a.out | dot -Tpng > test2.png

a.out: *.c *.h *.cc
	gcc -Wall -c parser.c
	g++ -Wall --std=c++11 parse_tree.cc cobjectgraph.cc parser.o

clean:
	rm -f *.o a.out test1.png test2.png
