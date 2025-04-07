#include "WeaponLoadoutTree.hpp"

#include "joaatToString.hpp"
#include "Util.hpp"

namespace Stand
{
	std::string WeaponLoadoutTreeReader::joaatToString(hash_t hash) const
	{
		if (!compact)
		{
			return ::Stand::joaatToString(hash);
		}
		return Util::to_padded_hex_string_with_0x(hash);
	}

	void* WeaponLoadoutTreeWriter::createChild(void* _parent, std::string&& name, std::string&& value) const
	{
		auto* const parent = reinterpret_cast<WeaponLoadoutTreeNodeList*>(_parent);
		if (parent->type == WeaponLoadoutTreeNode::ROOT)
		{
			auto w = reinterpret_cast<WeaponLoadoutTree::Weapon*>(parent->children.emplace_back(soup::make_unique<WeaponLoadoutTree::Weapon>()).get());
			w->hash = stringToJoaat(name);
			w->tint = (value.at(5) - '0');
			return w;
		}
		if (parent->type == WeaponLoadoutTreeNode::WEAP)
		{
			auto c = reinterpret_cast<WeaponLoadoutTree::Component*>(parent->children.emplace_back(soup::make_unique<WeaponLoadoutTree::Component>()).get());
			c->hash = stringToJoaat(name);
			return c;
		}
		throw 0;
	}
}
