g++ td=c++11 -c utils.cpp
g++ -std=c++11 -framework OpenGL -lm -w -o main.exe main.cpp utils.o && main.exe
