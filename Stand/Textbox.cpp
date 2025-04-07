#include "Textbox.hpp"

#include <soup/unicode.hpp>

#include "pointers.hpp"
#include "StringUtils.hpp"
#include "UnicodePrivateUse.hpp"
#include "Util.hpp"

#define DEBUG_INPUT false

#if DEBUG_INPUT
#include <fmt/format.h>
#include "ThreadContext.hpp"
#endif

namespace Stand
{
	void Textbox::setText(const std::wstring& text)
	{
		resetSecondaryStates();
		setTextKeepSecondaryStates(text);
	}

	void Textbox::setText(std::wstring&& text)
	{
		resetSecondaryStates();
		setTextKeepSecondaryStates(std::move(text));
	}

	void Textbox::setTextKeepSecondaryStates(const std::wstring& text)
	{
		this->text = text;
		onTextAddedNoCursorAdjustment();
		this->cursor = this->text.size();
		onUpdate();
	}

	void Textbox::setTextKeepSecondaryStates(std::wstring&& text)
	{
		this->text = std::move(text);
		onTextAddedNoCursorAdjustment();
		this->cursor = this->text.size();
		onUpdate();
	}

	void Textbox::appendText(const std::wstring& text)
	{
		this->text.insert(cursor, text);
		cursor += text.size();
		onUpdate();
	}

	void Textbox::eraseTextBack(size_t len)
	{
		size_t count = 0;
		while (len != 0 && cursor != 0)
		{
			--len;
			--cursor;
			++count;
		}
		text.erase(cursor, count);
		onUpdate();
	}

	void Textbox::onTextAdded()
	{
		const auto prev_len = this->text.length();
		onTextAddedNoCursorAdjustment();
		this->cursor += (this->text.length() - prev_len);
	}

	void Textbox::onTextAddedNoCursorAdjustment()
	{
		UnicodePrivateUse::fromGta(this->text);
	}

	void Textbox::resetInput()
	{
		cursor = 0;
		text.clear();
		resetSecondaryStates();
		onUpdate();
	}

	void Textbox::resetSecondaryStates()
	{
		history_i = 0;
		text_before_history_navigation.clear();
	}

	void Textbox::onFocus()
	{
	}

	void Textbox::onTick()
	{
		*pointers::rage_ioKeyboard_sm_AllowLocalKeyboardLayout = true;
		pointers::CNewHud_UpdateImeText();

		if (HUD::IS_IME_IN_PROGRESS())
		{
			ime_in_progress = true;
		}
		else if (ime_in_progress)
		{
			ime_in_progress = false;
			std::wstring app = pointers::rage_ioKeyboard_sm_TextBuffer;
			if (!app.empty())
			{
				text.insert(cursor, app);
				cursor += app.length();
				onUpdate();
			}
		}
	}

	void Textbox::onBlur()
	{
		*pointers::rage_ioKeyboard_sm_AllowLocalKeyboardLayout = false;
	}

	void Textbox::processKeydown(unsigned int vk)
	{
		switch (vk)
		{
		case VK_LEFT:
			onLeft();
			break;

		case VK_RIGHT:
			onRight();
			break;

		case VK_HOME:
			if (cursor != 0)
			{
				cursor = 0;
				onUpdate();
			}
			break;

		case VK_END:
			if (cursor != text.size())
			{
				cursor = text.size();
				onUpdate();
			}
			break;

		case VK_DELETE:
			if (cursor != text.size())
			{
				if (GetKeyState(VK_CONTROL) & 0x8000)
				{
					while (cursor < text.size() && isCursorStopper(text[cursor]))
					{
						text.erase(cursor, 1);
					}
					while (cursor < text.size() && !isCursorStopper(text[cursor]))
					{
						text.erase(cursor, 1);
					}
				}
				else
				{
					size_t count = 1;
					SOUP_IF_UNLIKELY (UTF16_IS_HIGH_SURROGATE(text[cursor]))
					{
						SOUP_IF_LIKELY (cursor + 1 != text.size())
						{
							++count;
						}
					}
					text.erase(cursor, count);
				}
				onUpdate();
			}
			break;

		case VK_UP:
			onUp();
			break;

		case VK_DOWN:
			onDown();
			break;
		}
	}

	void Textbox::processChar(wchar_t c)
	{
		switch (c)
		{
		default:
#if DEBUG_INPUT
			Util::toast(fmt::to_string(int16_t(c)), TC_OTHER);
#endif
			text.insert(cursor++, 1, c);
			onTextAdded();
			onUpdate();
			break;

		case 1: // Select All
		case 2: // Ctrl+B
		case 3: // Copy
		case 4: // Ctrl+D
		case 5: // Ctrl+E
		case 6: // Ctrl+F
		case 7: // Ctrl+G
		case 10: // Ctrl+J
		case 11: // Ctrl+K
		case 12: // Ctrl+L
		case 14: // Ctrl+N
		case 15: // Ctrl+O
		case 16: // Ctrl+P
		case 17: // Ctrl+Q
		case 18: // Ctrl+R
		case 19: // Ctrl+S
		case 20: // Ctrl+T
		case 23: // Ctrl+W
		case 24: // Cut
		case 25: // Redo
		case 26: // Undo
			break;

		case 21: // Ctrl+U
			onCtrlU();
			break;

		case 8: // Backspace
			if (cursor != 0)
			{
				--cursor;
				size_t count = 1;
				SOUP_IF_UNLIKELY (UTF16_IS_LOW_SURROGATE(text[cursor]))
				{
					SOUP_IF_LIKELY (cursor != 0)
					{
						--cursor;
						++count;
					}
				}
				text.erase(cursor, count);
				onUpdate();
			}
			break;

		case 127: // Ctrl+Backspace
			if (cursor != 0)
			{
				do
				{
					--cursor;
					if (!isCursorStopper(text[cursor]))
					{
						++cursor;
						break;
					}
					text.erase(cursor, 1);
				} while (cursor != 0);
				while (cursor != 0 && !isCursorStopper(text[cursor - 1]))
				{
					text.erase(--cursor, 1);
				}
				onUpdate();
			}
			break;

		case 13: // Enter
			onEnter();
			break;

		case 22: // Paste
			{
				auto clipboard = Util::getClipboardTextUTF16();
				StringUtils::replace_all(clipboard, L"\r", L"");
				StringUtils::replace_all(clipboard, L"\n", L"; ");
				appendText(clipboard);
			}
			break;

		case 27: // Escape
			onEscape();
			break;
		}
	}

	void Textbox::onUpdate()
	{
	}

	void Textbox::onLeft()
	{
		if (cursor != 0)
		{
			--cursor;
			if (GetKeyState(VK_CONTROL) & 0x8000)
			{
				do
				{
					--cursor;
					if (!isCursorStopper(text[cursor]))
					{
						++cursor;
						break;
					}
				} while (cursor != 0);
				while (cursor != 0 && !isCursorStopper(text[cursor - 1]))
				{
					--cursor;
				}
			}
			else
			{
				SOUP_IF_UNLIKELY (UTF16_IS_LOW_SURROGATE(text[cursor]))
				{
					SOUP_IF_LIKELY (cursor != 0)
					{
						--cursor;
					}
				}
			}
			onUpdate();
		}
	}

	void Textbox::onRight()
	{
		if (GetKeyState(VK_CONTROL) & 0x8000)
		{
			while (cursor < text.size() && isCursorStopper(text[++cursor]));
			while (cursor < text.size() && !isCursorStopper(text[++cursor]));
		}
		else
		{
			if (cursor != text.size())
			{
				++cursor;
				SOUP_IF_UNLIKELY (UTF16_IS_HIGH_SURROGATE(text[cursor - 1]))
				{
					SOUP_IF_LIKELY (cursor != text.size())
					{
						++cursor;
					}
				}
			}
		}
		onUpdate();
	}

	void Textbox::onUp()
	{
		auto new_history_i = (history_i + 1);
		auto i = history.size() - new_history_i;
		if (i < history.size())
		{
			if (history_i == 0)
			{
				text_before_history_navigation = std::move(text);
			}
			history_i = new_history_i;
			setTextKeepSecondaryStates(history.at(i));
		}
	}

	void Textbox::onDown()
	{
		if (!history.empty() && history_i != 0)
		{
			if (--history_i == 0)
			{
				setTextKeepSecondaryStates(std::move(text_before_history_navigation));
			}
			else
			{
				setTextKeepSecondaryStates(history.at(history.size() - history_i));
			}
		}
	}

	void Textbox::onEnter()
	{
	}

	void Textbox::onEscape()
	{
		if (!keep_content_on_close)
		{
			resetInput();
		}
	}

	void Textbox::onCtrlU()
	{
	}

	bool Textbox::isCursorStopper(wchar_t c) noexcept
	{
		return c == L' '
			|| c == L'/'
			|| c == L'\\'
			;
	}
}
