// SearchFileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "xcc mixer.h"
#include "MainFrm.h"
#include "SearchFileDlg.h"

#include "string_conversion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSearchFileDlg dialog


CSearchFileDlg::CSearchFileDlg(CWnd* pParent /*=NULL*/)
	: ETSLayoutDialog(CSearchFileDlg::IDD, pParent, "find_file_dlg")
{
	m_reg_key = "find_file_dlg";
	//{{AFX_DATA_INIT(CSearchFileDlg)
	//}}AFX_DATA_INIT
	m_buffer_w = 0;
	m_filename = AfxGetApp()->GetProfileString(m_reg_key, "file_name");
}


void CSearchFileDlg::DoDataExchange(CDataExchange* pDX)
{
	ETSLayoutDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSearchFileDlg)
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Text(pDX, IDC_FILENAME, m_filename);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSearchFileDlg, ETSLayoutDialog)
	//{{AFX_MSG_MAP(CSearchFileDlg)
	ON_BN_CLICKED(IDOK, OnFind)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST, OnGetdispinfoList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchFileDlg message handlers

void CSearchFileDlg::set(CMainFrame* main_frame)
{
	m_main_frame = main_frame;
}

BOOL CSearchFileDlg::OnInitDialog() 
{
	ETSLayoutDialog::OnInitDialog();
	m_list.SetExtendedStyle(m_list.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	m_list.InsertColumn(0, "Name");
	CreateRoot(VERTICAL)
		<< (pane(HORIZONTAL, ABSOLUTE_VERT)
			<< item(IDC_FILENAME_STATIC, NORESIZE)
			<< item(IDC_FILENAME, GREEDY)
			)
		<< (pane(HORIZONTAL, GREEDY)
			<< item(IDC_LIST, GREEDY)
			)
		<< (pane(HORIZONTAL, ABSOLUTE_VERT)
			<< itemGrowing(HORIZONTAL)
			<< item(IDOK, NORESIZE)
			<< item(IDCANCEL, NORESIZE)
			);
	UpdateLayout();
	m_list.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	return true;
}

void CSearchFileDlg::find(Cmix_file& f, string file_name, string mix_name, int mix_id)
{
	{
		for (int i = 0; i < f.get_c_files(); i++)
		{
			const int id = f.get_id(i);
			string name = f.get_name(id);
			if (name.empty())
			{
				name = nh(8, id);
				if (Cmix_file::get_id(f.get_game(), file_name) == id)
					add(mix_name + " - " + name, mix_id, id);
			}
			else if (fname_filter(name, file_name))
				add(mix_name + " - " + name, mix_id, id);
		}
	}
	{
		const t_mix_map_list& mix_list = m_main_frame->mix_map_list();
		for (t_mix_map_list::const_iterator i = mix_list.begin(); i != mix_list.end(); i++)
		{
			if (i->second.parent != mix_id)
				continue;
			Cmix_file g;
			if (!g.open(i->second.id, f))
			{
				find(g, file_name, mix_name + " - " + (i->second.name.empty() ? nh(8, i->second.id) : i->second.name), i->first);
				g.close();
			}
		}
	}
}

void CSearchFileDlg::OnFind() 
{
	if (UpdateData(true))
	{
		CWaitCursor wait;
		m_list.DeleteAllItems();
		m_map.clear();
		const t_mix_map_list& mix_list = m_main_frame->mix_map_list();
		for (t_mix_map_list::const_iterator i = mix_list.begin(); i != mix_list.end(); i++)
		{
			if (i->second.fname.empty())
				continue;
			Cmix_file f;
			if (!f.open(i->second.fname))
			{
				const t_mix_map_list_entry& e = mix_list.find(i->second.parent)->second;
				find(f, get_filename(), e.name + " - " + i->second.name, i->first);
				f.close();
			}
		}
		{
			m_list.SetItemCount(m_map.size());
			for (t_map::const_iterator i = m_map.begin(); i != m_map.end(); i++)
				m_list.SetItemData(m_list.InsertItem(m_list.GetItemCount(), LPSTR_TEXTCALLBACK), i->first);
		}
	}
}

void CSearchFileDlg::add(string name, int mix_id, int file_id)
{
	t_map_entry& e = m_map[m_map.size()];
	e.name = name;
	e.id = file_id;
	e.parent = mix_id;
}

void CSearchFileDlg::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int index = m_list.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if (index != -1)
		open_mix(m_list.GetItemData(index));
	*pResult = 0;
}

void CSearchFileDlg::OnSize(UINT nType, int cx, int cy) 
{
	ETSLayoutDialog::OnSize(nType, cx, cy);
	if (m_list.GetSafeHwnd())
		m_list.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
}

void CSearchFileDlg::open_mix(int id)
{
	const t_map_entry& e = m_map.find(id)->second;
	m_main_frame->left_mix_pane()->open_location_mix(m_main_frame->mix_map_list().find(e.parent), e.id);
	EndDialog(IDCANCEL);
}

void CSearchFileDlg::OnDestroy() 
{
	ETSLayoutDialog::OnDestroy();
	m_filename = AfxGetApp()->WriteProfileString(m_reg_key, "file_name", m_filename);
}

void CSearchFileDlg::OnGetdispinfoList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	int id = pDispInfo->item.lParam;
	const t_map_entry& e = m_map.find(id)->second;
	m_buffer[m_buffer_w] = e.name;
	pDispInfo->item.pszText = const_cast<char*>(m_buffer[m_buffer_w].c_str());
	m_buffer_w--;
	if (m_buffer_w < 0)
		m_buffer_w += 4;
	*pResult = 0;
}
