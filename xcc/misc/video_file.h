// video_file.h: interface for the Cvideo_file class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIDEO_FILE_H__030A5D34_EE6E_48A3_B1DD_0B58BA678BB2__INCLUDED_)
#define AFX_VIDEO_FILE_H__030A5D34_EE6E_48A3_B1DD_0B58BA678BB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cc_file_sh.h"

template <class T>
class Cvideo_file: public Ccc_file_sh<T>
{
public:
	virtual int cf() const = 0;
	virtual int cx() const = 0;
	virtual int cy() const = 0;
};

#endif // !defined(AFX_VIDEO_FILE_H__030A5D34_EE6E_48A3_B1DD_0B58BA678BB2__INCLUDED_)
