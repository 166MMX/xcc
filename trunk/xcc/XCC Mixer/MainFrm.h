// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__9452141A_D8CC_11D3_9F8E_0000F81AF9BE__INCLUDED_)
#define AFX_MAINFRM_H__9452141A_D8CC_11D3_9F8E_0000F81AF9BE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "XCCFileView.h"
#include "XCC MixerView.h"

#include <ddraw.h>
#include <mmsystem.h>
#include <dsound.h>
#include "cc_structures.h"
#include "mix_file.h"
#include "pal_file.h"
#include "xcc_apps.h"
#include "xm_types.h"

struct t_pal_map_list_entry
{
	string name;
	int parent;
};

struct t_pal_list_entry
{
	string name;
	t_palet palet;
	int parent;
};

typedef map<int, string> t_mix_list;
typedef map<int, t_pal_map_list_entry> t_pal_map_list;
typedef map<int, t_pal_list_entry> t_pal_list;

class CMainFrame : public CFrameWnd
{
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	CXCCMixerView* left_mix_pane();
	CXCCMixerView* right_mix_pane();
	CXCCFileView* file_info_pane();

// Operations
public:
	void do_mix(Cmix_file& f, const string& mix_name, int mix_parent, int pal_parent);
	void find_mixs(const string& dir, t_game game, string filter);
	// void find_paks(const string& dir, t_game game);
	void initialize_lists();
	void launch_xtw(t_game game);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	bool auto_select(t_game game, string palet);
	void set_palet(int id);
	void clean_pal_map_list();
	int mix_list_create_map(string name, string fname, int id, int parent);
	int pal_list_create_map(string name, int parent);
	BOOL OnIdle(LONG lCount);
	void close_dd();
	void open_dd();
	LPDIRECTDRAW get_dd();
	void close_ds();
	void open_ds();
	LPDIRECTSOUND get_ds();
	t_game get_game();
	string get_mix_name(int i) const;
	const t_paletentry* get_game_palet(t_game game);
	const t_paletentry* get_pal_data();
	int get_vxl_mode() const;
	void set_msg(const string& s);
	virtual ~CMainFrame();

	bool combine_shadows() const
	{
		return m_combine_shadows;
	}

	bool convert_from_td() const
	{
		return m_convert_from_td;
	}

	bool convert_from_ra() const
	{
		return m_convert_from_ra;
	}

	bool enable_compression() const
	{
		return m_enable_compression;
	}

	bool fix_shadows() const
	{
		return m_fix_shadows;
	}

	const t_mix_map_list& mix_map_list() const
	{
		return m_mix_map_list;
	}

	const t_mix_list& mix_list() const
	{
		return m_mix_list;
	}

	bool remap_team_colors() const
	{
		return m_remap_team_colors;
	}

	bool split_shadows() const
	{
		return m_split_shadows;
	}

	bool use_palet_for_conversion() const
	{
		return m_use_palet_for_conversion;
	}
protected:
	t_game m_game;
	t_palet m_td_palet;
	t_palet m_ra_palet;
	t_palet m_ts_palet;
	int m_palet_i;
	int m_vxl_mode;
	bool m_lists_initialized;
	int m_mix_i[game_unknown];
	int m_pal_i[game_unknown];
	t_mix_list m_mix_list;
	t_mix_map_list m_mix_map_list;
	t_pal_map_list m_pal_map_list;
	t_pal_list m_pal_list;
	bool m_combine_shadows;
	bool m_convert_from_td;
	bool m_convert_from_ra;
	bool m_enable_compression;
	bool m_fix_shadows;
	bool m_remap_team_colors;
	bool m_split_shadows;
	bool m_use_palet_for_conversion;
	CXCCMixerView* m_left_mix_pane;
	CXCCMixerView* m_right_mix_pane;
	CXCCFileView* m_file_info_pane;
	CSplitterWnd m_wndSplitter;
	CStatusBar m_wndStatusBar;
	LPDIRECTDRAW m_dd;
	LPDIRECTSOUND m_ds;
	CString m_reg_key;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewGameTD();
	afx_msg void OnViewGameRA();
	afx_msg void OnViewGameTS();
	afx_msg void OnUpdateViewGameTD(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewGameRA(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewGameTS(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileFoundUpdate(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewPaletUpdate(CCmdUI* pCmdUI);
	afx_msg void OnViewGameAuto();
	afx_msg void OnUpdateViewGameAuto(CCmdUI* pCmdUI);
	afx_msg void OnViewPaletAuto();
	afx_msg void OnUpdateViewPaletAuto(CCmdUI* pCmdUI);
	afx_msg void OnViewPaletPrev();
	afx_msg void OnViewPaletNext();
	afx_msg void OnViewPaletUseForConversion();
	afx_msg void OnUpdateViewPaletUseForConversion(CCmdUI* pCmdUI);
	afx_msg void OnViewPaletConvertFromTD();
	afx_msg void OnUpdateViewPaletConvertFromTD(CCmdUI* pCmdUI);
	afx_msg void OnViewPaletConvertFromRA();
	afx_msg void OnUpdateViewPaletConvertFromRA(CCmdUI* pCmdUI);
	afx_msg void OnInternetFsSite();
	afx_msg void OnInternetSeSite();
	afx_msg void OnInternetXccSite();
	afx_msg void OnViewVoxelNormal();
	afx_msg void OnUpdateViewVoxelNormal(CCmdUI* pCmdUI);
	afx_msg void OnViewVoxelSurfaceNormals();
	afx_msg void OnUpdateViewVoxelSurfaceNormals(CCmdUI* pCmdUI);
	afx_msg void OnViewVoxelDepthInformation();
	afx_msg void OnUpdateViewVoxelDepthInformation(CCmdUI* pCmdUI);
	afx_msg void OnConversionSplitShadows();
	afx_msg void OnUpdateConversionSplitShadows(CCmdUI* pCmdUI);
	afx_msg void OnUtilitiesXccAvPlayer();
	afx_msg void OnUpdateUtilitiesXccAvPlayer(CCmdUI* pCmdUI);
	afx_msg void OnUtilitiesXccEditor();
	afx_msg void OnUpdateUtilitiesXccEditor(CCmdUI* pCmdUI);
	afx_msg void OnUtilitiesXccMixEditor();
	afx_msg void OnUpdateUtilitiesXccMixEditor(CCmdUI* pCmdUI);
	afx_msg void OnViewDirectories();
	afx_msg void OnViewDirectxOptions();
	afx_msg void OnUtilitiesFS();
	afx_msg void OnUpdateUtilitiesFS(CCmdUI* pCmdUI);
	afx_msg void OnUtilitiesSEMM();
	afx_msg void OnUpdateUtilitiesSEMM(CCmdUI* pCmdUI);
	afx_msg void OnUtilitiesSE();
	afx_msg void OnUpdateUtilitiesSE(CCmdUI* pCmdUI);
	afx_msg void OnLaunchDune2();
	afx_msg void OnLaunchTD();
	afx_msg void OnLaunchRA();
	afx_msg void OnLaunchDune2000();
	afx_msg void OnLaunchRA2();
	afx_msg void OnLaunchTS();
	afx_msg void OnUpdateLaunchDune2(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLaunchDune2000(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLaunchRA(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLaunchRA2(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLaunchTD(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLaunchTS(CCmdUI* pCmdUI);
	afx_msg void OnLaunchXccThemeWriter();
	afx_msg void OnUpdateLaunchXccThemeWriter(CCmdUI* pCmdUI);
	afx_msg void OnFileSearch();
	afx_msg void OnFileCreateSFL();
	afx_msg void OnLaunchXOE_RA2();
	afx_msg void OnUpdateLaunchXOE_RA2(CCmdUI* pCmdUI);
	afx_msg void OnLaunchXOE_TS();
	afx_msg void OnUpdateLaunchXOE_TS(CCmdUI* pCmdUI);
	afx_msg void OnLaunchXSTE();
	afx_msg void OnUpdateLaunchXSTE(CCmdUI* pCmdUI);
	afx_msg void OnLaunchXSE();
	afx_msg void OnUpdateLaunchXSE(CCmdUI* pCmdUI);
	afx_msg void OnLaunchFA();
	afx_msg void OnUpdateLaunchFA(CCmdUI* pCmdUI);
	afx_msg void OnLaunchRAGE();
	afx_msg void OnUpdateLaunchRAGE(CCmdUI* pCmdUI);
	afx_msg void OnConversionEnableCompression();
	afx_msg void OnUpdateConversionEnableCompression(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnLaunchXTW_TS();
	afx_msg void OnUpdateLaunchXTW_TS(CCmdUI* pCmdUI);
	afx_msg void OnLaunchXTW_RA2();
	afx_msg void OnUpdateLaunchXTW_RA2(CCmdUI* pCmdUI);
	afx_msg void OnConversionCombineShadows();
	afx_msg void OnUpdateConversionCombineShadows(CCmdUI* pCmdUI);
	afx_msg void OnViewReport();
	afx_msg void OnUpdateViewReport(CCmdUI* pCmdUI);
	afx_msg void OnLaunchXSTE_RA2();
	afx_msg void OnUpdateLaunchXSTE_RA2(CCmdUI* pCmdUI);
	afx_msg void OnLaunchXSTE_RA2_YR();
	afx_msg void OnUpdateLaunchXSTE_RA2_YR(CCmdUI* pCmdUI);
	afx_msg void OnLaunchXTW_RA2_YR();
	afx_msg void OnUpdateLaunchXTW_RA2_YR(CCmdUI* pCmdUI);
	afx_msg void OnViewPaletSelect();
	afx_msg void OnViewPaletAutoSelect();
	afx_msg void OnUpdateViewPaletAutoSelect(CCmdUI* pCmdUI);
	afx_msg void OnLaunchXSTE_GR();
	afx_msg void OnUpdateLaunchXSTE_GR(CCmdUI* pCmdUI);
	afx_msg void OnLaunchXSTE_GR_ZH();
	afx_msg void OnUpdateLaunchXSTE_GR_ZH(CCmdUI* pCmdUI);
	afx_msg void OnLaunchXSTE_Open();
	afx_msg void OnConversionFixShadows();
	afx_msg void OnUpdateConversionFixShadows(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg void OnViewPalet(dword ID);
	afx_msg void OnUpdateViewPalet(CCmdUI* pCmdUI);
	afx_msg void OnLaunchApp(dword ID);
	afx_msg void OnUpdateLaunchApp(CCmdUI* pCmdUI);
	afx_msg void OnConversionRemapTeamColors();
	afx_msg void OnUpdateConversionRemapTeamColors(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
private:
	Cxcc_apps m_apps;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__9452141A_D8CC_11D3_9F8E_0000F81AF9BE__INCLUDED_)