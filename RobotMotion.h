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

// 各个动作已经封装到MotionHandler子类中

// 全局运动状态变量声明
extern RobotMotionId currentMotionId;
extern RobotMotionId nextMotionId;
extern RobotMotionState currentMotionState;
extern uint16_t sharedCounter;

class MotionHandler {
public:
    RobotMotionId motionId; // 运动ID
    // 处理运动状态的虚函数
    virtual void handleMotion();
    virtual void handleNotStarted();
    virtual void handleInProgress();
    virtual void handleCompleted();
};

extern MotionHandler *motionHandlers[];

#endif // ROBOT_MOTION_H
