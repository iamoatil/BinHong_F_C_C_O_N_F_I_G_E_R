#include "StdAfx.h"
#include "OXGridList.h"


void COXGridList::OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult)
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
       //添加线程操作  
       //VERIFY(menu.LoadMenu(IDR_MENU2));           //这里是我们在1中定义的MENU的文件名称  
       //CMenu* popup = menu.GetSubMenu(0);  
       //ASSERT(popup != NULL);  
       //popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);  
 
        if (menu.CreatePopupMenu()==FALSE) return;

        menu.AppendMenu(0,2000,TEXT("菜单节点"));

        menu.AppendMenu(0,2001,TEXT("菜单节点"));

        menu.AppendMenu(0,2002,TEXT("菜单节点"));
		menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON,point.x,point.y,this);

       //下面的两行代码主要是为了后面的操作为准备的  
       //获取列表视图控件中第一个被选择项的位置    
       POSITION m_pstion = GetFirstSelectedItemPosition();  
       //该函数获取由pos指定的列表项的索引，然后将pos设置为下一个位置的POSITION值  
       //m_nIndex = GetNextSelectedItem(m_pstion);  
 
       //CString str;  
      // str.Format(L" m_nIndex = %d", m_nIndex);  
       //MessageBox(str, str, 0);  
   } 

}
