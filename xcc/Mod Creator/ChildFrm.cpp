#include "stdafx.h"

#include "ChildFrm.h"
#include "LeftView.h"
#include "XCC Mod CreatorView.h"

#include "fname.h"
#include "resource.h"

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_COMMAND(ID_FILE_EXPORT, OnFileExport)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_COMMAND(ID_EDIT_INVERT_SELECTION, OnEditInvertSelection)
	ON_COMMAND(ID_VIEW_REFRESH, OnViewRefresh)
	ON_COMMAND(ID_VIEW_LAUNCH, OnViewLaunch)
	ON_COMMAND(ID_MOD_CLEAR_GAME_DIRECTORY, OnModClearGameDirectory)
	ON_COMMAND(ID_MOD_ACTIVATE, OnModActivate)
	ON_COMMAND(ID_MOD_DEACTIVATE, OnModDeactivate)
	ON_COMMAND(ID_MOD_OPTIONS, OnModOptions)
	ON_COMMAND(ID_MOD_REPORT, OnModReport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CChildFrame::CChildFrame()
{
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::OnCreateClient( LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
		return FALSE;

	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CLeftView), CSize(100, 100), pContext) ||
		!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CXCCModCreatorView), CSize(100, 100), pContext))
	{
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}

	static_cast<CLeftView*>(m_wndSplitter.GetPane(0, 0))->set_other_pane(static_cast<CXCCModCreatorView*>(m_wndSplitter.GetPane(0, 1)));

	return TRUE;
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}

void CChildFrame::ActivateFrame(int nCmdShow)
{
	nCmdShow = SW_SHOWMAXIMIZED;
	CMDIChildWnd::ActivateFrame(nCmdShow);
}

CXCCModCreatorView* CChildFrame::GetRightPane()
{
	CWnd* pWnd = m_wndSplitter.GetPane(0, 1);
	CXCCModCreatorView* pView = DYNAMIC_DOWNCAST(CXCCModCreatorView, pWnd);
	return pView;
}

void CChildFrame::OnFileExport()
{
	GetRightPane()->export();
}

void CChildFrame::OnEditSelectAll()
{
	GetRightPane()->select_all();
}

void CChildFrame::OnEditInvertSelection()
{
	GetRightPane()->invert_selection();
}

void CChildFrame::OnViewRefresh()
{
	GetRightPane()->sync();
}

void CChildFrame::OnModActivate()
{
	CWaitCursor wait;
	if (GetRightPane()->GetDocument()->activate())
		MessageBox("Error activating mod.", NULL, MB_ICONERROR);
}

void CChildFrame::OnViewLaunch()
{
	GetRightPane()->launch();
}

void CChildFrame::OnModDeactivate()
{
	CWaitCursor wait;
	GetRightPane()->GetDocument()->deactivate();
}

void CChildFrame::OnModClearGameDirectory()
{
	GetRightPane()->clear_game_dir();
}

void CChildFrame::OnModOptions()
{
	GetRightPane()->options();
}

void CChildFrame::OnModReport()
{
	CWaitCursor wait;
	string fname = get_temp_path() + GetRightPane()->GetDocument()->options().mod_name + " Report.html";
	GetRightPane()->GetDocument()->report(fname);
	ShellExecute(m_hWnd, "open", fname.c_str(), NULL, NULL, SW_SHOW);
}


