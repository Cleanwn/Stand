#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include <soup/TransientToken.hpp>

#include "fwddecl.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class Config
	{
	private:
		std::wstring name{};
		bool save_requested = false;
	public:
		soup::TransientToken transient_token;

		explicit Config() = default;
		explicit Config(std::wstring&& name);
		explicit Config(const std::string& name);

		virtual ~Config() noexcept;

		[[nodiscard]] std::wstring getName() const;
		[[nodiscard]] std::wstring getPath() const;

		void setName(std::wstring&& name);
		void setName(const std::string& name);

		void load();
	protected:
		virtual void loadImpl(std::unordered_map<std::string, std::string>&& data) = 0;

		virtual bool canSaveNow() const;

	public:
		void save();
	private:
		void saveJob();
		static void saveJob(std::shared_ptr<soup::WeakRef<Config>>&& weakref);
	public:
		void setSaveNoLongerNeeded() noexcept;
		virtual void saveToFileNow() = 0;

		[[nodiscard]] bool isInitialised() const noexcept;
		void reset();
	};
#pragma pack(pop)
}
