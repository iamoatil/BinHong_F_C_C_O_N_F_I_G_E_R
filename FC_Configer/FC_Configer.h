
// FC_Configer.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CFC_ConfigerApp:
// �йش����ʵ�֣������ FC_Configer.cpp
//

class CFC_ConfigerApp : public CWinApp
{
public:
	CFC_ConfigerApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CFC_ConfigerApp theApp;