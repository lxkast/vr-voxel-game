#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware_stuff.h"
#include <math.h>

#define DEADZONE 0.1f


const int SWITCH_PINS[SWITCH_COUNT] = {19, 5, 6, 27, 15}; // Adjust as needed

int hardware_init() {

    // Init switches with pull-ups
    for (int i = 0; i < SWITCH_COUNT; i++) {
        gpio_init(SWITCH_PINS[i]);
        gpio_set_dir(SWITCH_PINS[i], GPIO_IN);
        gpio_pull_up(SWITCH_PINS[i]);
    }

    // Init ADC for joystick (ADC1 = GPIO27, ADC2 = GPIO28)
    adc_init();
    adc_gpio_init(26); // ADC1
    adc_gpio_init(28); // ADC2

}

int read_data(ReadData* data) {
        // Read switches
        for (int i = 0; i < SWITCH_COUNT; i++) {
            data->buttons[i] = !gpio_get(SWITCH_PINS[i]); // active low
        }

        // Read joystick X (ADC1)
        adc_select_input(0);
        uint16_t joy_x = adc_read();

        // Read joystick Y (ADC2)
        adc_select_input(2);
        uint16_t joy_y = adc_read();
        data->dx = (float) joy_x / 4098.0f;
        data->dy = (float) joy_y / 4098.0f;
        if (fabs(data->dx) < DEADZONE) {
            data->dx = 0.f;
        }
        if (fabs(data->dy) < DEADZONE) {
            data->dy = 0.f;
        }
}