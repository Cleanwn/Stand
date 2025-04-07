#pragma once

#include "atArray.hpp"

namespace rage
{
	// https://alexguirre.github.io/rage-parser-dumps/diff.html?game=gta5&build-a=3179&build-b=3258#fwProfanityFilter
	struct fwProfanityFilter
	{
		atArray<const char*> m_profaneTerms;
		atArray<const char*> m_wholeWords;
		atArray<const char*> m_profaneWords;
		atArray<const char*> m_reservedTerms; // similar to validate_file_name
		atArray<const char*> m_reservedFSCharacters; // <, >, :, ", /, \, |, ?, *
	};
}
