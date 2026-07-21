all:
	gcc main.c cpu.c util.c terminal.c ram.c disk.c main_loop.c alerts.c -o ztop -lm
