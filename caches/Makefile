all: simulator

run: cache-sim simulator
	./cache-sim trace3.txt DEBUG.txt

gdb: cache-sim simulator
	sudo gdb ./cache-sim

simulator: main.cpp caches.cpp caches.h
	g++ main.cpp caches.cpp -o cache-sim -std=c++11 -g

clean:
	rm -f cache-sim
