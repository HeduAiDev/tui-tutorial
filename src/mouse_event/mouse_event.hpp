#if defined(_WIN32) || defined(_WIN64)
    #ifdef MOUSE_EVENT_EXPORTS
        #define MOUSE_EVENT_API __declspec(dllexport)
    #else
        #define MOUSE_EVENT_API __declspec(dllimport)
    #endif
#else
    #define MOUSE_EVENT_API __attribute__((visibility("default")))
#endif

using mouse_event_func = void (*) (int x, int y, bool mouse_click, bool mouse_press);

MOUSE_EVENT_API void register_mouse_event(mouse_event_func func);
MOUSE_EVENT_API void start_mouse_event_loop();
