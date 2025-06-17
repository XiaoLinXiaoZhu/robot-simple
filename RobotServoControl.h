#ifndef ROBOT_SERVO_CONTROL_H
#define ROBOT_SERVO_CONTROL_H

#include <Arduino.h>
#include <Servo.h>
#include "RobotDefines.h"
#include "loadReverse.h"
#include "loadTrim.h"

// 初始化舵机
void initServos();

// 设置舵机角度函数
// 对于 hip（髋关节）和 leg（腿部），
// leg + 代表 放下腿， leg - 代表 抬起腿
// hip + 代表 逆时针旋转，hip - 代表 顺时针旋转
void setServo(int id, int target);

// 旧版本的设置舵机函数，保留用于兼容性
void _setServo(int id, int target);

#endif // ROBOT_SERVO_CONTROL_H
