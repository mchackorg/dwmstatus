/*
 * dwmstatus - a small program to update dwm window manager status line.
 *
 * Copyright (c) 2019 Michael Cardell Widerkrantz, mc at the domain hack.org.
 *
 * https://hack.org/mc/
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/soundcard.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <stdio.h>
#include <xcb/xcb.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xcb_aux.h>
#include <string.h>

int sigcode;

void sigcatch(int sig)
{
    sigcode = sig;
}

int main(void) {
    int bat;
    int batstate;
    int err;
    size_t len;
    int temp;
    char *batstring;
    xcb_connection_t *conn;
    xcb_screen_t *screen;
    char status[80];
    int scrno;
    char defaultmixer[] = "/dev/mixer";
    int mixfd, vol = 0;

    conn = xcb_connect(NULL, &scrno);
    if (!conn)
    {
        fprintf(stderr, "can't connect to an X server\n");
        exit(1);
    }

    screen = xcb_aux_get_screen(conn, scrno);

    if ((mixfd = open(defaultmixer, O_RDWR)) < 0) {
        perror("open mixer");
        exit(1);
    }

    if (SIG_ERR == signal(SIGHUP, sigcatch))
    {
        perror("signal");
        exit(1);
    }

    len = 4;
    while (1)
    {
        if (ioctl(mixfd, SOUND_MIXER_READ_VOLUME, &vol) == -1) {
            printf("OSS: Cannot read mixer information\n");
        }

        err = sysctlbyname("hw.acpi.battery.life", &bat, &len, NULL, 0);
        if (err != 0)
        {
            perror("sysctl");
        }

        err = sysctlbyname("hw.acpi.battery.state", &batstate, &len, NULL, 0);
        if (err != 0)
        {
            perror("sysctl");
        }

        switch (batstate)
        {
        case 0:
            batstring = "=";
            break;
        case 1:
            batstring = "-";
            break;
        case 2:
            batstring = "+";
            break;
        default:
            batstring = "U";
            break;
        }

        err = sysctlbyname("dev.cpu.0.temperature", &temp, &len, NULL, 0);
        if (err != 0) {
            perror("sysctl");
        }

        // Temperature is measured in deciKelvins, so convert it to
        // degrees Celsius.
        temp = (temp - 2732) / 10;

        time_t rawtime;
        struct tm *timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        char timestr[17];
        strftime((char *)timestr, 17, "%F %R", timeinfo);
        snprintf(status, 80, "Bat %s%d%% | %d°C | Vol %d%% | %s", batstring, bat, temp, vol & 0x7f, timestr);

        xcb_change_property(conn, XCB_PROP_MODE_REPLACE, screen->root, XCB_ATOM_WM_NAME,
                            XCB_ATOM_STRING, 8, strlen(status), status);

        xcb_flush(conn);

        // Interrupted if we're sent a SIGHUP
        sleep(10);
    }
}
