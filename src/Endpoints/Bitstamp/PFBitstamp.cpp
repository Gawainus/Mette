#include "PFBitstamp.h"
#include "MarketData/MTTrade.h"

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
		*/

		myWebSocket.sendBytes(TradeRequest, std::strlen(TradeRequest));
		myLogger.information(TradeRequest);
		numBytesRecv = myWebSocket.receiveBytes(m_buff, BUFF_SIZE);
		recvStr = std::string(m_buff, numBytesRecv);
		myLogger.information(recvStr);

		while (true)
		{
			myWebSocket.sendBytes(PongReply, std::strlen(PongReply));

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

				if (eventName.compare("pusher:error") == 0)
				{
					myWebSocket.sendBytes(PongReply, std::strlen(PongReply));
					myLogger.information("Error event handled");
					continue;
				}

				if (eventName.compare("trade") == 0)
				{
					const auto channel = obj->get("channel");
					if (!channel.isEmpty())
					{
						MTStr channelName = channel.convert<MTStr>();
						if (channelName.compare("live_trades") == 0)
						{
							const auto & dataStr = obj->get("data");
							const auto objData = myJSONParser.parse(dataStr);
							myJSONParser.reset();
							const auto data = objData.extract<Poco::JSON::Object::Ptr>();
							assert(data->size());

							const auto qty = data->getValue<double>("amount");
							const auto px = data->getValue<double>("price");
							MTTrade trade(qty, px, data->getValue<int64_t>("timestamp")*1000000);
							
							myEMADiff = px - mySnapshotBook._ema10;

							myLogger.information("EMA Diff [" + std::to_string(myEMADiff) + "]");

							bool issAboveEMAPrev = myIsAboveEMA;
							myEMADiff < 0 ? myIsAboveEMA = false : myIsAboveEMA = true;
							
							if (myIsAboveEMA != issAboveEMAPrev)
							{
								myLogger.information("Trade crossed EMA");
								if (myIsAboveEMA)
								{
									MTPx strikePx{ mySnapshotBook._bids[0].first };

									myPnL += strikePx;
									MTSstrm ss;
									ss << "Sold @ [" << strikePx << "]";
									myLogger.information(ss.str());
								}
								else
								{
									MTPx strikePx{ mySnapshotBook._asks[0].first };
									myPnL -= strikePx;
									MTSstrm ss;
									ss << "Bout @ [" << strikePx << "]";
									myLogger.information(ss.str());
								}

								myLogger.information("PnL [" + std::to_string(myPnL) + "]");
							}
							myLogger.information(trade.toString());
						}
					}

					continue;
				}

				if (eventName.compare("data") != 0)
				{
					myWebSocket.sendBytes(PongReply, std::strlen(PongReply));
					myLogger.information("Error event handled");
					continue;
				}
				const auto channel = obj->get("channel");
				if (!channel.isEmpty())
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

						const auto objAsks = data->get("asks");
						const auto asks = objAsks.extract<Poco::JSON::Array::Ptr>();

						assert(asks->size());

						for (size_t i = 0; i < asks->size(); i ++)
						{
							const auto ask = asks->getArray(i);
							const auto px = ask->get(0).convert<double>();
							const auto qty = ask->get(1).convert<double>();
							mySnapshotBook._asks[i] = std::make_pair(px, qty);
						}

						const auto objBids = data->get("bids");
						const auto bids = objBids.extract<Poco::JSON::Array::Ptr>();

						assert(bids->size());

						for (size_t i = 0; i < bids->size(); i++)
						{
							const auto bid = bids->getArray(i);
							const auto px = bid->get(0).convert<double>();
							const auto qty = bid->get(1).convert<double>();
							mySnapshotBook._bids[i] = std::make_pair(px, qty);
						}


						myLogger.information("EMA10 [" + std::to_string(mySnapshotBook.calcEMA10()) + "]");
						myLogger.debug(mySnapshotBook.toString());
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


