// bmp_file.cpp: implementation of the Cbmp_file class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "bmp_file.h"

bool Cbmp_file::is_valid() const
{
	const byte* s = get_data();
	int size = get_size();
	return !(size < 4 
		|| s[0] != 'B'
		|| s[1] != 'M'
		|| s[2] != '6' && s[2] != '8');
}
