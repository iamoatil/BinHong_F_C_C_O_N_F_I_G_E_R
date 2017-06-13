
// FC_ConfigerDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "serialport.h"
#include "afxcmn.h"
#include "OxGridList.h"
// CFC_ConfigerDlg 对话框
class CFCGrid : public COXGridList
{
public:
	virtual ~CFCGrid();
public:
	afx_msg void OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult);
	void OnMenu1(void);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDelete();
};
