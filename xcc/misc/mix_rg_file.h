// mix_rg_file.h: interface for the Cmix_rg_file class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MIX_RG_FILE_H__7206A0C0_2457_11D6_B861_0000B4936994__INCLUDED_)
#define AFX_MIX_RG_FILE_H__7206A0C0_2457_11D6_B861_0000B4936994__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "cc_file.h"
#include "cc_structures.h"

using namespace std;

class Cmix_rg_file: public Ccc_file
{
public:
	int get_c_files() const
	{
		return m_index.size();
	}

	string get_name(int index) const
	{
		return m_old_index[index];
		t_index::const_iterator i = m_index.begin();
		while (index--)
			i++;
		return i->first;
	}

	int get_offset(const string& name) const
    {
        return m_index.find(name)->second.offset;
    }

    int get_size() const
	{
		return Ccc_file::get_size();
	}

	int get_size(const string& name) const
    {
        return m_index.find(name)->second.size;
    }

	bool is_valid();
	int post_open();
	Cmix_rg_file();
private:
	typedef map<string, t_mix_rg_index_entry> t_index;
	typedef vector<string> t_old_index;

	t_index m_index;
	t_old_index m_old_index;
};

#endif // !defined(AFX_MIX_RG_FILE_H__7206A0C0_2457_11D6_B861_0000B4936994__INCLUDED_)
