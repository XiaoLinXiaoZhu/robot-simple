#pragma once
/*
从 EEPROM中读取和写入快速加载标志的代码，从而实现辨别是否可以快速加载的功能。

当机器开始工作时，禁用快速加载，使得重启时能够提供覆盖程序的窗口。
*/

#include <EEPROM.h>
#define EEPROM_FASTLOAD_MAGIC 0xD2  // EEPROM地址(用于设置是否快速启动)
#define EEPROM_FASTLOAD_OFFSET 6   // EEPROM偏移量(用于设置是否快速启动)

inline bool getEEPROMFastLoad() {
  // 获取EEPROM快速加载标志
  return EEPROM.read(EEPROM_FASTLOAD_OFFSET) == EEPROM_FASTLOAD_MAGIC;
}
inline void setEEPROMFastLoad(bool enable) {
  // 设置EEPROM快速加载标志
  EEPROM.write(EEPROM_FASTLOAD_OFFSET, enable ? EEPROM_FASTLOAD_MAGIC : 0);
}