main: main.o Compiler.o
	g++ -o main main.o Compiler.o

main.o: main.cpp
	g++ -c -o main.o main.cpp

Compiler.o: Compiler.cpp Compiler.hpp
	g++ -c -o Compiler.o Compiler.cpp

clean:
	rm main *.o
