
#include "Endpoints/Gdax/PFGdax.h"
#include "Endpoints/Bitstamp/PFBitstamp.h"

int main() {

	const char * host = "ws.pusherapp.com";
	const char * uri = "/app/de504dc5763aeef9ff52?client=pushcpp&version=1.0&protocol=5";

	Mette::Bitstamp::PFBitstamp pfBitstamp(host, uri);
	pfBitstamp.drive();

    return 0;
}