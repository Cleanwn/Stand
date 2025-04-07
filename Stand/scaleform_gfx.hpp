#pragma once

#include "gta_fwddecl.hpp"

struct GFxMovieView
{
	virtual void _000() = 0;
	virtual void _008() = 0;
	virtual void _010() = 0;
	virtual void _018() = 0;
	virtual void _020() = 0;
	virtual void _028() = 0;
	virtual void _030() = 0;
	virtual void _038() = 0;
	virtual void _040() = 0;
	virtual void _048() = 0;
	virtual bool IsAvailable(const char* ppathToVar) = 0;
	virtual void _058() = 0;
	virtual void _060() = 0;
	virtual void _068() = 0;
	virtual void _070() = 0;
	virtual void _078() = 0;
	virtual void _080() = 0;
	virtual bool GetVariable(GFxValue* pval, const char* ppathToVar) = 0; // 0x88
	virtual void _090() = 0;
	virtual void _098() = 0;

	enum SetArrayType
	{
		SA_Int,     // Array of 'SInt'
		SA_Double,  // Array of 'Double' in memory
		SA_Float,   // Array of 'Float' in memory
		SA_String,  // Array of 'const char*'.      (Deprecated; use SA_Value)
		SA_StringW, // Array of 'const wchar_t*'.   (Deprecated; use SA_Value)
		SA_Value    // Array of GFxValue.
	};

	virtual unsigned int GetVariableArraySize(const char* ppathToVar) = 0; // 0xA0
	virtual bool GetVariableArray(SetArrayType type, const char* ppathToVar, unsigned int index, void* pdata, unsigned int count) = 0; // 0xA8

	bool GetVariableArray(const char* ppathToVar, unsigned int index, GFxValue* pdata, unsigned int count)
	{
		return GetVariableArray(SA_Value, ppathToVar, index, pdata, count);
	}
};

struct GFxMovieRoot : public GFxMovieView
{
};

struct GFxValue
{
	enum ValueTypeControl
	{
		// Explicit coercion to type requested 
		VTC_ConvertBit = 0x80,

		// Flag that denotes managed resources
		VTC_ManagedBit = 0x40,

		VTC_TypeMask = VTC_ConvertBit | 0x0F,
	};

	enum ValueType
	{
		// Basic types
		VT_Undefined = 0x00,
		VT_Null = 0x01,
		VT_Boolean = 0x02,
		VT_Number = 0x03,
		VT_String = 0x04,

		// StringW can be passed as an argument type, but it will only be returned 
		// if VT_ConvertStringW was specified, as it is not a native type.
		VT_StringW = 0x05,  // wchar_t* string

		// ActionScript VM objects
		VT_Object = 0x06,
		VT_Array = 0x07,
		// Special type for stage instances (MovieClips, Buttons, TextFields)
		VT_DisplayObject = 0x08,

		// Specify this type to request SetVariable/Invoke result to be converted 
		// to the specified type. After return of the function, the specified type
		// will *ALWAYS* be stored in the value.
		VT_ConvertBoolean = VTC_ConvertBit | VT_Boolean,
		VT_ConvertNumber = VTC_ConvertBit | VT_Number,
		VT_ConvertString = VTC_ConvertBit | VT_String,
		VT_ConvertStringW = VTC_ConvertBit | VT_StringW
		// Cannot convert to VM objects
	};

	struct ObjectInterface
	{
		/* 0x00 */ GFxMovieRoot* pMovieRoot;

		bool GetMember(void* pdata, const char* name, GFxValue* pval) const;

		[[nodiscard]] unsigned int GetArraySize(void* pdata) const;
	};
	static_assert(sizeof(ObjectInterface) == 0x08);

	/* 0x00 */ ObjectInterface* pObjectInterface;

	union ValueUnion
	{
		double NValue;
		bool BValue;
		const char* pString;
		const char** pStringManaged;
		const wchar_t* pStringW;
		void* pData;
	};

	/* 0x08 */ ValueType Type;
	/* 0x10 */ ValueUnion Value;

	GFxValue()
		: pObjectInterface(nullptr), Type(VT_Undefined)
	{
	}

	[[nodiscard]] bool IsManagedValue() const
	{
		return (Type & VTC_ManagedBit) != 0;
	}

	[[nodiscard]] ValueType GetType() const
	{
		return ValueType(Type & VTC_TypeMask);
	}

	[[nodiscard]] const char* GetString() const
	{
		return IsManagedValue() ? *Value.pStringManaged : Value.pString;
	}

	bool GetMember(const char* name, GFxValue* pval) const
	{
		return pObjectInterface->GetMember(Value.pData, name, pval);
	}

	[[nodiscard]] unsigned int GetArraySize() const
	{
		return pObjectInterface->GetArraySize(Value.pData);
	}
};
static_assert(offsetof(GFxValue, Value) == 0x10);

