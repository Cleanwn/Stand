#pragma once

#include "CommandAction.hpp"

#include "CommandPersonalVehicles.hpp"
#include "CommandSavePv.hpp"
#include "get_appdata_path.hpp"

namespace Stand
{
	class CommandSaveAllPersonalVehicles : public CommandAction
	{
	private:
		std::vector<std::wstring> vehicles;

		[[nodiscard]] int countVehicleNames(std::wstring& name)
		{
			int duplicates = 0;

			for (const auto& s : vehicles)
			{
				if (s == name)
				{
					++duplicates;
				}
			}

			return duplicates;
		}

		[[nodiscard]] std::wstring formatAgainstDuplicates(std::wstring&& name)
		{
			vehicles.emplace_back(name);

			if (const auto count = countVehicleNames(name); count > 1)
			{
				name.append(fmt::format(fmt::runtime(StringUtils::utf8_to_utf16(soup::ObfusString(" (#{})").str())), count));
			}

			return name;
		}

	public:
		explicit CommandSaveAllPersonalVehicles(CommandList* const parent)
			: CommandAction(parent, LOC("PVSVEA"), CMDNAMES("saveallpvs"), LOC("PVSVEA_H"))
		{
		}

		void onClick(Click& click) final
		{
			click.setNoResponse(); // Ditch generic.

			parent->as<CommandPersonalVehicles>()->refresh([click, this]
			{
				bool failed = false;

				parent->recursiveForEachChild([&failed, click, this](const std::unique_ptr<Command>& c) mutable
				{
					SOUP_IF_LIKELY (c->isPhysical())
					{
						if (c->getMenuName() == ATSTRINGHASH("SVEVEH"))
						{
							std::error_code ec;
							auto path = get_appdata_path(soup::ObfusString("Vehicles\\Personal Vehicles\\").str());

							if (!std::filesystem::exists(path, ec))
							{
								std::filesystem::create_directory(path, ec);
							}

							if (ec)
							{
								click.setResponse(LOC("GENFAIL"));
								failed = true;
								return false;
							}

							auto args = fmt::format(fmt::runtime(StringUtils::utf8_to_utf16(soup::ObfusString("Personal Vehicles\\{}").str())), formatAgainstDuplicates(c->parent->getMenuName().getEnglishUtf16()));
							c->as<CommandSavePv>()->onCommand(click, args);

							if (click.response == ATSTRINGHASH("GENFAIL")) // std::ofstream failed somehow.
							{
								failed = true;
								return false;
							}
						}
					}

					return true;
				});
				
				SOUP_IF_LIKELY (!failed)
				{
					click.setResponse(LOC("PVSVEA_S"));
				}

				vehicles.clear();
			});
		}
	};
}