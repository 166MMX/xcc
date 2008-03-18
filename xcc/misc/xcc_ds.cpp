#include "stdafx.h"
#include "xcc_ds.h"

Cxcc_ds::Cxcc_ds()
{
	pds = 0;
}

Cxcc_ds::~Cxcc_ds()
{
}

long Cxcc_ds::create(HWND ihWnd)
{
	m_hWnd = ihWnd;
	if (DS_OK != DirectSoundCreate(0, &pds, 0)) 
	{
		handle_error("Create failed");
		return 1;
	}
	if (DS_OK != pds->SetCooperativeLevel(m_hWnd, DSSCL_NORMAL))
	{
		handle_error("SetCooperativeLevel failed");
		return 1;
	}
	return 0;
}

void Cxcc_ds::handle_error(const string &s) const
{
	MessageBox(m_hWnd, s.c_str(), "DirectSound error", MB_ICONERROR | MB_OK);
}