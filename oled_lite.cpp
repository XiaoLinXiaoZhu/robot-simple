#include "oled_lite.h"

namespace OLED_Lite {
OLEDTYPE o;                             // 实际定义
struct oled_lite_config state(0, true); // 实际定义

void init() {
  if (!needInit)
    return;
  needInit = false; // 标记已初始化
  o.begin();        // 初始化屏幕
  o.setFont(FONT);  // 设置字体
  o.clear();        // 清除屏幕

  o.drawString(0, debugCount & 0x03, ">00 OLED Initialized");
}

void checkInited() {
  if (needInit) {
    init();
  }
}

// 在指定位置显示文本(char*类型)
void displayText(const char *text, int x, int y) {
  checkInited(); // 确保OLED已初始化
  o.drawString(x, y, text);
}

// 清除屏幕
void clear() {
  checkInited(); // 确保OLED已初始化
  o.clear();
}

void newLine() {
  checkInited(); // 确保OLED已初始化

  // 上一行清除掉开头的指示器 >
  o.drawString(0, debugCount & 0x03, " ");

  // 更新行号
  debugCount = (debugCount + 1) % (MaxDebugCount + 1);

  // 写入一个>指示器
  o.clearLine(debugCount & 0x03);
  o.setCursor(0, debugCount & 0x03); // 设置光标位置
  o.print(">");
  o.print(debugCount < 10 ? "0" : ""); // 前导零
  o.print(debugCount);
  o.print(" ");
}

void print(const char *text) {
  checkInited(); // 确保OLED已初始化

  // 写入文本
  for (uint8_t i = 0; i < 16 && text[i]; i++) {
    o.print(text[i]); // 写入字符
  }
}

void println(const char *text) {
  checkInited(); // 确保OLED已初始化

  // 写入，并且换行
  print(text);
  newLine(); // 换行
}

void print(const __FlashStringHelper *text) {
  checkInited(); // 确保OLED已初始化

  // 写入PROGMEM中的文本
  // 注意：U8x8lib已经支持__FlashStringHelper*类型，所以我们可以直接传递
  // 但为了保持与print(const char*)一致的16个字符限制，我们应该自己处理
  o.print(text); // 使用U8x8lib的内置支持
}

void println(const __FlashStringHelper *text) {
  checkInited(); // 确保OLED已初始化

  // 写入PROGMEM中的文本，并换行
  print(text);
  newLine(); // 换行
}

// 基本数据类型的print实现
void print(char value) {
  checkInited();
  o.print(value);
}

void print(unsigned char value) {
  checkInited();
  o.print(value);
}

void print(int value) {
  checkInited();
  o.print(value);
}

void print(unsigned int value) {
  checkInited();
  o.print(value);
}

void print(long value) {
  checkInited();
  o.print(value);
}

void print(unsigned long value) {
  checkInited();
  o.print(value);
}

void print(double value, int digits) {
  checkInited();
  char buffer[16];
  dtostrf(value, 7, digits, buffer); // 7个字符总宽度，digits个小数位
  o.print(buffer);
}

// 基本数据类型的println实现
void println(char value) {
  checkInited();
  print(value);
  newLine();
}

void println(unsigned char value) {
  checkInited();
  print(value);
  newLine();
}

void println(int value) {
  checkInited();
  print(value);
  newLine();
}

void println(unsigned int value) {
  checkInited();
  print(value);
  newLine();
}

void println(long value) {
  checkInited();
  print(value);
  newLine();
}

void println(unsigned long value) {
  checkInited();
  print(value);
  newLine();
}

void println(double value, int digits) {
  checkInited();
  print(value, digits);
  newLine();
}
} // namespace OLED_Lite