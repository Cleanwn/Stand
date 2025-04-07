#pragma once

#include "CommandToggleCustom.hpp"

#include "Exceptional.hpp"
#include "FiberPool.hpp"
#include "lang.hpp"

namespace Stand
{
	class CommandPackage : public CommandToggleCustom
	{
	private:
		Package* const pkg;

		[[nodiscard]] static std::string generateHelpText(const Package* const pkg)
		{
			std::string help_text = pkg->description;

			if (std::string version = (pkg->existsOnRemote() ? pkg->remote.id : pkg->local.id); !version.empty())
			{
				if (!help_text.empty())
				{
					help_text.push_back('\n');
				}
				help_text.append(LANG_GET("VER"));
				help_text.append(": ");
				help_text.append(std::move(version));
			}

			if (!pkg->author.empty())
			{
				if (!help_text.empty())
				{
					help_text.push_back('\n');
				}
				help_text.append(LANG_GET("ATR"));
				help_text.append(": ");
				help_text.append(pkg->author);
			}

			return help_text;
		}

	public:
		explicit CommandPackage(CommandList* const parent, Package* const pkg)
			: CommandToggleCustom(parent, pkg->getMenuName(), {}, LIT(generateHelpText(pkg)), (CMDFLAGS_TOGGLE_CUSTOM & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS) | CMDFLAG_FEATURELIST_SKIP), pkg(pkg)
		{
		}

		void onEnable(Click& click) final
		{
			if (!click.isAuto() && !pkg->isBusy() && pkg->existsOnRemote())
			{
				pkg->setBusyForInstall();
				FiberPool::queueJob([pkg{ pkg }]
				{
					pkg->userInstall();
				});
			}
		}

		void onDisable(Click& click) final
		{
			if (!click.isAuto() && !pkg->isBusy())
			{
				if (pkg->isDependency())
				{
					auto entry = std::find(PackageMgr::deps_manually_installed.begin(), PackageMgr::deps_manually_installed.end(), pkg->name);
					if (entry != PackageMgr::deps_manually_installed.end())
					{
						PackageMgr::deps_manually_installed.erase(entry);
						if (pkg->isDependedOnByInstalledPackage()) // Can't uninstall?
						{
							click.setResponse(LOC("DEP_M_F"));
							PackageMgr::saveInstalledPackages();
							return;
						}
					}
					else if (pkg->isDependedOnByInstalledPackage()) // Can't uninstall?
					{
						click.setResponse(LOC("DEP_M_O"));
						PackageMgr::deps_manually_installed.emplace_back(pkg->name);
						PackageMgr::saveInstalledPackages();
						return;
					}
				}
				Exceptional::createManagedExceptionalThread(__FUNCTION__, [pkg{ pkg }]
				{
					pkg->uninstall();
					PackageMgr::autoRemoveDependencies();
					PackageMgr::saveInstalledPackages();
				});
			}
		}

		void onTickInGameViewport() final
		{
			setIndicators(
				pkg->isInstalled(),
				pkg->status != NONE
			);
		}

		void onTickInWebViewport() final
		{
			return onTickInWebViewportImplRedirect();
		}
	};
}
