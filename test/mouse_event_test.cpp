#include <iostream>
#include "mouse_event.hpp"
#include <locale>
#include "utils.hpp"
using namespace std;

int main() {
    std::locale::global(std::locale("en_US.UTF-8"));
    cout << string(10, '\n');
    cout << "\033[?25l";
    mouse_event_func func = [](int x, int y, bool mouse_click, bool mouse_press) {
        cout << str_format("\033[%d;%dH", y, x);
        if (mouse_press) {
            wcout << L"\u25CF";
        }
    };
    register_mouse_event(func);
    start_mouse_event_loop();
    return 0;
}