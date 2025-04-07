#pragma once

#include "Toaster.hpp"

namespace Stand
{
	class GameToaster : public Toaster
	{
	private:
		int persistent = 0;

		int doToast(const std::wstring& message) const;

	public:
		void toast(const std::wstring& message) const final;

		void setPersistentToast(const std::wstring& message) final;
		void clearPersistentToast() final;
	};

	inline GameToaster g_game_toaster{};
}
