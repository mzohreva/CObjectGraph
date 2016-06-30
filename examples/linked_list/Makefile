all: a.out

a.out: *.h *.cc
	g++ -Wall --std=c++11 cobjectgraph.cc list_graph.cc

run: a.out
	./a.out | dot -Tpng > OutputGraph.png

clean:
	rm -f *.o a.out OutputGraph.png
