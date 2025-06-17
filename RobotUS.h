#ifndef ROBOT_US_H
#define ROBOT_US_H

#include <Arduino.h>
#include "RobotDefines.h"
#include "US.h"

// 初始化超声波传感器
void setupUS();

// 获取超声波传感器测量的距离
int getUSDistance();

#endif // ROBOT_US_H
