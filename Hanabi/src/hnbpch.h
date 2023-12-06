#pragma once

#if defined(HNB_PLATFORM_WINDOWS)
#ifndef NOMINMAX
// See github.com/skypjack/entt/wiki/Frequently-Asked-Questions#warning-c4003-the-min-the-max-and-the-macro
#define NOMINMAX
#endif
#endif

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Hanabi/Core/Base.h"
#include "Hanabi/Core/Log.h"
#include "Hanabi/Core/Buffer.h"
#include "Hanabi/Core/Assert.h"

#if defined(HNB_PLATFORM_WINDOWS)
#include <Windows.h>
#endif 
