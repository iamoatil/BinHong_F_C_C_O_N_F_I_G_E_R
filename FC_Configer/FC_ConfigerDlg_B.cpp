
// FC_ConfigerDlg.cpp : 实现文件
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


#import "msxml3.dll"
 
using namespace MSXML2;

BOOL CFC_ConfigerDlg::Verify_FC_Ethernet(void)
{
	int count = FC_DID_ETH_Map.GetCount();
	if(count<2)
		return TRUE;
	//DID不能重复
	for(int i=0;i<count;i++)
	{
		int a = FC_DID_ETH_Map.GetAt(i).x;
		for(int j=i+1;j<count;j++)
		{
			int b = FC_DID_ETH_Map.GetAt(j).x;
			if( b==a )
			{
				CString err;
				err.Format(_T("第%d行与第%d行的ID或IP重复!"),j+1,i+1);
				MessageBox(err,_T("光纤数据帧到以太网的转发表"),MB_OK|MB_ICONWARNING);
				return FALSE;
			}
		}
	}

	
	return TRUE;
}

BOOL CFC_ConfigerDlg::Verify_Ethernet_FC(void)
{
	int count = FC_IP_DID_Map_UniCast.GetCount();
	if(count<2)
		return TRUE;
	//IP地址与DID不能重复
	for(int i=0;i<count;i++)
	{
		int a = FC_IP_DID_Map_UniCast.GetAt(i).x;
		int c = FC_IP_DID_Map_UniCast.GetAt(i).y;
		for(int j=i+1;j<count;j++)
		{
			int b = FC_IP_DID_Map_UniCast.GetAt(j).x;
			int d = FC_IP_DID_Map_UniCast.GetAt(j).y;
			if(( b==a )||( d==c ))
			{
				CString err;
				err.Format(_T("第%d与第%d条的IP或DID重复!"),j+1,i+1);
				MessageBox(err,_T("以太网到光纤端口的转发表"),MB_OK|MB_ICONWARNING);
				return FALSE;
			}
		}
	}
	return TRUE;
}
//广播
BOOL CFC_ConfigerDlg::Verify_Ethernet_FC_Broad(void)
{
	int count = FC_IP_DID_Map_BroadCast.GetCount();
	if(count<2)
		return TRUE;
	//IP地址与DID不能重复
	for(int i=0;i<count;i++)
	{
		int a = FC_IP_DID_Map_BroadCast.GetAt(i).x;
		int c = FC_IP_DID_Map_BroadCast.GetAt(i).y;
		for(int j=i+1;j<count;j++)
		{
			int b = FC_IP_DID_Map_BroadCast.GetAt(j).x;
			int d = FC_IP_DID_Map_BroadCast.GetAt(j).y;
			if(( b==a )||( d==c ))
			{
				CString err;
				err.Format(_T("第%d与第%d条的IP或DID重复!"),j+1,i+1);
				MessageBox(err,_T("以太网到光纤端口的转发表(广播)"),MB_OK|MB_ICONWARNING);
				return FALSE;
			}
		}
	}
	return TRUE;
}

//数据读写到板卡
BOOL CFC_ConfigerDlg::Verify_BroadIPPort(void)
{
	int count = m_Map_BroadIPPort.GetCount();
	if (count<2)
		return TRUE;
	//IP地址不能重复
	for (int i = 0; i<count; i++)
	{
		int a = m_Map_BroadIPPort.GetAt(i).x;

		for (int j = i + 1; j<count; j++)
		{
			int b = m_Map_BroadIPPort.GetAt(j).x;
			if (b == a)
			{
				CString err;
				err.Format(_T("第%d与第%d条的IP重复!"), j + 1, i + 1);
				MessageBox(err, _T("组播IP到网口的转发表"), MB_OK | MB_ICONWARNING);
				return FALSE;
			}
		}
	}
	return TRUE;
}

BOOL CFC_ConfigerDlg::Verify_SourceID()
{	
	int count=sscanf(m_SourceIDStr, "0x%x", &m_SourceID);
	if (count ==0)
	{
		CString err="SourceID的格式必须是0x开头的16进制数";
		MessageBox(err, _T("顶部SourceID"), MB_OK | MB_ICONWARNING);
		return FALSE;
	}
	if (m_SourceID >> 24 != 0)
	{
		CString err = "SourceID长度最大为3个字节";
		MessageBox(err, _T("顶部SourceID"), MB_OK | MB_ICONWARNING);
		return FALSE;
		return false;
	}
	return true;
}

void CFC_ConfigerDlg::IP_DID_TableToArray_Broad(void)
{	//显示表格转换为数组
	int count = m_UGGridCtrl_BroadCast.GetItemCount();
	if( 0==count)
	{
		FC_IP_DID_Map_BroadCast.RemoveAll();
		return;
	}
	CArray<CPoint,CPoint> Temp_Map;
	Temp_Map.RemoveAll();
	for(int i=0;i<count;i++)
	{
		//IP地址
		CString strItem = m_UGGridCtrl_BroadCast.GetItemText(i,1);
		DWORD  dwIP = inet_addr(strItem); 
		if(i==0)
		{
			int k = strItem.ReverseFind('.');
			if((k>=5)&&(k<12))
			{			
				m_strIP_HighPart_Broad = strItem.Mid(0,k);
				Broad_IP_H[0] = (dwIP)&0xff;
				Broad_IP_H[1] = (dwIP>>8)&0xff;
				Broad_IP_H[2] = (dwIP>>16)&0xff;
			}
		}
		//DID标号
		strItem = m_UGGridCtrl_BroadCast.GetItemText(i,2);
		int did = _ttoi(strItem);
		if((did<0)||(did>256))
		{
			CString err;
			err.Format(_T("第%d行的DID数值错误!"),i+1);
			MessageBox(err,_T("以太网到光纤端口的转发表(广播)"),MB_OK|MB_ICONWARNING);
			return;
		}
		Temp_Map.Add(CPoint((int)(dwIP>>24),did&0xff));
	}

	//更新转发表对应的数据
	FC_IP_DID_Map_BroadCast.RemoveAll();
	for(int i=0;i<Temp_Map.GetCount();i++)
		FC_IP_DID_Map_BroadCast.Add(Temp_Map.GetAt(i));
	Temp_Map.RemoveAll();
	return;
}
//
void CFC_ConfigerDlg::IP_DID_TableToArray(void)
{	//显示表格转换为数组
	int count = m_UGGridCtrl.GetItemCount();
	if( 0==count)
	{
		FC_IP_DID_Map_UniCast.RemoveAll();
		return;
	}
	CArray<CPoint,CPoint> Temp_Map;
	Temp_Map.RemoveAll();
	for(int i=0;i<count;i++)
	{
		//IP地址
		CString strItem = m_UGGridCtrl.GetItemText(i,1);
		DWORD  dwIP = inet_addr(strItem); 
		if(i==0)
		{
			int k = strItem.ReverseFind('.');
			if((k>=5)&&(k<12))
			{			
				m_strIP_HighPart_Uni = strItem.Mid(0,k);
				Uni_IP_H[0] = (dwIP)&0xff;
				Uni_IP_H[1] = (dwIP>>8)&0xff;
				Uni_IP_H[2] = (dwIP>>16)&0xff;
			}
		}
		//DID标号
		strItem = m_UGGridCtrl.GetItemText(i,2);
		int did = _ttoi(strItem);
		if((did<0)||(did>256))
		{
			CString err;
			err.Format(_T("第%d行的DID数值错误!"),i+1);
			MessageBox(err,_T("以太网到光纤端口的转发表"),MB_OK|MB_ICONWARNING);
			return;
		}
		Temp_Map.Add(CPoint((int)(dwIP>>24),did&0xff));
	}

	//更新转发表对应的数据
	FC_IP_DID_Map_UniCast.RemoveAll();
	for(int i=0;i<Temp_Map.GetCount();i++)
		FC_IP_DID_Map_UniCast.Add(Temp_Map.GetAt(i));
	Temp_Map.RemoveAll();
	return;
}

void CFC_ConfigerDlg::DID_Port_TableToArray(void)
{	//显示表格转换为数组
	int count = m_UGGridDID_Eth.GetItemCount();
	if( 0==count)
	{
		FC_DID_ETH_Map.RemoveAll();
		return;
	}
	CArray<CPoint,CPoint> Temp_Map;
	Temp_Map.RemoveAll();
	for (int i = 0; i < count; i++)
	{
		//网口
		CString strItem = m_UGGridDID_Eth.GetItemText(i, 2);
		strItem.Replace(_T("，"), _T(","));
		CString resToken;
		int curPos = 0;

		resToken = strItem.Tokenize(_T(", "), curPos);
		BYTE port = 0;
		while (resToken != _T(""))
		{
			BYTE port_num = _ttoi(resToken);
			if ((port_num == 0) || (port_num>4))
			{
				CString err;
				err.Format(_T("第%d行的端口号错误!"), i + 1);
				MessageBox(err, _T("光纤到以太网转发表错误"), MB_OK | MB_ICONWARNING);
			}
			port += (0x01 << (port_num - 1));
			resToken = strItem.Tokenize(_T(", "), curPos);
		};

		//CAN
		strItem = m_UGGridDID_Eth.GetItemText(i, 3);
		strItem.Replace(_T("，"), _T(","));
		curPos = 0;

		resToken = strItem.Tokenize(_T(", "), curPos);
		BYTE portHigh = 0;
		while (resToken != _T(""))
		{
			BYTE port_num = _ttoi(resToken);
			if ((port_num == 0) || (port_num > 4))
			{
				CString err;
				err.Format(_T("第%d行的端口号错误!"), i + 1);
				MessageBox(err, _T("光纤到以太网转发表错误"), MB_OK | MB_ICONWARNING);
			}
			portHigh += (0x01 << (port_num - 1));
			resToken = strItem.Tokenize(_T(", "), curPos);
		};

		if (0 == (portHigh + port))
		{
			CString err;
			err.Format(_T("第%d行没有选择端口!"), i + 1);
			MessageBox(err, _T("光纤到以太网转发表错误"), MB_OK | MB_ICONWARNING);
		}
		//DID
		strItem = m_UGGridDID_Eth.GetItemText(i, 1);
		int did = 0;
		if (portHigh == 0)
		{
			did = htonl(inet_addr(strItem));
		}
		else
		{
			did = _ttoi(strItem);
		}
		port += (portHigh << 4);
		Temp_Map.Add(CPoint(did, (int)port));
	}
	//更新转发表对应的数据
	FC_DID_ETH_Map.RemoveAll();
	for(int i=0;i<Temp_Map.GetCount();i++)
		FC_DID_ETH_Map.Add(Temp_Map.GetAt(i));
	Temp_Map.RemoveAll();
}

void CFC_ConfigerDlg::TableToArray_BroadIPPort(void)
{	//显示表格转换为数组
	int count = m_Grid_BroadIPPort.GetItemCount();
	if (0 == count)
	{
		m_Map_BroadIPPort.RemoveAll();
		return;
	}
	CArray<CPoint, CPoint> Temp_Map;
	Temp_Map.RemoveAll();
	for (int i = 0; i<count; i++)
	{
		//IP地址
		CString strItem = m_Grid_BroadIPPort.GetItemText(i, 1);
		DWORD  dwIP = htonl(inet_addr(strItem));
		//网口
		strItem = m_Grid_BroadIPPort.GetItemText(i, 2);
		strItem.Replace(_T("，"), _T(","));
		CString resToken;
		int curPos = 0;

		resToken = strItem.Tokenize(_T(", "), curPos);
		BYTE port = 0;
		while (resToken != _T(""))
		{
			BYTE port_num = _ttoi(resToken);
			if ((port_num == 0) || (port_num>4))
			{
				CString err;
				err.Format(_T("第%d行的端口号错误!"), i + 1);
				MessageBox(err, _T("组播IP到网口转发表错误"), MB_OK | MB_ICONWARNING);
			}
			port += (0x01 << (port_num - 1));
			resToken = strItem.Tokenize(_T(", "), curPos);
		};

		Temp_Map.Add(CPoint(dwIP, (int)port));
	}
	//更新转发表对应的数据
	m_Map_BroadIPPort.RemoveAll();
	for (int i = 0; i<Temp_Map.GetCount(); i++)
		m_Map_BroadIPPort.Add(Temp_Map.GetAt(i));
	Temp_Map.RemoveAll();
}

//保存配置文件
void CFC_ConfigerDlg::SaveFile()
{
	::CoInitialize(NULL);  
	UpdateData(TRUE);
	MSXML2::IXMLDOMDocumentPtr ptr_doc;  
	MSXML2::IXMLDOMElementPtr ptr_elmt_root;  

	IP_DID_TableToArray();
	if( !Verify_Ethernet_FC())
		return;
	IP_DID_TableToArray_Broad();
	if( !Verify_Ethernet_FC_Broad())
		return;
	DID_Port_TableToArray();
	if( !Verify_FC_Ethernet())
		return;
	TableToArray_BroadIPPort();
	if (!Verify_BroadIPPort())
		return;
	if (!Verify_SourceID())
	{
		return;
	}
	if( !VerifyDID_CAN_PORT())
		return;
	if( !VerifyDID_CAN_IP_FC())
		return;
	HRESULT HR = ptr_doc.CreateInstance(_uuidof(MSXML2::DOMDocument30));  
	if(!SUCCEEDED(HR))  
	{  
		MessageBox("faild!!");  
		return;  
	}  
	ptr_elmt_root = ptr_doc->createElement("ROOT");  
	ptr_elmt_root->setAttribute("ID","FC_AE_1553");  //设置根标签的属性;  
	ptr_doc->appendChild(ptr_elmt_root);  
	CString str_temp;  
	MSXML2::IXMLDOMElementPtr ptr_node,ptr_nodeA,ptr_nodeB;  

	ptr_nodeA = ptr_doc->createElement((_bstr_t)("单播以太网到光纤端口")); 
	str_temp.Format("IP Address to DID Number");  
	ptr_nodeA->setAttribute("ETH_DID_UNI",(_variant_t)str_temp);  

	ptr_elmt_root->appendChild(ptr_nodeA);
	for(int i=0;i<FC_IP_DID_Map_UniCast.GetCount();i++)  
	{  
		str_temp.Format("%d",i+1);  
		ptr_node = ptr_doc->createElement((_bstr_t)("序号"+str_temp));  
		ptr_node->put_text((_bstr_t)"转发规则");//设置标签的文本内容; 
		str_temp.Format("%d",FC_IP_DID_Map_UniCast.GetAt(i).y);  
		ptr_node->setAttribute("DID",(_variant_t)str_temp);  
		str_temp.Format("%d",FC_IP_DID_Map_UniCast.GetAt(i).x); // 192.168.0.
		ptr_node->setAttribute("IP",(_variant_t)str_temp);//设置标签的属性及内容;  
		ptr_nodeA->appendChild(ptr_node);  
	}  

//广播
	ptr_nodeA = ptr_doc->createElement((_bstr_t)("广播以太网到光纤端口")); 
	str_temp.Format("IP Address to DID Number");  
	ptr_nodeA->setAttribute("ETH_DID_BROAD",(_variant_t)str_temp);  

	ptr_elmt_root->appendChild(ptr_nodeA);
	for(int i=0;i<FC_IP_DID_Map_BroadCast.GetCount();i++)  
	{  
		str_temp.Format("%d",i+1);  
		ptr_node = ptr_doc->createElement((_bstr_t)("序号"+str_temp));  
		ptr_node->put_text((_bstr_t)"转发规则");//设置标签的文本内容; 
		str_temp.Format("%d",FC_IP_DID_Map_BroadCast.GetAt(i).y);  
		ptr_node->setAttribute("DID",(_variant_t)str_temp);  
		str_temp.Format("%d",FC_IP_DID_Map_BroadCast.GetAt(i).x); // 192.168.0.
		ptr_node->setAttribute("IP",(_variant_t)str_temp);//设置标签的属性及内容;  
		ptr_nodeA->appendChild(ptr_node);  
	} 
//

	ptr_nodeB = ptr_doc->createElement((_bstr_t)("光纤端口到以太网"));  
	str_temp.Format("DID Number to Ports");  
	ptr_nodeB->setAttribute("DID_Ethernet_Ports",(_variant_t)str_temp);  
	ptr_elmt_root->appendChild(ptr_nodeB);
 
	for(int i=0;i<FC_DID_ETH_Map.GetCount();i++)  
	{  
		str_temp.Format("%d",i+1);  
		ptr_node = ptr_doc->createElement((_bstr_t)("序号"+str_temp));  
		ptr_node->put_text((_bstr_t)"转发规则");//设置标签的文本内容; 
		str_temp.Format("%d",FC_DID_ETH_Map.GetAt(i).y);  
		ptr_node->setAttribute("Port",(_variant_t)str_temp);  
		str_temp.Format("%d",FC_DID_ETH_Map.GetAt(i).x); // 192.168.0.
		ptr_node->setAttribute("DID",(_variant_t)str_temp);//设置标签的属性及内容;  
		ptr_nodeB->appendChild(ptr_node);  
	}

	//组播IP到网口
	ptr_nodeB = ptr_doc->createElement((_bstr_t)("组播IP到网口"));
	str_temp.Format("BroadIP To Ports");
	ptr_nodeB->setAttribute("BroadIP_To_Ports", (_variant_t)str_temp);
	ptr_elmt_root->appendChild(ptr_nodeB);
	for (int i = 0; i<m_Map_BroadIPPort.GetCount(); i++)
	{
		str_temp.Format("%d", i + 1);
		ptr_node = ptr_doc->createElement((_bstr_t)("序号" + str_temp));
		ptr_node->put_text((_bstr_t)"转发规则");//设置标签的文本内容; 
		str_temp.Format("%d", m_Map_BroadIPPort.GetAt(i).y); // 192.168.0.
		ptr_node->setAttribute("Port", (_variant_t)str_temp);//设置标签的属性及内容;  
		str_temp.Format("%d", m_Map_BroadIPPort.GetAt(i).x);
		ptr_node->setAttribute("BroadIP", (_variant_t)str_temp);		
		ptr_nodeB->appendChild(ptr_node);
	}
	//SourceID
	ptr_node = ptr_doc->createElement((_bstr_t)("SourceID"));
	ptr_node->put_text((_bstr_t)"ID号");
	str_temp= m_SourceIDStr;
	ptr_node->setAttribute("SourceIDStr", (_variant_t)str_temp);
	ptr_elmt_root->appendChild(ptr_node);

	//CAN速率
	ptr_nodeA = ptr_doc->createElement((_bstr_t)("CAN速率"));  
	str_temp.Format("CAN Speed");  
	ptr_nodeA->setAttribute("CAN_Speed",(_variant_t)str_temp);  
	ptr_elmt_root->appendChild(ptr_nodeA);
	for(int i=0;i<4;i++)  
	{  
		str_temp.Format("%d",i+1);  
		ptr_node = ptr_doc->createElement((_bstr_t)("CAN通道"+str_temp));  

		ptr_node->put_text((_bstr_t)"CAN速率设定");//设置标签的文本内容; 
		str_temp.Format("%d",m_CAN_SPD[i]&0x3);  
		ptr_node->setAttribute("CAN_SPEED",(_variant_t)str_temp);  
		ptr_nodeA->appendChild(ptr_node);  
	}
//CAN 的port Id
	ptr_nodeA = ptr_doc->createElement((_bstr_t)("CAN端口ID"));  
	str_temp.Format("CAN_PORTID");  
	ptr_nodeA->setAttribute("CAN_PORTID",(_variant_t)str_temp);  
	ptr_elmt_root->appendChild(ptr_nodeA);
	for(int i=0;i<4;i++)  
	{  
		str_temp.Format("%d",i+1);  
		ptr_node = ptr_doc->createElement((_bstr_t)("CAN通道"+str_temp));  

		ptr_node->put_text((_bstr_t)"CAN端口ID");//设置标签的文本内容; 
		str_temp.Format("%d",m_nCAN_Port_ID[i]);  
		ptr_node->setAttribute("CAN_PORTID",(_variant_t)str_temp);  
		ptr_nodeA->appendChild(ptr_node);  
	}
//CAN 的发送DID
	ptr_nodeA = ptr_doc->createElement((_bstr_t)("CAN发送DID"));  
	str_temp.Format("CAN_SENDDID");  
	ptr_nodeA->setAttribute("CAN_SENDDID",(_variant_t)str_temp);  
	ptr_elmt_root->appendChild(ptr_nodeA);
	for(int i=0;i<4;i++)  
	{  
		str_temp.Format("%d",i+1);  
		ptr_node = ptr_doc->createElement((_bstr_t)("CAN通道"+str_temp));  

		ptr_node->put_text((_bstr_t)"CAN发送DID");//设置标签的文本内容; 
		str_temp.Format("%d",m_nCAN_Send_DID[i]&0xff);  
		ptr_node->setAttribute("CAN_SENDDID",(_variant_t)str_temp);  
		ptr_nodeA->appendChild(ptr_node);  
	}

	ptr_nodeB = ptr_doc->createElement((_bstr_t)("以太网端口优先级"));  
	str_temp = _T("Port_Prioriy");  
	ptr_nodeB->setAttribute("Ports_Prioriy",(_variant_t)str_temp);  
	ptr_elmt_root->appendChild(ptr_nodeB);
 
	for(int i=0;i<4;i++)  
	{  
		str_temp.Format("%d",i+1);  
		ptr_node = ptr_doc->createElement((_bstr_t)("端口"+str_temp));  
		ptr_node->put_text((_bstr_t)"优先级");//设置标签的文本内容; 
		str_temp.Format("%d",m_nPriorityEth[i]);  
		ptr_node->setAttribute("Priority",(_variant_t)str_temp);  
		ptr_nodeB->appendChild(ptr_node);  
	}

	ptr_node = ptr_doc->createElement((_bstr_t)("交换机ID"));  
	ptr_node->put_text((_bstr_t)"ID号");
	str_temp.Format("%d",m_nSwitchID);  
	ptr_node->setAttribute("SwitchID",(_variant_t)str_temp);  
	ptr_elmt_root->appendChild(ptr_node);

	ptr_node = ptr_doc->createElement((_bstr_t)("单播IP地址高字节"));  
	ptr_node->put_text((_bstr_t)"单播IP高字节");
	ptr_node->setAttribute("UniIPHighPart",(_variant_t)m_strIP_HighPart_Uni);  
	ptr_elmt_root->appendChild(ptr_node)
		;
	ptr_node = ptr_doc->createElement((_bstr_t)("组播IP地址高字节"));  
	ptr_node->put_text((_bstr_t)"组播IP高字节");
	ptr_node->setAttribute("BroadIPHighPart",(_variant_t)m_strIP_HighPart_Broad);  
	ptr_elmt_root->appendChild(ptr_node);

	ptr_doc->save("FC_IP_DID转发表格.XML");  
	if(NULL != ptr_node)
		ptr_node.Release();  
	if(NULL != ptr_nodeA)
		ptr_nodeA.Release();
	if(NULL != ptr_nodeB)
		ptr_nodeB.Release();
	ptr_elmt_root.Release();  
	ptr_doc.Release();  
	::CoUninitialize();  

}

//读取配置文件
void CFC_ConfigerDlg::OPenFile()
{
	::CoInitialize(NULL);  
	MSXML2::IXMLDOMDocumentPtr ptr_doc;   
	MSXML2::IXMLDOMElementPtr ptr_elmt_root,ptr_element;  
	MSXML2::IXMLDOMNodeListPtr ptr_node_list,ptr_node_list_sub; //某个节点的所以字节点  
	MSXML2::IXMLDOMNamedNodeMapPtr ptr_xml_atts;//某个节点的所有属性;  
	MSXML2::IXMLDOMNodePtr ptr_one_node,ptr_one_node_sub;  
	HRESULT HR = ptr_doc.CreateInstance(_uuidof(MSXML2::DOMDocument30));  
	if(!SUCCEEDED(HR))  
	{  
		MessageBox("XML 文件创建失败!");  
		return;  
	}
	VARIANT_BOOL isSuccessFul;
	isSuccessFul = ptr_doc->load("FC_IP_DID转发表格.XML");  
	if(isSuccessFul!=VARIANT_TRUE)
	{  
		MessageBox("读取文件失败!");  
		return;  
	}
	ptr_elmt_root = ptr_doc->GetdocumentElement();//获得根节点;  
	ptr_elmt_root->get_childNodes(&ptr_node_list);//获得根节点的所有子节点;  
	long root_nodes_num,nodes_num,atts_num;  	
	CPoint pt;
	CArray<CPoint,CPoint> *ptrAry = NULL;
	int* ptrB;
	ptr_node_list->get_length(&root_nodes_num);

	for(int i_root=0;i_root<root_nodes_num;i_root++)
	{
		ptrAry = NULL;
		ptrB = NULL;
		ptr_node_list->get_item(i_root,&ptr_one_node);
		ptr_one_node->get_attributes(&ptr_xml_atts); 
		ptr_xml_atts->get_item(0,&ptr_one_node_sub);
		//根节点下的名称
		CString str_name = (char*)(_bstr_t)ptr_one_node_sub->nodeName;  
		if( 0<= str_name.Find(_T("ETH_DID_UNI")))
			ptrAry = &FC_IP_DID_Map_UniCast;
		//广播
		else if( 0<= str_name.Find(_T("ETH_DID_BROAD")))
			ptrAry = &FC_IP_DID_Map_BroadCast;
		//
		else if( 0<= str_name.Find(_T("DID_Ethernet_Ports")))
			ptrAry = &FC_DID_ETH_Map;
		else if (0 <= str_name.Find(_T("BroadIP_To_Ports")))
			ptrAry = &m_Map_BroadIPPort;
		else if( 0<= str_name.Find(_T("CAN_Speed")))
			ptrB =m_CAN_SPD;
		else if( 0<= str_name.Find(_T("CAN_SENDDID")))
			ptrB = (int*)m_nCAN_Send_DID;
		else if( 0<= str_name.Find(_T("CAN_PORTID")))
			ptrB =(int*)m_nCAN_Port_ID;
		else if( 0<= str_name.Find(_T("Ports_Prioriy")))
			ptrB = (int*)m_nPriorityEth;
		else if( 0<= str_name.Find(_T("SwitchID")))
		{
			CString text = (char*)(_bstr_t)ptr_one_node_sub->text; 
			m_nSwitchID = _tstoi(text);
		}
		else if (0 <= str_name.Find(_T("SourceIDStr")))
		{
			CString text = (char*)(_bstr_t)ptr_one_node_sub->text;
			m_SourceIDStr = text;
			sscanf(m_SourceIDStr, "%x", &m_SourceID);
		}
		
		else if( 0<= str_name.Find(_T("UniIPHighPart")))
		{
			m_strIP_HighPart_Uni = (char*)(_bstr_t)ptr_one_node_sub->text; 
		}
		else if( 0<= str_name.Find(_T("BroadIPHighPart")))
		{
			m_strIP_HighPart_Broad = (char*)(_bstr_t)ptr_one_node_sub->text; 
		}

		ptr_one_node->get_childNodes(&ptr_node_list_sub);
		ptr_node_list_sub->get_length(&nodes_num);
		if(NULL!=ptrAry)
			ptrAry->RemoveAll();	
			//最多读取256条
		for(int i=0;i<min(nodes_num,256);i++)  
		{  
			ptr_node_list_sub->get_item(i,&ptr_one_node);//获得某个子节点;  
			ptr_one_node->get_attributes(&ptr_xml_atts);//获得某个节点的所有属性;  
			if(NULL!=ptr_xml_atts)
			{
				ptr_xml_atts->get_length(&atts_num);//获得所有属性的个数;  
				for(int j=0;j<atts_num;j++)  
				{  
					ptr_xml_atts->get_item(j,&ptr_one_node);//获得某个属性;  
					CString text = (char*)(_bstr_t)ptr_one_node->text; 
					if( 0 == j)
						pt.y = _tstoi(text);
					else
						pt.x = _tstoi(text);
				} 
				if(NULL!=ptrAry)
					ptrAry->Add(pt);
				if(NULL!=ptrB)
					*(ptrB+i) = pt.y;
			}
		}
	}
	 if(NULL != ptr_node_list)
		 ptr_node_list.Release();
	 if(NULL != ptr_xml_atts)
		 ptr_xml_atts.Release();
	ShowIP_DID_Map();
	ShowID_EthPort_Map();
	ShowBroadIP_Port_Map();
	CString read_info;
	read_info.Format(_T("从文件读入:单播IP到DID规则%d条，广播IP到DID规则%d条，DID到网口规则%d条"),
		FC_IP_DID_Map_UniCast.GetCount(),FC_IP_DID_Map_BroadCast.GetCount(),FC_DID_ETH_Map.GetCount());
	ShowMessgeInfor(TRUE,read_info);
	UpdateData(false);
}

int CFC_ConfigerDlg::Read_IP_DID_Map(LPARAM lp)
{ 
	 MSXML2::IXMLDOMNodeListPtr ptr_node_list; //某个节点的所以字节点  
	 MSXML2::IXMLDOMNamedNodeMapPtr ptr_xml_atts;//某个节点的所有属性;  
	 MSXML2::IXMLDOMNodePtr ptr_one_node = (MSXML2::IXMLDOMNodePtr)lp;
	 long nodes_num,atts_num,i;
	 ptr_one_node->get_childNodes(&ptr_node_list);
	 ptr_node_list->get_length(&nodes_num);
	 FC_IP_DID_Map_UniCast.RemoveAll();
	 CPoint pt;
	 //最多读取256条
	 for(i=0;i<nodes_num,i<256;i++)  
	 {  
		  ptr_node_list->get_item(i,&ptr_one_node);//获得某个子节点;  
		  ptr_one_node->get_attributes(&ptr_xml_atts);//获得某个节点的所有属性;  
		  ptr_xml_atts->get_length(&atts_num);//获得所有属性的个数;  
		  for(int j=0;j<atts_num;j++)  
		  {  
		   ptr_xml_atts->get_item(j,&ptr_one_node);//获得某个属性;  
		   CString T1 = (char*)(_bstr_t)ptr_one_node->nodeName;  
		   CString T2 = (char*)(_bstr_t)ptr_one_node->text; 
		   if( T1.Find(_T("DID"))>=0 )
		   {
			   pt.y = _tstoi(T2);
		   }
		   if( T1.Find(_T("IP"))>=0 )
		   {
			   pt.x = _tstoi(T2);
		   }
		  } 
		  FC_IP_DID_Map_UniCast.Add(pt);
	}
	 if(NULL != ptr_node_list)
		 ptr_node_list.Release();
	 if(NULL != ptr_xml_atts)
		 ptr_xml_atts.Release();
	 return i;
}

BOOL CFC_ConfigerDlg::VerifyDID_CAN_PORT(void)
{
	//CAN端口DID不能重复
	for(int i=0;i<3;i++)
	{
		BYTE a = m_nCAN_Send_DID[i];
		for(int j=i+1;j<4;j++)
		{
			BYTE b = m_nCAN_Send_DID[j];
			if( b==a )
			{
				CString err;
				err.Format(_T("第%d与第%d个CAN的DID重复!"),j+1,i+1);
				MessageBox(err,_T("CAN端口DID"),MB_OK|MB_ICONWARNING);
				return FALSE;
			}
		}
	}
	return TRUE;
}
BOOL CFC_ConfigerDlg::VerifyDID_CAN_IP_FC(void)
{
	//CAN端口DID与IP
	CString err;
	for(int i=0;i<4;i++)
	{
		int a = (int)m_nCAN_Send_DID[i];
		for(int j=0;j<FC_IP_DID_Map_UniCast.GetCount();j++)
		{
			int b = FC_IP_DID_Map_UniCast.GetAt(j).y;
			if( b==a )
			{
				err.Format(_T("第%d个CAN端口的DID与单播以太网转发表的第%d条DID重复!"),i+1,j+1);
				MessageBox(err,_T("CAN端口的DID与单播以太网转发表"),MB_OK|MB_ICONWARNING);
				return FALSE;
			}
		}
		for(int j=0;j<FC_IP_DID_Map_BroadCast.GetCount();j++)
		{
			int b = FC_IP_DID_Map_BroadCast.GetAt(j).y;
			if( b==a )
			{
				err.Format(_T("第%dCAN端口的DID与广播以太网转发表的第%d条DID重复!"),i+1,j+1);
				MessageBox(err,_T("CAN端口的DID与广播以太网转发表"),MB_OK|MB_ICONWARNING);
				return FALSE;
			}
		}
		for(int j=0;j<FC_DID_ETH_Map.GetCount();j++)
		{
			int b = FC_DID_ETH_Map.GetAt(j).x;
			if( b==a )
			{
				err.Format(_T("第%dCAN端口的DID与FC转发表的第%d条DID重复!"),i+1,j+1);
				MessageBox(err,_T("CAN端口的DID与FC转发表"),MB_OK|MB_ICONWARNING);
				return FALSE;
			}
		}
	}
	for(int i=0;i<FC_IP_DID_Map_UniCast.GetCount();i++)
	{
		int a = FC_IP_DID_Map_UniCast.GetAt(i).y;
		for(int j=0;j<FC_DID_ETH_Map.GetCount();j++)
		{
			int b = FC_DID_ETH_Map.GetAt(j).x;
			if( b==a )
			{
				err.Format(_T("单播以太网转发表第%d条与FC转发表的第%d条DID重复!"),i+1,j+1);
				MessageBox(err,_T("单播以太网与FC转发表"),MB_OK|MB_ICONWARNING);
				return FALSE;
			}
		}
	}
	for(int i=0;i<FC_IP_DID_Map_BroadCast.GetCount();i++)
	{
		int a = FC_IP_DID_Map_BroadCast.GetAt(i).y;
		for(int j=0;j<FC_DID_ETH_Map.GetCount();j++)
		{
			int b = FC_DID_ETH_Map.GetAt(j).x;
			if( b==a )
			{
				err.Format(_T("广播以太网转发表第%d条与FC转发表的第%d条DID重复!"),i+1,j+1);
				MessageBox(err,_T("广播以太网与FC转发表"),MB_OK|MB_ICONWARNING);
				return FALSE;
			}
		}
	}
	for(int i=0;i<FC_IP_DID_Map_BroadCast.GetCount();i++)
	{
		int a = FC_IP_DID_Map_BroadCast.GetAt(i).y;
		for(int j=0;j<FC_IP_DID_Map_UniCast.GetCount();j++)
		{
			int b = FC_IP_DID_Map_UniCast.GetAt(j).y;
			if( b==a )
			{
				err.Format(_T("广播以太网转发表第%d条与单播第%d条DID重复!"),i+1,j+1);
				MessageBox(err,_T("广播与单播以太网转发表"),MB_OK|MB_ICONWARNING);
				return FALSE;
			}
		}
	}
	return TRUE;
}