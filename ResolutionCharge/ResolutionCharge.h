
// ResolutionCharge.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CResolutionChargeApp:
// �йش����ʵ�֣������ ResolutionCharge.cpp
//

class CResolutionChargeApp : public CWinApp
{
public:
	CResolutionChargeApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CResolutionChargeApp theApp;