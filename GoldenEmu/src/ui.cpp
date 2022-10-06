#include "ui.h"

static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }

static const char* PatchFormatStringFloatToInt(const char* fmt)
{
    if (fmt[0] == '%' && fmt[1] == '.' && fmt[2] == '0' && fmt[3] == 'f' && fmt[4] == 0) // Fast legacy path for "%.0f" which is expected to be the most common case.
        return "%d";
    const char* fmt_start = ImParseFormatFindStart(fmt);    // Find % (if any, and ignore %%)
    const char* fmt_end = ImParseFormatFindEnd(fmt_start);  // Find end of format specifier, which itself is an exercise of confidence/recklessness (because snprintf is dependent on libc or user).
    if (fmt_end > fmt_start && fmt_end[-1] == 'f')
    {
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
        if (fmt_start == fmt && fmt_end[0] == 0)
            return "%d";
        const char* tmp_format;
        ImFormatStringToTempBuffer(&tmp_format, NULL, "%.*s%%d%s", (int)(fmt_start - fmt), fmt, fmt_end); // Honor leading and trailing decorations, but lose alignment/precision.
        return tmp_format;
#else
        IM_ASSERT(0 && "DragInt(): Invalid format string!"); // Old versions used a default parameter of "%.0f", please replace with e.g. "%d"
#endif
    }
    return fmt;
}

ImGui::SingleFAnimation::SingleFAnimation()
{
    Base = 0.0f;

    Animated = 1.0f;

    LastStepNumber = 20;
}
ImGui::SingleFAnimation::SingleFAnimation(float base, float animated, float lastStepNumber)
{
    Base = base;

    Animated = animated;

    LastStepNumber = lastStepNumber;
}
void ImGui::SingleFAnimation::Update()
{
    if (GoingUp && StepNumber <= LastStepNumber)
    {
        Current = Interpolate(Animated, Base, LastStepNumber - StepNumber, LastStepNumber);
        StepNumber++;
        Done = false;
    }
    else if (!GoingUp && StepNumber >= 0)
    {
        Current = Interpolate(Base, Animated, StepNumber, LastStepNumber);
        StepNumber--;
        Done = false;
    }
    else {
        Done = true;
    }
}

ImGui::ColorAnimation::ColorAnimation()
{
    Base[0] = ImGui::GetStyleColorVec4(ImGuiCol_FrameBg).x;
    Base[1] = ImGui::GetStyleColorVec4(ImGuiCol_FrameBg).y;
    Base[2] = ImGui::GetStyleColorVec4(ImGuiCol_FrameBg).z;
    Base[3] = 1.0f;

    Animated[0] = ImGui::GetStyleColorVec4(ImGuiCol_CheckMark).x;
    Animated[1] = ImGui::GetStyleColorVec4(ImGuiCol_CheckMark).y;
    Animated[2] = ImGui::GetStyleColorVec4(ImGuiCol_CheckMark).z;
    Animated[3] = 1.0f;
}

ImGui::ColorAnimation::ColorAnimation(float base[4], float animated[4], float lastStepNumber)
{
    Base[0] = base[0];
    Base[1] = base[1];
    Base[2] = base[2];
    Base[3] = base[3];

    Animated[0] = animated[0];
    Animated[1] = animated[1];
    Animated[2] = animated[2];
    Animated[3] = animated[3];

    LastStepNumber = lastStepNumber;
}

void ImGui::ColorAnimation::Update()
{
    if (GoingUp && StepNumber <= LastStepNumber)
    {
        Current[0] = Interpolate(Animated[0], Base[0], LastStepNumber - StepNumber, LastStepNumber);
        Current[1] = Interpolate(Animated[1], Base[1], LastStepNumber - StepNumber, LastStepNumber);
        Current[2] = Interpolate(Animated[2], Base[2], LastStepNumber - StepNumber, LastStepNumber);
        Current[3] = Interpolate(Animated[3], Base[3], LastStepNumber - StepNumber, LastStepNumber);
        StepNumber++;
    }
    else if (!GoingUp && StepNumber >= 0)
    {
        Current[0] = Interpolate(Base[0], Animated[0], StepNumber, LastStepNumber);
        Current[1] = Interpolate(Base[1], Animated[1], StepNumber, LastStepNumber);
        Current[2] = Interpolate(Base[2], Animated[2], StepNumber, LastStepNumber);
        Current[3] = Interpolate(Base[3], Animated[3], StepNumber, LastStepNumber);
        StepNumber--;
    }
}

float ImGui::ColorAnimation::Interpolate(float startValue, float endValue, int stepNumber, int lastStepNumber)
{
    return (endValue - startValue) * stepNumber / lastStepNumber + startValue;
}


float ImGui::SingleFAnimation::Interpolate(float startValue, float endValue, int stepNumber, int lastStepNumber)
{
    return (endValue - startValue) * stepNumber / lastStepNumber + startValue;
}

IMGUI_API bool ImGui::UI::SliderFloatAnimated(const char* label, float* v, float v_min, float v_max, const char* format, bool disabled, ImGuiSliderFlags flags)
{
    return animatedSliders[label].Render(label, v, v_min, v_max, disabled, format);
}

IMGUI_API bool ImGui::UI::SliderIntAnimated(const char* label, int* v, int v_min, int v_max, const char* format, bool disabled, ImGuiSliderFlags flags)
{
    return animatedIntSliders[label].Render(label, v, v_min, v_max, disabled, format);
}

IMGUI_API bool ImGui::UI::SliderFloat(SingleFAnimation* animation, const char* label, float* v, float v_min, float v_max, const char* format, bool disabled, ImGuiSliderFlags flags)
{
    return SliderScalar(animation, label, ImGuiDataType_Float, v, &v_min, &v_max, format, disabled, flags);
}

IMGUI_API bool ImGui::UI::SliderInt(SingleFAnimation* animation, const char* label, int* v, int v_min, int v_max, const char* format, bool disabled, ImGuiSliderFlags flags)
{
    return SliderScalar(animation, label, ImGuiDataType_S32, v, &v_min, &v_max, format, disabled, flags);
}

IMGUI_API bool ImGui::UI::SliderScalar(SingleFAnimation* animation, const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, bool disabled, ImGuiSliderFlags flags)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const float w = CalcItemWidth();

    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f));
    const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));
    frame_bb.Min.y += label_size.y + 3;
    frame_bb.Max.y += label_size.y + 3;


    const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;
    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id, &frame_bb, temp_input_allowed ? ImGuiItemFlags_Inputable : 0))
        return false;

    // Default format string when passing NULL
    if (format == NULL)
        format = DataTypeGetInfo(data_type)->PrintFmt;
    else if (data_type == ImGuiDataType_S32 && strcmp(format, "%d") != 0) // (FIXME-LEGACY: Patch old "%.0f" format string to use "%d", read function more details.)
        format = PatchFormatStringFloatToInt(format);

    const bool hovered = ItemHoverable(frame_bb, id);
    bool temp_input_is_active = temp_input_allowed && TempInputIsActive(id);
    if (!temp_input_is_active)
    {
        // Tabbing or CTRL-clicking on Slider turns it into an input box
        const bool input_requested_by_tabbing = temp_input_allowed && (g.LastItemData.StatusFlags & ImGuiItemStatusFlags_FocusedByTabbing) != 0;
        const bool clicked = (hovered && g.IO.MouseClicked[0]);
        const bool make_active = (input_requested_by_tabbing || clicked || g.NavActivateId == id || g.NavActivateInputId == id);
        if (make_active && temp_input_allowed)
            if (input_requested_by_tabbing || (clicked && g.IO.KeyCtrl) || g.NavActivateInputId == id)
                temp_input_is_active = true;

        if (make_active && !temp_input_is_active)
        {
            SetActiveID(id, window);
            SetFocusID(id, window);
            FocusWindow(window);
            g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
        }
    }

    if (temp_input_is_active)
    {
        // Only clamp CTRL+Click input when ImGuiSliderFlags_AlwaysClamp is set
        const bool is_clamp_input = (flags & ImGuiSliderFlags_AlwaysClamp) != 0;
        return TempInputScalar(frame_bb, id, label, data_type, p_data, format, is_clamp_input ? p_min : NULL, is_clamp_input ? p_max : NULL);
    }


    // Slider behavior
    ImRect grab_bb;
    const bool value_changed = SliderBehavior(frame_bb, id, data_type, p_data, p_min, p_max, format, flags, &grab_bb);
    if (value_changed)
        MarkItemEdited(id);

    // Draw frame
    RenderNavHighlight(frame_bb, id);
    window->DrawList->AddLine({ frame_bb.Min.x, frame_bb.Min.y + (frame_bb.Max.y - frame_bb.Min.y) / 2 }, { frame_bb.Max.x, frame_bb.Min.y + (frame_bb.Max.y - frame_bb.Min.y) / 2 }, GetColorU32(ImGuiCol_FrameBg), 3.f);
    window->DrawList->AddLine({ frame_bb.Min.x, frame_bb.Min.y + (frame_bb.Max.y - frame_bb.Min.y) / 2 }, { grab_bb.Max.x, frame_bb.Min.y + (frame_bb.Max.y - frame_bb.Min.y) / 2 }, GetColorU32(ImGuiCol_SliderGrab), 3.f);
    //RenderFrame(frame_bb.Min, frame_bb.Max, GetColorU32(ImGuiCol_ControlBg), true, g.Style.FrameRounding);

    // Render grab
    if (grab_bb.Max.x > grab_bb.Min.x)
    {
        ImVec4 col = GetStyleColorVec4(ImGuiCol_SliderGrab);
        col.w = 1.0f;
        window->DrawList->AddCircleFilled({ grab_bb.Max.x, frame_bb.Min.y + (frame_bb.Max.y - frame_bb.Min.y) / 2 }, 7.f, ColorConvertFloat4ToU32(col));

        animation->GoingUp = g.ActiveId == id;
        col.w = 0.3f;
        window->DrawList->AddCircleFilled({ grab_bb.Max.x, frame_bb.Min.y + (frame_bb.Max.y - frame_bb.Min.y) / 2 }, animation->Current, ColorConvertFloat4ToU32(col));
        // slider grab
        //window->DrawList->AddRectFilled(grab_bb.Min, grab_bb.Max, GetColorU32(g.ActiveId == id ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab), style.FrameRounding);
        // gradient
        //window->DrawList->AddRectFilledMultiColor(grab_bb.Min, grab_bb.Max, GetColorU32(ImVec4(0.0f, 0.0f, 0.0f, 0.05f)), GetColorU32(ImVec4(0.0f, 0.0f, 0.0f, 0.05f)), GetColorU32(ImVec4(0.0f, 0.0f, 0.0f, 0.38f)), GetColorU32(ImVec4(0.0f, 0.0f, 0.0f, 0.38f)));
    }

    // Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
    char value_buf[64];
    const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);
    RenderText(ImVec2(frame_bb.Max.x - CalcTextSize(value_buf).x - 1.0f, total_bb.Min.y + style.FramePadding.y), value_buf, value_buf_end);

    if (label_size.x > 0.0f)
        RenderText(ImVec2(frame_bb.Min.x, total_bb.Min.y + style.FramePadding.y), label);

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
    return value_changed;
}

ImGui::AnimatedSlider::AnimatedSlider()
{
    float base[4] = {
        ImGui::GetStyleColorVec4(ImGuiCol_FrameBg).x,
        ImGui::GetStyleColorVec4(ImGuiCol_FrameBg).y,
        ImGui::GetStyleColorVec4(ImGuiCol_FrameBg).z,
        1.0f };
    float animated[4] = {
        ImGui::GetStyleColorVec4(ImGuiCol_SliderGrab).x,
        ImGui::GetStyleColorVec4(ImGuiCol_SliderGrab).y,
        ImGui::GetStyleColorVec4(ImGuiCol_SliderGrab).z,
        1.0f };

    m_Animation = ColorAnimation(base, animated);

    float baseText[4] = {
        ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled).x,
        ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled).y,
        ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled).z,
        1.0f };
    float animatedText[4] = {
        ImGui::GetStyleColorVec4(ImGuiCol_Text).x,
        ImGui::GetStyleColorVec4(ImGuiCol_Text).y,
        ImGui::GetStyleColorVec4(ImGuiCol_Text).z,
        1.0f };

    m_AnimationText = ColorAnimation(baseText, animatedText);

    m_CircleAnimation = SingleFAnimation(7.0f, 12.f, 15.f);
}

bool ImGui::AnimatedSlider::Render(const char* label, float* v, float v_min, float v_max, bool disabled, const char* format, float power)
{
    m_Animation.GoingUp = !disabled;
    m_Animation.Update();
    m_AnimationText.GoingUp = !disabled;
    m_AnimationText.Update();


    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(m_AnimationText.Current[0], m_AnimationText.Current[1], m_AnimationText.Current[2], m_AnimationText.Current[3]));
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(m_Animation.Current[0], m_Animation.Current[1], m_Animation.Current[2], m_Animation.Current[3]));
    if (disabled) {
        ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(m_Animation.Current[0], m_Animation.Current[1], m_Animation.Current[2], m_Animation.Current[3]));
    }
    bool ret = ImGui::UI::SliderFloat(&m_CircleAnimation, label, v, v_min, v_max, format, power);
    m_CircleAnimation.Update();
    ImGui::PopStyleColor(disabled ? 3 : 2);

    return ret;
}

ImGui::AnimatedSliderInt::AnimatedSliderInt()
{
    float base[4] = {
    ImGui::GetStyleColorVec4(ImGuiCol_FrameBg).x,
    ImGui::GetStyleColorVec4(ImGuiCol_FrameBg).y,
    ImGui::GetStyleColorVec4(ImGuiCol_FrameBg).z,
    1.0f };
    float animated[4] = {
        ImGui::GetStyleColorVec4(ImGuiCol_SliderGrab).x,
        ImGui::GetStyleColorVec4(ImGuiCol_SliderGrab).y,
        ImGui::GetStyleColorVec4(ImGuiCol_SliderGrab).z,
        1.0f };

    m_Animation = ColorAnimation(base, animated);

    float baseText[4] = {
        ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled).x,
        ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled).y,
        ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled).z,
        1.0f };
    float animatedText[4] = {
        ImGui::GetStyleColorVec4(ImGuiCol_Text).x,
        ImGui::GetStyleColorVec4(ImGuiCol_Text).y,
        ImGui::GetStyleColorVec4(ImGuiCol_Text).z,
        1.0f };

    m_AnimationText = ColorAnimation(baseText, animatedText);

    m_CircleAnimation = SingleFAnimation(7.0f, 12.f, 15.f);
}

bool ImGui::AnimatedSliderInt::Render(const char* label, int* v, int v_min, int v_max, bool disabled, const char* format)
{
    m_Animation.GoingUp = !disabled;
    m_Animation.Update();
    m_AnimationText.GoingUp = !disabled;
    m_AnimationText.Update();

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(m_AnimationText.Current[0], m_AnimationText.Current[1], m_AnimationText.Current[2], m_AnimationText.Current[3]));
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(m_Animation.Current[0], m_Animation.Current[1], m_Animation.Current[2], m_Animation.Current[3]));
    if (disabled) {
        ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(m_Animation.Current[0], m_Animation.Current[1], m_Animation.Current[2], m_Animation.Current[3]));
    }
    bool ret = ImGui::UI::SliderInt(&m_CircleAnimation, label, v, v_min, v_max, format);
    m_CircleAnimation.Update();
    ImGui::PopStyleColor(disabled ? 3 : 2);

    return ret;
}
