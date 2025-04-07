#pragma once

#include <soup/TreeReader.hpp>

namespace Stand
{
	struct TreeReaderBase : public soup::TreeReader
	{
		[[nodiscard]] std::string getName(const void* node) const final
		{
			return reinterpret_cast<const CommandPhysical*>(node)->getNameForConfig();
		}

		[[nodiscard]] bool hasChildren(const void* node) const final
		{
			return reinterpret_cast<const CommandPhysical*>(node)->isList();
		}

		[[nodiscard]] std::vector<const void*> getChildren(const void* node) const final
		{
			std::vector<const void*> ret;
			ret.reserve(reinterpret_cast<const CommandList*>(node)->children.size());
			for (const auto& child : reinterpret_cast<const CommandList*>(node)->children)
			{
				ret.emplace_back(child.get());
			}
			return ret;
		}
	};

	struct TreeReaderState : public TreeReaderBase
	{
		[[nodiscard]] std::string getValue(const void* node) const final
		{
			return reinterpret_cast<const CommandPhysical*>(node)->getState();
		}
	};
}
