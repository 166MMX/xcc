// cps_file.h: interface for the Ccps_file class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CPS_FILE_H__7EF89731_D4B2_11D3_A718_0000F81AF8CC__INCLUDED_)
#define AFX_CPS_FILE_H__7EF89731_D4B2_11D3_A718_0000F81AF8CC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "cc_structures.h"
#include "image_file.h"
#include "palet.h"

class Ccps_file: public Cimage_file<t_cps_header>
{
public:
	void decode(void*) const;
	Cvirtual_file extract_as_pcx(t_file_type ft, const t_palet _palet) const;

	bool is_valid() const
	{
		const t_cps_header& header = *get_header();
		int size = get_size();
		if (sizeof(t_cps_header) > size ||
			header.image_size != 320 * 200 ||				
			header.zero)
			return false;
		switch (header.unknown)
		{
		/*
		case 3:
			return header.size == size;
		*/
		case 4:
			return 2 + header.size == size;
		default:
			return false;
		}
	}

	int cx() const
	{
		return 320;
	}

	int cy() const
	{
		return 200;
	}

	const byte* get_image() const
	{
		return get_data() + sizeof(t_cps_header) + get_header()->palet_size;
	}

    const t_palet_entry* get_palet() const
    {
        return reinterpret_cast<const t_palet_entry*>(get_data() + sizeof(t_cps_header));
    }

	bool has_palet() const
	{
		return get_header()->palet_size;
	}
};

Cvirtual_binary cps_file_write(const byte* s, const t_palet_entry* palet);


#endif // !defined(AFX_CPS_FILE_H__7EF89731_D4B2_11D3_A718_0000F81AF8CC__INCLUDED_)
