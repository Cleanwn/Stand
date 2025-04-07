#pragma once

template <typename T>
struct GASRefCountBase
{
	virtual ~GASRefCountBase() = default;

	PAD(0x08, 0x20);
};

struct GASObjectInterface
{
	virtual ~GASObjectInterface() = default;
	virtual void _0x08() = 0;
	virtual void _0x10() = 0;
	virtual void _0x18() = 0;
	virtual bool GetMember(GASEnvironment* penv, const GASString& name, GASValue* val) = 0;

	PAD(0x08, 0x10);
};

struct GASObject : public GASRefCountBase<GASObject>, public /* 0x20 */ GASObjectInterface
{
	virtual ~GASObject() = default;
};

struct GASArrayObject : public GASObject
{
	INIT_PAD(GASObject, 0x70) unsigned int size;
};
