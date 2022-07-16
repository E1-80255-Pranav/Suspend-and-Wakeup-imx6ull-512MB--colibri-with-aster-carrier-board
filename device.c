#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

int main()

{
    int ADC_value;
    FILE *f_standby;
    FILE *f_wakeup;
    FILE *f_ADC;

    time_t now = time(NULL);
    struct tm *tm_struct = localtime(&now);
    int min;

    int night_time = 42;

    {
        while (1)
        
        {
            printf("device on\n");
            sleep(2);
            now = time(NULL);
            tm_struct = localtime(&now);

            min = tm_struct->tm_min;
            printf("%d,%d", min, night_time);

            if (min == night_time)

            {
                f_ADC = fopen("/dev/colibri-ain0", "r");
                fscanf(f_ADC, "%d", &ADC_value);
                printf("Digital value=%d\n", ADC_value);
                fclose(f_ADC);
                
                if (sleep(5),ADC_value < 50)
                {
                    f_wakeup = fopen("/sys/class/rtc/rtc0/wakealarm", "w");
                    fprintf(f_wakeup, "+30");
                    printf("Device is On\n");
                    fclose(f_wakeup);

                    f_standby = fopen("/sys/power/state", "w");
                    printf("Going into suspend mode\n");
                    fprintf(f_standby, "standby");
                    fclose(f_standby);

                    sleep(2);
                }
            }
        }

        return 0;
    
    }
}