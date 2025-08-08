/**
 * @file modem_api.c
 * @brief Implementation of modem properties API
 */

#include "modem_api.h"
#include <stdio.h>

/** @brief D-Bus connection */
static GDBusConnection *connection = NULL;
/** @brief ModemManager manager */
static MMManager *manager = NULL;

/**
 * @brief Free a single ModemProperties structure
 * @param data Pointer to ModemProperties structure
 */
static void modem_properties_free(gpointer data) {
    ModemProperties *props = (ModemProperties *)data;
    if (NULL != props) {
        g_free(props->path);
        g_free(props->state);
        g_free(props->manufacturer);
        g_free(props->model);
        g_free(props->device_id);
        g_free(props->imei);
        g_free(props->iccid);
        g_free(props->imsi);
        g_free(props->carrier);
        g_free(props->access_tech);
        g_free(props);
    }
}

gboolean modem_api_init(void) {
    GError *error = NULL;
    gboolean success = TRUE;

    // Initialize GLib type system for older versions
#if !GLIB_CHECK_VERSION(2,35,0)
    g_type_init();
#endif

    // Establish D-Bus connection
    connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
    if (NULL == connection) {
        fprintf(stderr, "Error connecting to D-Bus: %s\n", error->message);
        g_error_free(error);
        success = FALSE;
    }

    // Create ModemManager manager
    if (TRUE == success) {
        manager = mm_manager_new_sync(connection, G_DBUS_OBJECT_MANAGER_CLIENT_FLAGS_NONE, NULL, &error);
        if (NULL == manager) {
            fprintf(stderr, "Error creating ModemManager manager: %s\n", error->message);
            g_error_free(error);
            success = FALSE;
        }
    }

    return success;
}

void modem_api_cleanup(void) {
    if (NULL != manager) {
        g_object_unref(manager);
        manager = NULL;
    }
    if (NULL != connection) {
        g_object_unref(connection);
        connection = NULL;
    }
}

gboolean modem_api_get_properties(GList **modems, GError **error) {
    GList *modem_list = NULL;
    gboolean success = TRUE;

    if (NULL == modems) {
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_INVALID_ARGUMENT, "Invalid modems pointer");
        return FALSE;
    }

    *modems = NULL;

    if (NULL == manager) {
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_FAILED, "Modem manager not initialized");
        return FALSE;
    }

    modem_list = g_dbus_object_manager_get_objects(G_DBUS_OBJECT_MANAGER(manager));
    if (NULL == modem_list) {
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND, "No modems detected");
        return FALSE;
    }

    for (GList *l = modem_list; NULL != l; l = g_list_next(l)) {
        MMObject *object = MM_OBJECT(l->data);
        MMModem *modem = NULL;
        ModemProperties *props = NULL;

        if (NULL == object) {
            continue;
        }

        modem = mm_object_peek_modem(object);
        if (NULL != modem) {
            props = g_new0(ModemProperties, 1);
            props->path = g_strdup(mm_object_get_path(object));
            props->state = g_strdup(mm_modem_state_get_string(mm_modem_get_state(modem)));
            props->manufacturer = g_strdup(mm_modem_get_manufacturer(modem));
            props->model = g_strdup(mm_modem_get_model(modem));
            props->device_id = g_strdup(mm_modem_get_device_identifier(modem));
            props->imei = g_strdup(mm_modem_get_equipment_identifier(modem));
            props->carrier = g_strdup(mm_modem_3gpp_get_operator_name(mm_object_get_modem_3gpp(object)));
            props->signal_quality = mm_modem_get_signal_quality(modem, &props->signal_recent);
            props->access_tech = g_strdup(mm_modem_access_technology_build_string_from_mask(
                mm_modem_get_access_technologies(modem)));

            // Get SIM information
            MMSim *sim = mm_modem_get_sim_sync(modem, NULL, error);
            if (NULL != sim) {
                props->imsi = g_strdup(mm_sim_get_imsi(sim));
                props->iccid = g_strdup(mm_sim_get_identifier(sim));
                g_object_unref(sim);
            } else if (NULL != *error) {
                fprintf(stderr, "Error getting SIM: %s\n", (*error)->message);
                g_error_free(*error);
                *error = NULL;
            }

            // Get extended signal quality
            MMModemSignal *modem_signal = mm_object_get_modem_signal(object);
            if (NULL != modem_signal) {
                if (TRUE == mm_modem_signal_setup_sync(modem_signal, 10, NULL, error)) {
                    MMSignal *signal = mm_modem_signal_peek_lte(modem_signal);
                    if (NULL != signal) {
                        props->lte_rsrp = mm_signal_get_rsrp(signal);
                        props->lte_rsrq = mm_signal_get_rsrq(signal);
                        props->lte_snr = mm_signal_get_snr(signal);
                        props->lte_rssi = mm_signal_get_rssi(signal);
                    }
                }
                if (NULL != *error) {
                    fprintf(stderr, "Error setting up signal: %s\n", (*error)->message);
                    g_error_free(*error);
                    *error = NULL;
                }
            }

            // Get PLMN
            const gchar *operator_id = mm_modem_3gpp_get_operator_code(mm_object_get_modem_3gpp(object));
            if (NULL != operator_id) {
                props->plmn = g_ascii_strtoull(operator_id, NULL, 10);
            }

            // Get data usage
            GList *bearers = mm_modem_list_bearers_sync(modem, NULL, error);
            if (NULL != bearers) {
                for (GList *b = bearers; NULL != b; b = g_list_next(b)) {
                    MMBearer *bearer = (MMBearer *)b->data;
                    if (NULL != bearer) {
                        MMBearerStats *stats = mm_bearer_peek_stats(bearer);
                        if (NULL != stats) {
                            props->tx_bytes += mm_bearer_stats_get_tx_bytes(stats);
                            props->rx_bytes += mm_bearer_stats_get_rx_bytes(stats);
                        }
                    }
                }
                g_list_free_full(bearers, g_object_unref);
            } else if (NULL != *error) {
                fprintf(stderr, "Error getting bearers: %s\n", (*error)->message);
                g_error_free(*error);
                *error = NULL;
            }

            *modems = g_list_append(*modems, props);
        }
    }

    g_list_free_full(modem_list, g_object_unref);
    return success;
}

void modem_api_free_properties(GList *modems) {
    if (NULL != modems) {
        g_list_free_full(modems, modem_properties_free);
    }
}
