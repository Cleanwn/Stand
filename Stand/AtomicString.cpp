#include "AtomicString.hpp"

namespace Stand
{
	AtomicString::AtomicString(const std::string& str)
	{
		operator =(str);
	}

	AtomicString::AtomicString(const AtomicString& b)
		: AtomicString(b.str())
	{
	}

	AtomicString::AtomicString(AtomicString&& b)
	{
		auto d = b.data.load();
		while (!b.data.compare_exchange_weak(d, nullptr));
		this->data = d;
	}

	AtomicString::~AtomicString() noexcept
	{
		auto d = data.load();
		if (d != nullptr)
		{
			delete[] d;
		}
	}

	void AtomicString::replace(char* r) noexcept
	{
		auto d = data.load();
		while (!data.compare_exchange_weak(d, r));
		if (d != nullptr)
		{
			delete[] d;
		}
	}

	void AtomicString::clear() noexcept
	{
		replace(nullptr);
	}

	void AtomicString::operator=(const std::string& str) noexcept
	{
		if (str.empty())
		{
			clear();
		}
		else
		{
			auto r = new char[str.size() + 1];
			memcpy(r, str.c_str(), str.size() + 1);
			replace(r);
		}
	}

	void AtomicString::operator=(const AtomicString& b) noexcept
	{
		operator =(b.str());
	}

	bool AtomicString::empty() const noexcept
	{
		return data.load() == nullptr;
	}

	std::string AtomicString::str() const noexcept
	{
		auto d = data.load();
		if (d == nullptr)
		{
			return {};
		}
		return d;
	}

	AtomicString::operator std::string () const noexcept
	{
		return str();
	}

	const char* AtomicString::c_str() const noexcept
	{
		return data.load();
	}

	bool AtomicString::operator<(const AtomicString& b) const noexcept
	{
		return str() < b.str();
	}

	bool AtomicString::operator==(const std::string& b) const noexcept
	{
		return str() == b;
	}

	bool AtomicString::operator==(const AtomicString& b) const noexcept
	{
		return operator==(b.str());
	}
}
