#include "windowEvent.h"

namespace bee
{

#define BEE_EVENT_STATIC(eventName, ...) \
	WindowEventList<__VA_ARGS__> *eventName##Dispatcher::handlers = nullptr

BEE_EVENT_STATIC(Key, int, int, int, int);

BEE_EVENT_STATIC(Char, unsigned int);

BEE_EVENT_STATIC(CharMods, unsigned int, int);

BEE_EVENT_STATIC(CursorPos, double, double);

BEE_EVENT_STATIC(CursorEnter, int);

BEE_EVENT_STATIC(MouseButton, int, int, int);

BEE_EVENT_STATIC(Scroll, double, double);

// BEE_EVENT_STATIC(Joystick, int, int);

BEE_EVENT_STATIC(Drop, int, const char**);

BEE_EVENT_STATIC(Render);

BEE_EVENT_STATIC(Update, double);

}