#include <bcm2835.h>
#include <stdio.h>
#include <sched.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <time.h>

#define MAX_TIME 45 

#define output BCM2835_GPIO_FSEL_OUTP
#define input BCM2835_GPIO_FSEL_INPT

uint8_t convert(int buff[], int a, int b);
int getData(uint8_t pin);
void writeToFile(float t, int h);
void format_time(char output_[]);
//void printError(int e);

double t0, t1, thirtySecMark1, thirtySecMark0;

int main(int argc, char** argv) {
	
    const struct sched_param priority = {5};
    sched_setscheduler(0, SCHED_FIFO, &priority);
    mlockall(MCL_CURRENT | MCL_FUTURE);
    if (!bcm2835_init())
        return 1;

    thirtySecMark1 = -1;
    int error = 0;
    while (1)
    {
        bcm2835_gpio_fsel(RPI_BPLUS_GPIO_J8_08, output);
        bcm2835_gpio_write(RPI_BPLUS_GPIO_J8_08, HIGH);
        bcm2835_delayMicroseconds(10000);
        error = getData(RPI_BPLUS_GPIO_J8_08); 
    }
    
    return 0;
}

int getData(uint8_t pin){
    //host pulls line to 0
    bcm2835_gpio_fsel(pin, output);
    bcm2835_gpio_write(pin, LOW);
    bcm2835_delay(2);

    //host pulls up to 1 and waits for response
    bcm2835_gpio_write(pin, HIGH);
    bcm2835_gpio_fsel(pin, input);

    int i;

    //wait sensor to pull down
    t0 = bcm2835_st_read();
    while(bcm2835_gpio_lev(pin)){
        t1 = bcm2835_st_read();
        if(t1-t0 > 500){
            return -1;
        }
    }

    //wait sensor to pull up
    t0 = bcm2835_st_read();
    while(!bcm2835_gpio_lev(pin)){
        t1 = bcm2835_st_read();
        if(t1-t0 > 500){
            return -2;
        }   
    }

    //wait sensor to pull down
    t0 = bcm2835_st_read();
    while(bcm2835_gpio_lev(pin)){
        t1 = bcm2835_st_read();
        if(t1-t0 > 500){
            return -3;
        }
    }

    //wait data 0 bit
    t0 = bcm2835_st_read();
    while(!bcm2835_gpio_lev(pin)){
        t1 = bcm2835_st_read();
        if(t1-t0 > 500){
            return -4;
        }
    }
    int buff[40];
    double t, a = 0;
    
    //read data
    for(i = 0; i < 40; i++){

        //save HIGH lenght
        t = bcm2835_st_read();
        t0 = bcm2835_st_read();
        while(bcm2835_gpio_lev(pin)){
            t1 = bcm2835_st_read();
            if(t1-t0 > 500){
                return -5;
            }
        }

        a = bcm2835_st_read();

        //save HIGH lenght to buffer
        buff[i] = a - t;

        //wait for LOW
        t0 = bcm2835_st_read();
        while(!bcm2835_gpio_lev(pin)){
            t1 = bcm2835_st_read();
            if(t1-t0 > 500){
                return -6;
            }
        }
    }


    //set 1 and 0
    for(i = 0; i < 40; i++){
        if(buff[i] > 15 && buff[i] < 40){
            buff[i] = 0;
        } else if (buff[i] < 90 && buff[i] > 65){
            buff[i] = 1;
        } else {
            return -7;
        }
    }

    //debug
    /*for(i = 0; i < 40; i++){
        if((i + 1)% 4 == 0)
        {
            printf("%d ", buff[i]);
        } else printf("%d", buff[i]);
    }*/
    //printf("\n");
    
    //convert bits to values
    uint8_t hum1 = convert(buff, 0, 7);
    uint8_t hum2 = convert(buff, 8, 15);
    uint8_t temp1 = convert(buff, 16, 23);
    uint8_t temp2 = convert(buff, 24, 31);
    uint8_t check = convert(buff, 32, 39);


    float hum = (float) ((hum1 << 8) | hum2) / 10.0;
    float temp = (float) ((temp1 << 8) | temp2);

    //check for negative temp
    if(temp <= 0x8000){
        temp = temp/10;
    } else {
        temp = ((temp - 0x8000) * -1)/10;
    }

    //calculate checksum
    uint16_t sum = hum1 + hum2 + temp1 + temp2;
    sum = sum & 0xFF; 

    //if checksum matches, print results
    if(check == sum){
        //printf("Humidity = %.1f\n", hum);
        //printf("Temperature = %.1f\n", temp);

        //printf("checksum = %i = %i\n\n", sum, check);

        writeToFile(temp, hum);
    } else printf("Checksum didn't match\n");

    return 1;
}

uint8_t convert(int buff[], int a, int b){
    uint8_t result = 0;
    for(int i = a; i <= b; i++){
        result = result << 1;
        result = result | buff[i];
    }

    return result;
}

void writeToFile(float t, int h){

    //save result every >30seconds
    thirtySecMark0 = bcm2835_st_read();
    if((thirtySecMark0 - thirtySecMark1) >= 30000000 || thirtySecMark1 < 0){
        FILE *file;
        file = fopen("/var/temperature/data.csv", "a");

        char output_[24] = {'\0'};
        format_time(output_);

        fprintf(file, "%s,%.1f,%d\n", output_, t, h);
        fclose(file);

        thirtySecMark1 = bcm2835_st_read();
    }
}

void format_time(char output_[]){
    time_t rawtime;
    struct tm * timeinfo;
    
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    
    sprintf(output_, "%02d-%02d-%02dT%02d:%02d:%02d", timeinfo->tm_year + 1900,
            timeinfo->tm_mon + 1, timeinfo->tm_mday, 
            timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

    /*sprintf(output_, "[\"%02d-%02d-%02dT%02d:%02d:%02d\"]", timeinfo->tm_year + 1900,
            timeinfo->tm_mon + 1, timeinfo->tm_mday, 
            timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);*/
}
