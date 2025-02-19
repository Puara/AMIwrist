/* GuitarAMI M5StickC - Main file
 * Edu Meneses
 * IDMIL - McGill University (2022)
 * Société des Arts Technologiques (SAT) - 2025 
 *
 * Info on the M5StickC: 
 *    https://shop.m5stack.com/products/stick-c
 *    https://github.com/m5stack/M5StickC
 */

#include <Arduino.h>
#include <puara.h>
#include <puara_device.hpp> // have to include this to use reboot_with_delay()
#include <iostream>
#include <M5Unified.h>
#include <puara/gestures.h>


Puara puara;
puara_gestures::Jab3D jab;
puara_gestures::Shake3D shake;
IMU_Orientation imu_orientation;
//puara_gestures::Button button;

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
    "Booting System...\n\n",puara.dmi_name(),puara.version());

    M5.Lcd.printf(puara.dmi_name().c_str());

    std::cout << "\n" 
    << "Custom AMIWrist settings (data/settings.json):\n" 
    << "lcdDelay: "   << puara.getVarNumber("lcdDelay")  << "\n"
    << std::endl;

    // Start the UDP instances 
    Udp.begin(puara.LocalPORT());

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

OSCMessage msgAccl(("/" + puara.dmi_name() + "/accl").c_str()); 
OSCMessage msgGyro(("/" + puara.dmi_name() + "/gyro").c_str()); 
OSCMessage msgJab(("/" + puara.dmi_name() + "/jab").c_str()); 
OSCMessage msgShake(("/" + puara.dmi_name() + "/shake").c_str()); 
OSCMessage msgYpr(("/" + puara.dmi_name() + "/ypr").c_str()); 
OSCMessage msgButton(("/" + puara.dmi_name() + "/button").c_str()); 


//////////
// LOOP //
//////////

void loop() {

    M5.update();
    auto imu_update = M5.Imu.update();

    if (imu_update){
        auto data = M5.Imu.getImuData();
        M5.Power.getBatteryLevel();
        // printFloatArray(data.value,6); 

        msgAccl.add(data.accel.x); msgAccl.add(data.accel.y); msgAccl.add(data.accel.z);
        if (puara.IP1_ready()) {
            Udp.beginPacket(puara.IP1().c_str(), puara.PORT1());
            msgAccl.send(Udp);
            Udp.endPacket();
        }
        if (puara.IP2_ready()) {
            Udp.beginPacket(puara.IP2().c_str(), puara.PORT2());
            msgAccl.send(Udp);
            Udp.endPacket();
        }
        msgAccl.empty();

        msgGyro.add(data.gyro.x); msgGyro.add(data.gyro.y); msgGyro.add(data.gyro.z);
        if (puara.IP1_ready()) {
            Udp.beginPacket(puara.IP1().c_str(), puara.PORT1());
            msgGyro.send(Udp);
            Udp.endPacket();
        }
        if (puara.IP2_ready()) {
            Udp.beginPacket(puara.IP2().c_str(), puara.PORT2());
            msgGyro.send(Udp);
            Udp.endPacket();
        }
        msgGyro.empty();

        jab.update(data.accel.x, data.accel.y, data.accel.z);
        shake.update(data.accel.x, data.accel.y, data.accel.z);
        imu_orientation.setAccelerometerValues(data.accel.x, data.accel.y, data.accel.z);
        imu_orientation.setGyroscopeDegreeValues(data.gyro.x, data.gyro.y, data.gyro.z, 10.);
        imu_orientation.update();

        msgJab.add(jab.x.current_value());
        msgJab.add(jab.y.current_value());
        msgJab.add(jab.z.current_value());
        if (puara.IP1_ready()) {
            Udp.beginPacket(puara.IP1().c_str(), puara.PORT1());
            msgJab.send(Udp);
            Udp.endPacket();
        }
        if (puara.IP2_ready()) {
            Udp.beginPacket(puara.IP2().c_str(), puara.PORT2());
            msgJab.send(Udp);
            Udp.endPacket();
        }
        msgJab.empty();

        msgShake.add(shake.x.current_value());
        msgShake.add(shake.y.current_value());
        msgShake.add(shake.z.current_value());
        if (puara.IP1_ready()) {
            Udp.beginPacket(puara.IP1().c_str(), puara.PORT1());
            msgShake.send(Udp);
            Udp.endPacket();
        }
        if (puara.IP2_ready()) {
            Udp.beginPacket(puara.IP2().c_str(), puara.PORT2());
            msgShake.send(Udp);
            Udp.endPacket();
        }
        msgShake.empty();

        msgYpr.add(imu_orientation.euler.azimuth);
        msgYpr.add(imu_orientation.euler.tilt);
        msgYpr.add(imu_orientation.euler.roll);
        if (puara.IP1_ready()) {
            Udp.beginPacket(puara.IP1().c_str(), puara.PORT1());
            msgYpr.send(Udp);
            Udp.endPacket();
        }
        if (puara.IP2_ready()) {
            Udp.beginPacket(puara.IP2().c_str(), puara.PORT2());
            msgYpr.send(Udp);
            Udp.endPacket();
        }
        msgYpr.empty();
    }

    if (M5.BtnA.wasChangePressed()) {
        if (M5.BtnA.wasPressed()) {
            msgButton.add(1);
        } else if (M5.BtnA.wasReleased()) {
            msgButton.add(0);
        }
        if (puara.IP1_ready()) {
            Udp.beginPacket(puara.IP1().c_str(), puara.PORT1());
            msgButton.send(Udp);
            Udp.endPacket();
        }
        if (puara.IP2_ready()) {
            Udp.beginPacket(puara.IP2().c_str(), puara.PORT2());
            msgButton.send(Udp);
            Udp.endPacket();
        }
        msgButton.empty();
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
        puara.set_persistentAP(true);
        puara.write_config_json();
        PuaraAPI::Device{}.reboot_with_delay();
    }

    // LCD info
    if (M5.BtnB.wasReleased()) {
        M5.Lcd.fillScreen(BLACK); lcd_on = true;
        M5.Lcd.setCursor(0,0);
        M5.Lcd.setTextSize(1);
        M5.Lcd.printf("%s\n \n", puara.dmi_name().c_str());
        M5.Lcd.setTextSize(3);
        M5.Lcd.printf("Bat:%u%%\n", M5.Power.getBatteryLevel());
        M5.Lcd.setTextSize(1);
        M5.Lcd.printf("charging: %d\n\n", M5.Power.isCharging());
        M5.Lcd.printf("Setup | IP: ");
        M5.Lcd.printf("%s\n", puara.staIP().c_str());
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
