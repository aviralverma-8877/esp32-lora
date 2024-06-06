#include <support_method.h>

DNSServer dnsServer;
Ticker TickerForBtnPresses;
Ticker TickerForLedNotification;
Ticker TickerForDNSRequest;

void handle_operations(JsonDocument doc)
{
    const char* request_type = doc["request-type"];
    serial_print(request_type);
    if(strcmp(request_type, "wifi_ssid_scan") == 0)
    {
        String json_string;
        xTaskCreate(scan_ssid, "scan_wifi", 6000, NULL, 2, NULL);
    }
    if(strcmp(request_type, "connect_wifi") == 0)
    {
        const char * wifi_ssid = doc["wifi_ssid"];
        serial_print(wifi_ssid);
        const char * psk = doc["wifi_pass"];
        serial_print(psk);
        // Reading wifi config
        String wifi_config = get_wifi_setting();
        // modifying wifi config
        serial_print(wifi_config);
        JsonDocument wifi_conf;
        deserializeJson(wifi_conf, wifi_config);
        wifi_conf["wifi_function"] = "STA";
        wifi_conf["wifi_ssid"] = wifi_ssid;
        wifi_conf["wifi_pass"] = psk;
        serializeJsonPretty(wifi_conf, wifi_config);
        serial_print(wifi_config);
        // writing wifi config
        save_wifi_settings(wifi_config);
        restart();
    }
    if(strcmp(request_type, "reset_device") == 0)
    {
        serial_print("Formatting SPIFFS");
        SPIFFS.format();
        JsonDocument doc;
        doc["response_type"] = "alert";
        doc["alert_msg"] = "Device reset successfully,\nPlease reconfigure the device by connecting to the AP.\nRebooting Now.";
        String return_response;
        serializeJsonPretty(doc, return_response);
        send_to_ws(return_response);
        TickerForTimeOut.once_ms(100, restart);
    }
}

void restart()
{
    ESP.restart();
}

void setup_mdns()
{
    String hostname = "project-voyager";
    WiFi.hostname(hostname);
    serial_print("Device hostname: "+hostname);
    if (MDNS.begin(hostname.c_str())) {
        MDNS.addService("http", "tcp", 80); 
        serial_print("Started mDNS service");
    }
}

void setup_dns()
{
    serial_print("DNS service started.");
    serial_print("Soft AP IP.");
    serial_print(WiFi.softAPIP().toString());
    dnsServer.setTTL(300);
    dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
    dnsServer.start(53, "*", WiFi.softAPIP());
    TickerForDNSRequest.attach_ms(10, dns_request_process);
}

void dns_request_process()
{
    dnsServer.processNextRequest();
}

void config_gpios()
{
    pinMode(LED, OUTPUT);
    pinMode(BTN1, INPUT_PULLDOWN);
    pinMode(BTN2, INPUT_PULLDOWN);
    digitalWrite(LED, LOW);
    setupTickers();
}

void serial_print(String msg)
{
    if(DEBUGGING)
    {
        // display_text_oled(msg, 0, 10);
        Serial.println(msg);
    }
}

void setupTickers()
{
    TickerForBtnPresses.attach_ms(10, btn_intrupt);
}

void stop_nortify_led()
{
    TickerForLedNotification.detach();
    digitalWrite(LED, LOW);
}

void nortify_led()
{
    serial_print("Nortify");
    TickerForLedNotification.attach_ms(500, led_nortifier);
}