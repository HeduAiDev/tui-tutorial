#include <iostream>
#include <windows.h>
#include <string>
#include <regex>
#include <vector>
#define MOUSE_EVENT_EXPORTS
#include "mouse_event.hpp"

std::tuple<int,int,bool,bool> getMousePosition(std::string csi) {
    std::regex re("\\<(\\d+);(\\d+);(\\d+)([m|M])");
    std::smatch match;
    if (std::regex_search(csi, match, re)) {
        bool press = std::stoi(match[1]) == 32;
        int row = std::stoi(match[2]);
        int col = std::stoi(match[3]);
        bool click = (match[4] == "m");
        return std::make_tuple(row, col, click ,press);
    } else {
        return std::make_tuple(-1, -1, false, false);
    }

}

void enableMouseInput(HANDLE hInput) {
    DWORD mode;
    if (!GetConsoleMode(hInput, &mode)) {
        std::cerr << "Error getting console mode: " << GetLastError() << std::endl;
        return;
    }
    mode |= ENABLE_MOUSE_INPUT;
    mode |= ENABLE_VIRTUAL_TERMINAL_INPUT;
    mode &= ~ENABLE_QUICK_EDIT_MODE;
    if (!SetConsoleMode(hInput, mode)) {
        std::cerr << "Error setting console mode: " << GetLastError() << std::endl;
    }
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

    enableMouseInput(hInput);
    // std::cout << "Mouse event enabled. Press 'q' to quit." << std::endl;

    INPUT_RECORD inputRecord[128];
    DWORD events;
    bool running = true;
    bool mouse_press = false;
    while (running) {
        if (!ReadConsoleInput(hInput, inputRecord, 128, &events)) {
            std::cerr << "Error reading console input: " << GetLastError() << std::endl;
            continue;
        }
        std::string csi;
        for (DWORD i = 0; i < events; ++i) {
            if (inputRecord[i].EventType == KEY_EVENT) {
                auto& keyEvent = inputRecord[i].Event.KeyEvent;
                csi += (keyEvent.uChar.AsciiChar == '\033') ? 'E' : keyEvent.uChar.AsciiChar;
                if (keyEvent.bKeyDown && keyEvent.uChar.AsciiChar == 'q') {
                    running = false;
                }
            }
        }
        // std::cout << "CSI: " << csi << std::endl;
        if (csi.length() > 0) {
            auto [x,y,click,press] = getMousePosition(csi);
            if (x != -1 && y != -1) {
                for (auto &func : event_pool) {
                    try {
                        func(x, y, click, press);
                    } catch(const std::exception& e) {
                        std::cerr << "Exception: " << e.what() << std::endl;
                    }
                }
            }
        }
    }
}
