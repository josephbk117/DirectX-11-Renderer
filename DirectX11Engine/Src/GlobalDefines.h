#pragma once

//Target Windows 7 or later
#define _WIN32_WINNT 0x0601
#include <sdkddkver.h>

#define WIN32_LEAN_AND_MEAN
#define NOMENUS
#define NOICONS
#define NOSYSCOMMANDS
#define NOKERNEL
#define NOHELP
#define NOMINMAX
#define NOIMAGE

#define STRICT

#include <Windows.h>