#pragma once

#include "scouttransport_global.h"
#include "ScoutTransport.h"
#include <QObject>

namespace xero
{
	namespace scouting
	{
		namespace transport
		{
			class SCOUTTRANSPORT_EXPORT ScoutServer : public QObject
			{
				Q_OBJECT

			public:
				ScoutServer(QObject *parent);
				virtual ~ScoutServer();

				virtual bool init() = 0;
				virtual void run() = 0;
				virtual QString name() = 0;
				virtual QString hwinfo() = 0;

			signals:
				void connected(ScoutTransport *);

			private:
			};
		}
	}
}

