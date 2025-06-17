#ifndef ROBOT_MOTION_H
#define ROBOT_MOTION_H

#include <Arduino.h>
#include "RobotDefines.h"

// 设置下一个动作ID
void setMovingState(RobotMotionId motionId);

// 检查是否有下一个动作，用于配置非阻断式动作
bool haveNextMotion();

// 同步动作状态
void SyncMovingState();

// 更新动作
void UpdateMotion();

// 各种动作处理函数
void handleMotionIdle();
void handleMotionWalk();
void handleMotionAutoWalk();
void handleMotionTurnLeft();
void handleMotionTurnRight();
void handleMotionDancing();
void handleMotionDebugUS();

// 全局运动状态变量声明
extern RobotMotionId currentMotionId;
extern RobotMotionId nextMotionId;
extern RobotMotionState currentMotionState;
extern uint16_t sharedCounter;

#endif // ROBOT_MOTION_H
