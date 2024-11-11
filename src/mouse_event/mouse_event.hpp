#if defined(_WIN32) || defined(_WIN64)
    #ifdef MOUSE_EVENT_EXPORTS
        #define MOUSE_EVENT_API __declspec(dllexport)
    #else
        #define MOUSE_EVENT_API __declspec(dllimport)
    #endif
#else
    #define MOUSE_EVENT_API __attribute__((visibility("default")))
#endif

#define MOUSE_EVENT_BUTTON_LEFT_PRESS   (32)
#define MOUSE_EVENT_BUTTON_LEFT_CLICK   (0)
#define MOUSE_EVENT_BUTTON_RIGHT_PRESS  (34)
#define MOUSE_EVENT_BUTTON_RIGHT_CLICK  (2)
#define MOUSE_EVENT_BUTTON_MIDDLE_PRESS (33)
#define MOUSE_EVENT_BUTTON_MIDDLE_CLICK (1)
#define MOUSE_EVENT_BUTTON_SCROLL_UP    (65)
#define MOUSE_EVENT_BUTTON_SCROLL_DOWN  (64)

using mouse_event_func = void (*) (int x, int y, int b);

MOUSE_EVENT_API void register_mouse_event(mouse_event_func func);
MOUSE_EVENT_API void start_mouse_event_loop();
