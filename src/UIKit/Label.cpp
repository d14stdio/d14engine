#include "Common/Precompile.h"

#include "UIKit/Label.h"

#include "Common/DirectXError.h"
#include "Common/MathUtils/2D.h"

#include "UIKit/ResourceUtils.h"

using namespace d14engine::renderer;

namespace d14engine::uikit
{
    Label::Label(WstrRefer text, const D2D_RECT_F& rect)
        :
        Panel(rect, resource_utils::solidColorBrush()),
        m_text(text)
    {
        // Here left blank intentionally.
    }

    void Label::initialize()
    {
        Panel::initialize();

        /////////////////
        // Text Layout //
        /////////////////

        TextLayoutParams layoutParams =
        {
            .text               = std::nullopt,
            .textFormat         = D14_FONT(defaultTextFormatName),
            .maxWidth           = std::nullopt,
            .maxHeight          = std::nullopt,
            .incrementalTabStop = 4.0f * 96.0f / 72.0f,
            .wordWrapping       = DWRITE_WORD_WRAPPING_NO_WRAP,
            .alignment          =
            {
                .text      = DWRITE_TEXT_ALIGNMENT_LEADING,
                .paragraph = DWRITE_PARAGRAPH_ALIGNMENT_CENTER,
            }
        };
        m_textLayout = getTextLayout(layoutParams);

        ////////////////////
        // Text Overhangs //
        ////////////////////

        updateTextOverhangs();
    }

    void Label::onTextLayoutChange()
    {
        onTextLayoutChangeHelper();

        if (f_onTextLayoutChange) f_onTextLayoutChange(this);
    }

    void Label::onTextLayoutChangeHelper()
    {
        // This method intentionally left blank.
    }

    Optional<Wstring> Label::normalizeRawText(WstrRefer text)
    {
        return std::nullopt;
    }

    const Wstring& Label::text() const
    {
        return m_text;
    }

    void Label::setText(WstrRefer text)
    {
        setTextHelper(text);
    }

    void Label::insertText(WstrRefer text, size_t offset)
    {
        insertTextHelper(text, offset);
    }

    void Label::appendText(WstrRefer text)
    {
        appendTextHelper(text);
    }

    void Label::eraseText(const CharacterRange& range)
    {
        eraseTextHelper(range);
    }

    bool Label::setTextHelper(WstrRefer text)
    {
        auto result = normalizeRawText(text);
        auto& source = result.has_value() ? result.value() : text;

        if (m_text != source)
        {
            m_text = source;
        }
        else return false;

        m_textLayout = getTextLayout();
        updateTextOverhangs();

        onTextLayoutChange();
        return true;
    }

    bool Label::insertTextHelper(WstrRefer text, size_t offset)
    {
        auto result = normalizeRawText(text);
        auto& source = result.has_value() ? result.value() : text;

        auto _Off = std::min(offset, m_text.size());

        if (!source.empty())
        {
            m_text.insert
            (
            /* _Off   */ _Off,
            /* _Ptr   */ source.data(),
            /* _Count */ source.size()
            );
        }
        else return false;

        m_textLayout = getTextLayout();
        updateTextOverhangs();

        onTextLayoutChange();
        return true;
    }

    bool Label::appendTextHelper(WstrRefer text)
    {
        return insertTextHelper(text, m_text.size());
    }

    bool Label::eraseTextHelper(const CharacterRange& range)
    {
        auto offset = std::min
        (
            range.offset,
            m_text.size()
        );
        auto count = std::min
        (
            range.count,
            m_text.size() - range.offset
        );
        if (count != 0)
        {
            m_text.erase(offset, count);

            m_textLayout = getTextLayout();
            updateTextOverhangs();

            onTextLayoutChange();
            return true;
        }
        else return false;
    }

    Wstring Label::defaultTextFormatName = L"Default/16";

    void Label::setTextFormat(IDWriteTextFormat* textFormat)
    {
        m_textLayout = getTextLayout
        ({
            .textFormat = textFormat
        });
        updateTextOverhangs();

        onTextLayoutChange();
    }

    void Label::copyTextStyle(Label* source, OptRefer<WstringView> text)
    {
        if (source != nullptr)
        {
            appearance() = source->appearance();

            ComPtr<IDWriteTextFormat> textFormat = {};
            source->m_textLayout.As(&textFormat);

            if (text.has_value())
            {
                m_text = text.value();
            }
            TextLayoutParams layoutParams =
            {
                .text               = text,
                .textFormat         = textFormat.Get(),
                .maxWidth           = std::nullopt,
                .maxHeight          = std::nullopt,
                .incrementalTabStop = source->m_textLayout->GetIncrementalTabStop(),
                .wordWrapping       = source->m_textLayout->GetWordWrapping(),
                .alignment          =
                {
                    .text      = source->m_textLayout->GetTextAlignment(),
                    .paragraph = source->m_textLayout->GetParagraphAlignment()
                }
            };
            m_textLayout = getTextLayout(layoutParams);

#define COPY_TEXT_LAYOUT_FONT_ATTR(Property_Name) \
do { \
    auto& src = source->m_textLayout; \
    decltype(src->GetFont##Property_Name()) value = {}; \
    if (FAILED(src->GetFont##Property_Name(0, &value))) \
    { \
        value = src->GetFont##Property_Name(); \
    } \
    m_textLayout->SetFont##Property_Name(value, { 0, (UINT32)m_text.size() }); \
} while (0)
            // Copy the format of the 1st character by default.

            COPY_TEXT_LAYOUT_FONT_ATTR(Size);
            COPY_TEXT_LAYOUT_FONT_ATTR(Weight);
            COPY_TEXT_LAYOUT_FONT_ATTR(Style);
            COPY_TEXT_LAYOUT_FONT_ATTR(Stretch);

#undef COPY_TEXT_LAYOUT_FONT_ATTR

            updateTextOverhangs();

            drawTextOptions = source->drawTextOptions;

            onTextLayoutChange();
        }
    }

    IDWriteTextLayout* Label::textLayout() const
    {
        return m_textLayout.Get();
    }

    ComPtr<IDWriteTextLayout> Label::getTextLayout(const TextLayoutParams& params) const
    {
        THROW_IF_NULL(Application::g_app);

        auto string = params.text.has_value() ? params.text.value().data() : m_text.data();
        auto stringLength = (UINT32)(params.text.has_value() ? params.text.value().size() : m_text.size());

        ComPtr<IDWriteTextFormat> textFormat = params.textFormat;
        if (textFormat == nullptr)
        {
            THROW_IF_FAILED(m_textLayout.As(&textFormat));
        }
        FLOAT maxWidth = params.maxWidth.has_value() ? params.maxWidth.value() :
            ((m_textLayout != nullptr) ? m_textLayout->GetMaxWidth() : width());

        FLOAT maxHeight = params.maxHeight.has_value() ? params.maxHeight.value() :
            ((m_textLayout != nullptr) ? m_textLayout->GetMaxHeight() : height());

        ComPtr<IDWriteTextLayout> textLayout = {};
        THROW_IF_FAILED(Application::g_app->renderer()->dwriteFactory()->CreateTextLayout
        (
        /* string       */ string,
        /* stringLength */ stringLength,
        /* textFormat   */ textFormat.Get(),
        /* maxWidth     */ maxWidth,
        /* maxHeight    */ maxHeight,
        /* textLayout   */ &textLayout)
        );
        THROW_IF_FAILED(textLayout->SetIncrementalTabStop
        (
            params.incrementalTabStop.has_value() ?
            params.incrementalTabStop.value() :
            m_textLayout->GetIncrementalTabStop()
        ));
        THROW_IF_FAILED(textLayout->SetTextAlignment
        (
            params.alignment.text.has_value() ?
            params.alignment.text.value() :
            m_textLayout->GetTextAlignment()
        ));
        THROW_IF_FAILED(textLayout->SetParagraphAlignment
        (
            params.alignment.paragraph.has_value() ?
            params.alignment.paragraph.value() :
            m_textLayout->GetParagraphAlignment()
        ));
        THROW_IF_FAILED(textLayout->SetWordWrapping
        (
            params.wordWrapping.has_value() ?
            params.wordWrapping.value() :
            m_textLayout->GetWordWrapping()
        ));
        return textLayout;
    }

    DWRITE_TEXT_METRICS Label::textMetrics() const
    {
        DWRITE_TEXT_METRICS metrics = {};
        THROW_IF_FAILED(m_textLayout->GetMetrics(&metrics));
        return metrics;
    }

    DWRITE_TEXT_METRICS Label::getTextMetrics(const TextMetricsParams& params) const
    {
        DWRITE_TEXT_METRICS metrics = {};
        THROW_IF_FAILED(getTextLayout(params)->GetMetrics(&metrics));
        return metrics;
    }

    const DWRITE_OVERHANG_METRICS& Label::textOverhangs() const
    {
        return m_textOverhangs;
    }

    void Label::updateTextOverhangs()
    {
        THROW_IF_FAILED(m_textLayout->GetOverhangMetrics(&m_textOverhangs));
    }

    D2D1_SIZE_F Label::textAreaSize() const
    {
        return
        {
            m_textLayout->GetMaxWidth() + m_textOverhangs.left + m_textOverhangs.right,
            m_textLayout->GetMaxHeight() + m_textOverhangs.top + m_textOverhangs.bottom
        };
    }

    Label::PointHitTestResult Label::hitTestPoint
    (
        FLOAT pointX, FLOAT pointY
    ){
        PointHitTestResult result = {};
        THROW_IF_FAILED(m_textLayout->HitTestPoint
        (
        /* pointX         */ pointX,
        /* pointY         */ pointY,
        /* isTrailingHit  */ &result.isTrailingHit,
        /* isInside       */ &result.isInside,
        /* hitTestMetrics */ &result.metrics
        ));
        return result;
    }

    Label::TextPositionHitTestResult Label::hitTestTextPosition
    (
        UINT32 textPosition,
        BOOL isTrailingHit
    ){
        TextPositionHitTestResult result = {};
        THROW_IF_FAILED(m_textLayout->HitTestTextPosition
        (
        /* textPosition   */ textPosition,
        /* isTrailingHit  */ isTrailingHit,
        /* pointX         */ &result.pointX,
        /* pointY         */ &result.pointY,
        /* hitTestMetrics */ &result.metrics
        ));
        return result;
    }

    Label::TextRangeHitTestResult Label::hitTestTextRange
    (
        UINT32 textPosition, UINT32 textLength,
        FLOAT originX, FLOAT originY
    ){
        UINT32 count = {};
        HRESULT hr = {};
        hr = m_textLayout->HitTestTextRange
        (
        /* _                         */ textPosition,
        /* _                         */ textLength,
        /* _                         */ originX,
        /* _                         */ originY,
        /* hitTestMetrics            */ nullptr,
        /* maxHitTestMetricsCount    */ 0,
        /* actualHitTestMetricsCount */ &count
        );
        if (hr != E_NOT_SUFFICIENT_BUFFER)
        {
            THROW_ERROR(L"Unexpected calling result.");
        }
        TextRangeHitTestResult result = { count };
        THROW_IF_FAILED(m_textLayout->HitTestTextRange
        (
        /* _                         */ textPosition,
        /* _                         */ textLength,
        /* _                         */ originX,
        /* _                         */ originY,
        /* hitTestMetrics            */ result.metrics.data(),
        /* maxHitTestMetricsCount    */ (UINT32)result.metrics.size(),
        /* actualHitTestMetricsCount */ &count
        ));
        return result;
    }

    void Label::onRendererDrawD2d1ObjectHelper(Renderer* rndr)
    {
        drawBackground(rndr);
        drawTextLayout(rndr);
        drawOutline(rndr);
    }

    void Label::drawBackground(Renderer* rndr)
    {
        auto& background = appearance().background;

        resource_utils::solidColorBrush()->SetColor(background.color);
        resource_utils::solidColorBrush()->SetOpacity(background.opacity);

        Panel::drawBackground(rndr);
    }

    void Label::drawTextLayout(Renderer* rndr)
    {
        auto& foreground = appearance().foreground;
        if (!m_enabled)
        {
            foreground = appearance().secondaryForeground;
        }
        resource_utils::solidColorBrush()->SetColor(foreground.color);
        resource_utils::solidColorBrush()->SetOpacity(foreground.opacity);

        auto origin = absolutePosition();
        switch (hardAlignment.horz)
        {
        case HorzAlignment::Left:
        {
            origin.x = std::round(origin.x + m_textOverhangs.left);
            break;
        }
        case HorzAlignment::Center:
        {
            auto offset = (width() - textAreaSize().width) * 0.5f;
            origin.x = std::round(origin.x + m_textOverhangs.left + offset);
            break;
        }
        case HorzAlignment::Right:
        {
            origin.x = std::round(origin.x - m_textOverhangs.right);
            break;
        }
        default: /* HorzAlignment::None */ break;
        }
        switch (hardAlignment.vert)
        {
        case VertAlignment::Top:
        {
            origin.y = std::round(origin.y + m_textOverhangs.top);
            break;
        }
        case VertAlignment::Center:
        {
            auto offset = (height() - textAreaSize().height) * 0.5f;
            origin.y = std::round(origin.y + m_textOverhangs.top + offset);
            break;
        }
        case VertAlignment::Bottom:
        {
            origin.y = std::round(origin.y - m_textOverhangs.bottom);
            break;
        }
        default: /* VertAlignment::None */ break;
        }
        rndr->d2d1DeviceContext()->DrawTextLayout
        (
        /* origin           */ origin,
        /* textLayout       */ m_textLayout.Get(),
        /* defaultFillBrush */ resource_utils::solidColorBrush(),
        /* options          */ drawTextOptions
        );
    }

    void Label::drawOutline(Renderer* rndr)
    {
        auto& stroke = appearance().stroke;

        resource_utils::solidColorBrush()->SetColor(stroke.color);
        resource_utils::solidColorBrush()->SetOpacity(stroke.opacity);

        auto frame = math_utils::inner(m_absoluteRect, stroke.width);
        D2D1_ROUNDED_RECT roundedRect = { frame, roundRadiusX, roundRadiusY };

        rndr->d2d1DeviceContext()->DrawRoundedRectangle
        (
        /* roundedRect */ roundedRect,
        /* brush       */ resource_utils::solidColorBrush(),
        /* strokeWidth */ stroke.width
        );
    }

    void Label::onSizeHelper(SizeEvent& e)
    {
        Panel::onSizeHelper(e);

        THROW_IF_FAILED(m_textLayout->SetMaxWidth(e.size.width));
        THROW_IF_FAILED(m_textLayout->SetMaxHeight(e.size.height));

        updateTextOverhangs();
    }

    void Label::onChangeThemeStyleHelper(const ThemeStyle& style)
    {
        Panel::onChangeThemeStyleHelper(style);

        appearance().changeTheme(style.name);
    }
}
