galaga: galaga.c list.c save.c
	gcc -o galaga galaga.c list.c save.c `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0`

clean:
	@rm galaga