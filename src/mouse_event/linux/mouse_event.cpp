#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <string>
#include <regex>
#define MOUSE_EVENT_EXPORTS
#include "mouse_event.hpp"

void enableCSI() {
    std::cout << "\033[?1003h\033[?1015h\033[?1006h";
    std::cout.flush();
}

void disableCSI() {
    std::cout << "\033[?1003l\033[?1015l\033[?1006l";
    std::cout.flush();
}


std::tuple<int,int,bool,bool> getMousePosition(std::string csi) {
    std::regex re("\\<(\\d+);(\\d+);(\\d+)([m|M])");
    std::smatch match;
    if (std::regex_search(csi, match, re)) {
        int press = std::stoi(match[1]) == 32;
        int row = std::stoi(match[2]);
        int col = std::stoi(match[3]);
        bool click = (match[4] == "m");
        return std::make_tuple(row, col, click, press);
    } else {
        return std::make_tuple(-1, -1, false, false);
    }

}

void setRawMode(termios& orig_termios) {
    termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO); // 禁用标准模式和回显
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

static std::vector<mouse_event_func> event_pool;
void register_mouse_event(mouse_event_func func) {
    event_pool.emplace_back(func);
}

void start_mouse_event_loop() {
    termios orig_termios;
    tcgetattr(STDIN_FILENO, &orig_termios); // 获取当前终端设置


    enableCSI();
    // std::cout << "Mouse tracking enabled. Press 'q' to quit." << std::endl;

    setRawMode(orig_termios);
    std::string csi;
    while (true) {
        char ch;
        ssize_t n = read(STDIN_FILENO, &ch, 1);
        if (n == -1) {
            perror("read");
            break;
        }
        csi += (ch == '\033') ? 'E' : ch;
        if (ch == 'm' || ch == 'M') {
            // std::cout << "CSI: " << csi << std::endl;
            auto [x,y, click, press] = getMousePosition(csi);
            if (x != -1 && y != -1) {
                for (auto &func : event_pool) {
                    try {
                        std::flush(std::cout);
                        func(x, y, click, press);
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

    disableCSI();
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); // 恢复终端设置
    std::cout << "Mouse tracking disabled." << std::endl;
}

