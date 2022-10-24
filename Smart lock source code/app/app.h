/***************************************************************************//**
 * @file
 * @brief Top level application functions
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef SMARTLOCK_APP_APP_H
#define SMARTLOCK_APP_APP_H

#include <sl_bluetooth.h>

void app_init(void);
void app_process_action(void);

sl_status_t updatePersonCountReport(uint8_t personCount);
sl_status_t sendBluetoothReportData(void);
void sl_bt_on_event(sl_bt_msg_t *evt);

#endif // SMARTLOCK_APP_APP_H
