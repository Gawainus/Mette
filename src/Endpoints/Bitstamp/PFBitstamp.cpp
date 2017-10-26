#include "PFBitstamp.h"

namespace Mette
{
	bool Bitstamp::PFBitstamp::drive()
	{
		auto numBytesRecv = myWebSocket.receiveBytes(m_buff, BUFF_SIZE);
		std::cout << "numBytes: " << numBytesRecv << '\n';
		std::cout << std::string(m_buff, numBytesRecv) << '\n';

		myWebSocket.sendBytes(TradeRequest, std::strlen(TradeRequest));
		myWebSocket.sendBytes(DiffOrderBookRequest, std::strlen(DiffOrderBookRequest));

		while (true)
		{
			numBytesRecv = myWebSocket.receiveBytes(m_buff, BUFF_SIZE);
			std::cout << "numBytes: " << numBytesRecv << '\n';

			std::string recvStr(m_buff, numBytesRecv);

			try
			{
				const auto obj = myJSONParser.parse(recvStr);
				myJSONParser.reset();
			}
			catch (...)
			{
				std::cout << "Invalid JSON [" << recvStr << "]\n";
			}

		}
	}
}


