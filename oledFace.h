// 使用olde显示表情
#ifndef OLED_FACE_H
#define OLED_FACE_H

#include "oled_lite.h"
#include "IDebug.h"

#ifdef VSCODE
#include <cstring>
#endif

// 显示表情,使用olde显示表情(字符画),当前zik
#define OLEDLENGTH 16 // OLED显示的字符长度

inline int centerX(const char* text) {
    int len = strlen(text);
    return (OLEDLENGTH - len) / 2;
}

void showFace(const char *faceName)
{
    // 显示表情
    if (faceName == nullptr || faceName[0] == '\0')
    {
        OLED_Lite::clear();
        OLED_Lite::displayText("  ^_^  ", centerX("  ^_^  "), 1);
        OLED_Lite::displayText(" Hello!", centerX(" Hello!"), 2);
        return;
    }
    if (strcmp(faceName, "none") == 0)
    {
        OLED_Lite::clear();
        OLED_Lite::displayText("  ^_^  ", centerX("  ^_^  "), 1);
        OLED_Lite::displayText(" Hello!", centerX(" Hello!"), 2);
        return;
    }
    if (strcmp(faceName, "hello") == 0){
        OLED_Lite::clear();
        OLED_Lite::displayText("  ^_^  ", centerX("  ^_^  "), 1);
        OLED_Lite::displayText(" Hello!", centerX(" Hello!"), 2);
        return;
    }
    else if (strcmp(faceName, "happy") == 0)
    {
        OLED_Lite::clear();
        OLED_Lite::displayText("  ^_^  ", centerX("  ^_^  "), 1);
        OLED_Lite::displayText(" Happy!", centerX(" Happy!"), 2);
    }
    else if (strcmp(faceName, "sad") == 0)
    {
        OLED_Lite::clear();
        OLED_Lite::displayText("  T_T  ", centerX("  T_T  "), 1);
        OLED_Lite::displayText(" I'm sad", centerX(" I'm sad"), 2);
    }
    else if (strcmp(faceName, "surprised") == 0)
    {
        OLED_Lite::clear();
        OLED_Lite::displayText("  O_O  ", centerX("  O_O  "), 1);
        OLED_Lite::displayText(" Wow!", centerX(" Wow!"), 2);
    }
    else if (strcmp(faceName, "confused") == 0)
    {
        OLED_Lite::clear();
        OLED_Lite::displayText("  o_O  ", centerX("  o_O  "), 1);
        OLED_Lite::displayText(" Huh?", centerX(" Huh?"), 2);
    }
    else if (strcmp(faceName, "thinking") == 0)
    {
        OLED_Lite::clear();
        OLED_Lite::displayText("  -_-  ", centerX("  -_-  "), 1);
        OLED_Lite::displayText(" Let me think", centerX(" Let me think"), 2);
    }
    else if (strcmp(faceName, "sleepy") == 0)
    {
        OLED_Lite::clear();
        OLED_Lite::displayText("  zzz  ", centerX("  zzz  "), 1);
        OLED_Lite::displayText(" I'm sleepy", centerX(" I'm sleepy"), 2);
    }
    else if (strcmp(faceName, "excited") == 0)
    {
        OLED_Lite::clear();
        OLED_Lite::displayText("  ^o^  ", centerX("  ^o^  "), 1);
        OLED_Lite::displayText(" Yay!", centerX(" Yay!"), 2);
    }
    else if (strcmp(faceName, "bored") == 0)
    {
        OLED_Lite::clear();
        OLED_Lite::displayText("  -_-; ", centerX("  -_-; "), 1);
        OLED_Lite::displayText(" So bored...", centerX(" So bored..."), 2);
    }
    else if (strcmp(faceName, "love") == 0)
    {
        OLED_Lite::clear();
        OLED_Lite::displayText(" <3 <3 ", centerX(" <3 <3 "), 1);
        OLED_Lite::displayText(" Love you!", centerX(" Love you!"), 2);
    }
    else if (strcmp(faceName, "shocked") == 0)
    {
        OLED_Lite::clear();
        OLED_Lite::displayText(" !O_O! ", centerX(" !O_O! "), 1);
        OLED_Lite::displayText(" No way!", centerX(" No way!"), 2);
    }
    else if (strcmp(faceName, "cool") == 0)
    {
        OLED_Lite::clear();
        OLED_Lite::displayText(" B-) B-) ", centerX(" B-) B-) "), 1);
        OLED_Lite::displayText(" Cool dude!", centerX(" Cool dude!"), 2);
    }
    else if (strcmp(faceName, "angry") == 0)
    {
        OLED_Lite::clear();
        OLED_Lite::displayText("  >_<  ", centerX("  >_<  "), 1);
        OLED_Lite::displayText(" Go away!", centerX(" Go away!"), 2);
    }
    else
    {
        OLED_Lite::clear();
        OLED_Lite::displayText("  ^_^  ", centerX("  ^_^  "), 1);
        OLED_Lite::displayText(" Hello!", centerX(" Hello!"), 2);
    }
}

#endif