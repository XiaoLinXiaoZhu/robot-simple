#pragma once


#ifndef OLED_LITE_H
#define OLED_LITE_H

#ifdef VSCODE
#include <cstdint>
#endif

#include <U8x8lib.h>
namespace OLED_Lite {
#define OLEDTYPE U8X8_SH1106_128X32_VISIONOX_HW_I2C
#define FONT u8x8_font_5x7_f
#define MaxDebugCount 99
#define debugCount state._debugCount
#define needInit state._needInit

    extern OLEDTYPE o; // 声明而非定义
    extern struct oled_lite_config
    {
        uint8_t _debugCount :7; // 当前行号(0-99)
        bool _needInit :1;
        oled_lite_config(uint8_t debugCount_ = 0, bool needInit_ = true) // 默认需要初始化
            : _debugCount(debugCount_), _needInit(needInit_){
        }
    } state;

    void init();
    void checkInited();
    void displayText(const char *text, int x, int y);
    void clear();    
    void newLine();
    void print(const char *text);
    void print(const __FlashStringHelper *text);
    void print(char value);
    void print(unsigned char value);
    void print(int value);
    void print(unsigned int value);
    void print(long value);
    void print(unsigned long value);
    void print(double value, int digits = 2);
    
    void println(const char *text);
    void println(const __FlashStringHelper *text);
    void println(char value);
    void println(unsigned char value);
    void println(int value);
    void println(unsigned int value);
    void println(long value);
    void println(unsigned long value);
    void println(double value, int digits = 2);
} // namespace OLED_Lite

#endif