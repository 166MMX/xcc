// pkt_ts_ini_reader.h: interface for the Cpkt_ts_ini_reader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PKT_TS_INI_READER_H__27675600_FE41_11D4_B606_0000B4936994__INCLUDED_)
#define AFX_PKT_TS_INI_READER_H__27675600_FE41_11D4_B606_0000B4936994__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ini_reader.h"

class Cmap_data
{
public:
	Cmap_data();

	string m_description;
	string m_cd;
	int m_min_players;
	int m_max_players;
	string m_gamemode;
};

class Cpkt_ts_ini_reader: public Cini_reader  
{
public:
	enum t_section_id {sei_multi_maps, sei_unknown};
	enum t_map_id {mai_description, mai_cd, mai_minplayers, mai_maxplayers, mai_gamemode, mai_unknown};
	typedef map<string, Cmap_data> t_map_list;

	Cpkt_ts_ini_reader();
	void erase();
	int process_section_start(const string& line);
	bool process_section() const;
	int process_key(const string& name, const string& value);
	bool is_valid() const;
	void write_report(ostream& os) const;

	const t_map_list& get_map_list() const
	{
		return m_map_list;
	}
private:
	string m_current_map;
	t_section_id m_section;
	t_map_list m_map_list;
};

#endif // !defined(AFX_PKT_TS_INI_READER_H__27675600_FE41_11D4_B606_0000B4936994__INCLUDED_)
