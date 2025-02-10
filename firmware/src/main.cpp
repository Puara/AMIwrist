/* GuitarAMI M5StickC - Main file
 * Edu Meneses
 * IDMIL - McGill University (2022)
 * Société des Arts Technologiques (SAT) - 2025 
 *
 * Info on the M5StickC: 
 *    https://shop.m5stack.com/products/stick-c
 *    https://github.com/m5stack/M5StickC
 */

#include "Arduino.h"
#include "puara.h"
#include "puara_device.hpp"
#include <iostream>
#include <M5Unified.h>
#include <puara/gestures.h>

Puara puara;

byte led_pin = 10;
bool lcd_on = false;

#include <WiFiUdp.h>
#include <OSCMessage.h>

WiFiUDP Udp;

///////////
// Extra //
///////////

void printFloatArray(float* myArray, int arraySize) {
    Serial.print("[");
    for (int i = 0; i < arraySize; i++) {
        Serial.print(myArray[i]);
        if (i < arraySize - 1) {
            Serial.print(", ");
        }
    }
    Serial.println("]");
}


///////////
// SETUP //
///////////

void setup() {
    #ifdef Arduino_h
        Serial.begin(115200);
    #endif

    // Initialize the M5StickC
    auto cfg = M5.config();
    M5.begin(cfg);
    M5.Lcd.fillScreen(BLACK);
    lcd_on = true;
    M5.Lcd.setCursor(0,0);
    M5.Lcd.setTextColor(ORANGE);
    M5.Lcd.setRotation(3);
    M5.Lcd.setTextSize(2);
    M5.Lcd.printf("Booting...\n\n");

    puara.start();

    printf( "\n"
    "AMIwrist module - Puara framework\n"
    "Edu Meneses\n"
    "IDMIL - CIRMMT - McGill University\n"
    "Societe des Arts Tecnologiques (SAT)\n"
    "module ID: %03i\n"
    "Version %d\n"
    "\n"
    "Booting System...\n\n",puara.get_dmi_name(),puara.get_version());

    M5.Lcd.printf(puara.get_dmi_name().c_str());

    std::cout << "\n" 
    << "Custom AMIWrist settings (data/settings.json):\n" 
    << "lcdDelay: "   << puara.getVarNumber("lcdDelay")  << "\n"
    << std::endl;

    // Start the UDP instances 
    Udp.begin(puara.getLocalPORT());

    // Setting Deep sleep wake button
    //     esp_sleep_enable_ext0_wakeup(GPIO_NUM_15,0); // 1 = High, 0 = Low
        
    printf("\nBoot complete\n");

    delay(puara.getVarNumber("lcdDelay"));
    M5.Lcd.fillScreen(BLACK); lcd_on = true;
    M5.Lcd.setCursor(0,0);
    M5.Lcd.setTextSize(2);
    M5.Lcd.printf("Done!\n\n");
    M5.Lcd.setTextSize(3);
    M5.Lcd.printf("Have\n"); M5.Lcd.printf("Fun!\n");
    M5.Lcd.setCursor(0,0);
    M5.Lcd.setTextSize(3);
    delay(puara.getVarNumber("lcdDelay"));
    M5.Lcd.fillScreen(BLACK);
    M5.Power.Axp192.setLDO2(0);
    M5.Power.Axp192.setLDO3(0);
    lcd_on = false;
    
}

//////////
// LOOP //
//////////

void loop() {

    auto imu_update = M5.Imu.update();
    if (imu_update){
        auto data = M5.Imu.getImuData();
        M5.Power.getBatteryLevel();
        // printFloatArray(data.value,6); 

        if (puara.IP1_ready()) { // set namespace and send OSC message for address 1
            OSCMessage msg1(("/" + puara.get_dmi_name() + "/imu").c_str()); 
            msg1.add(data.accel.x); msg1.add(data.accel.y); msg1.add(data.accel.z);
            msg1.add(data.gyro.x); msg1.add(data.gyro.y); msg1.add(data.gyro.z);
            Udp.beginPacket(puara.getIP1().c_str(), puara.getPORT1());
            msg1.send(Udp);
            Udp.endPacket();
            msg1.empty();
            //std::cout << "Message send to " << puara.getIP1() << std::endl;
        }
        if (puara.IP2_ready()) { // set namespace and send OSC message for address 2
            OSCMessage msg2(("/" + puara.get_dmi_name() + "/imu").c_str()); 
            msg2.add(data.accel.x); msg2.add(data.accel.y); msg2.add(data.accel.z);
            msg2.add(data.gyro.x); msg2.add(data.gyro.y); msg2.add(data.gyro.z);
            Udp.beginPacket(puara.getIP2().c_str(), puara.getPORT2());
            msg2.send(Udp);
            Udp.endPacket();
            msg2.empty();
            //std::cout << "Message send to " << puara.getIP2() << std::endl;
        }
    }

    M5.update();

    if (M5.BtnA.wasPressed()) {
        if (puara.IP1_ready()) { // set namespace and send OSC message for address 1
            OSCMessage msg1(("/" + puara.get_dmi_name() + "/button").c_str()); 
            msg1.add(1);
            Udp.beginPacket(puara.getIP1().c_str(), puara.getPORT1());
            msg1.send(Udp);
            Udp.endPacket();
            msg1.empty();
            //std::cout << "Message send to " << puara.getIP1() << std::endl;
        }
        if (puara.IP2_ready()) { // set namespace and send OSC message for address 2
            OSCMessage msg2(("/" + puara.get_dmi_name() + "/button").c_str()); 
            msg2.add(1);
            Udp.beginPacket(puara.getIP2().c_str(), puara.getPORT2());
            msg2.send(Udp);
            Udp.endPacket();
            msg2.empty();
            //std::cout << "Message send to " << puara.getIP2() << std::endl;
        }
    }

    if (M5.BtnA.wasReleased()) {
        if (puara.IP1_ready()) { // set namespace and send OSC message for address 1
            OSCMessage msg1(("/" + puara.get_dmi_name() + "/button").c_str()); 
            msg1.add(0);
            Udp.beginPacket(puara.getIP1().c_str(), puara.getPORT1());
            msg1.send(Udp);
            Udp.endPacket();
            msg1.empty();
            //std::cout << "Message send to " << puara.getIP1() << std::endl;
        }
        if (puara.IP2_ready()) { // set namespace and send OSC message for address 2
            OSCMessage msg2(("/" + puara.get_dmi_name() + "/button").c_str()); 
            msg2.add(0);
            Udp.beginPacket(puara.getIP2().c_str(), puara.getPORT2());
            msg2.send(Udp);
            Udp.endPacket();
            msg2.empty();
            //std::cout << "Message send to " << puara.getIP2() << std::endl;
        }
    }

    // Show LCD instructions
    if (M5.BtnB.wasPressed()) {
        M5.Power.Axp192.setLDO2(2800);
        M5.Power.Axp192.setLDO3(3000);
        M5.Lcd.fillScreen(BLACK); lcd_on = true;
        M5.Lcd.setCursor(0,0);
        M5.Lcd.setTextSize(1);
        M5.Lcd.printf("- Release for info\n\n"
                      "- Hold for setup mode\n\n"
                      "- Hold Power button to\n"
                      "  turn off");
    }

    // Check if setup mode has been called
    if (M5.BtnB.pressedFor(6000)) {
        printf("\nLong button B press, entering setup mode\n\n");
        M5.Lcd.fillScreen(BLACK); lcd_on = true;
        M5.Lcd.setCursor(0,0);
        M5.Lcd.setTextSize(2);
        M5.Lcd.printf("Entering\nsetup\nmode...");
        // PuaraAPI::DeviceConfiguration{}.persistentAP = true;
        // puara.write_config_json();
        PuaraAPI::Device{}.reboot_with_delay();
    }

    // LCD info
    if (M5.BtnB.wasReleased()) {
        M5.Lcd.fillScreen(BLACK); lcd_on = true;
        M5.Lcd.setCursor(0,0);
        M5.Lcd.setTextSize(1);
        M5.Lcd.printf("%s\n \n", puara.get_dmi_name().c_str());
        M5.Lcd.setTextSize(3);
        M5.Lcd.printf("Bat:%u%%\n", M5.Power.getBatteryLevel());
        M5.Lcd.setTextSize(1);
        M5.Lcd.printf("charging: %d\n\n", M5.Power.isCharging());
        M5.Lcd.printf("Setup | IP: ");
        //M5.Lcd.printf("%s\n", puara.get_staip_name().c_str()); // Needs https://github.com/Puara/puara-module/issues/16 fixed
        M5.Lcd.println("            192.168.4.1");
    }

    // turn LCD off after inactivity
    if (M5.BtnB.releasedFor(puara.getVarNumber("lcdDelay")) &&  lcd_on == true) {
        M5.Lcd.fillScreen(BLACK);
        M5.Power.Axp192.setLDO2(0);
        M5.Power.Axp192.setLDO3(0);
        lcd_on = false;
    }

    // run at 100 Hz
    vTaskDelay(10 / portTICK_PERIOD_MS);
}
