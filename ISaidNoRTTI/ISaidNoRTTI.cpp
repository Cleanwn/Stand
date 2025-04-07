#include <fstream>
#include <iostream>
#include <vector>

static void eraseRttiByPattern(std::string& str, const char* pattern)
{
	size_t pattern_i = 0;
	size_t pattern_len = strlen(pattern);
	std::string::iterator rtti_start;
	std::string rttibuf{};
	for (auto i = str.begin(); i != str.end(); ++i)
	{
		const auto c = *i;
		if (pattern_i == pattern_len)
		{
			if (c != 0)
			{
				rttibuf.push_back(c);
				continue;
			}
			if (rttibuf.find("Stand") != std::string::npos
				|| rttibuf.find("soup") != std::string::npos
				)
			{
				for (auto j = rtti_start; j != i; ++j)
				{
					*j = 0;
				}
				//std::cout << "RTTI erased: " << std::move(rttibuf) << std::endl;
			}
			rttibuf.clear();
		}
		else if (pattern[pattern_i] == c)
		{
			if (pattern_i++ == 0)
			{
				rtti_start = i;
			}
			continue;
		}
		pattern_i = 0;
	}
}

int main(int, const char** args)
{
	const char* filename = args[1];

	std::ifstream file(filename, std::ios::binary);
	file.unsetf(std::ios::skipws);

	std::streampos file_size;
	file.seekg(0, std::ios::end);
	file_size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::string str;
	str.reserve(file_size);
	str.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();

	eraseRttiByPattern(str, ".?AV");
	eraseRttiByPattern(str, ".?AU");
	eraseRttiByPattern(str, ".PEA");
	eraseRttiByPattern(str, ".P6AX");

	std::ofstream ofile(filename, std::ios::binary);
	ofile.write(str.data(), str.size());

	return 0;
}
