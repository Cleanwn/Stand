#pragma once

#include <cstdlib> // malloc
#include <cstring> // memcpy
#include <string>

namespace Stand
{
	class Mixed
	{
	private:
		size_t size;
		void* data;

	public:
		constexpr Mixed() noexcept
			: size(0), data(nullptr)
		{
		}

		template <typename T>
		Mixed(const T& t)
			: size((sizeof(T) << 1) | 1), data(malloc(sizeof(T)))
		{
			memcpy(data, &t, sizeof(T));
		}

		Mixed(const Mixed& b) noexcept;
		Mixed(Mixed&& b) noexcept;
		Mixed(const char* str);
		Mixed(const std::string& str);

		[[nodiscard]] bool isString() const noexcept;
		[[nodiscard]] size_t getAllocSize() const;

		~Mixed();

		void operator =(const Mixed& b);
		void operator =(Mixed&& b);
		void operator =(const std::string& str);

	private:
		void initString(size_t len, const char* cstr);
		void copy(const Mixed& b);
		void copy(const char* str);
		void copy(const std::string& str);
	public:
		void release();
		void reset() noexcept;

		[[nodiscard]] char* c_str() const;

		template <typename T>
		[[nodiscard]] T* as() const;

		template <typename T>
		[[nodiscard]] T& get() const
		{
			return *as<T>();
		}

		template <typename T>
		operator T&() const
		{
			return get<T>();
		}
	};

	template<>
	inline char** Mixed::as() const
	{
		SOUP_IF_UNLIKELY (!isString())
		{
			throw 0;
		}
		return (char**)&size;
	}

	template<>
	inline const char** Mixed::as() const
	{
		SOUP_IF_UNLIKELY (!isString())
		{
			throw 0;
		}
		return (const char**)&size;
	}

	template<typename T>
	inline T* Mixed::as() const
	{
		SOUP_IF_UNLIKELY (getAllocSize() != sizeof(T))
		{
			throw 0;
		}
		return reinterpret_cast<T*>(data);
	}
}
