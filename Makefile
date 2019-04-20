CFLAGS+=-g -std=c99 -pedantic -Wall -Wextra -I/usr/local/include
LDFLAGS+=-L/usr/local/lib -lxcb -lxcb-util

dwmstatus: dwmstatus.c
	$(CC) $(CFLAGS) dwmstatus.c $(LDFLAGS) -o $@
