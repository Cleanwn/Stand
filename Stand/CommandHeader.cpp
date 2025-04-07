#include "CommandHeader.hpp"

#include <filesystem>

#include <soup/ObfusString.hpp>

#include "alphanum.hpp"
#include "HttpRequestBuilder.hpp"
#include "bin.hpp"
#include "ensure_folder.hpp"
#include "Exceptional.hpp"
#include "Gui.hpp"
#include "MenuGrid.hpp"
#include "Renderer.hpp"
#include "StringUtils.hpp"
#include "Util.hpp"

namespace Stand
{
	static const std::string unanimated_headers[] = {
		"WillMa Techno by Cahya.jpg",
		"Untitled (2) by moue.png",
		"epic3dblenderrendernightmare by Thimo.png",
		"Komp 1 by Cahya.jpg",
		"Shitty Attempt #1 by super saiyan ginger.jpg",
		"Shitty Attempt #2 by super saiyan ginger.jpg",
		"Shitty Attempt #3 by super saiyan ginger.jpg",
		soup::ObfusString("2Horny2Stand.png"),
		"Amogus by boah.jpg",
		"Paragon by Denzy.png",
		"Minimalistic by PootisGodAnimations.png",
		"Quantum by Denzy.png",
		"JoJo by Spicy.png",
		"Paragon (Cyan) by Denzy.png",
		"Paragon (Purple) by Denzy.png",
		"GlitchFav4 by SourGrapeApe.png",
		"Take-Two Anti-Profit Tool.png",
		"Logo Concept 3 by Hollywood Collins.png",
		"Stan by synn420.jpg",
		"Rockstar Admin Tools.png",
		"Logo Concept 5 by Hollywood Collins.png",
		"A Tribute by Super Saiyan Ginger.png",
		"Stand And Fight by TGP.png",
	};

	struct AnimatedHeaderData
	{
		const std::string format;
		const uint16_t files;
	};

	static const AnimatedHeaderData animated_headers[] = {
		{"Untitled Animated by moue/STAND Header_{:0>5}.png", 197},
		{"2Horny2Stand Animated by super saiyan ginger/obligatory{:0>2}.jpg", 78},
		{"2Horny2Stand Animated 2 by super saiyan ginger/2horny2stand 2021{:0>3}.jpg", 271},
	};

	static std::wstring getCustomHeaderDirectory()
	{
		auto path = std::wstring(_wgetenv(L"appdata")).append(LR"(\Stand\Headers)");
		ensure_folder(path);
		path.append(L"\\Custom Header");
		ensure_folder(path);
		path.push_back(L'\\');
		return path;
	}

	static void ensureCustomHeader(std::wstring&& path)
	{
		if (std::filesystem::is_empty(path))
		{
			path.append(L"0.png");
			Util::bin_file(std::move(path), bin::defaultCustomHeader, sizeof(bin::defaultCustomHeader));
		}
	}

	CommandHeader::CommandHeader(CommandList* const parent)
		: CommandListSelect(parent, LOC("HDR"), CMDNAMES("header"), NOLABEL, {
			{0, LOC("2HIDE"), CMDNAMES("hide", "gone")},
			{23, LIT("Stand And Fight by TGP")},
			{22, LIT("A Tribute by Super Saiyan Ginger")},
			{21, LIT("Logo Concept 5 by Hollywood Collins")},
			{11, LIT("Minimalistic by PootisGodAnimations")},
			{20, LIT("Rockstar Admin Tools")},
			{10, LIT("Paragon by Denzy")},
			{15, LIT("Paragon (Purple) by Denzy")},
			{14, LIT("Paragon (Cyan) by Denzy")},
			{16, LIT("GlitchFav4 by SourGrapeApe")},
			{12, LIT("Quantum by Denzy")},
#ifdef STAND_DEV
			{8, LIT("2Horny2Stand"), {}, NOLABEL, COMMANDPERM_USERONLY, CMDFLAG_FEATURELIST_SKIP},
			{101, LIT("2Horny2Stand Animated by super saiyan ginger"), {}, NOLABEL, COMMANDPERM_USERONLY, CMDFLAG_FEATURELIST_SKIP},
			{102, LIT("2Horny2Stand Animated 2 by super saiyan ginger"), {}, NOLABEL, COMMANDPERM_USERONLY, CMDFLAG_FEATURELIST_SKIP},
#endif
			{9, LIT("Amogus by boah")},
			{13, LIT("JoJo by Spicy")},
			{6, LIT("Shitty Attempt #2 by super saiyan ginger")},
			{18, LIT("Logo Concept 3 by Hollywood Collins")},
			{19, LIT("Stan by synn420")},
			{17, LIT("Take-Two Anti-Profit Tool")},
			{1, LIT("WillMa Techno by Cahya")},
			{2, LIT("Untitled (2) by moue")},
			{3, LIT("epic3dblenderrendernightmare by Thimo")},
			{4, LIT("Komp 1 by Cahya")},
			{5, LIT("Shitty Attempt #1 by super saiyan ginger")},
			{7, LIT("Shitty Attempt #3 by super saiyan ginger")},
			{100, LIT("Untitled Animated by moue")},
			{200, LOC("CUS"), CMDNAMES("custom")},
		}, 0)
	{
		ensureCustomHeader(getCustomHeaderDirectory());
	}

	static void finishLoadingNoHeader()
	{
		g_renderer.header_state = HeaderState::HIDE;
		g_renderer.header_goal = 0;
		g_menu_grid.update();
	}

	static void loadHeader(std::vector<std::wstring>&& files)
	{
		if (files.empty())
		{
			return finishLoadingNoHeader();
		}
		g_renderer.header_state = HeaderState::LOADING;
		g_renderer.header_progress = 0;
		g_renderer.header_goal = (uint16_t)files.size();
		Exceptional::createManagedExceptionalThread(__FUNCTION__, [files{ std::move(files) }]
		{
			TextureArray header{};
			for (const auto& file : files)
			{
				if (g_renderer.loadTexture(file, header))
				{
					g_renderer.header_progress = (uint16_t)header.textures.size();
				}
				if (g_renderer.header_state != HeaderState::LOADING)
				{
					return;
				}
			}
			if (header.textures.empty())
			{
				return finishLoadingNoHeader();
			}
			g_renderer.header = std::make_unique<TextureArray>(std::move(header));
			g_renderer.header_state = HeaderState::SHOW;
			g_renderer.header_goal = 0;
			g_renderer.header_i = 0;
			g_renderer.header_last_frame = 0;
			g_menu_grid.update();
		});
	}

	static void downloadHeader(const std::string* filenameformat, uint16_t num = 1, std::vector<std::wstring>&& files = {})
	{
		const auto i = (uint16_t)files.size();
		g_renderer.header_progress = i;
		g_renderer.header_goal = num;
		auto callback = [filenameformat, num, files{std::move(files)}](std::wstring&& path) mutable
		{
			files.emplace_back(std::move(path));
			if (files.size() >= num)
			{
				loadHeader(std::move(files));
			}
			else
			{
				downloadHeader(filenameformat, num, std::move(files));
			}
		};
		auto path = std::wstring(_wgetenv(L"appdata")).append(LR"(\Stand\Headers\)");
		if (!std::filesystem::is_directory(path))
		{
			std::filesystem::create_directory(path);
		}
		const auto filename = fmt::format(fmt::runtime(*filenameformat), i);
		path.append(StringUtils::utf8_to_utf16(filename));
		if (std::filesystem::is_regular_file(path))
		{
			callback(std::move(path));
		}
		else
		{
			HttpRequestBuilder{ HttpRequestBuilder::GET, soup::ObfusString("stand.sh"), std::move(std::string("/dl/headers/").append(filename)) }
				.setResponseCallback([filenameformat, callback{ std::move(callback) }, path{ std::move(path) }](soup::HttpResponse&& resp) mutable
			{
				Util::bin_file_write(path, std::move(resp.body));
				callback(std::move(path));
			})
				.dispatch();
		}
	}

	void CommandHeader::onChange(Click& click, long long prev_value)
	{
		if (click.type == CLICK_AUTO || !g_gui.canUpdateThemeSetting())
		{
			return;
		}
		if (!applyValue())
		{
			auto click_ = click.deriveAuto();
			setValue(click_, prev_value);
		}
	}

	void CommandHeader::setState(Click& click, const std::string& state)
	{
		// -> 0.80.11
		if (state == "Stand And Fight by PRB Dess")
		{
			return CommandListSelect::setState(click, "Stand And Fight by TGP");
		}
		return CommandListSelect::setState(click, state);
	}

	bool CommandHeader::applyValue() const
	{
		g_renderer.header_goal = 0;
		if (value == 0)
		{
			g_renderer.header_state = HeaderState::HIDE;
			g_renderer.header.reset();
			g_menu_grid.update();
		}
		else if (value < 100)
		{
			g_renderer.header_state = HeaderState::DOWNLOADING;
			g_renderer.header.reset();
			downloadHeader(&unanimated_headers[value - 1]);
		}
		else if (value < 200)
		{
			auto path = std::wstring(_wgetenv(L"appdata")).append(LR"(\Stand\Headers\)").append(getCurrentValueMenuName().getEnglishUtf16());
			path.push_back(L'\\');
			if (!std::filesystem::is_directory(path))
			{
				std::filesystem::create_directory(path);
			}
			g_renderer.header_state = HeaderState::DOWNLOADING;
			g_renderer.header.reset();
			g_menu_grid.update();
			const auto& header = animated_headers[value - 100];
			g_renderer.header_goal = header.files;
			downloadHeader(&header.format, header.files);
		}
		else
		{
			g_renderer.header_state = HeaderState::LOADING;
			g_renderer.header.reset();
			auto path = getCustomHeaderDirectory();
			ensureCustomHeader(std::wstring(path));
			std::error_code ec{};
			std::vector<std::wstring> files{};
			for (const auto& file : std::filesystem::directory_iterator(path, ec))
			{
				if (file.is_regular_file())
				{
					files.emplace_back(file.path().wstring());
				}
			}
			if (files.size() > 1)
			{
				g_menu_grid.update();
				std::sort(files.begin(), files.end(), (bool(*)(const std::wstring&, const std::wstring&))&alphanum_less);
			}
			loadHeader(std::move(files));
		}
		return true;
	}
}
