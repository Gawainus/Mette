

#ifndef PFBITSTAMP_H
#define PFBITSTAMP_H

#include "CommonDef.h"

#include "MarketData/MTSnapshotBook.h"

#include <Poco/Net/WebSocket.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>

#include <Poco/JSON/Parser.h>
#include <Poco/Logger.h>


namespace Mette {

	namespace Bitstamp {
		using MTStr = std::string;
	

	const unsigned int BUFF_SIZE = 512*1024;
	
	class PFBitstamp {
	public:
		PFBitstamp(const MTStr & host, const MTStr & uri ):
			myClientSession(host,80),
			myRequest(Poco::Net::HTTPRequest::HTTP_GET, uri, "HTTP/1.1"),
			myWebSocket(myClientSession, myRequest, myResponse),
			myLogger(Poco::Logger::get("PFLoggerBitstamp")),
			myEMADiff(0),
			myIsAboveEMA(true),
			myPnL(0)
			{}

		bool drive();

	private:
		


		char m_buff[BUFF_SIZE];

		Poco::Net::HTTPClientSession myClientSession;
		Poco::Net::HTTPRequest myRequest;
		Poco::Net::HTTPResponse myResponse;

		Poco::Net::WebSocket myWebSocket;

		Poco::JSON::Parser myJSONParser;
		Poco::Logger & myLogger;


		MTSnapshotBook mySnapshotBook;

		MTPx	myEMADiff;
		bool	myIsAboveEMA;

		MTPx	myPnL;


	};
	}


}

#endif
