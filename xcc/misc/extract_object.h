// extract_object.h: interface for the Cextract_object class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXTRACT_OBJECT_H__C5FFFB40_8F20_11D4_B606_0000B4936994__INCLUDED_)
#define AFX_EXTRACT_OBJECT_H__C5FFFB40_8F20_11D4_B606_0000B4936994__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include "art_ts_ini_reader.h"
#include "fname.h"
#include "multi_line.h"
#include "mix_file.h"
#include "neat_ini_reader.h"
#include "rules_ts_ini_reader.h"
#include "sound_ts_ini_reader.h"
#include "xif_key.h"

class Cextract_object  
{
public:
	enum t_object_type {ot_aircraft, ot_animation, ot_art, ot_building, ot_credits, ot_infantry, ot_infantry_sequence, ot_particle, ot_particle_system, ot_projectile, ot_sound, ot_vehicle, ot_voxel_animation, ot_warhead, ot_weapon, ot_unknown};

	Cextract_object();
	~Cextract_object();
	void open_default(t_game game) ;
	void close_all();
	int add_mix(string name, Cmix_file* g = NULL);
	void add_path(string name);
	int load();
	int load_art();
	int load_rules();
	int load_sound();
	Cxif_key extract(string name);
	int insert(const Cxif_key& k);
	int read_ini(const string& name, Cneat_ini_reader& ir);
	static string get_object_name(const Cxif_key& k);
	static int report(const Cxif_key& k, char* d, const string& files_url);
	void get_credits(const Cxif_key& k, string& name, string& mail, string& link_title, string& link);
	t_game get_game(Cxif_key& k) const;
	t_object_type get_object_type(const Cxif_key& k) const;
	int prepare_for_web(Cfname fname, byte* d);
	static void set_credits(Cxif_key& k, string name, string mail, string link_title, string link);

	const Crules_ts_ini_reader& get_rir() const
	{
		return m_rir;
	}
private:
	typedef map<string, t_object_type> t_art_list;
	typedef map<string, t_object_type> t_object_list;
	typedef map<string, string> t_key_list;
	typedef map<string, t_key_list> t_object_type_list;
	typedef list<Cmix_file> t_mix_list;
	typedef list<string> t_path_list;
	
	const t_object_type_list& get_object_type_list(t_object_type ot) const;
	void add_one(string name, t_object_type ot);
	void add(Cmulti_line l, t_object_type ot);
	void add(const t_key_list& kl, string s, t_object_type ot);
	int open(Ccc_file& f, string name);
	void store_file(string name, Cxif_key& k, int& n);
	void store_file(const t_key_list& kl, string name, string fext, Cxif_key& k, int& n);
	void store_art_list(Cxif_key& l);

	t_game m_game;
	t_art_list m_art_list;
	t_object_list m_object_list;
	t_mix_list m_mix_list;
	t_path_list m_path_list;
	Cart_ts_ini_reader m_air;
	Crules_ts_ini_reader m_rir;
	Csound_ts_ini_reader m_sir;
};

#endif // !defined(AFX_EXTRACT_OBJECT_H__C5FFFB40_8F20_11D4_B606_0000B4936994__INCLUDED_)
