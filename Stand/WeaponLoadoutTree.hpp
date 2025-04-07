#pragma once

#include <fmt/format.h>

#include <soup/TreeReader.hpp>
#include <soup/TreeWriter.hpp>
#include <soup/UniquePtr.hpp>

namespace Stand
{
	struct WeaponLoadoutTreeNode
	{
		enum Type : uint8_t
		{
			ROOT,
			WEAP,
			COMP,
		};

		const Type type;

		explicit WeaponLoadoutTreeNode(Type type)
			: type(type)
		{
		}

		virtual ~WeaponLoadoutTreeNode() = default;

		[[nodiscard]] bool isList() const noexcept
		{
			return type != COMP;
		}
	};

	struct WeaponLoadoutTreeNodeList : public WeaponLoadoutTreeNode
	{
		using WeaponLoadoutTreeNode::WeaponLoadoutTreeNode;

		std::vector<soup::UniquePtr<WeaponLoadoutTreeNode>> children;
	};

	struct WeaponLoadoutTree : public WeaponLoadoutTreeNodeList
	{
		struct Component : public WeaponLoadoutTreeNode
		{
			hash_t hash;

			Component()
				: WeaponLoadoutTreeNode(COMP)
			{
			}
		};

		struct Weapon : public WeaponLoadoutTreeNodeList
		{
			hash_t hash;
			int tint;

			Weapon()
				: WeaponLoadoutTreeNodeList(WEAP)
			{
			}
		};

		WeaponLoadoutTree()
			: WeaponLoadoutTreeNodeList(ROOT)
		{
		}
	};

	struct WeaponLoadoutTreeReader : public soup::TreeReader
	{
		bool compact = false;

		[[nodiscard]] std::string joaatToString(hash_t hash) const;

		[[nodiscard]] std::string getName(const void* node) const final
		{
			if (reinterpret_cast<const WeaponLoadoutTreeNode*>(node)->type == WeaponLoadoutTreeNode::WEAP)
			{
				return joaatToString(reinterpret_cast<const WeaponLoadoutTree::Weapon*>(node)->hash);
			}
			if (reinterpret_cast<const WeaponLoadoutTreeNode*>(node)->type == WeaponLoadoutTreeNode::COMP)
			{
				return joaatToString(reinterpret_cast<const WeaponLoadoutTree::Component*>(node)->hash);
			}
			return {};
		}

		[[nodiscard]] std::string getValue(const void* node) const final
		{
			if (reinterpret_cast<const WeaponLoadoutTreeNode*>(node)->type == WeaponLoadoutTreeNode::WEAP)
			{
				std::string val = "Tint ";
				val.append(fmt::to_string(reinterpret_cast<const WeaponLoadoutTree::Weapon*>(node)->tint));
				return val;
			}
			return {};
		}

		[[nodiscard]] bool canHaveChildren(const void* node) const final
		{
			return reinterpret_cast<const WeaponLoadoutTreeNode*>(node)->isList();
		}

		[[nodiscard]] size_t getNumChildren(const void* node) const final
		{
			return reinterpret_cast<const WeaponLoadoutTreeNodeList*>(node)->children.size();
		}

		[[nodiscard]] const void* getChild(const void* node, size_t i) const final
		{
			return reinterpret_cast<const WeaponLoadoutTreeNodeList*>(node)->children.at(i).get();
		}
	};

	struct WeaponLoadoutTreeWriter : public soup::TreeWriter
	{
		void* createChild(void* _parent, std::string&& name, std::string&& value) const final;
	};
}
