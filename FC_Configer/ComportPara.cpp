// ComportPara.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FC_Configer.h"
#include "ComportPara.h"
#include "afxdialogex.h"


// CComportPara �Ի���

IMPLEMENT_DYNAMIC(CComportPara, CDialogEx)

CComportPara::CComportPara(CWnd* pParent /*=NULL*/)
	: CDialogEx(CComportPara::IDD, pParent)
	, m_baudrate(0)
{

}

CComportPara::~CComportPara()
{
}

void CComportPara::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_ctl_com);
	DDX_Control(pDX, IDC_COMBO2, m_ctrl_baudrate);
}



BOOL CComportPara::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON2);//IDR_ICONΪͼ����Դ��
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	CString strTempSerialName;
	m_ctl_com.ResetContent();
	if (UsingQueryDosDevice(ports))
	{
		for (int i=0; i<ports.GetSize(); i++)
		{
			strTempSerialName.Format(_T("COM%u"),ports[i]);
			m_ctl_com.AddString(strTempSerialName);
		}
	}
	strTempSerialName.Format(_T("COM%u"),m_comport_portnr);
	m_ctl_com.SetCurSel(m_ctl_com.FindStringExact(-1, strTempSerialName));
	CString strbps;
	strbps.Format(_T("%d"),m_baudrate);
	m_ctrl_baudrate.SetCurSel(m_ctrl_baudrate.FindStringExact(-1, strbps));
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}
BEGIN_MESSAGE_MAP(CComportPara, CDialogEx)
	ON_BN_CLICKED(IDOK, &CComportPara::OnBnClickedOk)
END_MESSAGE_MAP()


// CComportPara ��Ϣ�������


void CComportPara::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnOK();
	int index = m_ctrl_baudrate.GetCurSel();
	int combps[] = {9600,19200,57600,115200};
	m_baudrate = combps[index];
	index = m_ctl_com.GetCurSel();
	if(index>=0)
		m_comport_portnr = ports[index];
	else
		m_comport_portnr = 0;
	UpdateData(TRUE);
}
