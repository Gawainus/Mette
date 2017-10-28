

#ifndef MTBOOK_H
#define MTBOOK_H

#include "CommonDef.h"

#include <array>

#include <functional>
#include <stack>

namespace Mette
{
	
	struct MTBook
	{
		
		
		using MtSide = std::array<std::pair<double, double>, MAX_BOOK_LEVEL>;


		std::string getLogStr() const
		{

			std::stringstream ss;

			ss << '\n';
			
			size_t i = 0;
			std::stack<std::pair<double, double>> stack;
			for (const auto ask : _asks)
			{
				if (i >= LOG_LEVEL) break;
				stack.emplace(ask.first, ask.second);
				i++;
			}
			while (stack.size())
			{
				const auto pair = stack.top();
				ss << LOG_OFFSET << pair.second << " @ " << pair.first << '\n';
				stack.pop();
			}

			i = 0;
			for ( const auto bid: _bids)
			{
				if (i >= LOG_LEVEL) break;
				ss << bid.second << " @ " << bid.first << '\n';
				i++;
			}
			return ss.str();
		}

		MtSide _asks;
		MtSide _bids;

	};

}


#endif
