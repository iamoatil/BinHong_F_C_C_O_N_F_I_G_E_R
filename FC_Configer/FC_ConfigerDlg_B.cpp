
// FC_ConfigerDlg.cpp : ʵ���ļ�
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
	//DID�����ظ�
	for(int i=0;i<count;i++)
	{
		int a = FC_DID_ETH_Map.GetAt(i).x;
		for(int j=i+1;j<count;j++)
		{
			int b = FC_DID_ETH_Map.GetAt(j).x;
			if( b==a )
			{
				CString err;
				err.Format(_T("��%d�����%d�е�ID��IP�ظ�!"),j+1,i+1);
				MessageBox(err,_T("��������֡����̫����ת����"),MB_OK|MB_ICONWARNING);
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
	//IP��ַ��DID�����ظ�
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
				err.Format(_T("��%d���%d����IP��DID�ظ�!"),j+1,i+1);
				MessageBox(err,_T("��̫�������˶˿ڵ�ת����"),MB_OK|MB_ICONWARNING);
				return FALSE;
			}
		}
	}
	return TRUE;
}
//�㲥
BOOL CFC_ConfigerDlg::Verify_Ethernet_FC_Broad(void)
{
	int count = FC_IP_DID_Map_BroadCast.GetCount();
	if(count<2)
		return TRUE;
	//IP��ַ��DID�����ظ�
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
				err.Format(_T("��%d���%d����IP��DID�ظ�!"),j+1,i+1);
				MessageBox(err,_T("��̫�������˶˿ڵ�ת����(�㲥)"),MB_OK|MB_ICONWARNING);
				return FALSE;
			}
		}
	}
	return TRUE;
}

//���ݶ�д���忨
BOOL CFC_ConfigerDlg::Verify_BroadIPPort(void)
{
	int count = m_Map_BroadIPPort.GetCount();
	if (count<2)
		return TRUE;
	//IP��ַ�����ظ�
	for (int i = 0; i<count; i++)
	{
		int a = m_Map_BroadIPPort.GetAt(i).x;

		for (int j = i + 1; j<count; j++)
		{
			int b = m_Map_BroadIPPort.GetAt(j).x;
			if (b == a)
			{
				CString err;
				err.Format(_T("��%d���%d����IP�ظ�!"), j + 1, i + 1);
				MessageBox(err, _T("�鲥IP�����ڵ�ת����"), MB_OK | MB_ICONWARNING);
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
		CString err="SourceID�ĸ�ʽ������0x��ͷ��16������";
		MessageBox(err, _T("����SourceID"), MB_OK | MB_ICONWARNING);
		return FALSE;
	}
	if (m_SourceID >> 24 != 0)
	{
		CString err = "SourceID�������Ϊ3���ֽ�";
		MessageBox(err, _T("����SourceID"), MB_OK | MB_ICONWARNING);
		return FALSE;
		return false;
	}
	return true;
}

void CFC_ConfigerDlg::IP_DID_TableToArray_Broad(void)
{	//��ʾ���ת��Ϊ����
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
		//IP��ַ
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
		//DID���
		strItem = m_UGGridCtrl_BroadCast.GetItemText(i,2);
		int did = _ttoi(strItem);
		if((did<0)||(did>256))
		{
			CString err;
			err.Format(_T("��%d�е�DID��ֵ����!"),i+1);
			MessageBox(err,_T("��̫�������˶˿ڵ�ת����(�㲥)"),MB_OK|MB_ICONWARNING);
			return;
		}
		Temp_Map.Add(CPoint((int)(dwIP>>24),did&0xff));
	}

	//����ת�����Ӧ������
	FC_IP_DID_Map_BroadCast.RemoveAll();
	for(int i=0;i<Temp_Map.GetCount();i++)
		FC_IP_DID_Map_BroadCast.Add(Temp_Map.GetAt(i));
	Temp_Map.RemoveAll();
	return;
}
//
void CFC_ConfigerDlg::IP_DID_TableToArray(void)
{	//��ʾ���ת��Ϊ����
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
		//IP��ַ
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
		//DID���
		strItem = m_UGGridCtrl.GetItemText(i,2);
		int did = _ttoi(strItem);
		if((did<0)||(did>256))
		{
			CString err;
			err.Format(_T("��%d�е�DID��ֵ����!"),i+1);
			MessageBox(err,_T("��̫�������˶˿ڵ�ת����"),MB_OK|MB_ICONWARNING);
			return;
		}
		Temp_Map.Add(CPoint((int)(dwIP>>24),did&0xff));
	}

	//����ת�����Ӧ������
	FC_IP_DID_Map_UniCast.RemoveAll();
	for(int i=0;i<Temp_Map.GetCount();i++)
		FC_IP_DID_Map_UniCast.Add(Temp_Map.GetAt(i));
	Temp_Map.RemoveAll();
	return;
}

void CFC_ConfigerDlg::DID_Port_TableToArray(void)
{	//��ʾ���ת��Ϊ����
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
		//����
		CString strItem = m_UGGridDID_Eth.GetItemText(i, 2);
		strItem.Replace(_T("��"), _T(","));
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
				err.Format(_T("��%d�еĶ˿ںŴ���!"), i + 1);
				MessageBox(err, _T("���˵���̫��ת�������"), MB_OK | MB_ICONWARNING);
			}
			port += (0x01 << (port_num - 1));
			resToken = strItem.Tokenize(_T(", "), curPos);
		};

		//CAN
		strItem = m_UGGridDID_Eth.GetItemText(i, 3);
		strItem.Replace(_T("��"), _T(","));
		curPos = 0;

		resToken = strItem.Tokenize(_T(", "), curPos);
		BYTE portHigh = 0;
		while (resToken != _T(""))
		{
			BYTE port_num = _ttoi(resToken);
			if ((port_num == 0) || (port_num > 4))
			{
				CString err;
				err.Format(_T("��%d�еĶ˿ںŴ���!"), i + 1);
				MessageBox(err, _T("���˵���̫��ת�������"), MB_OK | MB_ICONWARNING);
			}
			portHigh += (0x01 << (port_num - 1));
			resToken = strItem.Tokenize(_T(", "), curPos);
		};

		if (0 == (portHigh + port))
		{
			CString err;
			err.Format(_T("��%d��û��ѡ��˿�!"), i + 1);
			MessageBox(err, _T("���˵���̫��ת�������"), MB_OK | MB_ICONWARNING);
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
	//����ת�����Ӧ������
	FC_DID_ETH_Map.RemoveAll();
	for(int i=0;i<Temp_Map.GetCount();i++)
		FC_DID_ETH_Map.Add(Temp_Map.GetAt(i));
	Temp_Map.RemoveAll();
}

void CFC_ConfigerDlg::TableToArray_BroadIPPort(void)
{	//��ʾ���ת��Ϊ����
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
		//IP��ַ
		CString strItem = m_Grid_BroadIPPort.GetItemText(i, 1);
		DWORD  dwIP = htonl(inet_addr(strItem));
		//����
		strItem = m_Grid_BroadIPPort.GetItemText(i, 2);
		strItem.Replace(_T("��"), _T(","));
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
				err.Format(_T("��%d�еĶ˿ںŴ���!"), i + 1);
				MessageBox(err, _T("�鲥IP������ת�������"), MB_OK | MB_ICONWARNING);
			}
			port += (0x01 << (port_num - 1));
			resToken = strItem.Tokenize(_T(", "), curPos);
		};

		Temp_Map.Add(CPoint(dwIP, (int)port));
	}
	//����ת�����Ӧ������
	m_Map_BroadIPPort.RemoveAll();
	for (int i = 0; i<Temp_Map.GetCount(); i++)
		m_Map_BroadIPPort.Add(Temp_Map.GetAt(i));
	Temp_Map.RemoveAll();
}

//���������ļ�
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
	ptr_elmt_root->setAttribute("ID","FC_AE_1553");  //���ø���ǩ������;  
	ptr_doc->appendChild(ptr_elmt_root);  
	CString str_temp;  
	MSXML2::IXMLDOMElementPtr ptr_node,ptr_nodeA,ptr_nodeB;  

	ptr_nodeA = ptr_doc->createElement((_bstr_t)("������̫�������˶˿�")); 
	str_temp.Format("IP Address to DID Number");  
	ptr_nodeA->setAttribute("ETH_DID_UNI",(_variant_t)str_temp);  

	ptr_elmt_root->appendChild(ptr_nodeA);
	for(int i=0;i<FC_IP_DID_Map_UniCast.GetCount();i++)  
	{  
		str_temp.Format("%d",i+1);  
		ptr_node = ptr_doc->createElement((_bstr_t)("���"+str_temp));  
		ptr_node->put_text((_bstr_t)"ת������");//���ñ�ǩ���ı�����; 
		str_temp.Format("%d",FC_IP_DID_Map_UniCast.GetAt(i).y);  
		ptr_node->setAttribute("DID",(_variant_t)str_temp);  
		str_temp.Format("%d",FC_IP_DID_Map_UniCast.GetAt(i).x); // 192.168.0.
		ptr_node->setAttribute("IP",(_variant_t)str_temp);//���ñ�ǩ�����Լ�����;  
		ptr_nodeA->appendChild(ptr_node);  
	}  

//�㲥
	ptr_nodeA = ptr_doc->createElement((_bstr_t)("�㲥��̫�������˶˿�")); 
	str_temp.Format("IP Address to DID Number");  
	ptr_nodeA->setAttribute("ETH_DID_BROAD",(_variant_t)str_temp);  

	ptr_elmt_root->appendChild(ptr_nodeA);
	for(int i=0;i<FC_IP_DID_Map_BroadCast.GetCount();i++)  
	{  
		str_temp.Format("%d",i+1);  
		ptr_node = ptr_doc->createElement((_bstr_t)("���"+str_temp));  
		ptr_node->put_text((_bstr_t)"ת������");//���ñ�ǩ���ı�����; 
		str_temp.Format("%d",FC_IP_DID_Map_BroadCast.GetAt(i).y);  
		ptr_node->setAttribute("DID",(_variant_t)str_temp);  
		str_temp.Format("%d",FC_IP_DID_Map_BroadCast.GetAt(i).x); // 192.168.0.
		ptr_node->setAttribute("IP",(_variant_t)str_temp);//���ñ�ǩ�����Լ�����;  
		ptr_nodeA->appendChild(ptr_node);  
	} 
//

	ptr_nodeB = ptr_doc->createElement((_bstr_t)("���˶˿ڵ���̫��"));  
	str_temp.Format("DID Number to Ports");  
	ptr_nodeB->setAttribute("DID_Ethernet_Ports",(_variant_t)str_temp);  
	ptr_elmt_root->appendChild(ptr_nodeB);
 
	for(int i=0;i<FC_DID_ETH_Map.GetCount();i++)  
	{  
		str_temp.Format("%d",i+1);  
		ptr_node = ptr_doc->createElement((_bstr_t)("���"+str_temp));  
		ptr_node->put_text((_bstr_t)"ת������");//���ñ�ǩ���ı�����; 
		str_temp.Format("%d",FC_DID_ETH_Map.GetAt(i).y);  
		ptr_node->setAttribute("Port",(_variant_t)str_temp);  
		str_temp.Format("%d",FC_DID_ETH_Map.GetAt(i).x); // 192.168.0.
		ptr_node->setAttribute("DID",(_variant_t)str_temp);//���ñ�ǩ�����Լ�����;  
		ptr_nodeB->appendChild(ptr_node);  
	}

	//�鲥IP������
	ptr_nodeB = ptr_doc->createElement((_bstr_t)("�鲥IP������"));
	str_temp.Format("BroadIP To Ports");
	ptr_nodeB->setAttribute("BroadIP_To_Ports", (_variant_t)str_temp);
	ptr_elmt_root->appendChild(ptr_nodeB);
	for (int i = 0; i<m_Map_BroadIPPort.GetCount(); i++)
	{
		str_temp.Format("%d", i + 1);
		ptr_node = ptr_doc->createElement((_bstr_t)("���" + str_temp));
		ptr_node->put_text((_bstr_t)"ת������");//���ñ�ǩ���ı�����; 
		str_temp.Format("%d", m_Map_BroadIPPort.GetAt(i).y); // 192.168.0.
		ptr_node->setAttribute("Port", (_variant_t)str_temp);//���ñ�ǩ�����Լ�����;  
		str_temp.Format("%d", m_Map_BroadIPPort.GetAt(i).x);
		ptr_node->setAttribute("BroadIP", (_variant_t)str_temp);		
		ptr_nodeB->appendChild(ptr_node);
	}
	//SourceID
	ptr_node = ptr_doc->createElement((_bstr_t)("SourceID"));
	ptr_node->put_text((_bstr_t)"ID��");
	str_temp= m_SourceIDStr;
	ptr_node->setAttribute("SourceIDStr", (_variant_t)str_temp);
	ptr_elmt_root->appendChild(ptr_node);

	//CAN����
	ptr_nodeA = ptr_doc->createElement((_bstr_t)("CAN����"));  
	str_temp.Format("CAN Speed");  
	ptr_nodeA->setAttribute("CAN_Speed",(_variant_t)str_temp);  
	ptr_elmt_root->appendChild(ptr_nodeA);
	for(int i=0;i<4;i++)  
	{  
		str_temp.Format("%d",i+1);  
		ptr_node = ptr_doc->createElement((_bstr_t)("CANͨ��"+str_temp));  

		ptr_node->put_text((_bstr_t)"CAN�����趨");//���ñ�ǩ���ı�����; 
		str_temp.Format("%d",m_CAN_SPD[i]&0x3);  
		ptr_node->setAttribute("CAN_SPEED",(_variant_t)str_temp);  
		ptr_nodeA->appendChild(ptr_node);  
	}
//CAN ��port Id
	ptr_nodeA = ptr_doc->createElement((_bstr_t)("CAN�˿�ID"));  
	str_temp.Format("CAN_PORTID");  
	ptr_nodeA->setAttribute("CAN_PORTID",(_variant_t)str_temp);  
	ptr_elmt_root->appendChild(ptr_nodeA);
	for(int i=0;i<4;i++)  
	{  
		str_temp.Format("%d",i+1);  
		ptr_node = ptr_doc->createElement((_bstr_t)("CANͨ��"+str_temp));  

		ptr_node->put_text((_bstr_t)"CAN�˿�ID");//���ñ�ǩ���ı�����; 
		str_temp.Format("%d",m_nCAN_Port_ID[i]);  
		ptr_node->setAttribute("CAN_PORTID",(_variant_t)str_temp);  
		ptr_nodeA->appendChild(ptr_node);  
	}
//CAN �ķ���DID
	ptr_nodeA = ptr_doc->createElement((_bstr_t)("CAN����DID"));  
	str_temp.Format("CAN_SENDDID");  
	ptr_nodeA->setAttribute("CAN_SENDDID",(_variant_t)str_temp);  
	ptr_elmt_root->appendChild(ptr_nodeA);
	for(int i=0;i<4;i++)  
	{  
		str_temp.Format("%d",i+1);  
		ptr_node = ptr_doc->createElement((_bstr_t)("CANͨ��"+str_temp));  

		ptr_node->put_text((_bstr_t)"CAN����DID");//���ñ�ǩ���ı�����; 
		str_temp.Format("%d",m_nCAN_Send_DID[i]&0xff);  
		ptr_node->setAttribute("CAN_SENDDID",(_variant_t)str_temp);  
		ptr_nodeA->appendChild(ptr_node);  
	}

	ptr_nodeB = ptr_doc->createElement((_bstr_t)("��̫���˿����ȼ�"));  
	str_temp = _T("Port_Prioriy");  
	ptr_nodeB->setAttribute("Ports_Prioriy",(_variant_t)str_temp);  
	ptr_elmt_root->appendChild(ptr_nodeB);
 
	for(int i=0;i<4;i++)  
	{  
		str_temp.Format("%d",i+1);  
		ptr_node = ptr_doc->createElement((_bstr_t)("�˿�"+str_temp));  
		ptr_node->put_text((_bstr_t)"���ȼ�");//���ñ�ǩ���ı�����; 
		str_temp.Format("%d",m_nPriorityEth[i]);  
		ptr_node->setAttribute("Priority",(_variant_t)str_temp);  
		ptr_nodeB->appendChild(ptr_node);  
	}

	ptr_node = ptr_doc->createElement((_bstr_t)("������ID"));  
	ptr_node->put_text((_bstr_t)"ID��");
	str_temp.Format("%d",m_nSwitchID);  
	ptr_node->setAttribute("SwitchID",(_variant_t)str_temp);  
	ptr_elmt_root->appendChild(ptr_node);

	ptr_node = ptr_doc->createElement((_bstr_t)("����IP��ַ���ֽ�"));  
	ptr_node->put_text((_bstr_t)"����IP���ֽ�");
	ptr_node->setAttribute("UniIPHighPart",(_variant_t)m_strIP_HighPart_Uni);  
	ptr_elmt_root->appendChild(ptr_node)
		;
	ptr_node = ptr_doc->createElement((_bstr_t)("�鲥IP��ַ���ֽ�"));  
	ptr_node->put_text((_bstr_t)"�鲥IP���ֽ�");
	ptr_node->setAttribute("BroadIPHighPart",(_variant_t)m_strIP_HighPart_Broad);  
	ptr_elmt_root->appendChild(ptr_node);

	ptr_doc->save("FC_IP_DIDת�����.XML");  
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

//��ȡ�����ļ�
void CFC_ConfigerDlg::OPenFile()
{
	::CoInitialize(NULL);  
	MSXML2::IXMLDOMDocumentPtr ptr_doc;   
	MSXML2::IXMLDOMElementPtr ptr_elmt_root,ptr_element;  
	MSXML2::IXMLDOMNodeListPtr ptr_node_list,ptr_node_list_sub; //ĳ���ڵ�������ֽڵ�  
	MSXML2::IXMLDOMNamedNodeMapPtr ptr_xml_atts;//ĳ���ڵ����������;  
	MSXML2::IXMLDOMNodePtr ptr_one_node,ptr_one_node_sub;  
	HRESULT HR = ptr_doc.CreateInstance(_uuidof(MSXML2::DOMDocument30));  
	if(!SUCCEEDED(HR))  
	{  
		MessageBox("XML �ļ�����ʧ��!");  
		return;  
	}
	VARIANT_BOOL isSuccessFul;
	isSuccessFul = ptr_doc->load("FC_IP_DIDת�����.XML");  
	if(isSuccessFul!=VARIANT_TRUE)
	{  
		MessageBox("��ȡ�ļ�ʧ��!");  
		return;  
	}
	ptr_elmt_root = ptr_doc->GetdocumentElement();//��ø��ڵ�;  
	ptr_elmt_root->get_childNodes(&ptr_node_list);//��ø��ڵ�������ӽڵ�;  
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
		//���ڵ��µ�����
		CString str_name = (char*)(_bstr_t)ptr_one_node_sub->nodeName;  
		if( 0<= str_name.Find(_T("ETH_DID_UNI")))
			ptrAry = &FC_IP_DID_Map_UniCast;
		//�㲥
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
			//����ȡ256��
		for(int i=0;i<min(nodes_num,256);i++)  
		{  
			ptr_node_list_sub->get_item(i,&ptr_one_node);//���ĳ���ӽڵ�;  
			ptr_one_node->get_attributes(&ptr_xml_atts);//���ĳ���ڵ����������;  
			if(NULL!=ptr_xml_atts)
			{
				ptr_xml_atts->get_length(&atts_num);//����������Եĸ���;  
				for(int j=0;j<atts_num;j++)  
				{  
					ptr_xml_atts->get_item(j,&ptr_one_node);//���ĳ������;  
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
	read_info.Format(_T("���ļ�����:����IP��DID����%d�����㲥IP��DID����%d����DID�����ڹ���%d��"),
		FC_IP_DID_Map_UniCast.GetCount(),FC_IP_DID_Map_BroadCast.GetCount(),FC_DID_ETH_Map.GetCount());
	ShowMessgeInfor(TRUE,read_info);
	UpdateData(false);
}

int CFC_ConfigerDlg::Read_IP_DID_Map(LPARAM lp)
{ 
	 MSXML2::IXMLDOMNodeListPtr ptr_node_list; //ĳ���ڵ�������ֽڵ�  
	 MSXML2::IXMLDOMNamedNodeMapPtr ptr_xml_atts;//ĳ���ڵ����������;  
	 MSXML2::IXMLDOMNodePtr ptr_one_node = (MSXML2::IXMLDOMNodePtr)lp;
	 long nodes_num,atts_num,i;
	 ptr_one_node->get_childNodes(&ptr_node_list);
	 ptr_node_list->get_length(&nodes_num);
	 FC_IP_DID_Map_UniCast.RemoveAll();
	 CPoint pt;
	 //����ȡ256��
	 for(i=0;i<nodes_num,i<256;i++)  
	 {  
		  ptr_node_list->get_item(i,&ptr_one_node);//���ĳ���ӽڵ�;  
		  ptr_one_node->get_attributes(&ptr_xml_atts);//���ĳ���ڵ����������;  
		  ptr_xml_atts->get_length(&atts_num);//����������Եĸ���;  
		  for(int j=0;j<atts_num;j++)  
		  {  
		   ptr_xml_atts->get_item(j,&ptr_one_node);//���ĳ������;  
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
	//CAN�˿�DID�����ظ�
	for(int i=0;i<3;i++)
	{
		BYTE a = m_nCAN_Send_DID[i];
		for(int j=i+1;j<4;j++)
		{
			BYTE b = m_nCAN_Send_DID[j];
			if( b==a )
			{
				CString err;
				err.Format(_T("��%d���%d��CAN��DID�ظ�!"),j+1,i+1);
				MessageBox(err,_T("CAN�˿�DID"),MB_OK|MB_ICONWARNING);
				return FALSE;
			}
		}
	}
	return TRUE;
}
BOOL CFC_ConfigerDlg::VerifyDID_CAN_IP_FC(void)
{
	//CAN�˿�DID��IP
	CString err;
	for(int i=0;i<4;i++)
	{
		int a = (int)m_nCAN_Send_DID[i];
		for(int j=0;j<FC_IP_DID_Map_UniCast.GetCount();j++)
		{
			int b = FC_IP_DID_Map_UniCast.GetAt(j).y;
			if( b==a )
			{
				err.Format(_T("��%d��CAN�˿ڵ�DID�뵥����̫��ת����ĵ�%d��DID�ظ�!"),i+1,j+1);
				MessageBox(err,_T("CAN�˿ڵ�DID�뵥����̫��ת����"),MB_OK|MB_ICONWARNING);
				return FALSE;
			}
		}
		for(int j=0;j<FC_IP_DID_Map_BroadCast.GetCount();j++)
		{
			int b = FC_IP_DID_Map_BroadCast.GetAt(j).y;
			if( b==a )
			{
				err.Format(_T("��%dCAN�˿ڵ�DID��㲥��̫��ת����ĵ�%d��DID�ظ�!"),i+1,j+1);
				MessageBox(err,_T("CAN�˿ڵ�DID��㲥��̫��ת����"),MB_OK|MB_ICONWARNING);
				return FALSE;
			}
		}
		for(int j=0;j<FC_DID_ETH_Map.GetCount();j++)
		{
			int b = FC_DID_ETH_Map.GetAt(j).x;
			if( b==a )
			{
				err.Format(_T("��%dCAN�˿ڵ�DID��FCת����ĵ�%d��DID�ظ�!"),i+1,j+1);
				MessageBox(err,_T("CAN�˿ڵ�DID��FCת����"),MB_OK|MB_ICONWARNING);
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
				err.Format(_T("������̫��ת�����%d����FCת����ĵ�%d��DID�ظ�!"),i+1,j+1);
				MessageBox(err,_T("������̫����FCת����"),MB_OK|MB_ICONWARNING);
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
				err.Format(_T("�㲥��̫��ת�����%d����FCת����ĵ�%d��DID�ظ�!"),i+1,j+1);
				MessageBox(err,_T("�㲥��̫����FCת����"),MB_OK|MB_ICONWARNING);
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
				err.Format(_T("�㲥��̫��ת�����%d���뵥����%d��DID�ظ�!"),i+1,j+1);
				MessageBox(err,_T("�㲥�뵥����̫��ת����"),MB_OK|MB_ICONWARNING);
				return FALSE;
			}
		}
	}
	return TRUE;
}