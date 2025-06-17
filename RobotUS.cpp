#include "RobotUS.h"
#include "IDebug.h"

// 创建超声波传感器对象
US usSensor;

void setupUS()
{
  // 初始化超声波传感器
  usSensor.init(PIN_Trigger, PIN_Echo);
  debuglnF("US Sensor initialized.");
}

int getUSDistance()
{
  // 获取超声波传感器的距离
  float distance = usSensor.read();                   // 读取距离，单位为毫米
  int distanceInt = static_cast<int>(distance); // 转换为整数毫米
  debugF("US Distance: ");
  debug(distance);
  debuglnF(" mm");
  return static_cast<int>(distanceInt); // 返回整数距离
}
