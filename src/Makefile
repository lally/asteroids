CXXFLAGS=-I../header -I. -I/usr/include/SDL -g -std=gnu++0x

asteroids: active.o ai.o common.o elementManager.o game.o graphics.o input.o item.o main.o passive.o physics.o shell.o ship.o text.o vec2d.o util.o
	g++ -g -o $@ $^ -lSDL -lSDL_net -lGL

