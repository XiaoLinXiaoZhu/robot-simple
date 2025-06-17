#include "RobotServoControl.h"
#include "IDebug.h"

// 外部引用加载器
extern IRobot::ServoTrim trimLoader;
extern IRobot::ServoReverse reverseLoader;

// 舵机引脚定义
uint8_t board_pins[8] = {2, 8, 3, 9, 4, 6, 5, 7};
Servo servos[8];                                  // 创建多个Servo对象
Servo servo;                                      // 创建单个Servo对象（用于兼容）
uint8_t currentServoPin = -1;                     // 当前连接的舵机引脚，-1表示未连接
bool ifServoInit = false;                         // 是否已初始化舵机

void initServos()
{
  debuglnF("Initializing servos...");
  for (int i = 0; i < 8; i++)
  {
    servos[i].attach(board_pins[i]); // 连接每个舵机到对应引脚
    servos[i].write(90);             // 初始化所有舵机到中心位置
    debugF("Servo ");
    debug(i);
    debugF(" attached to pin ");
    debugln(board_pins[i]);
  }
}

void setServo(int id, int target)
{
  if (!ifServoInit)
  {
    initServos();       // 如果舵机未初始化，先初始化
    ifServoInit = true; // 设置标志位，避免重复初始化
  }

  // 检查舵机ID是否在有效范围内
  if (id < 0 || id > 7)
  {
    debugF("Invalid servo ID: ");
    debugln(id);
    return;
  }

  // 首先限制目标角度在0-180度范围内，防止异常值传入
  if (target < 0)
  {
    debugF("Warning: Servo angle less than 0, corrected to: ");
    debugln(target);
    target = 0;
  }
  if (target > 180)
  {
    debugF("Warning: Servo angle greater than 180, corrected to: ");
    debugln(target);
    target = 180;
  }

  debugF("Setting servo ID: ");
  debug(id);
  debugF(", target angle: ");
  debug(target);
  debugF(", current pin: ");
  debug(currentServoPin);
  debuglnF(".");

  int angle;
  // 因为现在每个舵机都有自己的Servo对象，所以不需要切换引脚
  if (reverseLoader.get(id))
  {
    debugF(", reverse: true");
    angle = 180 - (target + trimLoader.get(id));
  }
  else
  {
    angle = target + trimLoader.get(id);
  }

  // 限制角度在有效范围内
  if (angle < 0)
    angle = 0;

  if (angle > 180)
    angle = 180;

  debugF(", final angle: ");
  debug(angle);
  debuglnF(".");
  // 写入角度到对应的舵机
  servos[id].write(angle);
  // 确保舵机有足够时间响应
  delay(20); // 添加短暂延时以避免同时移动所有舵机
}

void _setServo(int id, int target)
{
  if (id < 0 || id > 7)
    return;

  // 首先限制目标角度在0-180度范围内，防止异常值传入
  if (target < 0)
  {
    debugF("警告：舵机角度小于0，已修正：");
    debugln(target);
    target = 0;
  }
  if (target > 180)
  {
    debugF("警告：舵机角度大于180，已修正：");
    debugln(target);
    target = 180;
  }

  debugF("setServo id: ");
  debug(id);
  debugF(", target: ");
  debug(target);

  // 如果需要更改舵机引脚
  if (currentServoPin != board_pins[id])
  {
    // 如果之前有舵机连接，先分离
    if (currentServoPin >= 0)
    {
      servo.detach();
    }
    // 连接新舵机
    servo.attach(board_pins[id]);
    currentServoPin = board_pins[id];
  }

  // 计算实际角度，考虑修剪和反转
  int angle;
  if (reverseLoader.get(id))
  {
    debugF(", reverse: true");
    angle = 180 - (target + trimLoader.get(id));
  }
  else
  {
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
