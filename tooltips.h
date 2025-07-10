#include <iostream>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <deque>

#include "imgui.h"
#include <fstream>
#include "tinyfiledialogs.h" // Or any other file dialog lib you're using



#ifndef TOOLTIPS_H
#define TOOLTIPS_H

class ToolTips
{
public:
    void ShowToolTip(std::string text, std::string id_name )
    {
        ImGui::PushID(id_name.c_str());
        
        ImGui::SameLine();

        ImGui::Button("i");
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip(text.c_str());
        }
        ImGui::PopID();
    }

};

#endif