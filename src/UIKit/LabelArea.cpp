#include "Common/Precompile.h"

#include "UIKit/LabelArea.h"

#include "Common/RuntimeError.h"

#include "Renderer/TickTimer.h"

#include "UIKit/Application.h"
#include "UIKit/Cursor.h"
#include "UIKit/ResourceUtils.h"

using namespace d14engine::renderer;

namespace d14engine::uikit
{
    LabelArea::LabelArea(WstrRefer text, const D2D_RECT_F& rect)
        :
        Label(text, rect)
    {
        // Here left blank intentionally.
    }

    size_t LabelArea::hitTestCaretPosition(const D2D1_POINT_2F& point)
    {
        auto result = hitTestPoint(point.x, point.y);
        auto caretPosition = (size_t)result.metrics.textPosition;
        return result.isTrailingHit ? (caretPosition + 1) : caretPosition;
    }

    size_t LabelArea::caretPosition() const
    {
        return m_caretPosition;
    }

    void LabelArea::setCaretPosition(size_t position)
    {
        m_caretPosition = std::min(position, m_text.size());

        auto result = hitTestTextPosition((UINT32)m_caretPosition);
        m_caretGeometry.first = { result.pointX, result.pointY };
        m_caretGeometry.second = { result.pointX, result.pointY + result.metrics.height };
    }

    const Label::CharacterRange& LabelArea::selectedRange() const
    {
        return m_selectedRange;
    }

    void LabelArea::setSelectedRange(const CharacterRange& range)
    {
        if (m_text.size() != 0)
        {
            m_selectedRange.offset = std::min
            (
                range.offset,
                m_text.size() - 1_uz
            );
            m_selectedRange.count = std::min
            (
                range.count,
                m_text.size() - m_selectedRange.offset
            );
        }
        else  m_selectedRange = { 0, 0 };

        appearance().caret.visible = (m_selectedRange.count == 0);

        m_selectedRangeData = hitTestTextRange
        (
        /* textPosition */ (UINT32)m_selectedRange.offset,
        /* textLength   */ (UINT32)m_selectedRange.count
        );
    }

    void LabelArea::setSelectedText(WstrRefer text)
    {
        setSelectedTextHelper(text);
    }

    bool LabelArea::setSelectedTextHelper(WstrRefer text)
    {
        auto result = normalizeRawText(text);
        auto& source = result.has_value() ? result.value() : text;

        if (m_selectedRange.count > 0)
        {
            auto target = m_text.substr
            (
            /* _Off   */ m_selectedRange.offset,
            /* _Count */ m_selectedRange.count
            );
            if (source != target)
            {
                auto offset = std::min
                (
                    m_selectedRange.offset,
                    m_text.size()
                );
                auto count = std::min
                (
                    m_selectedRange.count,
                    m_text.size() - m_selectedRange.offset
                );
                m_text.erase(offset, count);
                m_text.insert
                (
                /* _Off   */ offset,
                /* _Ptr   */ source.data(),
                /* _Count */ source.size()
                );
                setCaretPosition(m_selectedRange.offset + source.size());
                setSelectedRange({ 0, 0 });

                m_textLayout = getTextLayout();
                updateTextOverhangs();

                onTextLayoutChange();
                return true;
            }
            else return false;
        }
        else // insert after the caret
        {
            if (insertTextHelper(source, m_caretPosition))
            {
                setCaretPosition(m_caretPosition + source.size());
                return true;
            }
            else return false;
        }
    }

    void LabelArea::performCommandSelectAll()
    {
        setCaretPosition(m_text.size());
        setSelectedRange({ 0, m_text.size() });
    }

    void LabelArea::performCommandCopySelection()
    {
        auto selectedText = m_text.substr
        (
        /* _Off   */ m_selectedRange.offset,
        /* _Count */ m_selectedRange.count
        );
        resource_utils::setClipboardText(selectedText);
    }

    void LabelArea::onRendererUpdateObject2DHelper(Renderer* rndr)
    {
        auto deltaSecs = (float)rndr->timer()->deltaSecs();
        auto& blinkingSecs = appearance().caret.animation.blinkingSecs;

        /////////////////
        // Blink Caret //
        /////////////////

        if (holdKeyboardFocus() && (m_caretBlinkingElapsedSecs += deltaSecs) >= blinkingSecs)
        {
            m_caretBlinkingFlag = !m_caretBlinkingFlag;
            m_caretBlinkingElapsedSecs = 0.0f;
        }
    }

    void LabelArea::onRendererDrawD2d1ObjectHelper(Renderer* rndr)
    {
        drawBackground(rndr);
        drawSelection(rndr);
        drawTextLayout(rndr);
        drawCaret(rndr);
        drawOutline(rndr);
    }

    void LabelArea::drawCaret(Renderer* rndr)
    {
        auto& setting = appearance().caret;

        if (setting.visible && m_caretBlinkingFlag)
        {
            auto result0 = math_utils::constrainToY(
                m_caretGeometry.first, caretConstrainedRect);

            auto result1 = math_utils::constrainToY(
                m_caretGeometry.second, caretConstrainedRect);

            if (result0.isOverlapped || result1.isOverlapped)
            {
                resource_utils::solidColorBrush()->SetColor(setting.background.color);
                resource_utils::solidColorBrush()->SetOpacity(setting.background.opacity);

                result0.point = math_utils::offset(math_utils::roundf(
                    selfCoordToAbsolute(result0.point)), { 0.5f, 0.0f });

                result1.point = math_utils::offset(math_utils::roundf(
                    selfCoordToAbsolute(result1.point)), { 0.5f, 0.0f });

                rndr->d2d1DeviceContext()->DrawLine
                (
                /* point0 */ result0.point,
                /* point1 */ result1.point,
                /* brush  */ resource_utils::solidColorBrush()
                );
            }
        }
    }

    void LabelArea::drawSelection(Renderer* rndr)
    {
        auto& setting = appearance().selection;

        for (auto& rect : m_selectedRangeData.metrics)
        {
            auto selectedRangeRect = math_utils::roundf(selfCoordToAbsolute(
                math_utils::rect(rect.left, rect.top, rect.width, rect.height)));

            ////////////////
            // Background //
            ////////////////

            auto& background = setting.background;

            resource_utils::solidColorBrush()->SetColor(background.color);
            resource_utils::solidColorBrush()->SetOpacity(background.opacity);

            rndr->d2d1DeviceContext()->FillRectangle
            (
            /* rect  */ selectedRangeRect,
            /* brush */ resource_utils::solidColorBrush()
            );

            /////////////
            // Outline //
            /////////////

            auto& stroke = setting.stroke;

            resource_utils::solidColorBrush()->SetColor(stroke.color);
            resource_utils::solidColorBrush()->SetOpacity(stroke.opacity);

            auto frame = math_utils::inner(selectedRangeRect, stroke.width);
            D2D1_ROUNDED_RECT roundedRect = { frame, roundRadiusX, roundRadiusY };

            rndr->d2d1DeviceContext()->DrawRoundedRectangle
            (
            /* roundedRect */ roundedRect,
            /* brush       */ resource_utils::solidColorBrush(),
            /* strokeWidth */ stroke.width
            );
        }
    }

    void LabelArea::onGetKeyboardFocusHelper()
    {
        Label::onGetKeyboardFocusHelper();

        m_caretBlinkingFlag = true;
        m_caretBlinkingElapsedSecs = 0.0f;

        increaseAnimationCount();
    }

    void LabelArea::onLoseKeyboardFocusHelper()
    {
        Label::onLoseKeyboardFocusHelper();

        if (!keepSelectedRange)
        {
            setSelectedRange({ 0, 0 });
        }
        m_caretBlinkingFlag = false;
        m_caretBlinkingElapsedSecs = 0.0f;

        if (!keepCaretPosition)
        {
            setCaretPosition(0);
        }
        decreaseAnimationCount();
    }

    void LabelArea::onSizeHelper(SizeEvent& e)
    {
        Label::onSizeHelper(e);

        setCaretPosition(m_caretPosition);
        setSelectedRange(m_selectedRange);
    }

    void LabelArea::onMouseMoveHelper(MouseMoveEvent& e)
    {
        Label::onMouseMoveHelper(e);

        THROW_IF_NULL(Application::g_app);

        Application::g_app->cursor()->setIcon(Cursor::Text);

        if (holdKeyboardFocus() && e.buttonState.leftPressed)
        {
            m_caretBlinkingFlag = true;
            m_caretBlinkingElapsedSecs = 0.0f;

            auto cursorPoint = absoluteToSelfCoord(e.cursorPoint);
            auto caretPosition = hitTestCaretPosition(cursorPoint);

            setSelectedRange(
            {
                std::min(caretPosition, m_selectedRangeOrigin),
                (size_t)std::abs((int)caretPosition - (int)m_selectedRangeOrigin)
            });
            setCaretPosition(caretPosition);
        }
    }

    void LabelArea::onMouseButtonHelper(MouseButtonEvent& e)
    {
        Label::onMouseButtonHelper(e);

        THROW_IF_NULL(Application::g_app);

        if (e.state.leftDown() || e.state.leftDblclk())
        {
            Application::g_app->focusUIObject
            (
                Application::FocusType::Keyboard, shared_from_this()
            );
            m_caretBlinkingFlag = true;
            m_caretBlinkingElapsedSecs = 0.0f;

            auto cursorPoint = absoluteToSelfCoord(e.cursorPoint);
            m_selectedRangeOrigin = hitTestCaretPosition(cursorPoint);

            setSelectedRange({ 0, 0 });

            setCaretPosition(m_selectedRangeOrigin);
        }
    }

    void LabelArea::onKeyboardHelper(KeyboardEvent& e)
    {
        Label::onKeyboardHelper(e);

        if (holdKeyboardFocus() && e.state.pressed() && e.CTRL())
        {
            switch (e.vkey)
            {
            case 'A': performCommandSelectAll(); break;
            case 'C': performCommandCopySelection(); break;
            default: break;
            }
        }
    }

    void LabelArea::onChangeThemeStyleHelper(const ThemeStyle& style)
    {
        Label::onChangeThemeStyleHelper(style);

        appearance().changeTheme(style.name);
    }

    void LabelArea::onTextLayoutChangeHelper()
    {
        Label::onTextLayoutChangeHelper();

        setCaretPosition(m_caretPosition);
        setSelectedRange(m_selectedRange);
    }
}
