/**
 * @file example_app.c
 * @brief Example application using modem API
 */

/*
 * Copyright (c) 2025 Rob Krakora
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "modem_api.h"
#include <stdio.h>

/**
 * @brief Print modem properties
 * @param props Modem properties to print
 */
static void print_modem_properties(const ModemProperties *props) {
    if (NULL == props) {
        return;
    }

    printf("Modem at path: %s\n", props->path ? props->path : "Unknown");
    printf("  State: %s\n", props->state ? props->state : "Unknown");
    printf("  Manufacturer: %s\n", props->manufacturer ? props->manufacturer : "Unknown");
    printf("  Model: %s\n", props->model ? props->model : "Unknown");
    printf("  Device ID: %s\n", props->device_id ? props->device_id : "Unknown");
    printf("  IMEI: %s\n", props->imei ? props->imei : "Unknown");
    printf("  ICCID: %s\n", props->iccid ? props->iccid : "Unknown");
    printf("  IMSI: %s\n", props->imsi ? props->imsi : "Unknown");
    printf("  Carrier: %s\n", props->carrier ? props->carrier : "Unknown");
    printf("  PLMN: %u\n", props->plmn);
    printf("  Signal Quality: %u%% (%s)\n", props->signal_quality,
           props->signal_recent ? "recent" : "not recent");
    printf("  Extended Signal Quality:\n");
    if (-1 != props->lte_rsrp) printf("    LTE RSRP: %d dBm\n", props->lte_rsrp);
    if (-1 != props->lte_rsrq) printf("    LTE RSRQ: %d dB\n", props->lte_rsrq);
    if (-1 != props->lte_snr) printf("    LTE SNR: %d dB\n", props->lte_snr);
    if (-1 != props->lte_rssi) printf("    LTE RSSI: %d dBm\n", props->lte_rssi);
    printf("  Access Technologies: %s\n", props->access_tech ? props->access_tech : "Unknown");
    printf("  Data Usage:\n");
    printf("    Transmitted Bytes: %" G_GUINT64_FORMAT " bytes\n", props->tx_bytes);
    printf("    Received Bytes: %" G_GUINT64_FORMAT " bytes\n", props->rx_bytes);
}

/**
 * @brief Main function
 * @param argc Argument count
 * @param argv Argument vector
 * @return int Exit status
 */
int main(int argc, char *argv[]) {
    GList *modems = NULL;
    GError *error = NULL;

    if (FALSE == modem_api_init()) {
        fprintf(stderr, "Failed to initialize modem API\n");
        return 1;
    }

    if (TRUE == modem_api_get_properties(&modems, &error)) {
        for (GList *l = modems; NULL != l; l = g_list_next(l)) {
            print_modem_properties((ModemProperties *)l->data);
        }
    } else if (NULL != error) {
        fprintf(stderr, "Error getting modem properties: %s\n", error->message);
        g_error_free(error);
    }

    modem_api_free_properties(modems);
    modem_api_cleanup();

    return 0;
}
