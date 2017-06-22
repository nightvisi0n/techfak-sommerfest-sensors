#include "WifiWrapper.h"

void WifiWrapper::connect(const char* ssid, const char* username, const char* password) {
    wifi_set_opmode(STATION_MODE);

    struct station_config wifi_config;
    memset(&wifi_config, 0, sizeof(struct station_config));

    strcpy((char *)wifi_config.ssid, ssid);

    if (!wifi_station_set_config(&wifi_config)) {
        Serial.println("[WifiWrapper] set config failed");
    }

    wifi_station_clear_cert_key();
    wifi_station_clear_enterprise_ca_cert();

    wifi_station_set_wpa2_enterprise_auth(1);
    wifi_station_set_reconnect_policy(true);

    if (wifi_station_set_enterprise_username((uint8 *)username, strlen(username))) {
        Serial.println("[WifiWrapper] set username failed");
    }
    if (wifi_station_set_enterprise_password((uint8 *)password, strlen(password))) {
        Serial.println("[WifiWrapper] set password failed");
    }
    wifi_station_set_enterprise_new_password((uint8 *)password, strlen(password));

    wifi_set_event_handler_cb(wifi_handle_event_cb);

    if (!wifi_station_connect()) {
        Serial.println("[WifiWrapper] connect failed");
    }

    while (WiFi.status() != WL_CONNECTED) {
        delay(400);
        digitalWrite(BUILTIN_LED, false);
        delay(50);
        digitalWrite(BUILTIN_LED, true);
    }

    digitalWrite(BUILTIN_LED, false);
    delay(500);
    digitalWrite(BUILTIN_LED, true);
}

void WifiWrapper::connect(const char *ssid, const char *password) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(400);
        digitalWrite(BUILTIN_LED, false);
        delay(50);
        digitalWrite(BUILTIN_LED, true);
    }

    digitalWrite(BUILTIN_LED, false);
    delay(500);
    digitalWrite(BUILTIN_LED, true);
}

void WifiWrapper::reconnect() {
    while (WiFi.status() != WL_CONNECTED) {
        delay(5000);
        digitalWrite(BUILTIN_LED, false);
        delay(50);
        digitalWrite(BUILTIN_LED, true);
    }
}

void WifiWrapper::wifi_handle_event_cb(System_Event_t *evt) {
    switch (evt->event) {
        case EVENT_STAMODE_CONNECTED:
            os_printf("[connect to ssid %s, channel %d]\n",
                evt->event_info.connected.ssid,
                evt->event_info.connected.channel);
            break;
        case EVENT_STAMODE_DISCONNECTED:
            os_printf("[disconnect from ssid %s, reason %d]\n",
                evt->event_info.disconnected.ssid,
                evt->event_info.disconnected.reason);
            break;
        case EVENT_STAMODE_AUTHMODE_CHANGE:
            os_printf("[mode: %d -> %d]\n",
                evt->event_info.auth_change.old_mode,
                evt->event_info.auth_change.new_mode);
            break;
        case EVENT_STAMODE_GOT_IP:
            os_printf("[ip:" IPSTR ",mask:" IPSTR ",gw:" IPSTR "]\n",
                IP2STR(&evt->event_info.got_ip.ip),
                IP2STR(&evt->event_info.got_ip.mask),
                IP2STR(&evt->event_info.got_ip.gw));
            break;
        default:
            os_printf("[event: %x]\n", evt->event);
            break;
    }
}
