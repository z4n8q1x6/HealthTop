all:
	gcc main.c cpu.c util.c terminal.c ram.c disk.c main_loop.c alerts.c config.c log.c health.c json.c modes.c -o ztop -lm
