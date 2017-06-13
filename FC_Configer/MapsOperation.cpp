
// 转发表格的操作 : 实现文件
//

#include "stdafx.h"
#include "FC_Configer.h"
#include "FC_ConfigerDlg.h"
#include "afxdialogex.h"
#include "WinUser.h"
#include "ComportPara.h"
#include "DataFrame.h"
#include "OxGridList.h"
#include "OXGridEdit.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
int CFC_ConfigerDlg::UpDateGridView(CFCGrid* pGrid,int cursel)
{
	int i,item_count = pGrid->GetItemCount();
	//当前表格为空
	if( (0==item_count )||(cursel>=item_count))
		return -1;

	for(i=0;i<item_count;i++)
	{
		CString strIdex;
		strIdex.Format(_T("%d"),i+1);
		pGrid->SetItemText(i, 0, strIdex);
		UINT lvis_v = 0;
		if( cursel==i )
			lvis_v =LVIS_SELECTED | LVIS_FOCUSED ;
		pGrid->SetItemState(i,lvis_v,LVIS_SELECTED | LVIS_FOCUSED);
	}
	return i;
}
int CFC_ConfigerDlg::GridInsertItem(CFCGrid* pGrid)
{
	if( pGrid->GetItemCount()>255 )
	{
		MessageBox(_T("转发规则最大为256条"),_T("转发规则超限"),MB_OK|MB_ICONWARNING);
		return -1;
	}
	LV_ITEM lvi;
	TCHAR sBuffer[20];
	lvi.mask       = LVIF_TEXT | LVIF_IMAGE;
	lvi.cchTextMax = 0;
	lvi.lParam     = 0;
	int cursel = pGrid->GetCurSel()+1;
	lvi.iItem      = cursel;
	lvi.iSubItem   = 0;
	lvi.iImage	  = 1;		
	wsprintf(sBuffer, _T("%d"),cursel+1);
	lvi.pszText = sBuffer;	
	if( pGrid->InsertItem(&lvi)< 0)
		return -1;
	//增加规则的内容
	if(pGrid==&m_UGGridCtrl)
	{
		//m_strIP_HighPart_Uni.Format(_T("%d.%d.%d"),((m_nIP_Input>>24)&0xff),((m_nIP_Input>>16)&0xff),((m_nIP_Input>>8)&0xff));	
		//m_nIP_map = m_nIP_Input&0xff;
		SetGridItemText(pGrid,cursel,m_nIP_map,m_nDID_map);
	}
	else if(pGrid==&m_UGGridCtrl_BroadCast)
	{
		//m_nIP_map = m_nIP_Input&0xff;
		//m_strIP_HighPart_Broad.Format(_T("%d.%d.%d"),((m_nIP_Input>>24)&0xff),((m_nIP_Input>>16)&0xff),((m_nIP_Input>>8)&0xff));	
		SetGridItemText(pGrid,cursel,m_nIP_map,m_nDID_map);
	}
	else
		SetGridItemText(pGrid,cursel,m_nDID_ETH,m_nEthPortSel);
	return cursel;
}
int CFC_ConfigerDlg::GridDeleteItems(CFCGrid* pGrid)
{
	int item_count = pGrid->GetItemCount(),nIdx = 0;
	if( 0>= item_count)
		return -1;
	for(int i=item_count;i>=0;i--)
	{
		if( pGrid->GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED )
		{
			pGrid->DeleteItem(i);
			nIdx = i;
		}
	}
	return nIdx;
}
CString CFC_ConfigerDlg::EthPortToString(BYTE ports_sel)
{
	TCHAR sBuffer[20];
	CString str_rtn;
	memset(sBuffer,0,20);
	for(int m=0,n=0;m<4;m++)
	{
		if((ports_sel>>m)&0x1)
		{
			if(0!=n)
			{	
				wsprintf(sBuffer+n, _T(", %d"),m+1);
				n += 2;
			}
			else
				wsprintf(sBuffer+n, _T("%d"),m+1);
			n++;
		}
	}
	str_rtn = sBuffer;
	return str_rtn;
}

int CFC_ConfigerDlg::SetGridItemText(CFCGrid* pGrid,int row_number,int x,int y)
{
	//增加规则的内容
	CString strTxt[3];
	if(pGrid==&m_UGGridCtrl)
	{
		
		strTxt[0].Format(m_strIP_HighPart_Uni+_T(".%d"),x);
		//strTxt[0].Format(_T("%d.%d.%d.%d"),((m_nIP_Input>>24)&0xff),((m_nIP_Input>>16)&0xff),((m_nIP_Input>>8)&0xff),(m_nIP_Input&0xff));
		strTxt[1].Format(_T("%d"),y);
	}
	else if(pGrid==&m_UGGridCtrl_BroadCast)
	{
		strTxt[0].Format(m_strIP_HighPart_Broad+_T(".%d"),x);
		//strTxt[0].Format(_T("%d.%d.%d.%d"),((m_nIP_Input>>24)&0xff),((m_nIP_Input>>16)&0xff),((m_nIP_Input>>8)&0xff),(m_nIP_Input&0xff));
		strTxt[1].Format(_T("%d"),y);
	}
	else
	{
		strTxt[0].Format(_T("%d"),x);
		strTxt[1] = EthPortToString(y&0x0f);
		strTxt[2] = EthPortToString((y>>4)&0x0f);
	}


	for(int i=0;i<2;i++)
		pGrid->SetItemText(row_number, i+1, strTxt[i]);

	if( pGrid!=&m_UGGridCtrl )
		pGrid->SetItemText(row_number, 3, strTxt[2]);
	return 0;
}

//增加以太网到光口的转发表
void CFC_ConfigerDlg::OnBnClickedBtnInsertIpdid()
{
	UpdateData(TRUE);
	int cur_sel;
	if(FALSE == m_bUniBroad)
	{
		m_strIP_HighPart_Uni.Format(_T("%d.%d.%d"),((m_nIP_Input>>24)&0xff),((m_nIP_Input>>16)&0xff),((m_nIP_Input>>8)&0xff));	
		m_nIP_map = m_nIP_Input&0xff;		
		cur_sel = GridInsertItem(&m_UGGridCtrl);
	}
	else
	{
		m_nIP_map = m_nIP_Input&0xff;
		m_strIP_HighPart_Broad.Format(_T("%d.%d.%d"),((m_nIP_Input>>24)&0xff),((m_nIP_Input>>16)&0xff),((m_nIP_Input>>8)&0xff));			
		cur_sel = GridInsertItem(&m_UGGridCtrl_BroadCast);
	}
	if(cur_sel<0 )
		return;
	if(FALSE == m_bUniBroad)
		UpDateGridView(&m_UGGridCtrl,cur_sel);
	else
		UpDateGridView(&m_UGGridCtrl_BroadCast,cur_sel);
	m_nIP_Input += m_b_IP_DID_IP_AutoInc;
	m_nDID_map  += m_b_IP_DID_IP_AutoInc;
	UpdateData(FALSE);
}
//广播_删除以太网到光口的转发表格显示
void CFC_ConfigerDlg::OnBnClickedButtonDelIpfc_Broad()
{
	CFCGrid* pGrid = &m_UGGridCtrl_BroadCast;
	int nIdx = GridDeleteItems(pGrid);
	if(nIdx>=0)
		UpDateGridView(pGrid,nIdx);
}
//删除以太网到光口的转发表格显示
void CFC_ConfigerDlg::OnBnClickedButtonDelIpfc()
{
	CFCGrid* pGrid = &m_UGGridCtrl;
	int nIdx = GridDeleteItems(pGrid);
	if(nIdx>=0)
		UpDateGridView(pGrid,nIdx);
}
//以太网到光口的转发表格显示
void CFC_ConfigerDlg::ShowIP_DID_Map(void)
{
	MapDataToGridView(&m_UGGridCtrl);
	//Broad
	MapDataToGridView(&m_UGGridCtrl_BroadCast);
	//end
}
//光口到以太网的转发表格显示
void CFC_ConfigerDlg::ShowID_EthPort_Map(void)
{
	MapDataToGridView(&m_UGGridDID_Eth);
}
//转发规则数据到表格显示
void CFC_ConfigerDlg::MapDataToGridView(CFCGrid* pGrid)
{
	CArray<CPoint,CPoint>* pMap;
	LV_ITEM lvi;
	TCHAR sBuffer[20];
	lvi.mask       = LVIF_TEXT | LVIF_IMAGE;
	lvi.cchTextMax = 0;
	lvi.lParam     = 0;
	lvi.iSubItem   = 0;
	lvi.iImage	  = 1;
	pGrid->DeleteAllItems();
	if(pGrid==&m_UGGridCtrl)
		pMap = &FC_IP_DID_Map_UniCast;
	else if(pGrid==&m_UGGridCtrl_BroadCast)
		pMap = &FC_IP_DID_Map_BroadCast;
	else
		pMap = &FC_DID_ETH_Map;
	for( int j = 0;j<pMap->GetCount();j++)
	{
		lvi.iItem = j;
		wsprintf(sBuffer, _T("%d"),j+1);
		lvi.pszText = sBuffer;		
		if( pGrid->InsertItem(&lvi)>=0)
			SetGridItemText(pGrid,j,pMap->GetAt(j).x,pMap->GetAt(j).y);
	}
}

//增加光口到以太网的转发规则
void CFC_ConfigerDlg::OnBnClickedButtonInsertDideth()
{
	UpdateData(TRUE);
	BYTE temp = 0;
	for(int i=0;i<4;i++)
		temp += m_bEthPort[i]*(1<<i);
	if(0 != m_nEthCan)
		temp <<= 4;
	if( 0==temp)
	{
		MessageBox(_T("请选择至少一个网口！"),_T("无效转发规则"),MB_OK|MB_ICONWARNING);
		return;
	}
	m_nEthPortSel = temp;
	int cur_sel = GridInsertItem(&m_UGGridDID_Eth);
	if(cur_sel<0 )
		return;
	UpDateGridView(&m_UGGridDID_Eth,cur_sel);
	m_nDID_ETH += m_b_IP_DID_IP_AutoInc;
	UpdateData(FALSE);
}


//删除光口到以太网的转发表格的条目
void CFC_ConfigerDlg::OnBnClickedButtonDelDid()
{
	CFCGrid* pGrid = &m_UGGridDID_Eth;
	int nIdx = GridDeleteItems(pGrid);
	if(nIdx>=0)
		UpDateGridView(pGrid,nIdx);
}

