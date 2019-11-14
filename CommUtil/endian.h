#pragma once

bool little_endian()
{
	const unsigned num = 0xAABBCCDD;
	return reinterpret_cast<const unsigned char*> (&num)[0] == 0xDD;
}

bool big_endian()
{
	const unsigned num = 0xAABBCCDD;
	return reinterpret_cast<const unsigned char*> (&num)[0] == 0xAA;
}
