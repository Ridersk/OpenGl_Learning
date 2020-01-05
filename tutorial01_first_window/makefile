all:
	g++ main.cpp -o main -L/usr/local/lib -lGL -lGLEW -lm -lglfw

run:
	./main

brun:
	g++ -I/common main.cpp ../common/shader.cpp -o main -L/usr/local/lib -lGL -lGLEW -lm -lglfw
	./main
