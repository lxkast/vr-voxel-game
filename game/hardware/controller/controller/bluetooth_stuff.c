
#define BTSTACK_FILE__ "bluetooth_stuff.c"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "bluetooth_stuff.h"
#include "hardware_stuff.h"

#include "btstack.h"

#include "ble/gatt-service/device_information_service_server.h"
#include "ble/gatt-service/hids_device.h"

// Generated using WARATA, from descriptor.wara
static const uint8_t hidReportDescriptor[] = 
{
    0x05, 0x01,    // UsagePage(Generic Desktop[0x0001])
    0x09, 0x04,    // UsageId(Joystick[0x0004])
    0xA1, 0x01,    // Collection(Application)
    0x85, 0x01,    // Report ID (1)
    0x09, 0x01,    //     UsageId(Pointer[0x0001])
    0xA1, 0x00,    //     Collection(Physical)
    0x09, 0x30,    //         UsageId(X[0x0030])
    0x09, 0x31,    //         UsageId(Y[0x0031])
    0x15, 0x80,    //         LogicalMinimum(-128)
    0x25, 0x7F,    //         LogicalMaximum(127)
    0x95, 0x02,    //         ReportCount(2)
    0x75, 0x08,    //         ReportSize(8)
    0x81, 0x02,    //         Input(Data, Variable, Absolute, NoWrap, Linear, PreferredState, NoNullPosition, BitField)
    0xC0,          //     EndCollection()
    0x05, 0x09,    //     UsagePage(Button[0x0009])
    0x19, 0x01,    //     UsageIdMin(Button 1[0x0001])
    0x29, 0x04,    //     UsageIdMax(Button 4[0x0004])
    0x15, 0x00,    //     LogicalMinimum(0)
    0x25, 0x01,    //     LogicalMaximum(1)
    0x95, 0x04,    //     ReportCount(4)
    0x75, 0x01,    //     ReportSize(1)
    0x81, 0x02,    //     Input(Data, Variable, Absolute, NoWrap, Linear, PreferredState, NoNullPosition, BitField)
    0x95, 0x01,    //     ReportCount(1)
    0x75, 0x04,    //     ReportSize(4)
    0x81, 0x03,    //     Input(Constant, Variable, Absolute, NoWrap, Linear, PreferredState, NoNullPosition, BitField)
    0xC0,          // EndCollection()
};


static btstack_packet_callback_registration_t hci_event_callback_registration;
static btstack_packet_callback_registration_t l2cap_event_callback_registration;
static btstack_packet_callback_registration_t sm_event_callback_registration;
static hci_con_handle_t con_handle = HCI_CON_HANDLE_INVALID;
static uint8_t protocol_mode = 1;

static void packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);

const uint8_t adv_data[] = {
    // Flags general discoverable, BR/EDR not supported
    0x02, BLUETOOTH_DATA_TYPE_FLAGS, 0x06,
    // Name
    0x0a, BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME, 'J', 'o', 'y', 's', 't', 'i', 'c', 'k', '2',
    // 16-bit Service UUIDs
    0x03, BLUETOOTH_DATA_TYPE_COMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS, ORG_BLUETOOTH_SERVICE_HUMAN_INTERFACE_DEVICE & 0xff, ORG_BLUETOOTH_SERVICE_HUMAN_INTERFACE_DEVICE >> 8,
    // Appearance HID - Mouse (Category 15, Sub-Category 2)
    0x03, BLUETOOTH_DATA_TYPE_APPEARANCE, 0xC4, 0x03,
};
const uint8_t adv_data_len = sizeof(adv_data);

static void bluetooth_setup(void){

    // setup l2cap and
    l2cap_init();

    // setup SM: Display only
    sm_init();
    sm_set_io_capabilities(IO_CAPABILITY_NO_INPUT_NO_OUTPUT);
    sm_set_authentication_requirements(SM_AUTHREQ_BONDING | SM_AUTHREQ_BONDING);

    // setup ATT server
    att_server_init(profile_data, NULL, NULL);

    // setup device information service
    device_information_service_server_init();

    // setup HID Device service
    hids_device_init(0, hidReportDescriptor, sizeof(hidReportDescriptor));

    // setup advertisements
    uint16_t adv_int_min = 0x0030;
    uint16_t adv_int_max = 0x0030;
    uint8_t adv_type = 0;
    bd_addr_t null_addr;
    memset(null_addr, 0, 6);
    gap_advertisements_set_params(adv_int_min, adv_int_max, adv_type, 0, null_addr, 0x07, 0x00);
    gap_advertisements_set_data(adv_data_len, (uint8_t*) adv_data);
    gap_advertisements_enable(1);

    // register for events
    hci_event_callback_registration.callback = &packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);

    // register for connection parameter updates
    l2cap_event_callback_registration.callback = &packet_handler;
    l2cap_add_event_handler(&l2cap_event_callback_registration);

    sm_event_callback_registration.callback = &packet_handler;
    sm_add_event_handler(&sm_event_callback_registration);

    hids_device_register_packet_handler(packet_handler);
}

static int dx;
static int dy;
static uint8_t buttons;
// // HID Report sending
static void send_report(void){
    ReadData data;
    read_data(&data);
    uint8_t report[] = { (int16_t) ((data.dx * 256) - 128), (int16_t) (data.dy * 256) - 128, 0};
    
    for (int i =0; i < SWITCH_COUNT; i++) {
        if (data.buttons[i]) {
            report[2] |= 1 << i;
        } else {
            report[2] &= ~(1 << i);
        }
    }
    
    hids_device_send_input_report(con_handle, report, sizeof(report));
        
}


static void mousing_can_send_now(void){
    send_report();
}

static void packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
    UNUSED(channel);
    UNUSED(size);
    uint16_t conn_interval;

    if (packet_type != HCI_EVENT_PACKET) return;

    uint8_t t = hci_event_packet_get_type(packet);
    switch (hci_event_packet_get_type(packet)) {
        case HCI_EVENT_DISCONNECTION_COMPLETE:
            con_handle = HCI_CON_HANDLE_INVALID;
            printf("Disconnected\n");
            break;
        case SM_EVENT_JUST_WORKS_REQUEST:
            printf("Just Works requested\n");
            sm_just_works_confirm(sm_event_just_works_request_get_handle(packet));
            break;  
        case HCI_EVENT_HIDS_META:
            switch (hci_event_hids_meta_get_subevent_code(packet)){
                case HIDS_SUBEVENT_INPUT_REPORT_ENABLE:
                    con_handle = hids_subevent_input_report_enable_get_con_handle(packet);
                    printf("Report Characteristic Subscribed %u\n", hids_subevent_input_report_enable_get_enable(packet));
                    break;
                case HIDS_SUBEVENT_CAN_SEND_NOW:
                    mousing_can_send_now();
                    break;
                default:
                    break;
            }
            break;
            
        default:
            break;
    }
}

static btstack_timer_source_t heartbeat;

static void heartbeat_handler(struct btstack_timer_source *ts){
    
    if (con_handle != HCI_CON_HANDLE_INVALID) {
        hids_device_request_can_send_now_event(con_handle);
    }
    
    // Schedule next heartbeat
    btstack_run_loop_set_timer(ts, 30); // 1 second
    btstack_run_loop_add_timer(ts);
}


int btstack_main(void)
{
    bluetooth_setup();

    hci_power_control(HCI_POWER_ON);

    heartbeat.process = &heartbeat_handler;
    btstack_run_loop_set_timer(&heartbeat, 30);
    btstack_run_loop_add_timer(&heartbeat);
    return 0;
}