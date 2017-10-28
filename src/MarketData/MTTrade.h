#ifndef MTTRADE_H
#define MTTRADE_H

#include "CommonDef.h"

#include <Poco/Timestamp.h>

namespace Mette {

    struct MTTrade {

        MTTrade (MTQty qty, MTPx px, int64_t mks)
    	: _qty(qty), _px(px), _ts(mks)
    	{}
      
		std::string toString() const
        {
			MTSstrm ss;
			ss << "Trade" << "[" << _qty << "] @ [" << _px << "]";
			return ss.str();
        }
    	
        MTQty _qty;
        MTPx _px;
		Poco::Timestamp _ts;
	};

}


#endif