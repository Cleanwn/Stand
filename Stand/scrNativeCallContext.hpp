#pragma once

#include <cstdint>
#include <utility>

#include <soup/type_traits.hpp>

#include "natives_decl.hpp"
#include "struct_base.hpp"

#include "scrValue.hpp"
#include "vector.hpp"
#include "XoredNativeHash.hpp"

#define EXPECT_INVOKE_WONT_FAIL true

namespace rage
{
#pragma pack(push, 8)
	class scrNativeCallContext // real name is scrThread::Info
	{
	public:
		/* 0x00 */ void* m_return_value = nullptr;
		/* 0x08 */ int32_t m_arg_count = 0;
		/* 0x10 */ void* m_args = nullptr;
		/* 0x18 */ int32_t buffer_count = 0;
		/* 0x20 */ scrValue* orig[4];
		/* 0x40 */ Vector3 buffer[4];

		scrNativeCallContext() = default;

		scrNativeCallContext(scrValue* resultPtr, int parameterCount, const scrValue* params)
			: m_return_value(resultPtr), m_arg_count(parameterCount), m_args(const_cast<scrValue*>(params)), buffer_count(0)
		{
		}

		void reset() noexcept
		{
			m_arg_count = 0;
			buffer_count = 0;
		}

		template <typename T, SOUP_RESTRICT(sizeof(T) <= sizeof(uint64_t))>
		void pushArg(T&& value)
		{
			*reinterpret_cast<std::remove_cv_t<std::remove_reference_t<T>>*>(reinterpret_cast<uint64_t*>(m_args) + (m_arg_count++)) = std::forward<T>(value);
		}

		void pushArg(const scrVector3& value);

		template <typename T>
		T& getArg(size_t index)
		{
			static_assert(sizeof(T) <= sizeof(uint64_t));
			return *reinterpret_cast<T*>(reinterpret_cast<uint64_t*>(m_args) + index);
		}

		template <typename T>
		void setArg(size_t index, T&& value)
		{
			static_assert(sizeof(T) <= sizeof(uint64_t));
			*reinterpret_cast<std::remove_cv_t<std::remove_reference_t<T>>*>(reinterpret_cast<uint64_t*>(m_args) + index) = std::forward<T>(value);
		}

		template <typename T>
		T* getReturnValuePtr()
		{
			return reinterpret_cast<T*>(m_return_value);
		}

		template <typename T>
		T& getReturnValue()
		{
			return *getReturnValuePtr<T>();
		}

		template <typename T>
		void setReturnValue(T&& value)
		{
			*reinterpret_cast<std::remove_cv_t<std::remove_reference_t<T>>*>(m_return_value) = std::forward<T>(value);
		}

#if EXPECT_INVOKE_WONT_FAIL
		[[nodiscard]] static constexpr bool canInvoke(::Stand::XoredNativeHash hash)
		{
			return true;
		}
#else
		[[nodiscard]] static bool canInvoke(::Stand::XoredNativeHash hash);
#endif

		void invoke(::Stand::XoredNativeHash x);
		void invokePassthrough(::Stand::XoredNativeHash x);
		void CopyReferencedParametersOut();
	};
	static_assert(sizeof(scrNativeCallContext) == 0x80);
#pragma pack(pop)
}
