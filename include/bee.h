#pragma once

#include "bee/runtime.h"

#	ifndef DEBUG
#		ifdef WIN32
#			pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
#		else
#		endif
#	endif
