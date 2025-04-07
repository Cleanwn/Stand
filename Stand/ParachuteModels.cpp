#include "ParachuteModels.hpp"

#include <fmt/format.h>

#include "Label.hpp"
#include "lang.hpp"

namespace Stand
{
	void generate_parachute_models()
	{
		auto para_gen = LANG_GET("PARA_GEN");
		g_parachute_models[0] = ParachuteModel(ATSTRINGHASH("sr_prop_specraces_para_s_01"), {
			ParachuteModelVariation{ 0, LIT(std::move(std::string(para_gen).append(": ").append(LANG_GET("CN_A")))) },
			ParachuteModelVariation{ 1, LIT(std::move(std::string(para_gen).append(": ").append(LANG_GET("CN_R")))) },
			ParachuteModelVariation{ 2, LIT(std::move(std::string(para_gen).append(": ").append(LANG_GET("CN_W")).append(", ").append(LANG_GET("CN_B")).append(", ").append(LANG_GET("CN_Y")))) },
			ParachuteModelVariation{ 3, LIT(std::move(std::string(para_gen).append(": ").append(LANG_GET("CN_L")).append(", ").append(LANG_GET("CN_R")).append(", ").append(LANG_GET("CN_W")))) },
			ParachuteModelVariation{ 4, LIT(std::move(std::string(para_gen).append(": ").append(LANG_GET("CN_R")).append(", ").append(LANG_GET("CN_W")).append(", ").append(LANG_GET("CN_B")))) },
			ParachuteModelVariation{ 5, LIT(std::move(std::string(para_gen).append(": ").append(LANG_GET("CN_B")))) },
			ParachuteModelVariation{ 6, LIT(std::move(std::string(para_gen).append(": ").append(LANG_GET("CN_L")))) },
			ParachuteModelVariation{ 7, LIT(std::move(std::string(para_gen).append(": ").append(LANG_GET("CN_L")).append(", ").append(LANG_GET("CN_Y")))) },
		});
		g_parachute_models[1] = ParachuteModel(ATSTRINGHASH("prop_v_parachute"), LIT(std::move(std::string(para_gen).append(": ").append(LANG_GET("CN_Y")).append(", ").append(LANG_GET("CN_B")).append(", ").append(LANG_GET("CN_W")))), true);
		g_parachute_models[2] = ParachuteModel(ATSTRINGHASH("gr_prop_gr_para_s_01"), LIT(std::move(std::string(para_gen).append(": ").append(LANG_GET("CN_O")))), true);
		g_parachute_models[3] = ParachuteModel(ATSTRINGHASH("p_parachute1_sp_dec"), LIT(std::move(std::string(para_gen).append(": ").append(LANG_GET("CN_W")))), true);
		g_parachute_models[4] = ParachuteModel(ATSTRINGHASH("imp_prop_impexp_para_s"), LOC("SECUROSERV"), true);
		g_parachute_models[5] = ParachuteModel(ATSTRINGHASH("p_para_broken1_s"), LOC("PARA_BRK"), false);
	}
}
