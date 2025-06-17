#ifndef ROBOT_OLED_H
#define ROBOT_OLED_H

#include <Arduino.h>
#include "oled_lite.h"

// OLED显示的字符长度
#define OLEDLENGTH 16

// 计算文本居中位置的函数声明
int centerX(const char* text);

// 显示表情函数声明
void showFace(const char* face);

#endif // ROBOT_OLED_H
