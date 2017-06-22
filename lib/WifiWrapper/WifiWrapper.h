#pragma once

extern "C" {
    #include <user_interface.h>
    #include <wpa2_enterprise.h>
}

#include <ESP8266WiFi.h>
#include <Client.h>

class WifiWrapper {
private:
    static void wifi_handle_event_cb(System_Event_t *evt);
public:
    static void connect(const char* ssid, const char* password);
    static void connect(const char* ssid, const char* username, const char* password);
    static void reconnect();
};
