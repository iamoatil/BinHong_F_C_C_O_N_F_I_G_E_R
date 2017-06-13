#pragma once

#include "Serialport.h"
#include "afxwin.h"
#include "EnumSerial.h"
// CComportPara �Ի���

class CComportPara : public CDialogEx
{
	DECLARE_DYNAMIC(CComportPara)

public:
	CComportPara(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CComportPara();

// �Ի�������
	enum { IDD = IDD_DIALOG_COMSETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	HICON m_hIcon;
	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_ctl_com;
	UINT  m_comport_portnr;		// portnumber (1..MaxSerialPortNum)
	char  m_comport_parity;		// parity 
	UINT  m_comport_databits;		// databits 
	UINT  m_comport_stopbits;		// stopbits 
	int m_baudrate;
	CComboBox m_ctrl_baudrate;
	afx_msg void OnBnClickedOk();
	CUIntArray ports;
};
