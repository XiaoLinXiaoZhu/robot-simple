#include "IDebug.h"
#include "SmartLoad.h"
#include "loadReverse.h"
#include "loadTrim.h"
#include "RobotDefines.h"
#include "RobotServoControl.h"
#include "RobotUS.h"
#include "RobotMotion.h"
#include "RobotCommands.h"
#include "RobotOLED.h"

#ifdef VSCODE
#include <cstdint>
#endif

//-=================== loader ========================
IRobot::ServoTrim trimLoader;
IRobot::ServoReverse reverseLoader;

//-=================== 主程序 ========================

void setup()
{
  Serial.begin(9600); // 初始化串口通信
  delay(100);
  debuglnF("Robot Simple Setup Start...");

  // 初始化 us传感器
  setupUS();
  debuglnF("US Sensor setup complete.");

  // 设置舵机修剪
  trimLoader.load();  // 加载修剪值
  trimLoader.print(); // 再次打印以确认加载成功
  trimLoader.store(); // 保存修剪值到EEPROM

  // 设置舵机反向
  reverseLoader.load();  // 加载反向值
  reverseLoader.print(); // 打印当前反向值
  reverseLoader.store(); // 保存反向值到EEPROM

  if (getEEPROMFastLoad())
  {
    Serial.println(F("Fast Reload"));
  }
  else
  {
    Serial.println(F("Slow Load"));
    delay(4000);
  }
  setEEPROMFastLoad(true); // 设置快速加载标志

  showFace("happy"); // 显示默认表情
}

void loop()
{
  setEEPROMFastLoad(false); // 禁用快速加载，确保重启时可以覆盖程序
  handleCommands();         // 处理串口命令

  SyncMovingState(); // 同步运动状态
  UpdateMotion();    // 更新运动状态
}



