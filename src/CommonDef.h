
#ifndef MT_COMMONDEF_H
#define MT_COMMONDEF_H

#include <string>
#include <sstream>
#include <cassert>

namespace Mette {

    using MTStr = std::string;
	using MTSstrm = std::stringstream;

	static const size_t MAX_BOOK_LEVEL = 100;
	static const size_t LOG_LEVEL = 10;
	static const const char * LOG_OFFSET = "							";

}

#endif