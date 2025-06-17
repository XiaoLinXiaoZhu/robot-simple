#ifndef ROBOT_COMMANDS_H
#define ROBOT_COMMANDS_H

#include <Arduino.h>

// 命令处理函数
void handleCommands();

class CommandHandler {
public:
    char command; // 命令字符
    virtual void handle(char *token) = 0; // 处理命令的虚函数
};

// 命令处理器数组
extern CommandHandler *commandHandlers[];

#endif // ROBOT_COMMANDS_H
