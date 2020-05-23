#pragma once

#include <QByteArray>

class CRCCCITT
{
public:
	static unsigned short checksum(const QByteArray& data);
	static unsigned short checksum(const QByteArray& data, int start, int len);

private:
	static const unsigned short CRC_CCITT_TABLE[256];
};

