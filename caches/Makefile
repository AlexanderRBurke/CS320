all: cache-sim

run: t1 t2 t3

t1: cache-sim
	./cache-sim trace1.txt my_trace1_output.txt

t2: cache-sim
	./cache-sim trace2.txt my_trace2_output.txt

t3: cache-sim
	./cache-sim trace3.txt my_trace3_output.txt

gdb: cache-sim
	sudo gdb ./cache-sim

cache-sim: main.cpp caches.cpp caches.h
	g++ main.cpp caches.cpp -o cache-sim -std=c++11 -g

clean:
	rm -f cache-sim
