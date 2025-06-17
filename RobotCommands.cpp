#include "RobotCommands.h"
#include "IDebug.h"
#include "RobotDefines.h"
#include "RobotMotion.h"
#include "RobotServoControl.h"
#include "loadReverse.h"
#include "loadTrim.h"

// 外部引用修剪器和反向加载器
extern IRobot::ServoTrim trimLoader;
extern IRobot::ServoReverse reverseLoader;

const char endChar = '\r'; // 定义命令结束符

// 定义串口命令处理函数

// 一般不接受参数的指令都是直接设置下一个动作ID的指令
// 例如：'W' - 设置为行走状态，'A' - 设置为自动行走状态，'L' -
// 设置为左转状态，'R' - 设置为右转状态，'D' - 设置为跳舞状态等。
class HandleCommand_MotionChange : public CommandHandler {
public:
  RobotMotionId nextMotionId; // 下一个动作ID
  HandleCommand_MotionChange(char commandChar, RobotMotionId motionId) {
    command = commandChar;   // 设置命令字符
    nextMotionId = motionId; // 设置下一个动作ID
  }
  void handle(char *token) override {
    // 处理运动状态切换命令
    debugF("Running command ");
    debug(command);
    debugF(" - Setting motion to ");
    debugln(static_cast<uint8_t>(nextMotionId));
    setMovingState(nextMotionId); // 设置为下一个动作状态
  }
};

class HandleCommand_C : public CommandHandler {
public:
  HandleCommand_C() { command = 'C'; } // 设置命令字符为 'C'
  void handle(char *token) override {
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
};

class HandleCommand_RV : public CommandHandler {
public:
  HandleCommand_RV() { command = 'V'; } // 设置命令字符为 'V'
  void handle(char *token) override {
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
};

class HandleCommand_T : public CommandHandler {
public:
  HandleCommand_T() { command = 'T'; } // 设置命令字符为 'T'
  void handle(char *token) override {
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
};

CommandHandler *commandHandlers[] = {
    new HandleCommand_MotionChange('W', RobotMotionId::Walking),
    new HandleCommand_MotionChange('A', RobotMotionId::AutoWalking),
    new HandleCommand_MotionChange('L', RobotMotionId::TurningLeft),
    new HandleCommand_MotionChange('R', RobotMotionId::TurningRight),
    new HandleCommand_MotionChange('D', RobotMotionId::Dancing),
    new HandleCommand_C(),
    new HandleCommand_RV(),
    new HandleCommand_T(),
    nullptr // 结束标志
};

void handleCommands() {
  static char buffer[32];      // 命令缓冲区
  static uint8_t bufIndex = 0; // 缓冲区索引
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

      bool commandFound = false; // 标记是否找到命令处理器
      // 遍历命令处理器数组
      for (int i = 0; commandHandlers[i] != nullptr; i++) {
        if (commandHandlers[i]->command == cmd) {
          commandHandlers[i]->handle(token); // 调用对应的处理函数
          commandFound = true;               // 找到命令处理器
          break;                             // 找到后退出循环
        }
      }
      if (!commandFound) {
        debugF("Unknown command: ");
        debugln(buffer);
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