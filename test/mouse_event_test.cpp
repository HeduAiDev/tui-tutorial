#include <iostream>
#include "mouse_event.hpp"
#include <locale>
#include <cstdlib>
#include "utils.hpp"
using namespace std;

int main() {
    std::locale::global(std::locale("en_US.UTF-8"));
    cout << string(10, '\n');
    cout << "\033[?25l"; // 隐藏光标
    mouse_event_func func = [](int x, int y, int b) {
        cout << str_format("\033[%d;%dH", y, x); // 光标跳转到鼠标所在位置
        if (b == MOUSE_EVENT_BUTTON_LEFT_PRESS) {
            wcout << L"\u25CF"; //●
        }
    };
    register_mouse_event(func);
    start_mouse_event_loop();
    return 0;
}