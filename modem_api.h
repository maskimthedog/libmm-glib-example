/**
 * @file modem_api.h
 * @brief Modem properties API using ModemManager
 *
 * This header defines the interface for retrieving modem properties
 * using ModemManager through D-Bus.
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

#ifndef MODEM_API_H
#define MODEM_API_H

#include <glib.h>
#include <libmm-glib.h>

/**
 * @struct ModemProperties
 * @brief Structure to hold modem properties
 */
typedef struct {
    gchar *path;                    /**< Modem D-Bus object path */
    gchar *state;                   /**< Modem state as string */
    gchar *manufacturer;            /**< Modem manufacturer */
    gchar *model;                   /**< Modem model */
    gchar *device_id;               /**< Device identifier */
    gchar *imei;                    /**< Equipment identifier (IMEI) */
    gchar *iccid;                   /**< SIM ICCID */
    gchar *imsi;                    /**< SIM IMSI */
    gchar *carrier;                 /**< Network operator name */
    guint plmn;                     /**< PLMN code */
    guint signal_quality;           /**< Signal quality percentage */
    gboolean signal_recent;         /**< Whether signal quality is recent */
    gint lte_rsrp;                  /**< LTE Reference Signal Received Power */
    gint lte_rsrq;                  /**< LTE Reference Signal Received Quality */
    gint lte_snr;                   /**< LTE Signal-to-Noise Ratio */
    gint lte_rssi;                  /**< LTE Received Signal Strength Indicator */
    gchar *access_tech;             /**< Access technologies */
    guint64 tx_bytes;               /**< Transmitted bytes */
    guint64 rx_bytes;               /**< Received bytes */
} ModemProperties;

/**
 * @brief Initialize the modem API
 * @return gboolean TRUE on success, FALSE otherwise
 */
gboolean modem_api_init(void);

/**
 * @brief Clean up modem API resources
 */
void modem_api_cleanup(void);

/**
 * @brief Get list of modem properties
 * @param[out] modems List of ModemProperties structures
 * @param[out] error GError for error reporting
 * @return gboolean TRUE on success, FALSE otherwise
 */
gboolean modem_api_get_properties(GList **modems, GError **error);

/**
 * @brief Free a list of ModemProperties
 * @param modems List of ModemProperties to free
 */
void modem_api_free_properties(GList *modems);

#endif /* MODEM_API_H */
