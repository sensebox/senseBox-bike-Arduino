/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef H_BLEHR_SENSOR_
#define H_BLEHR_SENSOR_

#include "nimble/ble.h"
#include "modlog/modlog.h"

#ifdef __cplusplus
extern "C" {
#endif

    /* Heart-rate configuration */
    #define GATT_SENSORS_UUID                       0x84, 0xBC, 0xB0, 0x1E, 0xBC, 0x8E, 0x04, 0xAD, 0xBE, 0xE0, 0x8E, 0xF6, 0x18, 0xA2, 0x06, 0xCF
    #define GATT_SURFACE_CLASSIFICATION_UUID        0x21, 0xB5, 0xCA, 0x18, 0x0D, 0x2F, 0x8F, 0x96, 0x60, 0x45, 0x95, 0xF4, 0x10, 0xAF, 0x44, 0xB9

    extern uint16_t surface_classification_hrm_handle;

    struct ble_hs_cfg;
    struct ble_gatt_register_ctxt;

    void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg);
    int gatt_svr_init(void);
    void host_task(void *param);
    void advertise(void);
    int gap_event(struct ble_gap_event *event, void *arg);
    void on_sync(void);
    void on_reset(int reason);
    void notify_surface_classification(uint8_t *value1, uint8_t *value2, uint8_t *value3, uint8_t *value4, uint8_t *value5);

#ifdef __cplusplus
}
#endif

#endif