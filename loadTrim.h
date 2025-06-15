#pragma once

#include <EEPROM.h>
#ifdef VSCODE
#include <cstdint>
#endif
#include <Arduino.h>
#include "IDebug.h"

namespace IRobot {

class ServoTrim {
private:
    static constexpr uint16_t EEPROM_MAGIC = 0xabcd;
    static constexpr uint8_t EEPROM_OFFSET = 2;
    int8_t trim[8] = {0}; // 每个舵机的修剪值

public:
    ServoTrim() {
        load(); // 构造时自动加载
    }

    void load() {
        uint16_t magic = (EEPROM.read(0) << 8) | EEPROM.read(1);
        if (magic == EEPROM_MAGIC) {
            for (int i = 0; i < 8; i++) {
                int16_t val = (EEPROM.read(i * 2 + EEPROM_OFFSET) << 8) |
                              EEPROM.read(i * 2 + EEPROM_OFFSET + 1);
                if (val >= -90 && val <= 90) {
                    trim[i] = static_cast<int8_t>(val);
                } else {
                    // 如果读取的值不在有效范围内，保持默认值0
                    trim[i] = 0;
                    debugF("Invalid trim value for servo ");
                    debug(i);
                    debugF(", resetting to default (0).");
                }
            }
        } else {
            // 第一次运行，写入魔术数并保存默认值
            store(); // 默认值为0，直接存储
        }
    }

    void store() const {
        EEPROM.write(0, EEPROM_MAGIC >> 8);
        EEPROM.write(1, EEPROM_MAGIC & 0xFF);

        for (int i = 0; i < 8; i++) {
            EEPROM.write(i * 2 + EEPROM_OFFSET, trim[i] >> 8);
            EEPROM.write(i * 2 + EEPROM_OFFSET + 1, trim[i] & 0xFF);
        }
    }

    void set(int index, int value) {
        if (index >= 0 && index < 8 && value >= -90 && value <= 90) {
            trim[index] = static_cast<uint8_t>(value);
        }
    }

    int get(int index) const {
        return static_cast<int>(trim[index]);
    }

    void print() const {
        Serial.println(F("Current Servo Trims:"));
        for (int i = 0; i < 8; i++) {
            debugF("Servo ");
            debug(i);
            debugF(": ");
            debugln(get(i));
        }
    }
};

} // namespace IRobot