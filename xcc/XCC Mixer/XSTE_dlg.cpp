// XSTE_dlg.cpp : implementation file
//

#include "stdafx.h"
#include "xcc mixer.h"
#include "XSTE_dlg.h"
#include "XSTE_edit_dlg.h"

#include "SearchStringDlg.h"
#include "mix_file.h"
#include "string_conversion.h"
#include "xcc_dirs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXSTE_dlg dialog


CXSTE_dlg::CXSTE_dlg(t_game game, CWnd* pParent /*=NULL*/)
	: ETSLayoutDialog(CXSTE_dlg::IDD, pParent, "XSTE_dlg")
{
	//{{AFX_DATA_INIT(CXSTE_dlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_buffer_w = 0;
	m_game = game;
}


void CXSTE_dlg::DoDataExchange(CDataExchange* pDX)
{
	ETSLayoutDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXSTE_dlg)
	DDX_Control(pDX, IDC_CAT_LIST, m_cat_list);
	DDX_Control(pDX, IDC_INSERT, m_insert);
	DDX_Control(pDX, IDC_EDIT, m_edit);
	DDX_Control(pDX, IDC_DELETE, m_delete);
	DDX_Control(pDX, IDC_LIST, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CXSTE_dlg, ETSLayoutDialog)
	//{{AFX_MSG_MAP(CXSTE_dlg)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnItemchangedList)
	ON_BN_CLICKED(IDC_INSERT, OnInsert)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST, OnEndlabeleditList)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST, OnGetdispinfoList)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST, OnColumnclickList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CAT_LIST, OnItemchangedCatList)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
	ON_BN_CLICKED(IDC_SEARCH, OnSearch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXSTE_dlg message handlers

static int c_colums = 3;
static char* column_label[] = {"Name", "Value", "Extra value"};

BOOL CXSTE_dlg::OnInitDialog() 
{
	ETSLayoutDialog::OnInitDialog();
	CreateRoot(VERTICAL)
		<< (pane(HORIZONTAL, GREEDY)
			<< item(IDC_CAT_LIST, ABSOLUTE_HORZ)
			<< item(IDC_LIST, GREEDY)
			)
		<< (pane(HORIZONTAL, ABSOLUTE_VERT)
			<< itemGrowing(HORIZONTAL)
			<< item(IDC_SEARCH, NORESIZE)
			<< item(IDC_INSERT, NORESIZE)
			<< item(IDC_EDIT, NORESIZE)
			<< item(IDC_DELETE, NORESIZE)
			<< item(IDOK, NORESIZE)
			<< item(IDCANCEL, NORESIZE)
			);
	UpdateLayout();
	SetRedraw(false);
	m_cat_list.SetExtendedStyle(m_cat_list.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	m_list.SetExtendedStyle(m_list.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	m_cat_list.InsertColumn(0, "");
	LV_COLUMN lvc;
	lvc.mask = LVCF_TEXT | LVCF_SUBITEM;
	for (int i = 0; i < c_colums; i++)
	{
		lvc.iSubItem = i;
		lvc.pszText = column_label[i];
		m_list.InsertColumn(i, &lvc);
	}
	if (m_fname.empty())
		m_fname = xcc_dirs::get_dir(m_game) + xcc_dirs::get_csf_fname(m_game);
	int error = m_f.open(m_fname);
	if (error)
	{
		switch (m_game)
		{
		case game_ra2:
		case game_ra2_yr:
			{
				Cmix_file language;
				error = language.open(xcc_dirs::get_language_mix(m_game));
				if (!error)
				{
					error = m_f.open(xcc_dirs::get_csf_fname(m_game), language);
					language.close();
				}
			}
			break;
		case game_gr:
		case game_gr_zh:
			{
				Cmix_file f;
				error = f.open(xcc_dirs::get_language_mix(m_game));
				if (!error)
				{
					error = m_f.open(xcc_dirs::get_csf_fname(m_game), f);
					if (!error)
						create_deep_dir(xcc_dirs::get_dir(m_game), m_game == game_gr ? "data/english/" : "data/englishzh/");
					f.close();
				}		
			}
			break;
		default:
			error = 1;
		}
	}
	if (!error)
	{
		create_cat_map();
		for (t_cat_map::const_iterator i = m_cat_map.begin(); i != m_cat_map.end(); i++)
			m_cat_list.SetItemData(m_cat_list.InsertItem(m_cat_list.GetItemCount(), i->second.c_str()), i->first);
		m_f.close();
	}	
	m_cat_list.SetColumnWidth(0, LVSCW_AUTOSIZE);
	check_selection();
	sort_list(0, false);
	SetRedraw(true);
	Invalidate();
	return true;
}

void CXSTE_dlg::open(const string& name)
{
	m_fname = name;
}

static string get_cat(const string& name)
{
	int a = name.find(':');
	int b = name.find('_');
	if (a == string::npos)
		return b == string::npos ? "Other" : to_upper(name.substr(0, b));
	return to_upper(b == string::npos ? name.substr(0, a) : name.substr(0, min(a, b)));
}

int CXSTE_dlg::get_cat_id(const string& name) const
{
	t_reverse_cat_map::const_iterator i = m_reverse_cat_map.find(get_cat(name));
	return (i == m_reverse_cat_map.end() ? m_reverse_cat_map.find("Other") : i)->second;
}

void CXSTE_dlg::create_cat_map()
{
	static int cat_id = 0;
	for (Ccsf_file::t_map::const_iterator i = m_f.get_map().begin(); i != m_f.get_map().end(); i++)
	{
		string cat = get_cat(i->first);
		if (m_reverse_cat_map.find(cat) == m_reverse_cat_map.end())
		{
			m_cat_map[cat_id] = cat;
			m_reverse_cat_map[cat] = cat_id++;
		}
		static int id = 0;
		t_map_entry& e = m_map[id++];
		e.i = i;
		e.cat_id = m_reverse_cat_map.find(cat)->second;
	}
	string cat = "Other";
	if (m_reverse_cat_map.find(cat) == m_reverse_cat_map.end())
	{
		m_cat_map[cat_id] = cat;
		m_reverse_cat_map[cat] = cat_id++;
	}
}

int CXSTE_dlg::insert(int id)
{
	int index = m_list.InsertItem(m_list.GetItemCount(), LPSTR_TEXTCALLBACK);
	m_list.SetItemData(index, id);
	return index;
}

int CXSTE_dlg::get_current_index()
{
	return m_list.GetNextItem(-1, LVNI_ALL | LVNI_FOCUSED);
}

void CXSTE_dlg::check_selection()
{
	int index = get_current_index();
	m_insert.EnableWindow(m_f.get_map().find("") == m_f.get_map().end());
	m_edit.EnableWindow(index != -1);
	m_delete.EnableWindow(index != -1);
}

void CXSTE_dlg::OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	check_selection();
	*pResult = 0;
}

int CXSTE_dlg::get_free_id()
{
	return m_map.empty() ? 0 : m_map.rbegin()->first + 1;
}

void CXSTE_dlg::set_map_entry(int id, const string& name)
{
	t_map_entry& e = m_map[id];
	e.i = m_f.get_map().find(name);
	e.cat_id = get_cat_id(name);
}

void CXSTE_dlg::OnInsert() 
{
	m_f.set_value("", wstring(), "");
	int id = get_free_id();
	set_map_entry(id, "");
	m_list.SetFocus();
	m_list.EditLabel(insert(id));
}

void CXSTE_dlg::OnEdit() 
{
	int index = get_current_index();
	CXSTE_edit_dlg dlg;
	string name = m_map.find(m_list.GetItemData(index))->second.i->first;
	Ccsf_file::t_map::const_iterator i = m_f.get_map().find(name);
	dlg.set(i->first, Ccsf_file::convert2string(i->second.value), i->second.extra_value);
	if (dlg.DoModal() == IDOK)
	{
		m_f.set_value(name, Ccsf_file::convert2wstring(dlg.get_value()), dlg.get_extra_value());
		m_list.Update(index);
	}
}

void CXSTE_dlg::OnDelete() 
{
	SetRedraw(false);
	int index;
	while ((index = m_list.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED)) != -1)
	{
		int id = m_list.GetItemData(index);
		m_f.erase_value(m_map.find(id)->second.i->first);
		m_map.erase(m_list.GetItemData(index));
		m_list.DeleteItem(index);
	}
	check_selection();
	SetRedraw(true);
	Invalidate();
}

void CXSTE_dlg::OnEndlabeleditList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	*pResult = false;
	const char* t = pDispInfo->item.pszText;
	if (t)
	{
		if (m_f.get_map().find(t) != m_f.get_map().end())
			return;
		t_map_entry& f = m_map[pDispInfo->item.lParam];
		string old_name = f.i->first;
		Ccsf_file::t_map_entry e = m_f.get_map().find(old_name)->second;
		m_f.erase_value(old_name);
		m_f.set_value(t, e.value, e.extra_value);
		f.i = m_f.get_map().find(t);
		f.cat_id = get_cat_id(t);
		if (old_name.empty())
			check_selection();
		*pResult = true;
	}
	else if (m_map.find(pDispInfo->item.lParam)->second.i->first.empty())
	{
		m_f.erase_value("");
		m_map.erase(pDispInfo->item.lParam);
		m_list.DeleteItem(pDispInfo->item.iItem);
		check_selection();
	}
}

void CXSTE_dlg::OnGetdispinfoList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	int id = m_list.GetItemData(pDispInfo->item.iItem);
	const t_map_entry& e = m_map.find(id)->second;
	switch (pDispInfo->item.iSubItem)
	{
	case 0:
		m_buffer[m_buffer_w] = e.i->first;
		break;
	case 1:
		m_buffer[m_buffer_w] = m_f.get_converted_value(e.i->first);
		break;
	case 2:
		m_buffer[m_buffer_w] = m_f.get_extra_value(e.i->first);
		break;
	}
	pDispInfo->item.pszText = const_cast<char*>(m_buffer[m_buffer_w].c_str());
	m_buffer_w--;
	if (m_buffer_w < 0)
		m_buffer_w += 4;
	*pResult = 0;
}

void CXSTE_dlg::OnOK() 
{
	ETSLayoutDialog::OnOK();
	m_f.erase_value("");
	m_f.write().export(m_fname);
}

void CXSTE_dlg::OnColumnclickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int column = reinterpret_cast<NM_LISTVIEW*>(pNMHDR)->iSubItem;
	sort_list(column, column == m_sort_column ? !m_sort_reverse : false);
	*pResult = 0;
}

static int compare_string(const string& a, const string& b)
{
	return a < b ? -1 : a != b;
}

int CXSTE_dlg::compare(int id_a, int id_b) const
{
	if (m_sort_reverse)
		swap(id_a, id_b);
	const t_map_entry& a = m_map.find(id_a)->second;
	const t_map_entry& b = m_map.find(id_b)->second;
	switch (m_sort_column)
	{
	case 0:
		return compare_string(a.i->first, b.i->first);
	case 1:
		return compare_string(m_f.get_converted_value(a.i->first), m_f.get_converted_value(b.i->first));
	case 2:
		return compare_string(m_f.get_extra_value(a.i->first), m_f.get_extra_value(b.i->first));
	default:
		return 0;
	}
}

static int CALLBACK Compare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	return reinterpret_cast<CXSTE_dlg*>(lParamSort)->compare(lParam1, lParam2);
}

void CXSTE_dlg::sort_list(int i, bool reverse)
{
	m_sort_column = i;
	m_sort_reverse = reverse;
	m_list.SortItems(Compare, reinterpret_cast<dword>(this));
}

void CXSTE_dlg::OnItemchangedCatList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	SetRedraw(false);
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->uNewState & LVIS_SELECTED)
	{
		if (~pNMListView->uOldState & LVIS_SELECTED)
		{
			m_list.DeleteAllItems();
			int cat_id = m_cat_list.GetItemData(pNMListView->iItem);
			for (t_map::const_iterator i = m_map.begin(); i != m_map.end(); i++)
			{
				if (i->second.cat_id == cat_id)
				{
					LVFINDINFO lvf;
					lvf.flags = LVFI_PARAM;
					lvf.lParam = i->first;
					int index = m_list.FindItem(&lvf, -1);
					if (index == -1)
						insert(i->first);
				}
			}
			m_sort_column = -1;
			for (int j = 0; j < c_colums; j++)
				m_list.SetColumnWidth(j, LVSCW_AUTOSIZE);
		}
	}
	SetRedraw(true);
	Invalidate();
	*pResult = 0;
}

void CXSTE_dlg::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnEdit();
}

void CXSTE_dlg::OnSearch() 
{
	CSearchStringDlg dlg;
	dlg.set(&m_f);
	if (IDOK == dlg.DoModal())
	{
		LVFINDINFO lfi;
		lfi.flags = LVFI_PARAM;
		lfi.lParam = m_reverse_cat_map.find(get_cat(dlg.m_selected))->second;
		int i = m_cat_list.FindItem(&lfi, -1);
		m_cat_list.SetItemState(i, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		m_cat_list.EnsureVisible(i, false);
		lfi.flags = LVFI_STRING;
		lfi.psz = dlg.m_selected.c_str();
		i = m_list.FindItem(&lfi, -1);
		m_list.SetItemState(i, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		m_list.EnsureVisible(i, false);
	}
}
