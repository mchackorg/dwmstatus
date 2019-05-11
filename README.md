# dwmstatus

Simple program that can update the dwm window manager status line.

It prints battery status and charge, a CPU temperature, current main
volume and prints a timestamp, then sleeps for 10 seconds and repeats.

If you change your volume with, for instance, keys captured by sxhkd
you can send dwmstatus a SIGHUP to make it update at once instead of
sleeping, like this in sxhdrc:

    XF86AudioLowerVolume
        mixer vol -10; pkill -HUP dwmstatus
