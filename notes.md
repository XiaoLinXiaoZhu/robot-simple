## 这是什么？

这里用于记录一些代码片段。作为备忘录，可以随时查看。

## 使用元件型号

主板型号：Arduino UNO
[Arduino UNO](https://www.arduino.cc/en/Guide/ArduinoUno)

拓展版型号：Gravity: IO Expansion & Motor Driver Shield
[Gravity: IO Expansion & Motor Driver Shield](https://wiki.dfrobot.com.cn/_SKU_DFR0502__Gravity__IO_Expansion_%26_Motor_Driver_Shield)

显示器型号：SH1106_128X32_VISIONOX_HW_I2C


## 装配接线

```
舵机引脚连接示意图：
__________ __________ _________________
|(pin9)_____)(pin8)  (pin2)(______(pin3)|  // 前腿舵机连接
|__|       |left FRONT right|        |__|
        |                |
        |                |
        |                |               // 身体部分
_________ |                | __________
|(pin7)_____)(pin6)__(pin4)(______(pin5)|  // 后腿舵机连接
|__|                                 |__|

超声波传感器：
TRIGGER引脚 - 12
ECHO引脚    - 11

蜂鸣器引脚 - 13
```

## 舵机编号
```c++
// trim[] for calibrating servo deviation,
// initial posture (home) should like below
// in symmetric
//    \       / front left
//     \_____/
//     |     |->
//     |_____|->
//     /     \
//    /       \ front right
    
// trim[FRONT_LEFT_HIP] = 0;
// trim[FRONT_RIGHT_HIP] = -8;
// trim[BACK_LEFT_HIP] = 8;
// trim[BACK_RIGHT_HIP] = 5;

// trim[FRONT_LEFT_LEG] = 2;
// trim[FRONT_RIGHT_LEG] = -6;
// trim[BACK_LEFT_LEG] = 6;
// trim[BACK_RIGHT_LEG] = 5;
```

## 偏移量记录

| 舵机编号 | 偏移量   | 俯视角/抬高 |
| -------- | -------- | ----------- |
| 0        | -5       | 逆时针      |
| 1        | 0        | 逆时针      |
| 2        | -5       | 抬升        |
| 3        | 反转 -35 | 降低        |
| 4        | -30      | 逆时针      |
| 5        | 30       | 逆时针      |
| 6        | -20      | 抬升        |
| 7        | 反转 30  | 降低        |




## 问题解决

### 为什么我的Arduino UNO 在上传代码之后能正常工作但是无法通过USB连接到电脑？

因为你已经成功上传过代码，所以说你的电脑这一部分其实是正常的。

问题出在Arduino UNO 上，当机器人尝试控制电机运动，但是点击没能成功移动（比如说被结构阻挡），这个时候Arduino UNO 会出现故障，导致 IO 口无法正常工作。

解决方法是避免电机被阻挡，不要将其直接放到地上，而是使用支架将其抬高，并且即检查电机的偏移量是否正确。  


## 端口功能

### 普通针脚操作功能

#### 设置工作状态

```c++
pinMode(PIN_INDEX, OUTPUT);
// 设置为输出模式,之后可以使用digitalWrite()函数来控制引脚的电平状态。
digitalWrite(PIN_INDEX, LOW);
```

```c++
pinMode(PIN_INDEX, INPUT);
// 设置为输入模式,之后可以使用digitalRead()函数来读取引脚的电平状态。
bool value = digitalRead(PIN_INDEX);
// value将会是HIGH或LOW，表示引脚的电平状态。
```

#### 读写功能

读写之前需要先设置引脚的工作状态。

```c++
digitalWrite(PIN_INDEX, HIGH);
// 将引脚设置为高电平
digitalWrite(PIN_INDEX, LOW);
// 将引脚设置为低电平
```

```c++
bool value = digitalRead(PIN_INDEX);
// 读取引脚的电平状态
// value将会是HIGH或LOW，表示引脚的电平状态。
```



## 自定义指令

默认状态（4）下，程序能够通过I/O口接收自定义指令，实现手动控制。

```cpp
  // 注册串口命令处理函数
  SerialCmd.addCommand("S", receiveStop);         // 停止命令
  SerialCmd.addCommand("L", receiveLED);          // LED控制
  SerialCmd.addCommand("M", receiveMovement);     // 运动控制
  SerialCmd.addCommand("H", receiveGesture);      // 手势控制
  SerialCmd.addCommand("K", receiveSing);         // 声音控制
  SerialCmd.addCommand("D", requestDistance);     // 距离请求
  SerialCmd.addCommand("I", requestProgramId);    // 程序ID请求
  SerialCmd.addCommand("J", requestMode);         // 模式请求
  SerialCmd.addCommand("C", requestCalibration);  // 校准请求
  SerialCmd.addDefaultHandler(receiveStop);       // 默认处理函数
```

| 指令 | 参数1             | 参数2    | 说明                                                           |
| ---- | ----------------- | -------- | -------------------------------------------------------------- |
| S    |                   |          | 停止命令，停止所有运动                                         |
| L    | 0/1               |          | LED控制，0为关闭，1为开启（因为不是点阵屏，该功能无效）        |
| M    | 工作模式          | 运行时间 | 运动控制，工作模式见下表，运行时间为毫秒                       |
| H    | 手势编号(1/2/3/7) |          | 手势控制 ，1为开心，2为超级开心，3为悲伤，7为爱心              |
| K    |                   |          | 因为没有加装蜂鸣器，所以该功能无效                             |
| D    |                   |          | 距离请求，返回超声波传感器测量的距离                           |
| I    |                   |          | 程序ID请求，返回当前程序的ID                                   |
| J    |                   |          | 模式请求，返回当前工作模式                                     |
| C    | 舵机编号（0-7）   | 偏移量   | 校准请求，设置舵机的偏移量（偏移量保存在EEPROM中，永久化存储） |

### 工作模式

下面是源代码片段，展示了如何 工作模式的定义和处理：

```cpp
// 动作指令处理
  switch (cmd) {
  case 1:
    robot.run(0);
    break;  // 停止
  case 2:
    robot.run(1);
    break;  // 前进
  case 3:
    robot.turnL(1, 550);
    break;  // 左转
  case 4:
    robot.turnR(1, 550);
    break;  // 右转
  case 5:
    robot.home();
    break;  // 归位
  case 6:
    robot.pushUp();
    break;  // 俯卧撑
  case 7:
    robot.upDown();
    break;  // 上下运动
  case 8:
    robot.waveHAND();
    break;  // 挥手
  case 9:
    robot.Hide();
    break;  // 隐藏
  case 10:
    robot.omniWalk(true);
    break;  // 全向行走(前)
  case 11:
    robot.omniWalk(false);
    break;  // 全向行走(后)
  case 12:
    robot.dance(1, 1000);
    break;  // 舞蹈1
  case 13:
    robot.frontBack(1, 750);
    break;  // 前后运动
  case 14:
    robot.jump();
    break;  // 跳跃
  case 15:
    robot.scared();
    break;  // 害怕动作
  case 90:
    robot.runSingle(0);
    break;  // 0 号舵机单独动作
  case 91:
    robot.runSingle(1);
    break;  // 1 号舵机单独动作
  case 92:
    robot.runSingle(2);
    break;  // 2 号舵机单独动作
  case 93:
    robot.runSingle(3);
    break;  // 3 号舵机单独动作
  case 94:
    robot.runSingle(4);
    break;  // 4 号舵机单独动作
  case 95:
    robot.runSingle(5);
    break;  // 5 号舵机单独动作
  case 96:
    robot.runSingle(6);
    break;  // 6 号舵机单独动作
  case 97:
    robot.runSingle(7);
    break;  // 7 号舵机单独动作
  default:
    taken = false;
    manualMode = true;
  }
```

比如，进入 前进模式（2），你可以在串口监视器中输入 `M 2 1000`，表示前进模式，运行时间为1000毫秒（1秒）。或者缺省时间，直接输入 `M 2`，表示前进模式，运行时间为默认值（550毫秒）。

