#include "IDebug.h"
#include "SmartLoad.h"
#include "US.h"
#include "loadReverse.h"
#include "loadTrim.h"
#include <Servo.h>

#ifdef VSCODE
#include <cstdint>
#endif

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

// #include <cstdint>

// 前向声明
enum class RobotMotionId : uint8_t;
enum class RobotMotionState : uint8_t;

//-=================== loader ========================
IRobot::ServoTrim trimLoader;
IRobot::ServoReverse reverseLoader;

//-=================== 超声波传感器 ========================
US usSensor; // 创建超声波传感器对象

void setupUS() {
  // 初始化超声波传感器
  usSensor.init(PIN_Trigger, PIN_Echo);
  debuglnF("US Sensor initialized.");
}

int getUSDistance() {
  // 获取超声波传感器的距离
  float distance = usSensor.read();                   // 读取距离
  int distanceInt = static_cast<int>(distance * 100); // 转换为整数厘米
  debugF("US Distance: ");
  debug(distanceInt);
  debuglnF(" cm");
  return static_cast<int>(distanceInt); // 返回整数距离
}

//-=================== 状态机 ========================
// 定义完整的枚举类型
enum class RobotMotionId : uint8_t {
  Idle,
  Walking,
  AutoWalking, // 自动行走
  TurningLeft,
  TurningRight,
  Dancing,
  Singing
};
enum class RobotMotionState : uint8_t { NotStarted, InProgress, Completed };

RobotMotionId currentMotionId = RobotMotionId::Idle; // 当前动作ID
RobotMotionId nextMotionId = RobotMotionId::Idle;    // 下一个动作ID
RobotMotionState currentMotionState =
    RobotMotionState::NotStarted; // 当前动作状态

void setMovingState(RobotMotionId motionId) {
  // 设置下一个动作ID
  nextMotionId = motionId;
  debugF("Setting motion to: ");
  debugln(static_cast<uint8_t>(motionId));
}

bool haveNextMotion() {
  // 检查是否有下一个动作
  return !(nextMotionId == currentMotionId);
}

void SyncMovingState() {
  // 如果下一个动作ID与当前动作ID不同，则更新当前动作ID
  if (nextMotionId != currentMotionId) {
    debugF("Scheduling next motion from ");
    debug(static_cast<uint8_t>(currentMotionId));
    debugF(" to ");
    debugln(static_cast<uint8_t>(nextMotionId));

    // 先进入上一个的 Completed 状态
    // 现在改为只有 当前状态手动进入 Completed 状态 之后才能切换。
    // if (currentMotionState == RobotMotionState::InProgress) {
    //   debuglnF("Current motion is in progress, completing it.");
    //   currentMotionState = RobotMotionState::Completed; // 完成当前动作
    //   return;
    // }

    // 如果上一个动作已完成，则更新当前动作ID
    if (currentMotionState == RobotMotionState::Completed) {
      debuglnF("Current motion is completed, updating to next motion.");
      currentMotionId = nextMotionId;                    // 更新当前动作ID
      currentMotionState = RobotMotionState::NotStarted; // 重置状态
      return;
    }
  }
}

//-=================== 主程序 ========================

void setup() {
  Serial.begin(9600); // 初始化串口通信
  delay(100);
  debugF("Robot Simple Setup Start...");

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

  if (getEEPROMFastLoad()) {
    Serial.println(F("Fast Reload"));
  } else {
    Serial.println(F("Slow Load"));
    delay(4000);
  }
  setEEPROMFastLoad(true); // 设置快速加载标志
}

void loop() {
  setEEPROMFastLoad(false); // 禁用快速加载，确保重启时可以覆盖程序
  handleCommands();         // 处理串口命令

  SyncMovingState(); // 同步运动状态
  UpdateMotion();    // 更新运动状态
}

//-=================== 处理串口命令 ========================
const char endChar = '\r'; // 定义命令结束符
void handleCommands() {
  static char buffer[32]; // 命令缓冲区
  static uint8_t bufIndex = 0;

  while (Serial.available() > 0) {
    char inChar = Serial.read();

    // 如果接收到了结束符或缓冲区即将溢出，则处理命令
    if (inChar == endChar || bufIndex >= sizeof(buffer) - 1) {
      buffer[bufIndex] = '\0'; // 字符串结束符

      const char cmd = buffer[0]; // 获取命令字符
      debugF("Received command: ");
      debugln(cmd);

      char *token = buffer + 1; // 跳过命令字符
      while (*token == ' ')
        token++; // 跳过可能的多余空格

      switch (cmd) {
      case 'C':
        handleCommand_C(token);
        break;
      case 'V':
        handleCommand_RV(token); // 处理反转命令
        break;
      case 'T':
        debuglnF("Command T received. Move servo to test position.");
        handleCommand_T(token); // 处理测试命令
        break;
      case 'M':
        handleCommand_M(token);
        break;
      case 'A':
        handleCommand_A(token);
        break;
      case 'L':
        handleCommand_TL(token);
        break;
      case 'R':
        handleCommand_TR(token);
        break;
      case 'S':
        debuglnF("Stop command received.");
        setMovingState(RobotMotionId::Idle);
        break;
      default:
        debugF("Unknown command: ");
        debugln(buffer);
        break;
      }

      // 重置缓冲区索引
      bufIndex = 0;
      return;
    } else if (inChar != '\r') { // 忽略回车符
      // 将字符添加到缓冲区
      buffer[bufIndex++] = inChar;
    }
  }
}

void handleCommand_C(char *token) {
  int index = -1, value = 0;

  // 解析舵机索引
  index = atoi(token);

  // 查找下一个空格
  while (*token && *token != ' ')
    token++;

  // 如果找到空格，表示后面有值
  if (*token == ' ') {
    // 跳过空格
    while (*token == ' ')
      token++;

    // 解析值
    value = atoi(token);

    // 设置修剪值
    debugF("Setting trim for servo ");
    debug(index);
    debugF(" to value ");
    debug(value);
    debuglnF(".");
    trimLoader.set(index, value);
    trimLoader.store();
    trimLoader.print(); // 打印当前修剪值

    // 重新执行当前动作
    currentMotionState = RobotMotionState::NotStarted; // 重置状态
  } else {
    debuglnF("Invalid command format.");
  }
}

void handleCommand_RV(char *token) {
  int index = -1, value = 0;

  // 解析舵机索引
  index = atoi(token);

  // 查找下一个空格
  while (*token && *token != ' ')
    token++;

  // 如果找到空格，表示后面有值
  if (*token == ' ') {
    // 跳过空格
    while (*token == ' ')
      token++;

    // 解析值
    value = atoi(token);

    // 只允许0或1，转换为bool
    bool reverse = (value != 0);

    // 设置反向值
    debugF("Setting reverse for servo ");
    debug(index);
    debugF(" to value ");
    debug(reverse ? 1 : 0);
    debuglnF(".");
    reverseLoader.set(index, reverse);
    reverseLoader.store();
    reverseLoader.print(); // 打印当前反向值

    // 重新执行当前动作
    currentMotionState = RobotMotionState::NotStarted; // 重置状态
  } else {
    debuglnF("Invalid command format.");
  }
}

void handleCommand_T(char *token) {
  // 测试命令，直接将所有舵机设置为测试位置
  debuglnF("Running command T - Test Servo Positions.");

  // 解析舵机索引
  int index = atoi(token);

  if (index < 0 || index >= 8) {
    debuglnF("Invalid servo index.");
    return;
  }

  int testAngle = 90; // 测试角度

  // 查找下一个空格
  while (*token && *token != ' ')
    token++;

  if (*token == ' ') {
    // 跳过空格
    while (*token == ' ')
      token++;

    // 解析值
    testAngle = atoi(token);
  } else {
    debugF("No angle specified, using default: ");
    debugln(testAngle);
  }

  // 设置指定舵机到测试位置
  setServo(index, testAngle);
  currentMotionState = RobotMotionState::Completed; // 结束状态
}

void handleCommand_M(char *token) {
  // 开始运行
  debuglnF("Running command M.Moving forward.");
  setMovingState(RobotMotionId::Walking); // 设置为行走状态
  // 这里可以添加具体的运行逻辑
}

void handleCommand_A(char *token) {
  debuglnF("Get Command Auto Mode.");
  // 自动模式命令，设置下一个动作为自动行走
  setMovingState(RobotMotionId::AutoWalking);
}

void handleCommand_TL(char *token) {
  // 开始左转
  debuglnF("Running command L - Turn Left.");
  setMovingState(RobotMotionId::TurningLeft); // 设置为左转状态
}

void handleCommand_TR(char *token) {
  // 开始右转
  debuglnF("Running command R - Turn Right.");
  setMovingState(RobotMotionId::TurningRight); // 设置为右转状态
}

//-===================== 执行动作 =====================-
uint16_t sharedCounter = 0; // 共享的计数器

void UpdateMotion() {
  switch (currentMotionId) {
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
    debuglnF("Robot is dancing.");
    break;
  case RobotMotionId::Singing:
    debuglnF("Robot is singing.");
    break;
  }
}

void handleMotionIdle() {
  if (currentMotionState == RobotMotionState::NotStarted) {
    debuglnF("Robot is idle.");
    // 所有的脚都设置为90度
    for (int i = 0; i < 8; i++) {
      setServo(i, 90);
    }
    currentMotionState = RobotMotionState::InProgress; // 设置为进行中状态
  } else if (currentMotionState == RobotMotionState::InProgress) {
    // 如果已经处于进行中状态，可以添加其他逻辑
    debuglnF("Robot is still idle.");
    currentMotionState = RobotMotionState::Completed; // 设置为完成状态
  } else if (currentMotionState == RobotMotionState::Completed) {
  }
}

void handleMotionWalk() {
  // 定义动作幅度系数
  const uint8_t amplitude = 20;     // 髋关节运动幅度
  const uint8_t legLiftHeight = 10; // 腿抬起高度
  const uint8_t centerPos = 90;     // 中心位置

  if (currentMotionState == RobotMotionState::NotStarted) {
    debuglnF("Robot starts walking.");
    debugF("Walking with amplitude: ");
    debug(amplitude);
    debuglnF(" degrees");

    sharedCounter = 0;
    // 初始化所有舵机位置，准备行走
    for (int i = 0; i < 8; i++) {
      setServo(i, centerPos); // 所有舵机回到中心位置
    }
    currentMotionState = RobotMotionState::InProgress;
  } else if (currentMotionState == RobotMotionState::InProgress) {
    // 机器人行走循环
    // 使用sharedCounter来决定当前的行走阶段
    uint8_t walkPhase = sharedCounter % 8; // 将行走分为8个阶段

    debugF("Walking phase: ");
    debugln(walkPhase);

    switch (walkPhase) {
    case 0: // 准备抬起前右腿和后左腿
      setServo(FRONT_RIGHT_LEG, centerPos - legLiftHeight); // 抬起前右腿
      setServo(BACK_LEFT_LEG, centerPos - legLiftHeight);   // 抬起后左腿
      break;
    case 1: // 前右腿和后左腿向前迈步
      setServo(FRONT_RIGHT_HIP, centerPos + amplitude); // 前右髋关节向前
      setServo(BACK_LEFT_HIP, centerPos - amplitude);   // 后左髋关节向前
      break;
    case 2:                                 // 放下前右腿和后左腿
      setServo(FRONT_RIGHT_LEG, centerPos); // 放下前右腿
      setServo(BACK_LEFT_LEG, centerPos);   // 放下后左腿
      break;
    case 3: // 准备移动身体
      // 稍作停顿，为下一步准备
      break;
    case 4: // 准备抬起前左腿和后右腿
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
    case 7: // 恢复所有髋关节到中心位置，准备下一个循环
      setServo(FRONT_RIGHT_HIP, centerPos); // 前右髋关节回中
      setServo(FRONT_LEFT_HIP, centerPos);  // 前左髋关节回中
      setServo(BACK_RIGHT_HIP, centerPos);  // 后右髋关节回中
      setServo(BACK_LEFT_HIP, centerPos);   // 后左髋关节回中
      break;
    }

    // 增加计数器，进入下一阶段
    sharedCounter += 1;

    // 如果需要停止行走，可以在这里检查某个条件，然后设置状态为Completed
    if (sharedCounter >= 64) { // 假设走64个阶段后停止
      debuglnF("Robot completed walking.");
      for (int i = 0; i < 8; i++) {
        setServo(i, 90);
      }
      currentMotionState = RobotMotionState::Completed; // 设置为完成状态
      nextMotionId = RobotMotionId::Idle; // 完成后设置下一个动作为Idle
    }
    // 这里我们让机器人一直走下去，不主动停止
  } else if (currentMotionState == RobotMotionState::Completed) {
  }
}

void handleMotionAutoWalk() {
  // 定义动作幅度系数
  const uint8_t amplitude = 20;     // 髋关节运动幅度
  const uint8_t legLiftHeight = 10; // 腿抬起高度
  const uint8_t centerPos = 90;     // 中心位置

  // 自动行走和 走路类似，不过会在循环的时候获取 us（超声波传感器）数据，
  // 并根据数据判断是否需要转向或停止。
  if (currentMotionState == RobotMotionState::NotStarted) {
    debuglnF("Robot starts auto walking.");
    debugF("Auto walking with amplitude: ");
    debug(amplitude);
    debuglnF(" degrees");

    sharedCounter = 0;
    // 初始化所有舵机位置，准备行走
    for (int i = 0; i < 8; i++) {
      setServo(i, centerPos); // 所有舵机回到中心位置
    }
    currentMotionState = RobotMotionState::InProgress;
  } else if (currentMotionState == RobotMotionState::InProgress) {
    // 获取超声波传感器数据
    int distance = getUSDistance(); // 假设有一个函数获取距离

    debugF("US Distance: ");
    debugln(distance);

    if (distance < 20) { // 如果距离小于20cm，转向或停止
      // 根据 sharedCounter 来决定转向的具体动作
      if (sharedCounter % 2 == 0) {
        debuglnF("Obstacle detected, turning left.");
        currentMotionState = RobotMotionState::NotStarted; // 重置状态，准备转向
        currentMotionId = RobotMotionId::TurningLeft;      // 设置为左转状态
        // 直接跳转到对应的状态
        // 设置下一个状态为回归 autoWalking
        nextMotionId = RobotMotionId::AutoWalking; // 转向后继续自动行走
      } else {
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

    switch (walkPhase) {
    case 0: // 准备抬起前右腿和后左腿
      setServo(FRONT_RIGHT_LEG, centerPos - legLiftHeight); // 抬起前右腿
      setServo(BACK_LEFT_LEG, centerPos - legLiftHeight);   // 抬起后左腿
      break;
    case 1: // 前右腿和后左腿向前迈步
      setServo(FRONT_RIGHT_HIP, centerPos + amplitude); // 前右髋关节向前
      setServo(BACK_LEFT_HIP, centerPos - amplitude);   // 后左髋关节向前
      break;
    case 2:                                 // 放下前右腿和后左腿
      setServo(FRONT_RIGHT_LEG, centerPos); // 放下前右腿
      setServo(BACK_LEFT_LEG, centerPos);   // 放下后左腿
      break;
    case 3: // 准备移动身体
      // 稍作停顿，为下一步准备
      break;
    case 4: // 准备抬起前左腿和后右腿
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
    case 7: // 恢复所有髋关节到中心位置，准备下一个循环
      setServo(FRONT_RIGHT_HIP, centerPos); // 前右髋关节回中
      setServo(FRONT_LEFT_HIP, centerPos);  // 前左髋关节回中
      setServo(BACK_RIGHT_HIP, centerPos);  // 后右髋关节回中
      setServo(BACK_LEFT_HIP, centerPos);   // 后左髋关节回中
      break;
    }
  } else if (currentMotionState == RobotMotionState::Completed) {
    // 如果当前状态已完成，可能需要重置或进入下一个动作
    debuglnF("Robot completed auto walking.");
    for (int i = 0; i < 8; i++) {
      setServo(i, 90); // 所有舵机回到中心位置
    }
    setMovingState(RobotMotionId::Idle); // 设置下一个动作为Idle
  }
}

void handleMotionTurnLeft() {
  // 定义动作幅度系数
  const uint8_t turnAmplitude = 20; // 转向幅度
  const uint8_t legLiftHeight = 10; // 腿抬起高度
  const uint8_t centerPos = 90;     // 中心位置

  if (currentMotionState == RobotMotionState::NotStarted) {
    debuglnF("Robot starts turning left.");
    debugF("Turning left with amplitude: ");
    debug(turnAmplitude);
    debuglnF(" degrees");

    sharedCounter = 0;
    // 初始化所有舵机位置，准备转弯
    for (int i = 0; i < 8; i++) {
      setServo(i, centerPos); // 所有舵机回到中心位置
    }
    currentMotionState = RobotMotionState::InProgress;
  } else if (currentMotionState == RobotMotionState::InProgress) {
    // 机器人左转循环
    uint8_t turnPhase = sharedCounter % 6; // 将左转分为6个阶段

    debugF("Left turning phase: ");
    debugln(turnPhase);

    switch (turnPhase) {
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
      if (sharedCounter >= 12) { // 完成2个完整转向周期
        currentMotionState = RobotMotionState::Completed;
        debuglnF("Left turn completed.");
      }
      break;
    }

    // 如果没有到case 5，也增加计数器进入下一阶段
    if (turnPhase != 5) {
      sharedCounter += 1;
    }

  } else if (currentMotionState == RobotMotionState::Completed) {
    // 转弯完成后的处理
    debuglnF("Left turn state completed.");

    // 如果有下一个动作ID设置，将自动切换到该状态
    // 否则默认回到空闲状态
    if (nextMotionId == currentMotionId) {
      setMovingState(RobotMotionId::Idle);
    }
  }
}

void handleMotionTurnRight() {
  // 定义动作幅度系数
  const uint8_t turnAmplitude = 20; // 转向幅度
  const uint8_t legLiftHeight = 10; // 腿抬起高度
  const uint8_t centerPos = 90;     // 中心位置

  if (currentMotionState == RobotMotionState::NotStarted) {
    debuglnF("Robot starts turning right.");
    debugF("Turning right with amplitude: ");
    debug(turnAmplitude);
    debuglnF(" degrees");

    sharedCounter = 0;
    // 初始化所有舵机位置，准备转弯
    for (int i = 0; i < 8; i++) {
      setServo(i, centerPos); // 所有舵机回到中心位置
    }
    currentMotionState = RobotMotionState::InProgress;
  } else if (currentMotionState == RobotMotionState::InProgress) {
    // 机器人右转循环
    uint8_t turnPhase = sharedCounter % 6; // 将右转分为6个阶段

    debugF("Right turning phase: ");
    debugln(turnPhase);

    switch (turnPhase) {
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
      if (sharedCounter >= 12) { // 完成2个完整转向周期
        currentMotionState = RobotMotionState::Completed;
        debuglnF("Right turn completed.");
      }
      break;
    }

    // 如果没有到case 5，也增加计数器进入下一阶段
    if (turnPhase != 5) {
      sharedCounter += 1;
    }

  } else if (currentMotionState == RobotMotionState::Completed) {
    // 转弯完成后的处理
    debuglnF("Right turn state completed.");

    // 如果有下一个动作ID设置，将自动切换到该状态
    // 否则默认回到空闲状态
    if (nextMotionId == currentMotionId) {
      setMovingState(RobotMotionId::Idle);
    }
  }
}

//-===================== 电机控制 =====================
//   board_pins[FRONT_RIGHT_HIP] = 2;
// board_pins[FRONT_LEFT_HIP] = 8;
// board_pins[FRONT_RIGHT_LEG] = 3;
// board_pins[FRONT_LEFT_LEG] = 9;
// board_pins[BACK_RIGHT_HIP] = 4;
// board_pins[BACK_LEFT_HIP] = 6;
// board_pins[BACK_RIGHT_LEG] = 5;
// board_pins[BACK_LEFT_LEG] = 7];
uint8_t board_pins[8] = {2, 8, 3, 9, 4, 6, 5, 7}; // 舵机引脚定义
Servo servo;                                      // 创建Servo对象
uint8_t currentServoPin = -1; // 当前连接的舵机引脚，-1表示未连接

// 设置舵机角度函数
// 对于 hip（髋关节）和 leg（腿部），
// leg + 代表 放下腿， leg - 代表 抬起腿
// hip + 代表 逆时针旋转，hip - 代表 顺时针旋转

void setServo(int id, int target) {
  if (id < 0 || id > 7)
    return;

  // 首先限制目标角度在0-180度范围内，防止异常值传入
  if (target < 0) {
    debugF("警告：舵机角度小于0，已修正：");
    debugln(target);
    target = 0;
  }
  if (target > 180) {
    debugF("警告：舵机角度大于180，已修正：");
    debugln(target);
    target = 180;
  }

  debugF("setServo id: ");
  debug(id);
  debugF(", target: ");
  debug(target);

  // 如果需要更改舵机引脚
  if (currentServoPin != board_pins[id]) {
    // 如果之前有舵机连接，先分离
    if (currentServoPin >= 0) {
      servo.detach();
    }
    // 连接新舵机
    servo.attach(board_pins[id]);
    currentServoPin = board_pins[id];
  }

  // 计算实际角度，考虑修剪和反转
  int angle;
  if (reverseLoader.get(id)) {
    debugF(", reverse: true");
    angle = 180 - (target + trimLoader.get(id));
  } else {
    angle = target + trimLoader.get(id);
  }

  // 限制角度在有效范围内
  if (angle < 0)
    angle = 0;
  if (angle > 180)
    angle = 180;


  debugF(", angle: ");
  debug(angle);
  debuglnF(".");
  // 写入角度
  servo.write(angle);

  // 确保舵机有足够时间响应
  delay(20); // 添加短暂延时以避免同时移动所有舵机
}