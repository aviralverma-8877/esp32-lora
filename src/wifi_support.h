#ifndef wifi_support
    #define wifi_support
    #include <Arduino.h>
    #include <WiFi.h>
    #include <support_method.h>
    #include <ArduinoJson.h>
    #include <web_sockets.h>
    void config_wifi();
    void setup_sta(const char* wifi_ssid, const char* wifi_pass);
    void setup_ap(const char* wifi_ssid);
    void scan_ssid(void* args);
#endif
