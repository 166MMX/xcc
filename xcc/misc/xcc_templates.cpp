// xcc_template_images.cpp: implementation of the Cxcc_templates class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xcc_templates.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

const char* theater_xif_fname = "theater.xif";

void* Cxcc_templates::bib[3];
byte* image_data = 0;
byte* Cxcc_templates::image_list[256];
t_palet Cxcc_templates::palet;
byte Cxcc_templates::shade_rp[256];
t_template_data_entry Cxcc_templates::template_data[256];
byte Cxcc_templates::template_list[256][64];

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Cxcc_templates::~Cxcc_templates()
{
	delete[] image_data;
}

enum
{
	vi_td_cx,
	vi_td_cy,
	vi_td_c_images,
	vi_td_buildable,
	vi_td_moveable,
	vi_td_fname,
	vi_td_flags,
};

long Cxcc_templates::load_data()
{
	Ccc_file f(false);
	f.open(xcc_dirs::get_data_dir() + theater_xif_fname);
	if (!f.is_open())
		return 1;
	const dword size = f.get_size();
	byte* data = new byte[size];
	f.read(data, size);
	f.close();
	Cxif_key base_key;
	base_key.load_key(data, size);
	delete[] data;
	for (long i = 0; i < 0xd8; i++)
	{
		t_template_data_entry& td = template_data[i];
		const Cxif_key& tk = base_key.get_key(i);
		td.cx = tk.get_value_int(vi_td_cx);
		td.cy = tk.get_value_int(vi_td_cy);
		td.c_images = tk.get_value_int(vi_td_c_images);
		td.buildable = 0;
		td.moveable = 0;
		td.flags = 0;
		const string fname = template_code[i];
		/*
		if (fname.substr(0, 1) == "s")
			td.flags |= td_flags_ridge;
		if (fname.substr(0, 1) == "d")
			td.flags |= td_flags_road;
		if (fname.substr(0, 2) == "rv")
			td.flags |= td_flags_river;
		if (fname.substr(0, 2) == "sh")
			td.flags |= td_flags_shore;
		*/
		for (t_xif_value_map::const_iterator i = tk.m_values.begin(); i != tk.m_values.end(); i++)
		{
			switch (i->first)
			{
			case vi_td_buildable:
				td.buildable = *reinterpret_cast<const __int64*>(i->second.get_data());
				break;
			case vi_td_moveable:
				td.moveable = *reinterpret_cast<const __int64*>(i->second.get_data());
				break;
			case vi_td_flags:
				td.flags = i->second.get_int();
				break;
			}
		}
	}
	return 0;
}

long Cxcc_templates::save_data()
{
	Cxif_key base_key;
	for (long i = 0; i < 0xd8; i++)
	{
		Cxif_key& template_key = base_key.set_key(i);
		const t_template_data_entry& td = template_data[i];
		template_key.set_value(vi_td_cx, td.cx);
		template_key.set_value(vi_td_cy, td.cy);
		template_key.set_value(vi_td_c_images, td.c_images);
		template_key.set_value_string(vi_td_fname, template_code[i]);
		if (td.buildable)
			template_key.set_value(vi_td_buildable, td.buildable);
		if (td.moveable)
			template_key.set_value(vi_td_moveable, td.moveable);
		if (td.flags)
			template_key.set_value(vi_td_flags, td.flags);
	}
	return base_key.vdata().export(xcc_dirs::get_data_dir() + theater_xif_fname);
	/*
	Cfile32 f;
	if (f.open_write(xcc_dirs::get_data_dir() + theater_xif_fname))
		return 1;
	dword size = base_key.key_size();
	byte* data = new byte[size];
	base_key.save_key(data);
	f.write(data, size);
	delete[] data;
	f.close();
	return 0;
	*/
}

long Cxcc_templates::load_images(t_theater_id theater)
{
	static t_theater_id loaded_theater = static_cast<t_theater_id>(-1);
	if (theater == loaded_theater)
		return 0;
	delete[] image_data;
	image_data = 0;
	const string fname = Cxcc_mixs::get_theater_fname(theater);
	const string ext = '.' + string(fname).substr(0, 3);
	Cmix_file& mixf = Cxcc_mixs::get_theater_mix(theater);
	Ctmp_file f;
	memset(image_list, -1, sizeof(image_list));
	memset(template_list, -1, sizeof(template_list));
	dword write_i = 0;
// #define RELOAD
	for (long i = 0; i < 0xd8; i++)
	{
#ifdef RELOAD
		t_template_data_entry& td = template_data[i];
		td.c_images &= ~(0xff << (theater << 3));
		f.open(string(template_code[i]) + ext, mixf);
		if (f.is_open())
		{
			f.read_header();
			td.c_images |= f.get_c_images() << (theater << 3);
			f.close();
		}
#endif
		write_i += template_data[i].c_images >> (theater << 3) & 0xff;
	}
	image_data = new byte[576 * write_i];
	write_i = 0;
	byte* write_p = image_data;
	for (i = 0; i < 0xd8; i++)
	{
		t_template_data_entry& td = template_data[i];
		const dword c_images = td.c_images >> (theater << 3) & 0xff;
		if (!c_images)
			continue;
		f.open(string(template_code[i]) + ext, mixf);
		assert(f.is_open());
		image_list[i] = write_p;
		const dword c_tiles = td.cx * td.cy;
		const byte* ofs_list = f.get_index1();
		for (long j = 0; j < c_tiles; j++)
		{
			if (ofs_list[j] != 0xff)
			{
				template_list[i][j] = (write_p - image_list[i]) / 576;
				memcpy(write_p, f.get_image(j), 576);
				write_i++;
				write_p += 576;
			}
		}
		f.close();
	}
	{
		Ccc_file f(false);
		f.open(fname + ".pal", mixf);
		f.read(reinterpret_cast<byte*>(palet), 768);
		f.close();
		convert_palet_18_to_24(palet);
		f.open(fname.substr(0, 1) + "shade.mrf", mixf);
		f.read(shade_rp, 256);
		f.close();
	}
	{
		Cshp_file f;
		for (long i = 0; i < 3; i++)
		{
			f.open("bib" + n(3 - i) + ext, mixf);
			shp_images::load_shp(f, bib[i]);
			f.close();
		}
	}
	loaded_theater = theater;
	return 0;
}