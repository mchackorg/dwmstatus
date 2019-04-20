CFLAGS+=-g -std=c99 -Wall -Wextra -I/usr/local/include #-DDEBUG #-DDMALLOC
LDFLAGS+=-L/usr/local/lib -lxcb -lxcb-util

dwmstatus: dwmstatus.c
	$(CC) $(CFLAGS) dwmstatus.c $(LDFLAGS) -o $@
