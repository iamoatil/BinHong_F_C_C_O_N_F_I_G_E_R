#include "StdAfx.h"
#include "oxgridedit.h"


void COXGridEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);

	pNewWnd->SetWindowTextA(_T(""));
	// TODO: 在此处添加消息处理程序代码
}
