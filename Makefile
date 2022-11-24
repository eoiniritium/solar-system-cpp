default:
	g++ main.cpp -o main.exe -O2 --static -static-libgcc -static-libstdc++ -Wall -Wno-missing-braces -I include/raylib/include -L include/raylib/lib/ -lraylib -lopengl32 -lgdi32 -lwinmm -I include -L include
	./main.exe