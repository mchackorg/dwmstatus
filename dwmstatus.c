#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <stdio.h>

int main(void) {
    int bat;
    int batstate;
    int err;
    size_t len;
    int temp;
    char *batstring;

    len = 4;
    err = sysctlbyname("hw.acpi.battery.life", &bat, &len, NULL, 0);
    if (err != 0) {
        perror("sysctl");
    }

    err = sysctlbyname("hw.acpi.battery.state", &batstate, &len, NULL, 0);
    if (err != 0) {
        perror("sysctl");
    }

    switch (batstate)
    {
    case 0:
        break;
    case 1:
        batstring = "B";
        break;
    case 2:
        batstring = "AC";
        break;
    default:
        batstring = "U";
        break;
    }

    err = sysctlbyname("dev.cpu.0.temperature", &temp, &len, NULL, 0);
    if (err != 0) {
        perror("sysctl");
    }

    temp = (temp - 2732) / 10;

    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    char timestr[17];
    strftime((char *)timestr, 17, "%F %R", timeinfo);
    printf("%s%d%% | %dC | %s\n", batstring, bat, temp, timestr);
}
