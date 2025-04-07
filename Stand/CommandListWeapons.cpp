#include "CommandListWeapons.hpp"

#include "weapons.hpp"

namespace Stand
{
	class CommandListWeaponCategory : public CommandList
	{
	private:
		std::string config_name;

	public:
		explicit CommandListWeaponCategory(CommandList* parent, Label&& menu_name, std::string&& config_name)
			: CommandList(parent, std::move(menu_name), {}, NOLABEL, CMDFLAGS_LIST | CMDFLAG_FEATURELIST_FINISHLIST), config_name(std::move(config_name))
		{
		}

		[[nodiscard]] std::string getNameForConfig() const final
		{
			return config_name;
		}
	};

	void CommandListWeapons::populate()
	{
		createCommand(this, LOC("ALLGNS"), LANG_GET_EN("ALLGNS"), Weapon::getAllHashes());

		auto cat = (Weapon::Category)-1;
		CommandList* list;
		for (const auto& weapon : Weapon::weapons)
		{
			if (weapon.category != cat)
			{
				cat = weapon.category;
				list = this->createChild<CommandListWeaponCategory>(LIT(Weapon::getCategoryName(cat)), Weapon::getCategoryAmericanName(cat));
			}
			createCommand(list, LIT(weapon.getName()), weapon.american_name, std::vector<hash_t>(1, weapon.hash));
		}
	}

	CommandList* CommandListWeapons::getHost(Command* cmd)
	{
		auto list = cmd->parent;
		if (list->flags == (CMDFLAGS_LIST | CMDFLAG_FEATURELIST_FINISHLIST))
		{
			list = list->parent;
		}
		return list;
	}
}
