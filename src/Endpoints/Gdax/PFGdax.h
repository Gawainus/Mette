

#ifndef PFGDAX_H
#define PFGDAX_H

#include <Poco/Net/WebSocket.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>

namespace Mette {

	const char * GdaxAddr = "wss://ws-feed.gdax.com";
	const char * GdaxHost = "ws-feed.gdax.com";

	const char * GdaxRequest = "{\
		\"type\": \"subscribe\",\
		\"channels\" : [{ \"name\": \"heartbeat\", \"product_ids\" : [\"ETH-EUR\"] }]\
	}";

	class PFGdax {
	public:
		PFGdax() {
			Poco::Net::HTTPClientSession cs(GdaxHost);
			Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, GdaxRequest);
			Poco::Net::HTTPResponse response;

			Poco::Net::WebSocket ws(cs, request, response);


		}
	};


}

#endif
