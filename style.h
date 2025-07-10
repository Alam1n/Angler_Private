#pragma once

#include <imgui.h>

namespace UIStyle {
    inline ImFont* defaultFont = nullptr;
    inline ImFont* headingFont = nullptr;
    inline ImFont* subheadingFont = nullptr;
    


    inline void ApplyDarkTheme() {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        ImVec4 bgColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);

        colors[ImGuiCol_WindowBg] = bgColor; // #2B2B2B
        colors[ImGuiCol_Text] = ImVec4(1, 1, 1, 1);
        colors[ImGuiCol_FrameBg] = ImVec4(0.07f, 0.07f, 0.1f, 1.0f);  // #3C3F41

        
        colors[ImGuiCol_TitleBg] = bgColor;
        colors[ImGuiCol_TitleBgActive] = bgColor;
        colors[ImGuiCol_TitleBgCollapsed] = bgColor;

        style.WindowBorderSize = 1.0f;  // Default is 1.0f, can be thicker
        style.WindowRounding = 6.0f;    // Rounded corners

        style.FrameBorderSize = 1.0f;   // Buttons and input box borders
        style.FrameRounding = 4.0f;

        style.Colors[ImGuiCol_Border] = ImVec4(0.1f, 0.1f, 0.1f, 0.5f);  // Subtle dark border
        style.Colors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0.0f);     // Usually unused
    }

    inline void ApplyLightTheme() {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        colors[ImGuiCol_WindowBg] = ImVec4(0.9f, 0.9f, 0.9f, 1.0f); // #E5E5E5
        colors[ImGuiCol_Text] = ImVec4(0, 0, 0, 1);
        colors[ImGuiCol_FrameBg] = ImVec4(0.5f, 0.35f, 0.36f, 1.0f);  // #F0F0F0

        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.3f, 0.3f, 0.4f, 1.0f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.1f, 0.1f, 0.2f, 1.0f);
    }

    inline void ApplyPrimaryButton() {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.48f, 0.8f, 1.0f));   // #007ACC
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.09f, 0.62f, 1.0f, 1.0f)); // Hover
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.38f, 0.63f, 1.0f));  // Active
    }

    inline void ApplyDangerButton() {  
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.85f, 0.33f, 0.31f, 1.0f));  // #D9534F
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.95f, 0.43f, 0.41f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.75f, 0.23f, 0.21f, 1.0f));
    }
    inline void ApplyFont(ImGuiIO& io) {
        
        defaultFont = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/arial.ttf", 16.0f);
        headingFont = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/arial.ttf", 24.0f);
        subheadingFont = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/arial.ttf", 20.0f);

    }
    inline void Revert(int colorsPopped = 3) {
        ImGui::PopStyleColor(colorsPopped);
    }
    
}