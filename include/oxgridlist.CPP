#include "StdAfx.h"
#include "OXGridList.h"


void COXGridList::OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
	*pResult = 0;
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;  
   if (pNMListView->iItem != -1)  
   {  
       DWORD dwPos = GetMessagePos();  
       CPoint point(LOWORD(dwPos), HIWORD(dwPos));  
       CMenu menu;  
       //�����̲߳���  
       //VERIFY(menu.LoadMenu(IDR_MENU2));           //������������1�ж����MENU���ļ�����  
       //CMenu* popup = menu.GetSubMenu(0);  
       //ASSERT(popup != NULL);  
       //popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);  
 
        if (menu.CreatePopupMenu()==FALSE) return;

        menu.AppendMenu(0,2000,TEXT("�˵��ڵ�"));

        menu.AppendMenu(0,2001,TEXT("�˵��ڵ�"));

        menu.AppendMenu(0,2002,TEXT("�˵��ڵ�"));
		menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON,point.x,point.y,this);

       //��������д�����Ҫ��Ϊ�˺���Ĳ���Ϊ׼����  
       //��ȡ�б���ͼ�ؼ��е�һ����ѡ�����λ��    
       POSITION m_pstion = GetFirstSelectedItemPosition();  
       //�ú�����ȡ��posָ�����б����������Ȼ��pos����Ϊ��һ��λ�õ�POSITIONֵ  
       //m_nIndex = GetNextSelectedItem(m_pstion);  
 
       //CString str;  
      // str.Format(L" m_nIndex = %d", m_nIndex);  
       //MessageBox(str, str, 0);  
   } 

}