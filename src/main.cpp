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
#include <puara/utils.h>

Puara puara;
puara_gestures::Jab3D jab;
puara_gestures::Shake3D shake;
//disable_orientation IMU_Orientation imu_orientation;
int m5button = 0;
puara_gestures::Button button(&m5button);

struct Discretizers {
    struct Jab {
        puara_gestures::utils::Discretizer<double> x;
        puara_gestures::utils::Discretizer<double> y;
        puara_gestures::utils::Discretizer<double> z;
    } jab;
    struct Shake {
        puara_gestures::utils::Discretizer<double> x;
        puara_gestures::utils::Discretizer<double> y;
        puara_gestures::utils::Discretizer<double> z;
    } shake;
    puara_gestures::utils::Discretizer<unsigned int> count;
    puara_gestures::utils::Discretizer<unsigned int> tap;
    puara_gestures::utils::Discretizer<unsigned int> dtap;
    puara_gestures::utils::Discretizer<unsigned int> ttap;
    puara_gestures::utils::Discretizer<unsigned int> time;
} discretizer;

byte led_pin = 10;
bool lcd_on = false;

#include <WiFiUdp.h>
#include <OSCBundle.h>

WiFiUDP Udp;
OSCBundle oscBundle;

struct OSCNamespace {
    OSCNamespace() {}

    OSCNamespace(const std::string& prefix) :
        accl(prefix + "/accl"),
        gyro(prefix + "/gyro"),
        jab(prefix + "/jab"),
        shake(prefix + "/shake"),
        ypr(prefix + "/ypr"),
        button(prefix + "/button"),
        count(prefix + "/count"),
        tap(prefix + "/tap"),
        dtap(prefix + "/dtap"),
        ttap(prefix + "/ttap"),
        time(prefix + "/time")
    {}

    std::string accl;
    std::string gyro;
    std::string jab;
    std::string shake;
    std::string ypr;
    std::string button;
    std::string count;
    std::string tap;
    std::string dtap;
    std::string ttap;
    std::string time;
} OSCnamespace;


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

    std::cout << "\n"
    << "AMIwrist module - Puara framework\n"
    << "Edu Meneses\n"
    << "IDMIL - CIRMMT - McGill University\n"
    << "Societe des Arts Tecnologiques (SAT)\n"
    << "module ID: " << puara.dmi_name() << "\n"
    << "Version " << puara.version() << "\n"
    << "\n"
    << "Booting System...\n\n"
    << std::endl;

    new (&OSCnamespace) OSCNamespace(("/" + puara.dmi_name()).c_str());

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


//////////
// LOOP //
//////////

void loop() {

    M5.update();
    auto imu_update = M5.Imu.update();
    button.update();

    if (imu_update){
        auto data = M5.Imu.getImuData();
        M5.Power.getBatteryLevel();

        jab.update(data.accel.x, data.accel.y, data.accel.z);
        shake.update(data.accel.x, data.accel.y, data.accel.z);

        //disable_orientation imu_orientation.setAccelerometerValues(data.accel.x, data.accel.y, data.accel.z);
        //disable_orientation imu_orientation.setGyroscopeDegreeValues(data.gyro.x, data.gyro.y, data.gyro.z, 10.);
        //disable_orientation imu_orientation.update();

        oscBundle.add(OSCnamespace.accl.c_str())
            .add(data.accel.x)
            .add(data.accel.y)
            .add(data.accel.z);
        oscBundle.add(OSCnamespace.gyro.c_str())
            .add(data.gyro.x)
            .add(data.gyro.y)
            .add(data.gyro.z);
        if (discretizer.jab.x.isNew(jab.x.current_value()) ||
            discretizer.jab.x.isNew(jab.x.current_value()) ||
            discretizer.jab.x.isNew(jab.x.current_value())) {
                oscBundle.add(OSCnamespace.jab.c_str())
                         .add(jab.x.current_value())
                         .add(jab.y.current_value())
                         .add(jab.z.current_value());
        }
        if (discretizer.shake.x.isNew(shake.x.current_value()) ||
            discretizer.shake.x.isNew(shake.x.current_value()) ||
            discretizer.shake.x.isNew(shake.x.current_value())) {
                oscBundle.add(OSCnamespace.jab.c_str())
                         .add(shake.x.current_value())
                         .add(shake.y.current_value())
                         .add(shake.z.current_value());
        }
        
        //disable_orientation oscBundle.add(OSCnamespace.ypr.c_str())
        //disable_orientation     .add(imu_orientation.euler.azimuth)
        //disable_orientation     .add(imu_orientation.euler.tilt)
        //disable_orientation     .add(imu_orientation.euler.roll);
    }

    if (M5.BtnA.wasChangePressed()) {
        if (M5.BtnA.wasPressed()) {
            m5button = 1;
            oscBundle.add(OSCnamespace.button.c_str()).add(1);
        } else if (M5.BtnA.wasReleased()) {
            m5button = 0;
            oscBundle.add(OSCnamespace.button.c_str()).add(0);
        } 
    }

    if (discretizer.count.isNew(button.count)) {
        oscBundle.add(OSCnamespace.count.c_str()).add(button.count);
    }
    if (discretizer.tap.isNew(button.tap)) {
        oscBundle.add(OSCnamespace.tap.c_str()).add(button.tap);
    }
    if (discretizer.dtap.isNew(button.doubleTap)) {
        oscBundle.add(OSCnamespace.dtap.c_str()).add(button.doubleTap);
    }
    if (discretizer.ttap.isNew(button.tripleTap)) {
        oscBundle.add(OSCnamespace.ttap.c_str()).add(button.tripleTap);
    }
    if (discretizer.time.isNew(button.pressTime)) {
        oscBundle.add(OSCnamespace.time.c_str()).add(button.pressTime);

    if (puara.IP1_ready()) {
        Udp.beginPacket(puara.IP1().c_str(), puara.PORT1());
        oscBundle.send(Udp);
        Udp.endPacket();
    }
    if (puara.IP2_ready()) {
        Udp.beginPacket(puara.IP2().c_str(), puara.PORT2());
        oscBundle.send(Udp);
        Udp.endPacket();
    }
    oscBundle.empty();

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
