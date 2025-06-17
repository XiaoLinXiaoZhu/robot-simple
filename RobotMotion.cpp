#include "RobotMotion.h"
#include "RobotServoControl.h"
#include "RobotUS.h"
#include "RobotOLED.h"
#include "IDebug.h"

// 全局运动状态变量定义
RobotMotionId currentMotionId = RobotMotionId::Idle; // 当前动作ID
RobotMotionId nextMotionId = RobotMotionId::Idle;    // 下一个动作ID
RobotMotionState currentMotionState = RobotMotionState::NotStarted; // 当前动作状态
uint16_t sharedCounter = 0; // 共享的计数器

void setMovingState(RobotMotionId motionId)
{
  // 设置下一个动作ID
  nextMotionId = motionId;
  debugF("Setting motion to: ");
  debugln(static_cast<uint8_t>(motionId));
}

bool haveNextMotion()
{
  // 检查是否有下一个动作
  return !(nextMotionId == currentMotionId);
}

void SyncMovingState()
{
  // 如果下一个动作ID与当前动作ID不同，则更新当前动作ID
  if (nextMotionId != currentMotionId)
  {
    debugF("Scheduling next motion from ");
    debug(static_cast<uint8_t>(currentMotionId));
    debugF(" to ");
    debugln(static_cast<uint8_t>(nextMotionId));

    // 如果上一个动作已完成，则更新当前动作ID
    if (currentMotionState == RobotMotionState::Completed)
    {
      debuglnF("Current motion is completed, updating to next motion.");
      currentMotionId = nextMotionId;                    // 更新当前动作ID
      currentMotionState = RobotMotionState::NotStarted; // 重置状态
      return;
    }
  }
}

void UpdateMotion()
{
  switch (currentMotionId)
  {
  case RobotMotionId::Idle:
    handleMotionIdle();
    break;
  case RobotMotionId::Walking:
    handleMotionWalk();
    break;
  case RobotMotionId::AutoWalking:
    handleMotionAutoWalk();
    break;
  case RobotMotionId::TurningLeft:
    handleMotionTurnLeft();
    break;
  case RobotMotionId::TurningRight:
    handleMotionTurnRight();
    break;
  case RobotMotionId::Dancing:
    handleMotionDancing();
    break;
  case RobotMotionId::Singing:
    debuglnF("Robot is singing.");
    break;
  case RobotMotionId::DebugUS:
    handleMotionDebugUS();
    break;
  }
}

void handleMotionIdle()
{
  if (currentMotionState == RobotMotionState::NotStarted)
  {
    showFace("happy"); // 显示默认表情
    debuglnF("Robot is idle.");
    // 所有的脚都设置为90度
    for (int i = 0; i < 8; i++)
    {
      setServo(i, 90);
    }
    sharedCounter = 0;                                 // 重置共享计数器
    currentMotionState = RobotMotionState::InProgress; // 设置为进行中状态
  }
  else if (currentMotionState == RobotMotionState::InProgress)
  {
    // 如果已经处于进行中状态，可以添加其他逻辑
    debuglnF("Robot is still idle.");

    currentMotionState = RobotMotionState::Completed; // 设置为完成状态
  }
  else if (currentMotionState == RobotMotionState::Completed)
  {
    sharedCounter++; // 增加计数器
    if (sharedCounter == 30)
    {
      showFace("sleepy"); // 显示困倦表情
      debuglnF("Robot is now sleepy.");
    }
  }
}

void handleMotionWalk()
{
  // 定义动作幅度系数
  const uint8_t amplitude = 20;     // 髋关节运动幅度
  const uint8_t legLiftHeight = 10; // 腿抬起高度
  const uint8_t centerPos = 90;     // 中心位置

  if (currentMotionState == RobotMotionState::NotStarted)
  {
    debuglnF("Robot starts walking.");
    debugF("Walking with amplitude: ");
    debug(amplitude);
    debuglnF(" degrees");

    sharedCounter = 0;
    // 初始化所有舵机位置，准备行走
    for (int i = 0; i < 8; i++)
    {
      setServo(i, centerPos); // 所有舵机回到中心位置
    }
    currentMotionState = RobotMotionState::InProgress;
  }
  else if (currentMotionState == RobotMotionState::InProgress)
  {
    // 机器人行走循环
    // 使用sharedCounter来决定当前的行走阶段
    uint8_t walkPhase = sharedCounter % 8; // 将行走分为8个阶段

    debugF("Walking phase: ");
    debugln(walkPhase);

    switch (walkPhase)
    {
    case 0:                                                 // 准备抬起前右腿和后左腿
      showFace("thinking");                                 // 显示思考表情
      setServo(FRONT_RIGHT_LEG, centerPos - legLiftHeight); // 抬起前右腿
      setServo(BACK_LEFT_LEG, centerPos - legLiftHeight);   // 抬起后左腿
      break;
    case 1:                                             // 前右腿和后左腿向前迈步
      setServo(FRONT_RIGHT_HIP, centerPos + amplitude); // 前右髋关节向前
      setServo(BACK_LEFT_HIP, centerPos - amplitude);   // 后左髋关节向前
      break;
    case 2:                                 // 放下前右腿和后左腿
      setServo(FRONT_RIGHT_LEG, centerPos); // 放下前右腿
      setServo(BACK_LEFT_LEG, centerPos);   // 放下后左腿
      break;
    case 3:                  // 准备移动身体
      showFace("surprised"); // 显示惊讶表情
      // 稍作停顿，为下一步准备
      break;
    case 4:                                                // 准备抬起前左腿和后右腿
      setServo(FRONT_LEFT_LEG, centerPos - legLiftHeight); // 抬起前左腿
      setServo(BACK_RIGHT_LEG, centerPos - legLiftHeight); // 抬起后右腿
      break;
    case 5:                                            // 前左腿和后右腿向前迈步
      setServo(FRONT_LEFT_HIP, centerPos - amplitude); // 前左髋关节向前
      setServo(BACK_RIGHT_HIP, centerPos + amplitude); // 后右髋关节向前
      break;
    case 6:                                // 放下前左腿和后右腿
      setServo(FRONT_LEFT_LEG, centerPos); // 放下前左腿
      setServo(BACK_RIGHT_LEG, centerPos); // 放下后右腿
      break;
    case 7:                                 // 恢复所有髋关节到中心位置，准备下一个循环
      setServo(FRONT_RIGHT_HIP, centerPos); // 前右髋关节回中
      setServo(FRONT_LEFT_HIP, centerPos);  // 前左髋关节回中
      setServo(BACK_RIGHT_HIP, centerPos);  // 后右髋关节回中
      setServo(BACK_LEFT_HIP, centerPos);   // 后左髋关节回中
      break;
    }

    // 增加计数器，进入下一阶段
    sharedCounter += 1;

    // 如果需要停止行走，可以在这里检查某个条件，然后设置状态为Completed
    if (sharedCounter >= 64)
    { // 假设走64个阶段后停止
      debuglnF("Robot completed walking.");
      for (int i = 0; i < 8; i++)
      {
        setServo(i, 90);
      }
      currentMotionState = RobotMotionState::Completed; // 设置为完成状态
      nextMotionId = RobotMotionId::Idle;               // 完成后设置下一个动作为Idle
    }
    // 这里我们让机器人一直走下去，不主动停止
  }
  else if (currentMotionState == RobotMotionState::Completed)
  {
  }
}

void handleMotionAutoWalk()
{
  // 定义动作幅度系数
  const uint8_t amplitude = 20;     // 髋关节运动幅度
  const uint8_t legLiftHeight = 10; // 腿抬起高度
  const uint8_t centerPos = 90;     // 中心位置

  // 自动行走和 走路类似，不过会在循环的时候获取 us（超声波传感器）数据，
  // 并根据数据判断是否需要转向或停止。
  if (currentMotionState == RobotMotionState::NotStarted)
  {
    debuglnF("Robot starts auto walking.");
    debugF("Auto walking with amplitude: ");
    debug(amplitude);
    debuglnF(" degrees");

    // 显示表情
    showFace("happy"); // 显示高兴表情

    sharedCounter = 0;
    // 初始化所有舵机位置，准备行走
    for (int i = 0; i < 8; i++)
    {
      setServo(i, centerPos); // 所有舵机回到中心位置
    }
    currentMotionState = RobotMotionState::InProgress;
  }
  else if (currentMotionState == RobotMotionState::InProgress)
  {
    // 获取超声波传感器数据
    int distance = getUSDistance(); // 假设有一个函数获取距离

    debugF("US Distance: ");
    debugln(distance);

    if (distance < 400)
    { // 如果距离小于400mm，转向或停止
      // 根据 sharedCounter 来决定转向的具体动作
      if (sharedCounter % 2 == 0)
      {
        showFace("confused"); // 显示困惑表情
        debuglnF("Obstacle detected, turning left.");
        currentMotionState = RobotMotionState::NotStarted; // 重置状态，准备转向
        currentMotionId = RobotMotionId::TurningLeft;      // 设置为左转状态
        // 直接跳转到对应的状态
        // 设置下一个状态为回归 autoWalking
        nextMotionId = RobotMotionId::AutoWalking; // 转向后继续自动行走
      }
      else
      {
        showFace("angry"); // 显示生气表情
        debuglnF("Obstacle detected, turning right.");
        setMovingState(RobotMotionId::TurningRight);
        currentMotionState = RobotMotionState::NotStarted; // 重置状态，准备转向
        currentMotionId = RobotMotionId::TurningRight;     // 设置为右转状态
        nextMotionId = RobotMotionId::AutoWalking;         // 转向后继续自动行走
      }
      return;
    }

    // 机器人行走循环
    uint8_t walkPhase = sharedCounter % 8; // 将行走分为8个阶段

    debugF("Walking phase: ");
    debugln(walkPhase);

    switch (walkPhase)
    {
    case 0:                                                 // 准备抬起前右腿和后左腿
      showFace("thinking");                                 // 显示思考表情
      setServo(FRONT_RIGHT_LEG, centerPos - legLiftHeight); // 抬起前右腿
      setServo(BACK_LEFT_LEG, centerPos - legLiftHeight);   // 抬起后左腿
      break;
    case 1:                                             // 前右腿和后左腿向前迈步
      setServo(FRONT_RIGHT_HIP, centerPos + amplitude); // 前右髋关节向前
      setServo(BACK_LEFT_HIP, centerPos - amplitude);   // 后左髋关节向前
      break;
    case 2:                                 // 放下前右腿和后左腿
      setServo(FRONT_RIGHT_LEG, centerPos); // 放下前右腿
      setServo(BACK_LEFT_LEG, centerPos);   // 放下后左腿
      break;
    case 3:                  // 准备移动身体
      showFace("surprised"); // 显示惊讶表情
      // 稍作停顿，为下一步准备
      break;
    case 4:                                                // 准备抬起前左腿和后右腿
      setServo(FRONT_LEFT_LEG, centerPos - legLiftHeight); // 抬起前左腿
      setServo(BACK_RIGHT_LEG, centerPos - legLiftHeight); // 抬起后右腿
      break;
    case 5:                                            // 前左腿和后右腿向前迈步
      setServo(FRONT_LEFT_HIP, centerPos - amplitude); // 前左髋关节向前
      setServo(BACK_RIGHT_HIP, centerPos + amplitude); // 后右髋关节向前
      break;
    case 6:                                // 放下前左腿和后右腿
      setServo(FRONT_LEFT_LEG, centerPos); // 放下前左腿
      setServo(BACK_RIGHT_LEG, centerPos); // 放下后右腿
      break;
    case 7:                                 // 恢复所有髋关节到中心位置，准备下一个循环
      setServo(FRONT_RIGHT_HIP, centerPos); // 前右髋关节回中
      setServo(FRONT_LEFT_HIP, centerPos);  // 前左髋关节回中
      setServo(BACK_RIGHT_HIP, centerPos);  // 后右髋关节回中
      setServo(BACK_LEFT_HIP, centerPos);   // 后左髋关节回中
      break;
    }
  }
  else if (currentMotionState == RobotMotionState::Completed)
  {
    // 如果当前状态已完成，可能需要重置或进入下一个动作
    debuglnF("Robot completed auto walking.");
    for (int i = 0; i < 8; i++)
    {
      setServo(i, 90); // 所有舵机回到中心位置
    }
    setMovingState(RobotMotionId::Idle); // 设置下一个动作为Idle
  }
}

void handleMotionTurnLeft()
{
  // 定义动作幅度系数
  const uint8_t turnAmplitude = 20; // 转向幅度
  const uint8_t legLiftHeight = 10; // 腿抬起高度
  const uint8_t centerPos = 90;     // 中心位置

  if (currentMotionState == RobotMotionState::NotStarted)
  {
    debuglnF("Robot starts turning left.");
    debugF("Turning left with amplitude: ");
    debug(turnAmplitude);
    debuglnF(" degrees");

    sharedCounter = 0;
    // 初始化所有舵机位置，准备转弯
    for (int i = 0; i < 8; i++)
    {
      setServo(i, centerPos); // 所有舵机回到中心位置
    }
    currentMotionState = RobotMotionState::InProgress;
  }
  else if (currentMotionState == RobotMotionState::InProgress)
  {
    // 机器人左转循环
    uint8_t turnPhase = sharedCounter % 6; // 将左转分为6个阶段

    debugF("Left turning phase: ");
    debugln(turnPhase);

    switch (turnPhase)
    {
    case 0:                                                 // 准备抬起所有腿
      setServo(FRONT_RIGHT_LEG, centerPos - legLiftHeight); // 抬起前右腿
      setServo(FRONT_LEFT_LEG, centerPos - legLiftHeight);  // 抬起前左腿
      setServo(BACK_RIGHT_LEG, centerPos - legLiftHeight);  // 抬起后右腿
      setServo(BACK_LEFT_LEG, centerPos - legLiftHeight);   // 抬起后左腿
      break;
    case 1:                                                 // 所有髋关节向左转
      setServo(FRONT_RIGHT_HIP, centerPos - turnAmplitude); // 前右髋关节左转
      setServo(FRONT_LEFT_HIP, centerPos - turnAmplitude);  // 前左髋关节左转
      setServo(BACK_RIGHT_HIP, centerPos - turnAmplitude);  // 后右髋关节左转
      setServo(BACK_LEFT_HIP, centerPos - turnAmplitude);   // 后左髋关节左转
      break;
    case 2:                                 // 放下所有腿
      setServo(FRONT_RIGHT_LEG, centerPos); // 放下前右腿
      setServo(FRONT_LEFT_LEG, centerPos);  // 放下前左腿
      setServo(BACK_RIGHT_LEG, centerPos);  // 放下后右腿
      setServo(BACK_LEFT_LEG, centerPos);   // 放下后左腿
      break;
    case 3:                                                 // 再次抬起所有腿
      setServo(FRONT_RIGHT_LEG, centerPos - legLiftHeight); // 抬起前右腿
      setServo(FRONT_LEFT_LEG, centerPos - legLiftHeight);  // 抬起前左腿
      setServo(BACK_RIGHT_LEG, centerPos - legLiftHeight);  // 抬起后右腿
      setServo(BACK_LEFT_LEG, centerPos - legLiftHeight);   // 抬起后左腿
      break;
    case 4:                                 // 所有髋关节回到中心位置
      setServo(FRONT_RIGHT_HIP, centerPos); // 前右髋关节回中
      setServo(FRONT_LEFT_HIP, centerPos);  // 前左髋关节回中
      setServo(BACK_RIGHT_HIP, centerPos);  // 后右髋关节回中
      setServo(BACK_LEFT_HIP, centerPos);   // 后左髋关节回中
      break;
    case 5:                                 // 放下所有腿
      setServo(FRONT_RIGHT_LEG, centerPos); // 放下前右腿
      setServo(FRONT_LEFT_LEG, centerPos);  // 放下前左腿
      setServo(BACK_RIGHT_LEG, centerPos);  // 放下后右腿
      setServo(BACK_LEFT_LEG, centerPos);   // 放下后左腿

      // 完成一个完整的转向周期后，增加计数器
      sharedCounter += 1;

      // 如果已经完成了足够的转向周期，则标记为完成
      if (sharedCounter >= 12)
      { // 完成2个完整转向周期
        currentMotionState = RobotMotionState::Completed;
        debuglnF("Left turn completed.");
      }
      break;
    }

    // 如果没有到case 5，也增加计数器进入下一阶段
    if (turnPhase != 5)
    {
      sharedCounter += 1;
    }
  }
  else if (currentMotionState == RobotMotionState::Completed)
  {
    // 转弯完成后的处理
    debuglnF("Left turn state completed.");

    // 如果有下一个动作ID设置，将自动切换到该状态
    // 否则默认回到空闲状态
    if (nextMotionId == currentMotionId)
    {
      setMovingState(RobotMotionId::Idle);
    }
  }
}

void handleMotionTurnRight()
{
  // 定义动作幅度系数
  const uint8_t turnAmplitude = 20; // 转向幅度
  const uint8_t legLiftHeight = 10; // 腿抬起高度
  const uint8_t centerPos = 90;     // 中心位置

  if (currentMotionState == RobotMotionState::NotStarted)
  {
    debuglnF("Robot starts turning right.");
    debugF("Turning right with amplitude: ");
    debug(turnAmplitude);
    debuglnF(" degrees");

    sharedCounter = 0;
    // 初始化所有舵机位置，准备转弯
    for (int i = 0; i < 8; i++)
    {
      setServo(i, centerPos); // 所有舵机回到中心位置
    }
    currentMotionState = RobotMotionState::InProgress;
  }
  else if (currentMotionState == RobotMotionState::InProgress)
  {
    // 机器人右转循环
    uint8_t turnPhase = sharedCounter % 6; // 将右转分为6个阶段

    debugF("Right turning phase: ");
    debugln(turnPhase);

    switch (turnPhase)
    {
    case 0:                                                 // 准备抬起所有腿
      setServo(FRONT_RIGHT_LEG, centerPos - legLiftHeight); // 抬起前右腿
      setServo(FRONT_LEFT_LEG, centerPos - legLiftHeight);  // 抬起前左腿
      setServo(BACK_RIGHT_LEG, centerPos - legLiftHeight);  // 抬起后右腿
      setServo(BACK_LEFT_LEG, centerPos - legLiftHeight);   // 抬起后左腿
      break;
    case 1:                                                 // 所有髋关节向右转
      setServo(FRONT_RIGHT_HIP, centerPos + turnAmplitude); // 前右髋关节右转
      setServo(FRONT_LEFT_HIP, centerPos + turnAmplitude);  // 前左髋关节右转
      setServo(BACK_RIGHT_HIP, centerPos + turnAmplitude);  // 后右髋关节右转
      setServo(BACK_LEFT_HIP, centerPos + turnAmplitude);   // 后左髋关节右转
      break;
    case 2:                                 // 放下所有腿
      setServo(FRONT_RIGHT_LEG, centerPos); // 放下前右腿
      setServo(FRONT_LEFT_LEG, centerPos);  // 放下前左腿
      setServo(BACK_RIGHT_LEG, centerPos);  // 放下后右腿
      setServo(BACK_LEFT_LEG, centerPos);   // 放下后左腿
      break;
    case 3:                                                 // 再次抬起所有腿
      setServo(FRONT_RIGHT_LEG, centerPos - legLiftHeight); // 抬起前右腿
      setServo(FRONT_LEFT_LEG, centerPos - legLiftHeight);  // 抬起前左腿
      setServo(BACK_RIGHT_LEG, centerPos - legLiftHeight);  // 抬起后右腿
      setServo(BACK_LEFT_LEG, centerPos - legLiftHeight);   // 抬起后左腿
      break;
    case 4:                                 // 所有髋关节回到中心位置
      setServo(FRONT_RIGHT_HIP, centerPos); // 前右髋关节回中
      setServo(FRONT_LEFT_HIP, centerPos);  // 前左髋关节回中
      setServo(BACK_RIGHT_HIP, centerPos);  // 后右髋关节回中
      setServo(BACK_LEFT_HIP, centerPos);   // 后左髋关节回中
      break;
    case 5:                                 // 放下所有腿
      setServo(FRONT_RIGHT_LEG, centerPos); // 放下前右腿
      setServo(FRONT_LEFT_LEG, centerPos);  // 放下前左腿
      setServo(BACK_RIGHT_LEG, centerPos);  // 放下后右腿
      setServo(BACK_LEFT_LEG, centerPos);   // 放下后左腿

      // 完成一个完整的转向周期后，增加计数器
      sharedCounter += 1;

      // 如果已经完成了足够的转向周期，则标记为完成
      if (sharedCounter >= 12)
      { // 完成2个完整转向周期
        currentMotionState = RobotMotionState::Completed;
        debuglnF("Right turn completed.");
      }
      break;
    }

    // 如果没有到case 5，也增加计数器进入下一阶段
    if (turnPhase != 5)
    {
      sharedCounter += 1;
    }
  }
  else if (currentMotionState == RobotMotionState::Completed)
  {
    // 转弯完成后的处理
    debuglnF("Right turn state completed.");

    // 如果有下一个动作ID设置，将自动切换到该状态
    // 否则默认回到空闲状态
    if (nextMotionId == currentMotionId)
    {
      setMovingState(RobotMotionId::Idle);
    }
  }
}

void handleMotionDancing()
{
  // 定义动作幅度系数
  const uint8_t hipSwingAmplitude = 30; // 髋关节摆动幅度
  const uint8_t legLiftHeight = 20;     // 腿抬起高度
  const uint8_t centerPos = 90;         // 中心位置

  if (currentMotionState == RobotMotionState::NotStarted)
  {
    debuglnF("Robot starts dancing.");
    sharedCounter = 0;
    // 初始化所有舵机位置，准备跳舞
    for (int i = 0; i < 8; i++)
    {
      setServo(i, centerPos); // 所有舵机回到中心位置
    }
    currentMotionState = RobotMotionState::InProgress;
  }
  else if (currentMotionState == RobotMotionState::InProgress)
  {
    // 机器人跳舞循环
    uint8_t dancePhase = sharedCounter % 12; // 将舞蹈分为12个阶段

    debugF("Dancing phase: ");
    debugln(dancePhase);

    switch (dancePhase)
    {
    case 0:                // 准备姿势 - 稍微抬起所有腿
      showFace("excited"); // 显示兴奋表情
      setServo(FRONT_RIGHT_LEG, centerPos - legLiftHeight / 2);
      setServo(FRONT_LEFT_LEG, centerPos - legLiftHeight / 2);
      setServo(BACK_RIGHT_LEG, centerPos - legLiftHeight / 2);
      setServo(BACK_LEFT_LEG, centerPos - legLiftHeight / 2);
      break;
    case 1: // 前腿下压，后腿抬起
      setServo(FRONT_RIGHT_LEG, centerPos);
      setServo(FRONT_LEFT_LEG, centerPos);
      setServo(BACK_RIGHT_LEG, centerPos - legLiftHeight);
      setServo(BACK_LEFT_LEG, centerPos - legLiftHeight);
      break;
    case 2: // 髋关节左右摆动
      setServo(FRONT_RIGHT_HIP, centerPos + hipSwingAmplitude);
      setServo(FRONT_LEFT_HIP, centerPos - hipSwingAmplitude);
      setServo(BACK_RIGHT_HIP, centerPos + hipSwingAmplitude);
      setServo(BACK_LEFT_HIP, centerPos - hipSwingAmplitude);
      break;
    case 3: // 髋关节反向摆动
      setServo(FRONT_RIGHT_HIP, centerPos - hipSwingAmplitude);
      setServo(FRONT_LEFT_HIP, centerPos + hipSwingAmplitude);
      setServo(BACK_RIGHT_HIP, centerPos - hipSwingAmplitude);
      setServo(BACK_LEFT_HIP, centerPos + hipSwingAmplitude);
      break;
    case 4: // 前腿抬起，后腿下压
      setServo(FRONT_RIGHT_LEG, centerPos - legLiftHeight);
      setServo(FRONT_LEFT_LEG, centerPos - legLiftHeight);
      setServo(BACK_RIGHT_LEG, centerPos);
      setServo(BACK_LEFT_LEG, centerPos);
      break;
    case 5: // 对角线动作 - 前右和后左抬高
      setServo(FRONT_RIGHT_LEG, centerPos - legLiftHeight);
      setServo(BACK_LEFT_LEG, centerPos - legLiftHeight);
      setServo(FRONT_LEFT_LEG, centerPos);
      setServo(BACK_RIGHT_LEG, centerPos);
      break;
    case 6:             // 对角线动作 - 前左和后右抬高
      showFace("love"); // 显示爱心表情
      setServo(FRONT_LEFT_LEG, centerPos - legLiftHeight);
      setServo(BACK_RIGHT_LEG, centerPos - legLiftHeight);
      setServo(FRONT_RIGHT_LEG, centerPos);
      setServo(BACK_LEFT_LEG, centerPos);
      break;
    case 7: // 全身"抖动" - 所有髋关节左转
      setServo(FRONT_RIGHT_HIP, centerPos - hipSwingAmplitude / 2);
      setServo(FRONT_LEFT_HIP, centerPos - hipSwingAmplitude / 2);
      setServo(BACK_RIGHT_HIP, centerPos - hipSwingAmplitude / 2);
      setServo(BACK_LEFT_HIP, centerPos - hipSwingAmplitude / 2);
      break;
    case 8: // 全身"抖动" - 所有髋关节右转
      setServo(FRONT_RIGHT_HIP, centerPos + hipSwingAmplitude / 2);
      setServo(FRONT_LEFT_HIP, centerPos + hipSwingAmplitude / 2);
      setServo(BACK_RIGHT_HIP, centerPos + hipSwingAmplitude / 2);
      setServo(BACK_LEFT_HIP, centerPos + hipSwingAmplitude / 2);
      break;
    case 9: // 再次全身"抖动" - 所有髋关节左转
      setServo(FRONT_RIGHT_HIP, centerPos - hipSwingAmplitude / 2);
      setServo(FRONT_LEFT_HIP, centerPos - hipSwingAmplitude / 2);
      setServo(BACK_RIGHT_HIP, centerPos - hipSwingAmplitude / 2);
      setServo(BACK_LEFT_HIP, centerPos - hipSwingAmplitude / 2);
      break;
    case 10: // 结束动作 - 髋关节回中
      setServo(FRONT_RIGHT_HIP, centerPos);
      setServo(FRONT_LEFT_HIP, centerPos);
      setServo(BACK_RIGHT_HIP, centerPos);
      setServo(BACK_LEFT_HIP, centerPos);
      break;
    case 11: // 结束动作 - 腿部回中
      setServo(FRONT_RIGHT_LEG, centerPos);
      setServo(FRONT_LEFT_LEG, centerPos);
      setServo(BACK_RIGHT_LEG, centerPos);
      setServo(BACK_LEFT_LEG, centerPos);

      // 增加计数器，用于确定是否完成舞蹈
      sharedCounter += 1;

      // 如果完成了3个完整的舞蹈循环，则标记为完成
      if (sharedCounter >= 36)
      { // 3 * 12 = 36
        currentMotionState = RobotMotionState::Completed;
        debuglnF("Dancing completed.");
      }
      break;
    }

    // 如果不是最后一个阶段，增加计数器
    if (dancePhase != 11)
    {
      sharedCounter += 1;
    }
  }
  else if (currentMotionState == RobotMotionState::Completed)
  {
    // 舞蹈完成后回到空闲状态
    debuglnF("Dance completed, returning to idle.");

    // 确保所有舵机回到中心位置
    for (int i = 0; i < 8; i++)
    {
      setServo(i, 90);
    }

    // 如果没有设置下一个状态，则默认回到空闲状态
    if (nextMotionId == currentMotionId)
    {
      setMovingState(RobotMotionId::Idle);
    }
  }
}

void handleMotionDebugUS()
{
  if (currentMotionState == RobotMotionState::NotStarted)
  {
    debuglnF("Robot starts debugging US sensor.");
    currentMotionState = RobotMotionState::InProgress; // 设置为进行中状态
  }
  else if (currentMotionState == RobotMotionState::InProgress)
  {
    // 获取超声波传感器数据
    int distance = getUSDistance(); // 假设有一个函数获取距离
    debugF("US Distance: ");
    debugln(distance);

    // 不阻断新的动作
    if (haveNextMotion())
    {
      currentMotionState = RobotMotionState::Completed; // 设置为完成状态
    }
  }
  else if (currentMotionState == RobotMotionState::Completed)
  {
    debuglnF("Debugging US sensor completed.");
  }
}
