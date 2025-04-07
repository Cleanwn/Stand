#pragma once

#include <string>

namespace Stand
{
#define NUM_RN_PRE100 5
#define NUM_RN_ALL_TOXIC 4
#define NUM_RN_EARLY_TOXIC 10
#define NUM_RN_USED100 1
#define NUM_RN_RNDBOOL_TRUE 3
#define NUM_RN_RNDBOOL_FALSE 3

	enum RootNameId : long long
	{
		RN_NULL = 0,
		RN_NAME_ONLY,
		RN_VERSION_ONLY,
		RN_STOCK,
		RN_FREE,
		RN_AUTHED,
		RN_PRE100,
		RN_ALL_TOXIC = RN_PRE100 + NUM_RN_PRE100,
		RN_EARLY_TOXIC = RN_ALL_TOXIC + NUM_RN_ALL_TOXIC,
		RN_USED100 = RN_EARLY_TOXIC + NUM_RN_EARLY_TOXIC,
		RN_RNDBOOL_TRUE = RN_USED100 + NUM_RN_USED100,
		RN_RNDBOOL_FALSE = RN_RNDBOOL_TRUE + NUM_RN_RNDBOOL_TRUE,
	};

	class RootNameMgr
	{
	public:
		static const std::string stock_root_name;
		inline static const char* authed_root_name;
		static const std::string pre100[NUM_RN_PRE100];
		static const std::string all_toxic[NUM_RN_ALL_TOXIC];
		static const std::string early_toxic[NUM_RN_EARLY_TOXIC];
		static const std::string used100[NUM_RN_USED100];
		static const std::string rndbool_true[NUM_RN_RNDBOOL_TRUE];
		static const std::string rndbool_false[NUM_RN_RNDBOOL_FALSE];

		inline static long long current = RN_NULL;

		[[nodiscard]] static long long getDefaultRootNameIdForState();
		[[nodiscard]] static std::string getCurrentRootNameForState();
	private:
		[[nodiscard]] static long long getRootNameIdForState();
		[[nodiscard]] static std::string idToString(long long id);
	};
}
