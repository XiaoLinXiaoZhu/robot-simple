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

// 在OLED上写入一行文本
void writeLine(const char *text) {
  checkInited(); // 确保OLED已初始化

  // 上一行清除掉开头的指示器 >
  o.drawString(0, debugCount & 0x03, " ");
  debugCount = (debugCount + 1) % (MaxDebugCount + 1);

  // 清除当前行 + 写入新内容
  o.clearLine(debugCount & 0x03);
  o.setCursor(0, debugCount & 0x03); // 设置光标位置
  o.print(">");
  o.print(debugCount < 10 ? "0" : ""); // 前导零
  o.print(debugCount);
  o.print(" ");

  // 只打印前12字符
  for (uint8_t i = 0; i < 12 && text[i]; i++) {
    o.write(text[i]);
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
} // namespace OLED_Lite