#pragma once

#include "imgui_internal.h"
#include "imgui.h"

#include <unordered_map>

namespace ImGui {

    class ColorAnimation
    {
    public:
        ColorAnimation();
        ColorAnimation(float base[4], float animated[4], float lastStepNumber = 20);

        void Update();

        float Base[4];
        float Animated[4];
        float Current[4];
        int LastStepNumber = 20;

        int StepNumber = 0;
        bool GoingUp = true;

    private:
        float Interpolate(float startValue, float endValue, int stepNumber, int lastStepNumber);
    };

    class SingleFAnimation
    {
    public:
        SingleFAnimation();
        SingleFAnimation(float base, float animated, float lastStepNumber = 20);

        void Update();

        float Base;
        float Animated;
        float Current;
        int LastStepNumber = 20;

        int StepNumber = 0;
        bool GoingUp = true;
        bool Done = true;
    private:
        float Interpolate(float startValue, float endValue, int stepNumber, int lastStepNumber);
    };


    class AnimatedSlider {
    public:
        AnimatedSlider();
        ColorAnimation m_Animation;
        ColorAnimation m_AnimationText;

        SingleFAnimation m_CircleAnimation;

        bool Render(const char* label, float* v, float v_min, float v_max, bool disabled = false, const char* format = "%.2f", float power = 1.0f);
    };

    class AnimatedSliderInt {
    public:
        AnimatedSliderInt();
        ColorAnimation m_Animation;
        ColorAnimation m_AnimationText;

        SingleFAnimation m_CircleAnimation;

        bool Render(const char* label, int* v, int v_min, int v_max, bool disabled = false, const char* format = "%d");
    };

    class UI {
    public:
        static IMGUI_API bool          SliderFloatAnimated(const char* label, float* v, float v_min, float v_max, const char* format = "%.3f", bool disabled = false, ImGuiSliderFlags flags = 0);
        static IMGUI_API bool          SliderIntAnimated(const char* label, int* v, int v_min, int v_max, const char* format = "%d", bool disabled = false, ImGuiSliderFlags flags = 0);

        static IMGUI_API bool          SliderFloat(SingleFAnimation* animation, const char* label, float* v, float v_min, float v_max, const char* format = "%.3f", bool disabled = false, ImGuiSliderFlags flags = 0);
        static IMGUI_API bool          SliderInt(SingleFAnimation* animation, const char* label, int* v, int v_min, int v_max, const char* format = "%d", bool disabled = false, ImGuiSliderFlags flags = 0);
        static IMGUI_API bool          SliderScalar(SingleFAnimation* animation, const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format = NULL, bool disabled = false, ImGuiSliderFlags flags = 0);
    private:
        inline static std::unordered_map<const char*, AnimatedSlider> animatedSliders;
        inline static std::unordered_map<const char*, AnimatedSliderInt> animatedIntSliders;
    };
}