#include "Config.hpp"

#include "ColonsAndTabs.hpp"
#include "Exceptional.hpp"
#include "Gui.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	Config::Config(std::wstring&& name)
		: name(std::move(name))
	{
	}

	Config::Config(const std::string& name)
		: name(StringUtils::utf8_to_utf16(name))
	{
	}

	Config::~Config() noexcept
	{
	}

	std::wstring Config::getName() const
	{
		return name;
	}

	std::wstring Config::getPath() const
	{
		return std::move(std::wstring(_wgetenv(L"appdata")).append(L"\\Stand\\").append(name).append(L".txt"));
	}

	void Config::setName(std::wstring&& name)
	{
		this->name = std::move(name);
	}

	void Config::setName(const std::string& name)
	{
		setName(StringUtils::utf8_to_utf16(name));
	}

	void Config::load()
	{
		std::ifstream file_in(getPath());
		std::unordered_map<std::string, std::string> data{};
		ColonsAndTabs::read(file_in, data);
		loadImpl(std::move(data));
	}

	bool Config::canSaveNow() const
	{
		return true;
	}

	void Config::save()
	{
		if (isInitialised()
			&& !save_requested
			)
		{
			save_requested = true;
			saveJob();
		}
	}

	void Config::saveJob()
	{
		saveJob(std::make_shared<soup::WeakRef<Config>>(this));
	}

	void Config::saveJob(std::shared_ptr<soup::WeakRef<Config>>&& weakref)
	{
		Exceptional::createManagedExceptionalThread(__FUNCTION__, [weakref{std::move(weakref)}]() mutable
		{
			if (auto c = weakref->getPointer())
			{
				if (!c->save_requested || g_gui.unload_state > UnloadState::SAVE_PENDING_CONFIGS)
				{
					return;
				}
				if (!c->canSaveNow())
				{
					return saveJob(std::move(weakref));
				}
				c->saveToFileNow();
				c->setSaveNoLongerNeeded();
			}
		});
	}

	void Config::setSaveNoLongerNeeded() noexcept
	{
		save_requested = false;
	}

	bool Config::isInitialised() const noexcept
	{
		return !name.empty();
	}

	void Config::reset()
	{
		name.clear();
		save_requested = false;
		transient_token.refresh();
	}
}
