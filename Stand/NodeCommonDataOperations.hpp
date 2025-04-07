#pragma once

#include "gta_fwddecl.hpp"

class ICommonDataOperations
{
public:
	virtual ~ICommonDataOperations() = default;
	virtual void ReadData(rage::datBitBuffer& bitBuffer, rage::netLoggingInterface* pLog) = 0;
	virtual void WriteData(rage::netSyncTreeTargetObject* pObj, rage::datBitBuffer& bitBuffer, rage::netLoggingInterface* pLog, bool extractFromObject = true) = 0;
	virtual void ApplyData(rage::netSyncTreeTargetObject* pObj) = 0;
	virtual unsigned GetDataSize(rage::netSyncTreeTargetObject& targetObject) const = 0;
	virtual void LogData(rage::netLoggingInterface& log) = 0;
	virtual void DisplayData(rage::netSyncTreeTargetObject* pObj/*, rage::netLogDisplay& displayLog*/) = 0;
};

template <class Node, class Accessor>
class NodeCommonDataOperations : public ICommonDataOperations
{
public:
	/* 0x08 */ Node* node;
};
