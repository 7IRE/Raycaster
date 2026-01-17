all: compile link

compile:
	g++ -c game.cpp player.cpp -I"C:\Users\lm341\Documents\Repository\Libraries\SFML-3.0.2\include" -DSFML_STATIC

link:
	g++ game.o player.o -o Raycaster -L"C:\Users\lm341\Documents\Repository\Libraries\SFML-3.0.2\lib" -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lsfml-window-s -lsfml-system-s -lopengl32 -lfreetype -lwinmm -lgdi32 -mwindows -lsfml-main

clean:
	rm -f game.o Raycaster player.o