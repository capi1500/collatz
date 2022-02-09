all:
	g++ -pthread -std=c++17 -o main main.cpp teams.cpp collatz.cpp
