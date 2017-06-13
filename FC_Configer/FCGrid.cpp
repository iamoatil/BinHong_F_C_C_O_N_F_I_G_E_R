#include "stdafx.h"
#include "FCGrid.h"
#include "resource.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
BEGIN_MESSAGE_MAP(CFCGrid,COXGridList)
	ON_NOTIFY_REFLECT(NM_RCLICK, &CFCGrid::OnNMRClick)
	ON_COMMAND(ID_32780, &CFCGrid::OnDelete)
END_MESSAGE_MAP()
CFCGrid::~CFCGrid()
{

}

void CFCGrid::OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;  
   if (pNMListView->iItem != -1)  
   {  
       DWORD dwPos = GetMessagePos();  
       CPoint point(LOWORD(dwPos), HIWORD(dwPos));  
       CMenu menu;  
       menu.LoadMenu(IDR_MENU2);   
       CMenu* popup = menu.GetSubMenu(0);  
       ASSERT(popup != NULL);  
       popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);  

   } 
}


void CFCGrid::OnDelete()
{
	HWND hwnd = this->GetParent()->m_hWnd;
	::SendMessage(hwnd, WM_USER+0x200,(WPARAM) this, (LPARAM) 1);
}
