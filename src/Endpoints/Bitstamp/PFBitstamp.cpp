#include "PFBitstamp.h"
#include <Poco/Net/NetException.h>

namespace
{
	const char * TradeRequest = "{\
		\"event\": \"pusher:subscribe\",\
		\"data\" : {\
		\"channel\": \"live_trades\",\
			\"event\" : \"trade\"}\
		}";

	const char * FullOrderBookRequest = "{\
		\"event\": \"pusher:subscribe\",\
		\"data\" : {\
		\"channel\": \"order_book\",\
			\"event\" : \"data\"}\
		}";

	const char * FullOrderBookTermi = "{\
		\"event\": \"pusher:unsubscribe\",\
		\"data\" : {\
		\"channel\": \"order_book\",\
			\"event\" : \"data\"}\
		}";

	const char * LiveOrderRequest = "{\
		\"event\": \"pusher:subscribe\",\
		\"data\" : {\
		\"channel\": \"live_orders\",\
			\"event\" : \"data\"}\
		}";

	const char * PongReply = "{\"event\": \"pusher:pong\"}";
}

namespace Mette
{
	bool Bitstamp::PFBitstamp::drive()
	{
		auto numBytesRecv = myWebSocket.receiveBytes(m_buff, BUFF_SIZE);
		myLogger.information(std::string(m_buff, numBytesRecv));


		myWebSocket.sendBytes(FullOrderBookRequest, std::strlen(FullOrderBookRequest));
		myLogger.information(FullOrderBookRequest);
		numBytesRecv = myWebSocket.receiveBytes(m_buff, BUFF_SIZE);
		std::string recvStr(m_buff, numBytesRecv);
		myLogger.information(recvStr);

		/*
		myWebSocket.sendBytes(FullOrderBookTermi, std::strlen(FullOrderBookTermi));
		myLogger.information(FullOrderBookTermi);
		numBytesRecv = myWebSocket.receiveBytes(m_buff, BUFF_SIZE);
		recvStr = std::string(m_buff, numBytesRecv);
		myLogger.information(recvStr);

		myWebSocket.sendBytes(TradeRequest, std::strlen(TradeRequest));
		myLogger.information(TradeRequest);
		*/
		

		while (true)
		{
			myWebSocket.sendBytes(PongReply, std::strlen(PongReply));
			myLogger.information("sent pong");
			try
			{
				numBytesRecv = myWebSocket.receiveBytes(m_buff, BUFF_SIZE);
				if (!numBytesRecv) continue;
				recvStr = std::string(m_buff, numBytesRecv);
			
				const auto rv = myJSONParser.parse(recvStr);
				myJSONParser.reset();



				const auto obj = rv.extract<Poco::JSON::Object::Ptr>();
				const auto event = obj->get("event");
				const auto eventName = event.convert<MTStr>();
				myLogger.critical("Event [" + eventName + "]");
				if (eventName.compare("pusher:ping") == 0)
				{
					myWebSocket.sendBytes(PongReply, std::strlen(PongReply));
					myLogger.information("Pong event handled");
					continue;
				}
				if (eventName.compare("pusher:error") == 0)
				{
					myWebSocket.sendBytes(PongReply, std::strlen(PongReply));
					myLogger.information("Error event handled");
					continue;
				}

				if (eventName.compare("data") != 0)
				{
					myWebSocket.sendBytes(PongReply, std::strlen(PongReply));
					myLogger.information("Error event handled");
					continue;
				}
				const auto channel = obj->get("channel");
				if (channel)
				{
					MTStr channelName = channel.convert<MTStr>();
					if (channelName.compare("order_book") == 0)
					{
						// I think this is lazy parsing. the object is not parsed in the initial call
						const auto & dataStr = obj->get("data");
						const auto objData = myJSONParser.parse(dataStr);
						myJSONParser.reset();
						const auto data = objData.extract<Poco::JSON::Object::Ptr>();
						assert(data->size());

						// build the book
						MTBook aBook;

						const auto objAsks = data->get("asks");
						const auto asks = objAsks.extract<Poco::JSON::Array::Ptr>();

						assert(asks->size());

						for (size_t i = 0; i < asks->size(); i ++)
						{
							const auto ask = asks->getArray(i);
							const auto px = ask->get(0).convert<double>();
							const auto qty = ask->get(1).convert<double>();
							aBook._asks[i] = std::make_pair(px, qty);
						}

						const auto objBids = data->get("bids");
						const auto bids = objBids.extract<Poco::JSON::Array::Ptr>();

						assert(bids->size());

						for (size_t i = 0; i < bids->size(); i++)
						{
							const auto bid = bids->getArray(i);
							const auto px = bid->get(0).convert<double>();
							const auto qty = bid->get(1).convert<double>();
							aBook._bids[i] = std::make_pair(px, qty);
						}



						myLogger.debug(aBook.getLogStr());

					}
					else
					{
						// TODO::
					}
				}

				else
				{
					// TODO:: log error
				}

			}
			catch(Poco::Net::WebSocketException e)
			{
				myLogger.fatal("exception recv [" + recvStr + "]");
				myLogger.fatal(e.what());
			}
			catch(Poco::SyntaxException e)
			{
				myLogger.fatal("exception recv [" + recvStr + "]");
				myLogger.fatal(e.what());
			}
			catch (...)
			{
				myLogger.fatal( "Unhandled exception recv [" + recvStr + "]");
			}
			
			
		}
	}
}


