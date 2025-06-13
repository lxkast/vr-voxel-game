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
    printf("Press any key to start...\n");
    getchar();
    
    printf("Starting controller program...\n");
    if (cyw43_arch_init()) {
        printf("failed to initialise cyw43_arch\n");
        return -1;
    }

    hardware_init();

    
    // hci_event_callback_registration.callback = &packet_handler;
    // hci_add_event_handler(&hci_event_callback_registration);

    
    btstack_main();
    btstack_run_loop_execute();
}
