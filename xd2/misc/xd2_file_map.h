// xd2_file_map.h: interface for the Cxd2_file_map class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XD2_FILE_MAP_H__8368B5EA_F681_11D6_B606_0000C006A2C2__INCLUDED_)
#define AFX_XD2_FILE_MAP_H__8368B5EA_F681_11D6_B606_0000C006A2C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "virtual_binary.h"
#include "xif_key.h"
#include "xif_key_r.h"

template<class T>
class Cxd2_file_map  
{
public:
	typedef map<string, T> t_map;

	void load(const Cxif_key_r& key)
	{
		m_map.clear();
		for (Cxif_key_r::t_key_map::const_iterator i = key.keys().begin(); i != key.keys().end(); i++)
			load_entry(i->second);
	}

	Cxif_key save() const
	{
		Cxif_key key;
		for (t_map::const_iterator i = m_map.begin(); i != m_map.end(); i++)
			key.open_key_write() = save(i);
		return key;
	}

	const T& get(const string& name) const
	{
		return m_map.find(name)->second;
	}
	
	bool has(const string& name) const
	{
		return m_map.find(name) != m_map.end();
	}

	const t_map& map() const
	{
		return m_map;
	}
	
	void set(const string& name, const T& d)
	{
		m_map[name] = d;
	}
private:
	enum
	{
		vi_name,
		vi_value,
	};

	void load_entry(const Cxif_key_r& key)
	{
		m_map[key.get_value_string(vi_name)] = T(key.get_key(vi_value));
	}

	static Cxif_key save(t_map::const_iterator i)
	{
		Cxif_key key;
		key.set_value_string(vi_name, i->first);
		key.open_key_write(vi_value) = i->second.save();
		return key;
	}

	t_map m_map;
};

void Cxd2_file_map<Cvirtual_binary>::load_entry(const Cxif_key_r& key)
{
	m_map[key.get_value_string(vi_name)] = key.get_value(vi_value).get_vdata();
}

Cxif_key Cxd2_file_map<Cvirtual_binary>::save(Cxd2_file_map<Cvirtual_binary>::t_map::const_iterator i)
{
	Cxif_key key;
	key.set_value_string(vi_name, i->first);
	key.set_value_binary(vi_value, i->second);
	return key;
}

typedef Cxd2_file_map<Cvirtual_binary> Cxd2_data_map;

#endif // !defined(AFX_XD2_FILE_MAP_H__8368B5EA_F681_11D6_B606_0000C006A2C2__INCLUDED_)
