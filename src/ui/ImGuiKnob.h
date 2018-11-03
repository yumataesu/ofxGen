#pragma once
#define _USE_MATH_DEFINES

#include "imgui.h"
#include "imgui_internal.h"
#include "imconfig.h"
#include <algorithm>

namespace ImGui {
    IMGUI_API bool Knob(const char* label, float* value_p, float minv, float maxv);
}
