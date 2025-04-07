#pragma once

#include <atomic>
#include <string>

namespace Stand
{
	class AtomicString
	{
	private:
		std::atomic<char*> data;

	public:
		constexpr AtomicString() noexcept
			: data(nullptr)
		{
		}

		AtomicString(const std::string& str);
		AtomicString(const AtomicString& b);
		AtomicString(AtomicString&& b);
		~AtomicString() noexcept;

	private:
		void replace(char* r) noexcept;

	public:
		void clear() noexcept;

		void operator=(const std::string& str) noexcept;
		void operator=(const AtomicString& b) noexcept;

		[[nodiscard]] bool empty() const noexcept;

		[[nodiscard]] std::string str() const noexcept;
		[[nodiscard]] operator std::string() const noexcept;
		[[nodiscard]] const char* c_str() const noexcept;

		[[nodiscard]] bool operator <(const AtomicString& b) const noexcept;

		[[nodiscard]] bool operator ==(const std::string& str) const noexcept;
		[[nodiscard]] bool operator ==(const AtomicString& b) const noexcept;
	};
}
