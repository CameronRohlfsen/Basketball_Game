basketball: main.c game.c
	gcc main.c game.c gfx.o -lX11 -lm -o basketball

clean:
	rm -f basketball
