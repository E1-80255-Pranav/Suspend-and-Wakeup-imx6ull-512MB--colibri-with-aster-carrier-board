#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "cjson/cJSON.h"

char buffer[100];

int fileread(char *filename, char *mode)

{

    FILE *fptr;
    fptr = fopen(filename, mode);
    if (fptr == NULL)
    {
        printf("Error opening file");
        exit(1);
    }

    fread(buffer, 500, 1, fptr);
    fclose(fptr);

}

int main()
{
    int ADC_value;
    FILE *f_standby;
    FILE *f_wakeup;
    FILE *f_ADC;

    fileread("/etc/gateway/dynamic.json","r");

    // ******parsing into cJSON file *******//

    cJSON *string_json = cJSON_Parse(buffer);

    // parsing the seperate data into cJSON format

    cJSON *offtime = cJSON_GetObjectItem(string_json, "Suspend_mode");
    cJSON *adcval = cJSON_GetObjectItem(string_json, "Threshold_value");
    cJSON *ontime = cJSON_GetObjectItem(string_json, "Wakeup_Time");

    // storing that value into integer

    char deviceoff[10];
    strcpy(deviceoff,offtime->valuestring);
    int thresholdval = adcval->valueint;
    char deviceon[5];
    strcpy(deviceon, ontime->valuestring);

    // pars the time value string
    int current_min;
    int current_hour;
    int wkup;
    int threshold;

    char min1[3];
    char hr1[3];

    char delim[] = ":";
    char *ptr1 = strtok(deviceoff, delim);
    strcpy(hr1, ptr1);
    ptr1 = ptr1 +3;
    ptr1 = strtok(ptr1, delim);
    strcpy(min1, ptr1);
 
    sscanf(ontime->valuestring, "+%3d", &wkup);

    int set_hour = atoi(hr1);
    int set_min = atoi(min1);

    // current time structure 

    time_t now = time(NULL);
    struct tm *tm_struct = localtime(&now);
    current_min = tm_struct->tm_min;
    current_hour = tm_struct->tm_hour;

    //printing the current hour time and hour
    
    printf("%d,%d\n", current_hour, current_min);

    while (1)
    
    {
        printf("hour:%d" " minute:%d\n",set_hour,set_min);  
        printf("device on\n");
        sleep(1);

        now = time(NULL);
        tm_struct = localtime(&now);
        current_min = tm_struct->tm_min;
        current_hour = tm_struct->tm_hour;
        printf("%d:%d\n", current_hour, current_min);

        if (set_hour<= current_hour && set_min<= current_min)
        {
            
            f_ADC = fopen("/dev/colibri-ain0", "r");
            fscanf(f_ADC,"%d",&ADC_value);
            printf("Sensing value=%d\n", ADC_value);
            fclose(f_ADC);

            if (ADC_value < thresholdval)
            
            {
                sleep(6);

                f_ADC = fopen("/dev/colibri-ain0", "r");
                fscanf(f_ADC,"%d",&ADC_value);
                printf("Digital value=%d\n", ADC_value);
                fclose(f_ADC);

                if (ADC_value<thresholdval)

                {
                    f_wakeup = fopen("/sys/class/rtc/rtc0/wakealarm","w");                
                    fprintf(f_wakeup,"+%d\n",wkup);
                    fclose(f_wakeup);

                    f_standby = fopen("/sys/power/state", "w");
                    printf("Going into suspend mode\n");
                    fprintf(f_standby, "standby");
                    fclose(f_standby);
    
                }
            }
        }
    }
    
    return 0;
}
