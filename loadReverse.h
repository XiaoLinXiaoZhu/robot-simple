#pragma once

#include <EEPROM.h>
#ifdef VSCODE
#include <cstdint>
#endif
#include <Arduino.h>
#include "IDebug.h"

namespace IRobot {

class ServoReverse {
private:
    static constexpr uint16_t EEPROM_MAGIC = 0xabcd;
    static constexpr uint8_t EEPROM_OFFSET = 2;
    // 用一个字节的每一位表示一个舵机的反转标志
    uint8_t reverse = 0;

public:
    ServoReverse() {
        load(); // 构造时自动加载
    }

    void load() {
        uint16_t magic = (EEPROM.read(0) << 8) | EEPROM.read(1);
        if (magic == EEPROM_MAGIC) {
            uint8_t val = EEPROM.read(EEPROM_OFFSET);
            reverse = val;
        } else {
            store(); // 默认值为0，直接存储
        }
    }

    void store() const {
        EEPROM.write(0, EEPROM_MAGIC >> 8);
        EEPROM.write(1, EEPROM_MAGIC & 0xFF);
        EEPROM.write(EEPROM_OFFSET, reverse);
    }

    void set(int index, bool isReverse) {
        if (index >= 0 && index < 8) {
            if (isReverse)
                reverse |= (1 << index);
            else
                reverse &= ~(1 << index);
        }
    }

    bool get(int index) const {
        if (index >= 0 && index < 8) {
            return (reverse & (1 << index)) != 0;
        }
        return false;
    }

    void print() const {
        Serial.println(F("Current Servo Reverse Flags:"));
        for (int i = 0; i < 8; i++) {
            debugF("Servo ");
            debug(i);
            debugF(": ");
            debugln(get(i) ? "Reversed" : "Normal");
        }
    }
};

} // namespace IRobot
