#pragma once

#include "ScoutTransport.h"

namespace xero
{
	namespace scouting
	{
		namespace transport
		{
			class SCOUTTRANSPORT_EXPORT BLETransport : public ScoutTransport
			{
			public:
				BLETransport(bool server);
				~BLETransport();

				virtual bool init(std::stringstream& messages) ;
				virtual bool write(const QByteArray& data) ;
				virtual QByteArray readAll() ;
				virtual void flush() ;
				virtual void run() ;
				virtual QString type() ;
				virtual QString description() ;
				virtual void close() ;
			};
		}
	}
}


