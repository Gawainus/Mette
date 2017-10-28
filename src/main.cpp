
#include "Endpoints/Gdax/PFGdax.h"
#include "Endpoints/Bitstamp/PFBitstamp.h"

#include "Poco/AsyncChannel.h"
#include "Poco/SimpleFileChannel.h"
#include "Poco/PatternFormatter.h"
#include "Poco/FormattingChannel.h"

int main() {

	Poco::AutoPtr<Poco::SimpleFileChannel> pChannel(new Poco::SimpleFileChannel);
	pChannel->setProperty("path", "./mette.log");

	Poco::AutoPtr<Poco::PatternFormatter> pPF(new Poco::PatternFormatter);
	pPF->setProperty("pattern", "%Y-%m-%d %H:%M:%S %s: %t");
	Poco::AutoPtr<Poco::FormattingChannel> pFC(new Poco::FormattingChannel(pPF, pChannel));

	Poco::AutoPtr<Poco::AsyncChannel> pAsync(new Poco::AsyncChannel(pFC));
	auto & loggerRoot = Poco::Logger::root();
	loggerRoot.setChannel(pAsync);
	loggerRoot.setChannel(pAsync);
	loggerRoot.setLevel(Poco::Message::PRIO_TRACE);

	const char * host = "ws.pusherapp.com";
	const char * uri = "/app/de504dc5763aeef9ff52?client=pushcpp&version=1.0&protocol=5";

	Mette::Bitstamp::PFBitstamp pfBitstamp(host, uri);
	pfBitstamp.drive();

    return 0;
}
