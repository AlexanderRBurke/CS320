all: simulator

run: t1 t2 t3

t1: cache-sim simulator
	./cache-sim trace1.txt my_trace1_output.txt

t2: cache-sim simulator
	./cache-sim trace2.txt my_trace2_output.txt

t3: cache-sim simulator
	./cache-sim trace3.txt my_trace3_output.txt

gdb: cache-sim simulator
	sudo gdb ./cache-sim

simulator: main.cpp caches.cpp caches.h
	g++ main.cpp caches.cpp -o cache-sim -std=c++11 -g

clean:
	rm -f cache-sim
