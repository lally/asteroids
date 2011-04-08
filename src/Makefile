CXXFLAGS=-I../header -I. -I/usr/include/SDL -g -std=gnu++0x -DBOOST_SP_USE_PTHREADS
CFLAGS=-I../header -g

asteroids: active.o ai.o common.o elementManager.o game.o graphics.o input.o item.o main.o passive.o physics.o shell.o ship.o text.o vec2d.o util.o asteroids.o flags.o
	g++ -g -o $@ $^ -lSDL -lSDL_net -lGL

