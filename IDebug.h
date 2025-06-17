#pragma once


#ifdef VSCODE
// #include <HardwareSerial0.cpp>
#include "WString.h"
struct FakeSerial {
    void begin(int) {}
    template<typename T> void print(T) {}
    void print(const char*) {}
    template<typename T> void println(T) {}
    void println(const char*) {}
    void print(const __FlashStringHelper*) {}
    void println(const __FlashStringHelper*) {}
} Serial;
#endif

#include <Arduino.h>

// 调试输出方式选择：
//   启用其中一个，或者都不启用以关闭调试输出
#define DEBUGOUTPUT_SERIAL
// #define DEBUGOUTPUT_OLED

#ifdef DEBUGOUTPUT_SERIAL
    // 分成两个宏，一个用于字面量字符串，一个用于变量
    #define debug(message) Serial.print(message)  // 用于字面量字符串
    #define debugln(message) Serial.println(message)  // 用于变量字符串

    #define debuglnF(message) Serial.println(F(message))  // 用于字面量字符串
    #define debugF(message) Serial.print(F(message))       // 用于变量字符串
#elif defined(DEBUGOUTPUT_OLED)
    #include "oled_lite.h"
    // 提供和 SERIAL 一样的接口
    #define debug(message)         do { Serial.print(message); OLED_Lite::print(message); } while(0)
    #define debugln(message)       do { Serial.println(message); OLED_Lite::println(message); } while(0)
    #define debugF(message)        do { Serial.print(F(message)); OLED_Lite::print(F(message)); } while(0)
    #define debuglnF(message)      do { Serial.println(F(message)); OLED_Lite::println(F(message)); } while(0)
#else
    #define debug(message)         do {} while (0)
    #define debugln(message)       do {} while (0)
    #define debugF(message)        do {} while (0)
    #define debuglnF(message)      do {} while (0)
#endif