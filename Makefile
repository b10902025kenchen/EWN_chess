agent: src/main.cpp src/MyAI.cpp
	g++ -o agent src/main.cpp src/MyAI.cpp

clean:
	rm -f agent
