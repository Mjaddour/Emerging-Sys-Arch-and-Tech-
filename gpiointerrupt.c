#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/UART2.h>
#include <ti/drivers/Timer.h>
#include "ti_drivers_config.h"

volatile int TimerFlag = 0;

// UART2 Global Variables
char output2[64]; // Buffer for data to be sent
int bytesToSend2; // Number of bytes to send
UART2_Handle uart2; // Handle for the UART2 module

#define DISPLAY2(x) UART2_write(uart2, output2, x)

void DISPLAY(int curTemp, int setTemp, int heat, int sec) {
    printf("Current Temp: %d, Set Temp: %d, Heat: %d, Sec: %d\n", curTemp, setTemp, heat, sec);
}

int readTemp() {
    // Simulated temperature reading
    return 25;
}

void timerCallback(Timer_Handle myHandle, int_fast16_t status) {
    // Implementation goes here
    TimerFlag = 1;
}

typedef struct {
    int curtemp;
    int settemp;
    int heat;
    int sec;
} TemperatureSetup;

typedef struct {
    int checkInput;
    int checkTemp;
    int displayTemp;
} InterruptSetup;

void initUART2(void) {
    UART2_Params uart2Params;

    // Set default UART2 parameters before making specific settings
    UART2_Params_init(&uart2Params);
    uart2Params.baudRate = 115200;

    // Try to open the UART2 channel
    uart2 = UART2_open(CONFIG_UART2_0, &uart2Params);
    if (uart2 == NULL) {
        printf("Failed to open UART2\n");
    } else {
        printf("UART2 initialized successfully\n");
    }
}

void initI2C() {
    I2C_Params i2cParams;
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz; // 400 kHz I2C speed
    I2C_Handle i2c = I2C_open(CONFIG_I2C_0, &i2cParams);
    if (i2c == NULL) {
        printf("Failed to open I2C\n");
    } else {
        printf("I2C initialized successfully\n");
    }
}

void initTimer() {
    Timer_Handle timer0;
    Timer_Params params;

    Timer_Params_init(&params);
    params.period = 1000000; // 1 second period
    params.periodUnits = Timer_PERIOD_US;
    params.timerMode = Timer_CONTINUOUS_CALLBACK;
    params.timerCallback = timerCallback;

    timer0 = Timer_open(CONFIG_TIMER_0, &params);
    if (timer0 == NULL) {
        printf("Failed to open timer\n");
    } else {
        Timer_start(timer0);
        printf("Timer initialized and started\n");
    }
}

void gpioButtonFxn0(uint_least8_t index) {
    GPIO_toggle(CONFIG_GPIO_LED_0);
}

void gpioButtonFxn1(uint_least8_t index) {
    GPIO_toggle(CONFIG_GPIO_LED_0);
}

void *mainThread(void *arg0) {
    GPIO_init();
    initUART2();
    initI2C();
    initTimer();

    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_GPIO_BUTTON_0, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);

    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);
    GPIO_setCallback(CONFIG_GPIO_BUTTON_0, gpioButtonFxn0);
    GPIO_enableInt(CONFIG_GPIO_BUTTON_0);

    if (CONFIG_GPIO_BUTTON_0 != CONFIG_GPIO_BUTTON_1) {
        GPIO_setConfig(CONFIG_GPIO_BUTTON_1, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);
        GPIO_setCallback(CONFIG_GPIO_BUTTON_1, gpioButtonFxn1);
        GPIO_enableInt(CONFIG_GPIO_BUTTON_1);
    }

    // Allocate memory for TemperatureSetup structure to hold temperature control settings
    TemperatureSetup *temp = malloc(sizeof(TemperatureSetup));
    // Allocate memory for InterruptSetup structure to manage custom interrupt timings
    InterruptSetup *customInterrupt = malloc(sizeof(InterruptSetup));
    // Initialize tick for counting time intervals, sec for seconds tracking
    int tick = 0, sec = 0;
    // Initialize flags for incrementing and decrementing the set temperature
    int increase = 0, decrease = 0;

    // Set initial heating status to 0 (off)
    temp->heat = 0;
    // Set initial temperature setting to 25 degrees
    temp->settemp = 25;


    customInterrupt->checkInput = 2;  // 200ms
    customInterrupt->checkTemp = 5;   // 500ms
    customInterrupt->displayTemp = 10;  // 1s

    while (1) {  // Infinite loop to continuously monitor and control the temperature
        temp->curtemp = readTemp();  // Read the current temperature from a sensor

        // Check if the tick count is a multiple of the checkInput value for button press handling
        if (tick % customInterrupt->checkInput == 0) {
            if (increase) temp->settemp++;  // Increase the set temperature if the increase flag is set
            if (!decrease) temp->settemp--;  // Decrease the set temperature if the decrease flag is not set
            increase = 0;  // Reset the increase flag
            decrease = 1;  // Set the decrease flag
        }

        // Check if the current tick aligns with the checkTemp interval for temperature control
        if ((tick & customInterrupt->checkTemp) == 0) {
            if (temp->settemp > temp->curtemp) {
                GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);  // Turn on the LED if the set temp is higher than the current temp
            } else {
                GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF);  // Turn off the LED if the set temp is not higher
            }
        }

        // Update the display if the current tick matches the displayTemp interval
        if (tick % customInterrupt->displayTemp == 0) {
            temp->sec = ++sec;  // Increment and update the seconds counter
            DISPLAY(temp->curtemp, temp->settemp, temp->heat, temp->sec);  // Display current state
        }

        // Wait here until the timer callback sets the TimerFlag, indicating that the timer interval has elapsed
        while (!TimerFlag) {}
        TimerFlag = 0;  // Reset the TimerFlag for the next cycle
        tick++;  // Increment the tick counter
    }

}
