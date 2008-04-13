#pragma once

#include "virtual_binary.h"
#include "xd2_file_map.h"

class Cxd2_image
{
public:
	Cxif_key save() const;
	Cxd2_image();
	Cxd2_image(const Cvirtual_binary& d, int cx, int cy);
	Cxd2_image(const Cxif_key_r& key);

	const byte* d() const
	{
		return m_d;
	}

	int cx() const
	{
		return m_cx;
	}

	int cy() const
	{
		return m_cy;
	}
private:
	Cvirtual_binary m_d;
	int m_cx;
	int m_cy;
};

typedef Cxd2_file_map<Cxd2_image> Cxd2_image_map;
