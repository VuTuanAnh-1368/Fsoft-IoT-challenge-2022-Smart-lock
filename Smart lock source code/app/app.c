#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <app_assert.h>
#include <em_common.h>
#include <em_gpio.h>
#include <gatt_db.h>
#include <sl_bluetooth.h>

#include "app/door.h"

#include "components/button.h"
#include "components/buzzer.h"
#include "components/clock.h"
#include "components/light.h"
#include "components/lock.h"
#include "components/oled.h"
#include "components/proximitySensor.h"
#include "components/temperatureSensor.h"

#include "app.h"

// The advertising set handle allocated from Bluetooth stack.
static uint8_t advertising_set_handle = 0xff;

static const uint32_t passkey = 135260;

void app_init(void) {
	oled_initialize();
	
	initializeButton();
	initializeBuzzerControl();
	initializeClock();
	initializeLightControl();
	initializeLockControl();
	initializeProximitySensor();

	initializeDoorHandler();
}

void app_process_action(void) {
	tickBuzzerControl();
	tickClock();
	
	tickDoorHandler();
}

bool app_is_ok_to_sleep(void) {
	return false;
}

// Updates the characteristic when people enter room.
sl_status_t updatePersonCountReport(uint8_t personCount) {
	sl_status_t sc;

	// Write attribute in the local GATT database.
	sc = sl_bt_gatt_server_write_attribute_value(
		gattdb_led_control, 0, sizeof(personCount), &personCount
	);
	/*if (sc == SL_STATUS_OK) {
		printf("Attribute written: 0x%02x", (int)data_send);
	}*/

	return sc;
}

// Sends notification of the updated characteristic above.
sl_status_t sendBluetoothReportData(void) {
	sl_status_t sc;
	uint8_t data_send;
	size_t data_len;

	// Read report button characteristic stored in local GATT database.
	sc = sl_bt_gatt_server_read_attribute_value(
		gattdb_led_control, 0, sizeof(data_send), &data_len, &data_send
	);
	if (sc != SL_STATUS_OK) {
		return sc;
	}

	// Send characteristic notification.
	sc = sl_bt_gatt_server_notify_all(
		gattdb_led_control, sizeof(data_send), &data_send
	);
	/*if (sc == SL_STATUS_OK) {
		//printf(" Notification sent: 0x%02x\n", (int)data_send);
	}*/
	return sc;
}

void sl_bt_on_event(sl_bt_msg_t *evt) {
	sl_status_t sc;
	bd_addr address;
	uint8_t address_type;
	uint8_t system_id[8];

	switch (SL_BT_MSG_ID(evt->header)) {
		// This event indicates the device has started and the radio is ready.
		// Do not call any stack command before receiving this boot event.
		case sl_bt_evt_system_boot_id:
			// Extract unique ID from BT Address.
			sc = sl_bt_system_get_identity_address(&address, &address_type);
			app_assert_status(sc);

			sl_bt_sm_configure(0x07, sl_bt_sm_io_capability_displayonly);
			sc = sl_bt_sm_set_passkey(passkey);
			sc = sl_bt_sm_set_bondable_mode(1);

			sc = sl_bt_sm_delete_bondings();

			// Pad and reverse unique ID to get System ID.
			system_id[0] = address.addr[5];
			system_id[1] = address.addr[4];
			system_id[2] = address.addr[3];
			system_id[3] = 0xFF;
			system_id[4] = 0xFE;
			system_id[5] = address.addr[2];
			system_id[6] = address.addr[1];
			system_id[7] = address.addr[0];

			sc = sl_bt_gatt_server_write_attribute_value(
				gattdb_system_id, 0, sizeof(system_id), system_id
			);
			app_assert_status(sc);

			// Create an advertising set.
			sc = sl_bt_advertiser_create_set(&advertising_set_handle);
			app_assert_status(sc);

			// Set advertising interval to 100ms.
			sc = sl_bt_advertiser_set_timing(
				advertising_set_handle,
				160, // min. adv. interval (milliseconds * 1.6)
				160, // max. adv. interval (milliseconds * 1.6)
				0,	 // adv. duration
				0
			); // max. num. adv. events
			app_assert_status(sc);
			// Start general advertising and enable connections.
			sc = sl_bt_advertiser_start(
				advertising_set_handle,
				sl_bt_advertiser_general_discoverable,
				sl_bt_advertiser_connectable_scannable
			);
			app_assert_status(sc);

			/* Purpose unclear, presumably manual open button which has been already implemented in `app/door.c`.
				// Button events can be received from now on.
				sl_button_enable(SL_SIMPLE_BUTTON_INSTANCE(0));
			*/

			// Check the report button state, then update the characteristic and
			// send notification.
			sc = updatePersonCountReport(personCount);
			//app_log_status_error(sc);

			if (sc == SL_STATUS_OK) {
				sc = sendBluetoothReportData();
				//app_log_status_error(sc);
			}
			break;

		// This event indicates that a new connection was opened.
		case sl_bt_evt_connection_opened_id:
			sc = sl_bt_sm_increase_security(evt->data.evt_connection_opened.connection);
			break;

		// This event indicates that a connection was closed.
		case sl_bt_evt_connection_closed_id:
			// Restart advertising after client has disconnected.
			sc = sl_bt_sm_delete_bondings();
			//printf("delete bondings\n");
			//printf("connection closed\n");
			sc = sl_bt_advertiser_start(
				advertising_set_handle,
				sl_bt_advertiser_general_discoverable,
				sl_bt_advertiser_connectable_scannable
			);
			app_assert_status(sc);
			break;

		///////////////////////////////////////////////////////////////////////////
		// Add additional event handlers here as your application requires!      //
		///////////////////////////////////////////////////////////////////////////
		case sl_bt_evt_sm_bonded_id:
			//printf("bond success\n");
			break;

		// This event is triggered if the pairing or bonding procedure fails.
		case sl_bt_evt_sm_bonding_failed_id:
			//printf("bond failed\n");
			sc = sl_bt_connection_close(evt->data.evt_sm_bonding_failed.connection);
			break;

		// This event occurs when the remote device enabled or disabled the
		// notification.
		case sl_bt_evt_gatt_server_characteristic_status_id:
			if (gattdb_led_control == evt->data.evt_gatt_server_characteristic_status.characteristic) {
				// A local Client Characteristic Configuration descriptor was changed in
				// the gattdb_report_button characteristic.
				if (
					evt->data.evt_gatt_server_characteristic_status.client_config_flags
					& sl_bt_gatt_notification
				) {
					// The client just enabled the notification. Send notification of the
					// current button state stored in the local GATT table.
					//printf("Notification enabled.");

					sc = sendBluetoothReportData();
					//app_log_status_error(sc);
				} else {
					//printf("Notification disabled.\n");
				}
			}
			break;

		case sl_bt_evt_gatt_server_user_write_request_id: {
			const int dataValue = evt->data.evt_gatt_server_user_write_request.value.data[0];
			if (dataValue == 1) setLockState(true);
			else if (dataValue == 0) setLockState(false);
			
			sc = sl_bt_gatt_server_send_user_write_response(
				evt->data.evt_gatt_server_user_write_request.connection,
				evt->data.evt_gatt_server_user_write_request.characteristic,
				(uint8_t) SL_STATUS_OK
			);
			//printf("write success\n");
			break;
		}

		case sl_bt_evt_gatt_server_attribute_value_id:
			//printf("change success");
			break;
			
		// Default event handler.
		default:
			break;
	}
}
