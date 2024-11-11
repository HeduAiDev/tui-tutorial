#include <iostream>
#include <windows.h>
#include <string>
#include <regex>
#include <vector>
#define MOUSE_EVENT_EXPORTS
#include "mouse_event.hpp"

std::tuple<int,int,int> get_mouse_info(std::string csi) {
    std::regex re("\\<(\\d+);(\\d+);(\\d+)([m|M])");
    std::smatch match;
    if (std::regex_search(csi, match, re)) {
        int b = std::stoi(match[1]);
        int x = std::stoi(match[2]);
        int y = std::stoi(match[3]);
        return std::make_tuple(x, y, b);
    } else {
        return std::make_tuple(-1, -1, -1);
    }
}

// windows端需要手动开启虚拟终端
bool open_ansi_control_char(HANDLE hInput)
{
    if (hInput == INVALID_HANDLE_VALUE)
    {
        return false;
    }
    // 获取控制台模式
    DWORD dwMode = 0;
    if (!GetConsoleMode(hInput, &dwMode))
    {
        return false;
    }
    // 允许捕获鼠标相当于1003
    dwMode |= ENABLE_MOUSE_INPUT; 
    // 增加控制台模式的选项：启用虚拟终端
    dwMode |= ENABLE_VIRTUAL_TERMINAL_INPUT;
    // 禁止框选字符
    dwMode &= ~ENABLE_QUICK_EDIT_MODE;
    // 禁用回显和行缓冲
    dwMode &= ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT);
    if (!SetConsoleMode(hInput, dwMode))
    {
        return false;
    }
    return true;
}

static std::vector<mouse_event_func> event_pool;
void register_mouse_event(mouse_event_func func) {
    event_pool.emplace_back(func);
}

void start_mouse_event_loop() {
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    if (hInput == INVALID_HANDLE_VALUE) {
        std::cerr << "Error getting input handle: " << GetLastError() << std::endl;
        return;
    }
    if (!open_ansi_control_char(hInput)) {
        std::cerr << "Error enabling ANSI control characters: " << GetLastError() << std::endl;
        return;
    }

    std::string csi;
    while (true) {
        char ch = getchar();
        csi += (ch == '\033') ? 'E' : ch;
        if (ch == 'm' || ch == 'M') {
            // std::cout << "CSI: " << csi << std::endl;
            auto [x, y, b] = get_mouse_info(csi);
            if (x != -1 && y != -1) {
                for (auto &func : event_pool) {
                    try {
                        std::flush(std::cout);
                        func(x, y, b);
                    } catch(const std::exception& e) {
                        std::cerr << "Exception: " << e.what() << std::endl;
                    }
                }
                // std::cout << "Mouse position: (" << x << ", " << y << ")" << std::endl;
            }
            csi = "";
        }

        if (ch == 'q') {
            break;
        }
    }
}
