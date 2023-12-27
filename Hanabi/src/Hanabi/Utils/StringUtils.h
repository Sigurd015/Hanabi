#pragma once

#include <string>
#include <vector>
#include <regex>

namespace Hanabi::Utils
{
	class String
	{
	public:
		static std::vector<std::string> SplitStringAndKeepDelims(std::string str)
		{
			const static std::regex re(R"((^\W|^\w+)|(\w+)|[:()])", std::regex_constants::optimize);

			std::regex_iterator<std::string::iterator> rit(str.begin(), str.end(), re);
			std::regex_iterator<std::string::iterator> rend;
			std::vector<std::string> result;

			while (rit != rend)
			{
				result.emplace_back(rit->str());
				++rit;
			}
			return result;
		}
	};
}