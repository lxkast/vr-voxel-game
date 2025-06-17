#include <stdio.h>
#include "pico/stdlib.h"

#include "btstack_event.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "btstack.h"

#include "bluetooth_stuff.c"
#include "hardware_stuff.h"

int main()
{
    stdio_init_all();
    
    printf("Starting controller program...\n");
    if (cyw43_arch_init()) {
        printf("failed to initialise cyw43_arch\n");
        return -1;
    }

    hardware_init();
    
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true); // turn LED on

    btstack_main();
    btstack_run_loop_execute();
}
