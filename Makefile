all:
	gcc main.c cpu.c util.c terminal.c ram.c disk.c main_loop.c alerts.c conf.c log.c -o ztop -lm
