#pragma once

#include "Common/Precompile.h"

#include "UIKit/Appearances/Label.h"
#include "UIKit/Panel.h"

namespace d14engine::uikit
{
    struct Label : appearance::Label, Panel
    {
        Label(WstrRefer text = {}, const D2D_RECT_F& rect = {});

        void initialize() override;

        _D14_SET_APPEARANCE_PROPERTY(Label)

        ////////////////////////
        // Callback Functions //
        ////////////////////////

        //------------------------------------------------------------------
        // Public Interfaces
        //------------------------------------------------------------------
    public:
        // Note: This callback function triggers ONLY when
        // the m_textLayout itself is replaced (m_textLayout = newTextLayout).
        // Therefore, updates to m_textLayout's properties
        // (m_textLayout->setFontSize(n, range) etc.) require manual handling.

        void onTextLayoutChange();

        Function<void(Label*)> f_onTextLayoutChange = {};

        //------------------------------------------------------------------
        // Protected Helpers
        //------------------------------------------------------------------
    protected:
        virtual void onTextLayoutChangeHelper();

        // This function will be called at the beginning of setText.
        // Return nullopt to have setText use the raw text directly.
        // Example: "raw\ntext" -> normalize -> "rawtext" -> setText
        virtual Optional<Wstring> normalizeRawText(WstrRefer text);

        //////////////////////////
        // Graphical Components //
        //////////////////////////

        //------------------------------------------------------------------
        // Text Content
        //------------------------------------------------------------------
    protected:
        Wstring m_text = {};

    public:
        const Wstring& text() const;
        virtual void setText(WstrRefer text);

        void insertText(WstrRefer text, size_t offset);
        void appendText(WstrRefer text);

        struct CharacterRange { size_t offset, count; };

        void eraseText(const CharacterRange& range);

    protected:
        // These helpers return whether m_text is changed after called.

        bool setTextHelper(WstrRefer text);

        bool insertTextHelper(WstrRefer text, size_t offset);
        bool appendTextHelper(WstrRefer text);

        bool eraseTextHelper(const CharacterRange& range);

        //------------------------------------------------------------------
        // Text Format
        //------------------------------------------------------------------
    public:
        static Wstring defaultTextFormatName;

        void setTextFormat(IDWriteTextFormat* textFormat);

        void copyTextStyle(Label* source, OptRefer<WstringView> text = {});

        //------------------------------------------------------------------
        // Text Layout
        //------------------------------------------------------------------
    protected:
        ComPtr<IDWriteTextLayout> m_textLayout = {};

    public:
        IDWriteTextLayout* textLayout() const;

        struct TextLayoutParams
        {
            Optional<WstringView> text = {};

            IDWriteTextFormat* textFormat = nullptr;

            Optional<float> maxWidth = {};
            Optional<float> maxHeight = {};

            Optional<float> incrementalTabStop = {};
            Optional<DWRITE_WORD_WRAPPING> wordWrapping = {};

            struct Alignment
            {
                Optional<DWRITE_TEXT_ALIGNMENT> text = {};
                Optional<DWRITE_PARAGRAPH_ALIGNMENT> paragraph = {};
            }
            alignment = {};
        };
        ComPtr<IDWriteTextLayout> getTextLayout
        (const TextLayoutParams& params = {}) const;

        //------------------------------------------------------------------
        // Text Metrics
        //------------------------------------------------------------------
    public:
        DWRITE_TEXT_METRICS textMetrics() const;

        using TextMetricsParams = TextLayoutParams;

        DWRITE_TEXT_METRICS getTextMetrics
        (const TextMetricsParams& params = {}) const;

        //------------------------------------------------------------------
        // Text Overhangs
        //------------------------------------------------------------------
    protected:
        DWRITE_OVERHANG_METRICS m_textOverhangs = {};

    public:
        const DWRITE_OVERHANG_METRICS& textOverhangs() const;

        void updateTextOverhangs();

        D2D1_SIZE_F textAreaSize() const;

        //------------------------------------------------------------------
        // Text Alignments
        //------------------------------------------------------------------
    public:
        // Note the difference between HardAlignment and TextLayoutAlignment.
        //
        // 1. HardAlignment:
        //
        //    It performs pixel-based alignment,
        //    causing text boundaries to exactly snap to specified positions.
        //
        // 2. TextLayoutAlignment:
        //
        //    It aligns based on glyph metrics,
        //    which incorporates typographic conventions for better readability.
        //
        // You should evaluate the specific case to decide which one to use.

        enum class HorzAlignment { Left, Right, Center, None };
        enum class VertAlignment { Top, Bottom, Center, None };

#define CONST_ENUM constexpr static auto

        CONST_ENUM Left = HorzAlignment::Left;
        CONST_ENUM Right = HorzAlignment::Right;
        CONST_ENUM HCenter = HorzAlignment::Center;
        CONST_ENUM HNone = HorzAlignment::None;
        CONST_ENUM Top = VertAlignment::Top;
        CONST_ENUM Bottom = VertAlignment::Bottom;
        CONST_ENUM VCenter = VertAlignment::Center;
        CONST_ENUM VNone = VertAlignment::None;

#undef CONST_ENUM

        // Uses the cached overhang metrics to enforce text alignments.
        struct HardAlignment
        {
            HorzAlignment horz = HorzAlignment::None;
            VertAlignment vert = VertAlignment::None;
        }
        hardAlignment = {};

        //------------------------------------------------------------------
        // Drawing Options
        //------------------------------------------------------------------
    public:
        D2D1_DRAW_TEXT_OPTIONS drawTextOptions = D2D1_DRAW_TEXT_OPTIONS_NONE;

        ///////////////////////
        // Interaction Logic //
        ///////////////////////

        //------------------------------------------------------------------
        // Point Hit Test
        //------------------------------------------------------------------
    public:
        struct PointHitTestResult
        {
            BOOL isTrailingHit = {};
            BOOL isInside = {};

            DWRITE_HIT_TEST_METRICS metrics = {};
        };
        PointHitTestResult hitTestPoint
        (
            FLOAT pointX, FLOAT pointY
        );

        //------------------------------------------------------------------
        // Position Hit Test
        //------------------------------------------------------------------
    public:
        struct TextPositionHitTestResult
        {
            FLOAT pointX = {};
            FLOAT pointY = {};

            DWRITE_HIT_TEST_METRICS metrics = {};
        };
        TextPositionHitTestResult hitTestTextPosition
        (
            UINT32 textPosition,
            BOOL isTrailingHit = false
        );

        //------------------------------------------------------------------
        // Range Hit Test
        //------------------------------------------------------------------
    public:
        struct TextRangeHitTestResult
        {
            TextRangeHitTestResult(UINT32 count = 0)
                :
                metrics(count) { }

            std::vector<DWRITE_HIT_TEST_METRICS> metrics = {};
        };
        TextRangeHitTestResult hitTestTextRange
        (
            UINT32 textPosition, UINT32 textLength,
            FLOAT originX = 0.0f, FLOAT originY = 0.0f
        );

        /////////////////////////
        // Interface Overrides //
        /////////////////////////

    protected:
        //------------------------------------------------------------------
        // IDrawObject2D
        //------------------------------------------------------------------

        void onRendererDrawD2d1ObjectHelper(Renderer* rndr) override;

        void drawBackground(Renderer* rndr);
        void drawTextLayout(Renderer* rndr);
        void drawOutline(Renderer* rndr);

        //------------------------------------------------------------------
        // Panel
        //------------------------------------------------------------------

        void onSizeHelper(SizeEvent& e) override;

        void onChangeThemeStyleHelper(const ThemeStyle& style) override;
    };
}
