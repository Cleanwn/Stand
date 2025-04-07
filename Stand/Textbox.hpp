#pragma once

#include <string>
#include <vector>

namespace Stand
{
#pragma pack(push, 1)
	class Textbox
	{
	public:
		std::wstring text = {};
		size_t cursor = 0;
		bool keep_content_on_close = false;
	protected:
		std::vector<std::wstring> history = {};
	private:
		size_t history_i = 0;
		std::wstring text_before_history_navigation = {};
		bool ime_in_progress = false;

	public:
		void setText(const std::wstring& text);
		void setText(std::wstring&& text);

		void setTextKeepSecondaryStates(const std::wstring& text);
		void setTextKeepSecondaryStates(std::wstring&& text);

		void appendText(const std::wstring& text);
		void eraseTextBack(size_t len);

		void onTextAdded();
		void onTextAddedNoCursorAdjustment();

		void resetInput();
		virtual void resetSecondaryStates();

		void onFocus();
		void onTick();
		void onBlur();

		void processKeydown(unsigned int vk);
		void processChar(wchar_t c);
	protected:
		virtual void onUpdate();
		virtual void onLeft();
		virtual void onRight();
		virtual void onUp();
		virtual void onDown();
		virtual void onEnter();
		virtual void onEscape();
		virtual void onCtrlU();

		[[nodiscard]] static bool isCursorStopper(wchar_t c) noexcept;
	};
#pragma pack(pop)
}
