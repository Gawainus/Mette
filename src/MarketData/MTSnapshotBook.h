

#ifndef MTBOOK_H
#define MTBOOK_H

#include "CommonDef.h"

#include <array>

#include <functional>
#include <stack>

namespace Mette
{
	namespace
	{
		double Peoriod10 = 10;
		double Multiplier_EMA10 = 2 / (Peoriod10 + 1);
	}
	
	struct MTSnapshotBook
	{
		
		MTSnapshotBook(): _ema10(0) {}
		
		using MtSide = std::array<std::pair<double, double>, MAX_BOOK_LEVEL>;

		double calcMktMiddle() const
		{
			assert(_asks.size() && _bids.size());

			const auto topAsk = _asks[0];
			const auto topBid = _bids[0];

			double numerator = topAsk.first * topBid.second + topBid.first * topAsk.second;
			double denominator = topAsk.second + topBid.second;
			return numerator / denominator;
		}

		double calcEMA10()
		{
			if (_ema10 == 0) _ema10 = calcMktMiddle();
			double mktMid = calcMktMiddle();
			_ema10 = (mktMid - _ema10)* Multiplier_EMA10 + _ema10;
			return _ema10;
		}

		std::string toString() const
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

		double _ema10;
	};

}


#endif
