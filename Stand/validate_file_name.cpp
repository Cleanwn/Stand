#include "validate_file_name.hpp"

#include "StringUtils.hpp"

namespace Stand
{
	bool validate_file_name(std::wstring name, bool allow_folder)
	{
		StringUtils::to_lower(name);
		return(name != L"con"
			&& name != L"prn"
			&& name != L"aux"
			&& name != L"nul"
			&& name != L"com1"
			&& name != L"com2"
			&& name != L"com3"
			&& name != L"com4"
			&& name != L"com5"
			&& name != L"com6"
			&& name != L"com7"
			&& name != L"com8"
			&& name != L"com9"
			&& name != L"lpt1"
			&& name != L"lpt2"
			&& name != L"lpt3"
			&& name != L"lpt4"
			&& name != L"lpt5"
			&& name != L"lpt6"
			&& name != L"lpt7"
			&& name != L"lpt8"
			&& name != L"lpt9"
			&& name.find(L'<') == std::wstring::npos
			&& name.find(L'>') == std::wstring::npos
			&& name.find(L':') == std::wstring::npos
			&& name.find(L'"') == std::wstring::npos
			&& name.find(L'|') == std::wstring::npos
			&& name.find(L'?') == std::wstring::npos
			&& name.find(L'*') == std::wstring::npos
			&& (
				allow_folder
				|| (
					name.find(L'/') == std::wstring::npos
					&& name.find(L'\\') == std::wstring::npos
					)
				)
			);
	}
}
