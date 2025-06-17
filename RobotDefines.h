#ifndef ROBOT_DEFINES_H
#define ROBOT_DEFINES_H

#include <Arduino.h>

// 舵机引脚常量定义
#define FRONT_RIGHT_HIP 0
#define FRONT_LEFT_HIP 1
#define FRONT_RIGHT_LEG 2
#define FRONT_LEFT_LEG 3
#define BACK_RIGHT_HIP 4
#define BACK_LEFT_HIP 5
#define BACK_RIGHT_LEG 6
#define BACK_LEFT_LEG 7

#define PIN_Trigger 12
#define PIN_Echo 11

// 机器人动作ID枚举
enum class RobotMotionId : uint8_t
{
  Idle,
  Walking,
  AutoWalking, // 自动行走
  TurningLeft,
  TurningRight,
  Dancing,
  Singing,
  DebugUS
};

// 机器人动作状态枚举
enum class RobotMotionState : uint8_t
{
  NotStarted,
  InProgress,
  Completed
};

#endif // ROBOT_DEFINES_H
