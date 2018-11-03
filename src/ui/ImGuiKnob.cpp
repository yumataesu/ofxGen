
#include "ImGuiKnob.h"
namespace ImGui {
    
    IMGUI_API bool Knob(const char* label, float* value_p, float minv, float maxv) {
        ImGuiStyle& style = ::ImGui::GetStyle();
        style.Colors[ImGuiCol_FrameBg]                = ImVec4(0.00f, 0.10f, 0.38f, 0.68f);
        style.Colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.00f, 0.10f, 0.38f, 0.68f);
        style.Colors[ImGuiCol_FrameBgActive]          = ImVec4(0.00f, 0.10f, 0.38f, 0.68f);
        
        float line_height = ::ImGui::GetTextLineHeight();
        
        ImVec2 p = ImGui::GetCursorScreenPos();
        float sz = 36.0f;
        float radio =  sz*0.5f;
        ImVec2 center = ImVec2(p.x + radio, p.y + radio + 8);
        float val1 = (value_p[0] - minv)/(maxv - minv);
        char textval[32];
        ImFormatString(textval, IM_ARRAYSIZE(textval), "%03.1f", value_p[0]);
        
        ImVec2 textpos = p;
        float gamma = M_PI * 0.25;//0 value in knob
        float alpha = (M_PI - gamma) * val1 * 2.0f + gamma;
        
        float x2 = -sinf(alpha)*radio + center.x;
        float y2 = cosf(alpha)*radio + center.y;
        
        float x3 = (-sinf(alpha) * radio * 0.2 + center.x);
        float y3 = (cosf(alpha) * radio * 0.2 + center.y);
        
        ImGui::InvisibleButton(label, ImVec2(sz, sz));
        
        bool is_active = ::ImGui::IsItemActive();
        bool is_hovered = ::ImGui::IsItemHovered();
        bool touched = false;
        
        if (is_active) {
            touched = true;
            ImVec2 mp = ImGui::GetIO().MousePos;
            alpha = atan2f(mp.x - center.x, center.y - mp.y) + M_PI;
            alpha = std::max(gamma, std::min((float)(2.0f * M_PI - gamma), (float)alpha));
            float value = 0.5f*(alpha-gamma)/(M_PI-gamma);
            value_p[0] = value*(maxv - minv) + minv;
        }
        
        ImVec2 offset = ImVec2(0, 25);
        
        ImU32 col32 = ImGui::GetColorU32(is_active ? ImGuiCol_FrameBgActive : is_hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
        ImU32 col32line = ImGui::GetColorU32(ImGuiCol_SliderGrabActive);
        ImU32 col32text = ImGui::GetColorU32(ImGuiCol_Text);
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        
        draw_list->AddCircleFilled(center, radio, col32, 8);
        draw_list->AddLine(ImVec2(x3, y3), ImVec2(x2, y2), col32line, 2);
        draw_list->AddText(ImVec2(p.x, p.y), col32text, label);
        //draw_list->AddText(ImVec2(p.x, p.y + sz + style.ItemInnerSpacing.y + 20), col32text, textval);
        
        
        style.Colors[ImGuiCol_FrameBg]                = ImVec4(0.00f, 0.10f, 0.38f, 0.68f);
        style.Colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.00f, 0.59f, 0.66f, 1.00f);
        style.Colors[ImGuiCol_FrameBgActive]          = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
        
        return touched;
    }
}
