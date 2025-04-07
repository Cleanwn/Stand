#include "CommandSetFont.hpp"

#include <algorithm>
#include <filesystem>

#include <Windows.h>

#include "bin.hpp"
#include "CommandListActionItem.hpp"
#include "ConsoleLogger.hpp"
#include "ensure_folder.hpp"
#include "Exceptional.hpp"
#include "get_display_names.hpp"
#include "lang.hpp"
#include "LangId.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "Renderer.hpp"
#include "RootNameMgr.hpp"
#include "StringUtils.hpp"
#include "Util.hpp"

namespace Stand
{
	static std::vector<CommandListActionItemData> font_options = {};
	
	static int EnumFontFamExProc(ENUMLOGFONTEX* lpelfe, NEWTEXTMETRICEX* lpntme, DWORD FontType, LPARAM lParam)
	{
		std::wstring name = lpelfe->elfLogFont.lfFaceName;
		if (!name.empty())
		{
			if (name.at(0) == L'@')
			{
				name.erase(0, 1);
			}
			std::string menu_name = StringUtils::utf16_to_utf8(name);
			for (const auto& option : font_options)
			{
				if (option.menu_name.isLiteralString(menu_name))
				{
					return 1;
				}
			}
			StringUtils::simplify(name);
#if COMPACT_COMMAND_NAMES
			CommandName command_name = StringUtils::utf16_to_utf8(name);
#else
			CommandName command_name = std::move(name);
#endif
			font_options.emplace_back(CommandListActionItemData{ (long long)font_options.size(), LIT(std::move(menu_name)), { std::move(command_name) } });
		}
		return 1;
	}

	static std::vector<CommandListActionItemData> getFontOptions()
	{
		font_options.clear();
		LOGFONT lf;
		lf.lfFaceName[0] = '\0';
		lf.lfCharSet = DEFAULT_CHARSET;
		HDC hDC = GetDC(pointers::hwnd);
		EnumFontFamiliesEx(hDC, &lf, (FONTENUMPROC)&EnumFontFamExProc, 0, 0);
		ReleaseDC(pointers::hwnd, hDC);
		std::sort(font_options.begin(), font_options.end(), [](const CommandListActionItemData& a, const CommandListActionItemData& b)
		{
			return a.menu_name.literal_str < b.menu_name.literal_str;
		});
		return std::move(font_options);
	}

	CommandSetFont::CommandSetFont(CommandList* parent)
		: CommandListAction(parent, LOC("STFNT"), CMDNAMES("setfont"), NOLABEL, getFontOptions(), CMDFLAGS_LIST_ACTION, CMDFLAG_FEATURELIST_SKIP)
	{
	}

	void CommandSetFont::onClick(Click& click)
	{
		setOptions(getFontOptions());
		CommandListAction::onClick(click);
	}

	static const wchar_t game_language_name_extras[] = L"語體";

	static const wchar_t chinese_display_names[] = L"·一丁七万上下不与丑专世丘业丛东丧丫中丸主义之乐乔乘九乱争事云五亚亡亨亵人什仇仕代伊伍伏优会伟伦伯佐体佛作你佩使依侠侦侯侵保信俱倾偿催傲像元充先光克入全公兰共兵其具典兹兽冒军农冠冥冯冲准凑凝凡凯出击刀刃分切刑列创初判利制刺刻刽剂剧割力加劣动努劫劲劳勇勒包匕北匣匪匹区千升半华卓单南卡卢卫危卷卸厄厉原厢厦叉双反发变叛口古叫可台史叶号吃吉吊后向君吧味命品哈哑哥商啤喉喷嘉嘟器嚎四回园固国图圆土圣地场圾坊坎坞坦垂垃型埃城域培基堂堡塔塞境墙墨壁士声壳复夏外多夜够大天太夫失头夺奇奏奔套奢奥女奴好妇妖姆威媒子孔学宇守安宗官定宝实审客室害家宽宾密寓寡导射尊小尔尖尤尸尼尾尿层屌属履山岬岸崎巡工左巨巫巴市布师希帕帝帮干平幻幽广庆库应庞废座康开异弄式引弗弹当录彗形彩影彻征径徒得微德心必快怀思急性怪恐恨恩息恶悠情惠想感慧戈成战截戴房所扁手扎托扩扳扼把投护拉拌拐拓拔拖拦挂指挽捆捍捕捷探推掷提握搅搬摇摩撒撞撬播攀收改攻放救敞敦文斑斗料斜斧斯新方施旅旋无日旧时明星普暗暴曲曳曼月有服朝木末本术朵机杀杆李村杖杜条杰松板极林枪枭柄柏柒染柜查柯标树样根格桑桶梅梦梭械棍棒森椋椒楼榴橙欧欲歌止正步武死歼殴段毒比毕毛氏气水汀汇汉汗江汤汽沃沙沸油治法波泪泰泵泽洋洒洛洞津洲活派流浩浪浮海涂消涌涡深混渎温游湖湾溅滚满滩漂漠漩潘潜潮澜灌火灭灵炎炮炸炼炽烂烈烟烧热焰煞燃燕爆爱爵爷版牙牛物牵特犀犬狂狄狗狙狩独狮狱狼猎猛猫猴獾玉王玛现珍珠班球理瑞瑟瓦瓶瓷生用由甲电男疾痛登白百皇皮监盒盗盛直相真眩眼睛瞄知短石矿码砂破硕确碎碧碳磁礼社神禄福私秃种科租移穆空穿突立竞笑筒箔管箭篷簧米类籽粗精糖素索紧繁红纤约级纯纳绍经给绝统维缆缇缩罗羊美羔羽耀老者耐耕耳聚肥胁胡能脉脚腕腾臣自至致舌舟航船艇色艳艾节花苔英范茵草荒荡荣药莎莫莱莲菲萄萝营萨落葡董蒂蒙蓝虎虫虹蚀蛇蛮蜜蜡蝗血行街补袭裁装裹西观视角解警认识诗话说诺调谈象豪豹贝贡货质贯费贾赌赛赫起超越跑路蹂躏车轨轮轴轿辆辐输辣边达迅运近进远迪迷追送逆逐途通速造逻道邦邪郁郎郡部都酒里重野量金钟钢钳钻铁铂铅银锈锋锤锥镀镖镜长门闪间闹阁队防阳阿陆限除险陶隆随隐雀雄雅雕雪雷霍霓霜霰露霸非面鞭韦音顶顿领频风飓飙飞餐饰饱饼首马驮驱驹验骑骗骡骨骷骸髅高鬼魂魅魇魔鲁鲨鲸鳍鸟鸭鸿鹰麦麻黏黑默鼓鼻龙（），";
	static const wchar_t chinese_windows_font_names[] = L"仿宋魏楷琥珀隶书幼软姚舒";
	static const wchar_t chinese_country_names[] = L"阿富汗奥兰群岛(芬属)尔巴尼亚及利美萨摩安道哥拉圭南极洲提瓜和布达根廷鲁澳大地塞拜疆哈马林孟加国多斯白俄罗比时伯兹贝宁百慕不丹玻维波黑那博茨瓦纳韦西英印度洋领文莱保基法索隆迪柬埔寨喀麦拿佛得角开曼中非共乍智圣诞科灵伦刚果民主库克黎特古浦路捷吉米厄埃赤几内立里爱沙福斐济区蓬冈乔治德直陀希腊格陵普关危恩绍海赫唐梵蒂城洪都香港匈牙冰伊朗以色列意买日本泽约旦坦肯朝鲜韩威老挝脱嫩托支敦士登陶宛卢森堡门其顿,前夫来代耳他毛塔求墨密蒙的山洛莫桑缅甸瑙泊荷新纽诺北挪帕劳勒当局秘菲律宾皮葡萄各卡留汪旺泰丁力舌昂坡伐所奇班苏扬延瑞典叙台湾东帝汶汤突土凯图乌干联合酋长小别努委越京撒也赞津";
	static const wchar_t chinese_geoip_extra[] = L"梯币亿鱼售各赔欧丝袜腿偷拍笼扎千淘铺微信群每偷稀嘛萌唰香港澳丹挪荷伐匈腊印";

	static const wchar_t korean_display_names[] = L"가간갈감갑강개갱거건검게겐격견결경고곤골곳공과관광구군굴귤그근글급기깎끼나남너널넘네넬노녹놀뇨눈뉴능니닉님닛닝다닥단당대더덕던덤덥덩데덴델도독돌동두듀듄드득들디딜딩라락란람랑래랙랜랠램랩랫러럭런럴럼럽럿렁레렉렌렛로록론롤롬롭롱뢰료루룩룬류르리릭린림립릿링마막만말맘망매맥맨머먼메멜모몬몰몽무문물뮬미믹밀밍바박반발밤밥방배밴밸뱃뱅버벅벌베벡벤벨변별병보복본볼봉부북불뷸브븐블비빅빈빌빔사산살상새샌샤샬샴샷섀서석선설성세센셀셈셉셔션셰소속손송쇠쇼수순숨쉐쉬슈슐스슨슬승시식신실싱써썬썰쏘쓰씨아안알앙애액앤야약얄어언얼엄업엇에엑엔엘엠열염영예오온올옴와왈요용우운울움웃워원월웨웬웰위윈윌윙유은음의이익인일임입잉자작잔잘잠잡장잭저적전점접정제젝젠져젼조좀죄주죽준줌중즈즌즐지진질징차착찰창채챔처척철체첼초촉총최츄츠치침칭카칸칼캐캔캠캣커컨컬컴케켄켓코콤콩쿠쿨쿼퀴큐크클키킨킬킷킹타탄탈탑탕태택탠탬탱터턴털텀테텍텐텔템토톨톰통투툰툴튤트특튼틀티틱틴팅파판팔팡패팩팬팻퍼펀펄펌펍페펙펜펠펫포폭폰폼푸퓨퓰프플피픽핀필핌하한할핫항해핸행허헌헐헤헥헬형호혼홀화확휘흐히힐";

	static const wchar_t japanese_display_names[] = L"、「」いえきくげさしてのびりァアィイゥウェエォオカガキギクグケゲコゴサザシジスズセゼソゾタダチッツテデトドナニネノハバパヒビピフブプヘベペホボポマミムメモャヤュユョヨラリルレロワンヴ・ー上中久二仕作信催傾先光全出刀分刈列刻剤動化危口号君器地坦型大太妻学宇完宙室密対小山岸市平式張弾彩彫径徹急性戦手投拡拳拶挨接搭撃改攻救散料斜斧枝柩棒業榴様標機武殊水海消涙準潜火炎煙爆物特玉瓶用甲痕発目着石砲稲積空筒粘精級缶耐肥能船艇良色芝花華薬血行術被裂装覆調警護豪貨貸路車軍載近返迷送通速連道都重金銃錠闘防険雪雷霊面革飛食高黄（）";

	static const wchar_t russian_display_names[] = L"ГравиокпдеуДльПХйтсКЛОгнжмчзябЮфА3Б1СЗю89э5BmceCvdXLEoFäSHsMPRVTWZшGfDIj6xzKwыqYNJхQO/ТРUщФВцМИЧШЭУНЦъ!Щ";
	static const wchar_t russian_fm2banner_extra[] = L"Ь"; // not the same as "ь"

	static const wchar_t vietnamese_lua_extra[] = L"ỔỜỘ";

	static void note_codepoints_in_utf16_string(std::set<uint16_t>& codepoints, const std::wstring& str)
	{
		for (auto codepoint : str)
		{
			if (codepoint <= 0x7E)
			{
				continue;
			}
			/*if (codepoint >= 0x4E00 && codepoint <= 0x9FFF)
			{
				continue;
			}*/
			codepoints.emplace(codepoint);
		}
	}

	static void note_codepoints_in_utf8_string(std::set<uint16_t>& codepoints, const std::string& str)
	{
		note_codepoints_in_utf16_string(codepoints, StringUtils::utf8_to_utf16(str));
	}

	void CommandSetFont::onItemClick(Click& click, CommandListActionItem* item)
	{
		CommandListAction::onItemClick(click, item);
		Exceptional::createManagedExceptionalThread(__FUNCTION__, [item]
		{
			// Forcing console init; this sets the codepage to UTF-8, which is needed for some Chinese fonts.
			const bool had_console = g_console.isInited();
			if (!had_console)
			{
				g_console.init();
			}

			auto bin_path = std::wstring(_wgetenv(L"appdata")).append(LR"(\Stand\Bin)");
			ensure_folder(bin_path);
			bin_path.push_back(L'\\');
			auto exe_path = std::move(std::wstring(bin_path).append(L"MakeSpriteFont.exe"));
			Util::bin_file_write(exe_path, bin::MakeSpriteFont_exe, sizeof(bin::MakeSpriteFont_exe));
			auto bat_path = std::move(std::wstring(bin_path).append(L"MakeSpriteFont.bat"));
			auto args_path = std::move(std::wstring(bin_path).append(L"MakeSpriteFont.txt"));
			auto font_path = std::move(Renderer::getThemePath().append(L"Font.spritefont"));

#define NONE 0
#define YAHEI 1
#define NANUMGOTHIC 2
#define BEVIETNAM 3

#define OPTIMISED_GEN NONE

#ifndef STAND_DEBUG
			static_assert(OPTIMISED_GEN == NONE);
#endif

			std::string args_extra{};
#if OPTIMISED_GEN == NONE
			args_extra.append(" /fastpack");
#endif
			args_extra.append(" /characterregion:0x0-0x7E");
			std::vector<std::pair<uint16_t, uint16_t>> regions{};
			{
				// Extract codepoints
				std::set<uint16_t> codepoints{};
				lang_t langs[] = {
					LANG_EN,
					LANG_NL,
#if OPTIMISED_GEN == NONE || OPTIMISED_GEN == YAHEI
					LANG_DE,
					LANG_LT,
					LANG_PT,
					LANG_ZH,
					LANG_RU,
					LANG_ES,
					LANG_FR,
					LANG_TR,
					LANG_PL,
					LANG_IT,
					LANG_JA,
#endif
#if OPTIMISED_GEN == NONE || OPTIMISED_GEN == NANUMGOTHIC
					LANG_KO,
#endif
#if OPTIMISED_GEN == NONE || OPTIMISED_GEN == BEVIETNAM
					LANG_VI,
#endif
				};
				codepoints.emplace(0xB0); // U+B0, DEGREE SIGN, °
				codepoints.emplace(L'»');
				codepoints.emplace(L'«');
				note_codepoints_in_utf16_string(codepoints, game_language_name_extras);
				for (const auto& lang : langs)
				{
					if (lang == LANG_ZH)
					{
						note_codepoints_in_utf16_string(codepoints, chinese_display_names);
						note_codepoints_in_utf16_string(codepoints, chinese_windows_font_names);
						note_codepoints_in_utf16_string(codepoints, chinese_country_names);
						note_codepoints_in_utf16_string(codepoints, chinese_geoip_extra);
						for (const auto& root_name : RootNameMgr::early_toxic)
						{
							note_codepoints_in_utf8_string(codepoints, root_name);
						}
					}
					else if (lang == LANG_KO)
					{
						note_codepoints_in_utf16_string(codepoints, korean_display_names);
					}
					else if (lang == LANG_RU)
					{
						note_codepoints_in_utf16_string(codepoints, russian_display_names);
						note_codepoints_in_utf16_string(codepoints, russian_fm2banner_extra);
					}
					else if (lang == LANG_JA)
					{
						note_codepoints_in_utf16_string(codepoints, japanese_display_names);
					}
					else if (lang == LANG_VI)
					{
						note_codepoints_in_utf16_string(codepoints, vietnamese_lua_extra);
					}
					for (auto entry : *Lang::id_to_map(lang))
					{
						note_codepoints_in_utf16_string(codepoints, entry.second);
					}
				}
				switch (LOCALIZATION::GET_CURRENT_LANGUAGE())
				{
				case 0: // en-US
				case 12: // zh-CN
					break;

				default:
					for (const auto& display_name : get_display_names())
					{
						note_codepoints_in_utf8_string(codepoints, display_name);
					}
				}
				//g_logger.log("Converting codepoints to ranges...");
				{
					auto i = codepoints.begin();
					uint16_t region_begin = *i;
					uint16_t region_size = 0;
					while (++i != codepoints.end())
					{
						if (region_begin + region_size + 1 == *i)
						{
							++region_size;
							continue;
						}
						regions.emplace_back(region_begin, region_begin + region_size);
						region_begin = *i;
						region_size = 0;
					}
				}
				//g_logger.log("Handing it over to MakeSpriteFont...");
			}

			{
				std::ofstream bat(bat_path);
				bat << '\"' << StringUtils::utf16_to_utf8(exe_path) << "\" \"" << item->menu_name.literal_str << "\" \"" << StringUtils::utf16_to_utf8(font_path) << "\"" << args_extra;
				if (!regions.empty())
				{
					bat << " \"/fromfile:" << StringUtils::utf16_to_utf8(args_path) << '\"';
				}
			}
			if (!regions.empty())
			{
				std::ofstream args(args_path);
				for (auto& region : regions)
				{
					args << "/characterregion:0x" << Util::to_hex_string(region.first);
					if (region.first != region.second)
					{
						args << "-0x" << Util::to_hex_string(region.second);
					}
					args << "\n";
				}
			}

			std::filesystem::remove(font_path);

			{
				std::wstring cmd(1, L'\"');
				cmd.append(bat_path);
				cmd.push_back(L'\"');
				_wsystem(cmd.c_str());
			}

			std::error_code ec{};
			std::filesystem::remove(args_path, ec);
			std::filesystem::remove(bat_path, ec);
			std::filesystem::remove(exe_path, ec);

			if (!had_console)
			{
				g_console.deinit();
			}
			if (g_renderer.inited)
			{
				g_renderer.loadUserFont();
			}
		});
	}
}
