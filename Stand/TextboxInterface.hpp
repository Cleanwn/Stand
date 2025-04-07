#pragma once

namespace Stand
{
	struct TextboxInterface
	{
		virtual void add(const std::wstring& text) const = 0;
		virtual void remove(size_t len) const = 0;
		[[nodiscard]] virtual std::wstring getText() const noexcept = 0;
		[[nodiscard]] virtual size_t getLength() const noexcept = 0;
	};
}

namespace Stand
{
	struct ChatboxInterface : public TextboxInterface
	{
		void add(const std::wstring& text) const final;
		void remove(size_t len) const final;
		[[nodiscard]] std::wstring getText() const noexcept final;
		[[nodiscard]] size_t getLength() const noexcept final;
	};
	inline ChatboxInterface g_chatbox_interface{};
}

namespace Stand
{
	struct CommandboxInterface : public TextboxInterface
	{
		void add(const std::wstring& text) const final;
		void remove(size_t len) const final;
		[[nodiscard]] std::wstring getText() const noexcept final;
		[[nodiscard]] size_t getLength() const noexcept final;
	};
	inline CommandboxInterface g_commandbox_interface{};
}
