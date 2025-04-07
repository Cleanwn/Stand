#include "Mixed.hpp"

namespace Stand
{
	Mixed::Mixed(const Mixed& b) noexcept
		: Mixed()
	{
		copy(b);
	}

	Mixed::Mixed(Mixed&& b) noexcept
		: size(b.size), data(b.data)
	{
		b.reset();
	}

	Mixed::Mixed(const char* str)
		: Mixed()
	{
		copy(str);
	}
	
	Mixed::Mixed(const std::string& str)
		: Mixed()
	{
		copy(str);
	}

	bool Mixed::isString() const noexcept
	{
		return (size & 1) == 0;
	}

	size_t Mixed::getAllocSize() const
	{
		SOUP_IF_UNLIKELY (isString())
		{
			throw 0;
		}
		return size >> 1;
	}

	Mixed::~Mixed()
	{
		release();
	}

	void Mixed::operator=(const Mixed& b)
	{
		release();
		copy(b);
	}

	void Mixed::operator=(Mixed&& b)
	{
		release();
		size = b.size;
		data = b.data;
		b.reset();
	}

	void Mixed::operator=(const std::string& str)
	{
		release();
		copy(str);
	}

	void Mixed::initString(size_t len, const char* cstr)
	{
		void* alloc = malloc(len + 1);
		size = reinterpret_cast<size_t>(alloc);
		SOUP_IF_UNLIKELY (!isString())
		{
			free(alloc);
			throw 0;
		}
		data = alloc;
		strcpy(reinterpret_cast<char*>(alloc), cstr);
	}

	void Mixed::copy(const Mixed& b)
	{
		if (b.size == 0)
		{
			size = 0;
			data = nullptr;
		}
		else if (b.isString())
		{
			copy(b.c_str());
		}
		else
		{
			size = b.size;
			if (b.data == nullptr)
			{
				data = nullptr;
			}
			else
			{
				data = malloc(getAllocSize());
				memcpy(data, b.data, getAllocSize());
			}
		}
	}

	void Mixed::copy(const char* str)
	{
		initString(strlen(str), str);
	}

	void Mixed::copy(const std::string& b)
	{
		initString(b.size(), b.c_str());
	}

	void Mixed::release()
	{
		if (data != nullptr)
		{
			free(data);
		}
	}

	void Mixed::reset() noexcept
	{
		size = 0;
		data = nullptr;
	}

	char* Mixed::c_str() const
	{
		SOUP_IF_UNLIKELY (!isString())
		{
			throw 0;
		}
		return reinterpret_cast<char*>(size);
	}
}
