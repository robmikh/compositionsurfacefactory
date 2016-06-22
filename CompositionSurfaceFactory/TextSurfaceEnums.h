#pragma once

namespace Robmikh
{
namespace CompositionSurfaceFactory
{
    public enum class TextHorizontalAlignment
    {
        Left = 0,
        Right = 1,
        Center = 2,
        Justified = 3
    };

    public enum class TextVerticalAlignment
    {
        Top = 0,
        Bottom = 1,
        Center = 2
    };

    public enum class WordWrapping
    {
        Wrap = 0,
        NoWrap = 1,
        EmergencyBreak = 2,
        WholeWord = 3,
        Character = 4
    };

    public value struct Padding
    {
    public:
        float Left;
        float Right;
        float Top;
        float Bottom;
    };
}
}