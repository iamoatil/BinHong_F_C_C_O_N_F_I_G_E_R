
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
#include <iostream>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_WM_TIMER()
	ON_MESSAGE(WM_USER+0x200, &CFC_ConfigerDlg::OnMessageDeal)
END_MESSAGE_MAP()


// CFC_ConfigerDlg 对话框

CFC_ConfigerDlg::~CFC_ConfigerDlg(void)
{
	mFile.Close();
}


CFC_ConfigerDlg::CFC_ConfigerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFC_ConfigerDlg::IDD, pParent)
	, m_nIP_map(0)
	, m_nDID_map(0)
	, m_nDID_ETH(0)
	, m_b_IP_DID_IP_AutoInc(FALSE)
	, m_nSwitchID(0)
	, m_nEthCan(0)
	, m_nTestFlashAdd(0)
	, m_nFlashReadValue(0)
	, m_nFlashWriteValue(0)
	, m_bUniBroad(FALSE)
	, m_nIP_Input(0)
	, m_BroadIP(0)
	, m_SourceID(1)
	, m_SourceIDStr("0x1")
{	
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_RcvIndex = 0;
	for(int i=0;i<4;i++)
	{
		m_bEthPort[i] = (FALSE);
		m_nPriorityEth[i] = i+1;
		m_CAN_SPD[i] = 0;
		m_nCAN_Send_DID[i] =i+1;
		m_nCAN_Port_ID[i] =i+1;
	}
	for(int i=0;i<32;i++)
		StatisData[i] = 0;
	nPollCounter = 0;
	m_bAutoReadStat = FALSE;
	m_nWaitCycles = 50;
	m_nTimeGap = 10;
	bStopTransfer = TRUE;
	m_strTestFlashAdd = _T("0x200");
	m_strTestFlashRead = _T("");
	m_strTestFlashWrite = _T("0x20");
	m_bUniBroad = FALSE;
	m_nIP_Input = ((192L*256L+168L)*256L+1)*256L+1;
	m_BroadIP = ((192L * 256L + 168L) * 256L + 1) * 256L + 1;
}

void CFC_ConfigerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SourceID, m_SourceIDStr);

	DDX_Control(pDX, IDC_STATIC_LOGO, m_Ctl_Logo);
	DDX_Control(pDX, IDC_RICHEDIT21, m_MessageShow);
	DDX_CBIndex(pDX, IDC_COMBO1, m_CAN_SPD[0]);
	DDX_CBIndex(pDX, IDC_COMBO2, m_CAN_SPD[1]);
	DDX_CBIndex(pDX, IDC_COMBO3, m_CAN_SPD[2]);
	DDX_CBIndex(pDX, IDC_COMBO4, m_CAN_SPD[3]);
	DDX_Control(pDX, IDC_LIST_IPMAP, m_UGGridCtrl);
	DDX_Text(pDX, IDC_EDIT5, m_nDID_map);
	DDX_Control(pDX, IDC_SPIN1, m_strlSpinIPMAP);
	DDX_Control(pDX, IDC_SPIN2, m_strlSpinDIDMAP);
	DDX_Control(pDX, IDC_SPIN_BroadIPInc, m_Spin_BroadIPPort);
	
	DDV_MinMaxByte(pDX, m_nDID_map, 0, 255);
	DDX_Text(pDX, IDC_EDIT7, m_nPriorityEth[0]);
	DDV_MinMaxByte(pDX, m_nPriorityEth[0], 0, 255);
	DDX_Text(pDX, IDC_EDIT8, m_nPriorityEth[1]);
	DDV_MinMaxByte(pDX, m_nPriorityEth[1], 0, 255);
	DDX_Text(pDX, IDC_EDIT9, m_nPriorityEth[2]);
	DDV_MinMaxByte(pDX, m_nPriorityEth[2], 0, 255);
	DDX_Text(pDX, IDC_EDIT10, m_nPriorityEth[3]);
	DDV_MinMaxByte(pDX, m_nPriorityEth[3], 0, 255);
	DDX_Control(pDX, IDC_SPIN3, m_SpinEth[0]);
	DDX_Control(pDX, IDC_SPIN4, m_SpinEth[1]);
	DDX_Control(pDX, IDC_SPIN5, m_SpinEth[2]);
	DDX_Control(pDX, IDC_SPIN6, m_SpinEth[3]);
	DDX_Check(pDX, IDC_CHECK1, m_bEthPort[0]);
	DDX_Text(pDX, IDC_EDIT11, m_nDID_ETH);
	DDV_MinMaxByte(pDX, m_nDID_ETH, 0, 255);
	DDX_Check(pDX, IDC_CHECK2, m_bEthPort[1]);
	DDX_Check(pDX, IDC_CHECK3, m_bEthPort[2]);
	DDX_Check(pDX, IDC_CHECK4, m_bEthPort[3]);
	DDX_Control(pDX, IDC_LIST_IPMAP2, m_UGGridDID_Eth);
	DDX_Control(pDX, IDC_LIST_IPMAP3, m_GridEthState);
	DDX_Control(pDX, IDC_LIST_IPMAP4, m_GridCanState);
	DDX_Control(pDX, IDC_LIST_IPMAP5, m_GridFCState);
	DDX_Control(pDX, IDC_SPIN7, m_SpinDIDETH);
	DDX_Check(pDX, IDC_CHECK6, m_b_IP_DID_IP_AutoInc);
	DDX_Text(pDX, IDC_EDIT6, m_nSwitchID);
	DDV_MinMaxByte(pDX, m_nSwitchID, 0, 255);
	DDX_Control(pDX, IDC_PROGRESS1, m_Ctl_Prgress);
	DDX_Control(pDX, IDC_STATIC_LK0, m_csLinkStatus[0]);
	DDX_Control(pDX, IDC_STATIC_LK1, m_csLinkStatus[1]);
	DDX_Control(pDX, IDC_STATIC_LK2, m_csLinkStatus[2]);
	DDX_Control(pDX, IDC_STATIC_LK3, m_csLinkStatus[3]);
	DDX_Control(pDX, IDC_STATIC_LK4, m_csLinkStatus[4]);
	DDX_Control(pDX, IDC_STATIC_LK5, m_csLinkStatus[5]);
	DDX_Radio(pDX, IDC_RADIO1, m_nEthCan);
	DDV_MinMaxInt(pDX, m_nEthCan, 0, 2);
	DDX_Text(pDX, IDC_EDIT_CAN_SD_DID, m_nCAN_Send_DID[0]);
	DDV_MinMaxByte(pDX, m_nCAN_Send_DID[0], 0, 255);
	DDX_Text(pDX, IDC_EDIT_CAN_SD_DID2, m_nCAN_Send_DID[1]);
	DDV_MinMaxByte(pDX, m_nCAN_Send_DID[1], 0, 255);
	DDX_Text(pDX, IDC_EDIT_CAN_SD_DID3, m_nCAN_Send_DID[2]);
	DDV_MinMaxByte(pDX, m_nCAN_Send_DID[2], 0, 255);
	DDX_Text(pDX, IDC_EDIT_CAN_SD_DID4, m_nCAN_Send_DID[3]);
	DDV_MinMaxByte(pDX, m_nCAN_Send_DID[3], 0, 255);

	DDX_Text(pDX, IDC_EDIT_CAN_PORT_ID, m_nCAN_Port_ID[0]);
	DDX_Text(pDX, IDC_EDIT_CAN_PORT_ID2, m_nCAN_Port_ID[1]);
	DDX_Text(pDX, IDC_EDIT_CAN_PORT_ID3, m_nCAN_Port_ID[2]);
	DDX_Text(pDX, IDC_EDIT_CAN_PORT_ID4, m_nCAN_Port_ID[3]);
	DDX_Text(pDX, IDC_EDIT_TST_ADD, m_strTestFlashAdd);

	DDX_Text(pDX, IDC_EDIT_TST_ADD2, m_strTestFlashRead);

	DDX_Text(pDX, IDC_EDIT_TST_ADD3, m_strTestFlashWrite);

	DDX_Control(pDX, IDC_LIST_IPMAP6, m_UGGridCtrl_BroadCast);
	DDX_Control(pDX, IDC_LIST_IPMAP7, m_Grid_BroadIPPort);
	DDX_Radio(pDX, IDC_RADIO_Uni, m_bUniBroad);
	DDX_IPAddress(pDX, IDC_IPADDRESS1, m_nIP_Input);
	DDX_IPAddress(pDX, IDC_IPADDRESS_Broad, m_BroadIP);
}

BEGIN_MESSAGE_MAP(CFC_ConfigerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_USER+0x200, &CFC_ConfigerDlg::OnMessageDeal)
	ON_COMMAND(ID_MENU_ABOUT, &CFC_ConfigerDlg::ShowAboutDlg)
	ON_COMMAND(ID_MENU_PORT_CFG, &CFC_ConfigerDlg::OnComPara)
	ON_BN_CLICKED(IDC_BTN_DEVICE_INFO, &CFC_ConfigerDlg::OnBnClickedGetDeviceInfo)
	ON_BN_CLICKED(IDC_BTN_RESET, &CFC_ConfigerDlg::OnBnClickedResetDevice)
	ON_COMMAND(ID_MENU_RESET, &CFC_ConfigerDlg::OnBnClickedResetDevice)

	ON_COMMAND(ID_MENU_SAVE_FILE, &CFC_ConfigerDlg::SaveFile)
	ON_COMMAND(ID_MENU_OPEN_FILE, &CFC_ConfigerDlg::OPenFile)	
	ON_BN_CLICKED(IDC_BUTTON_INSERT_DIDETH, &CFC_ConfigerDlg::OnBnClickedButtonInsertDideth)

	ON_MESSAGE(DM_GETDEFID, OnGetDefID)
	ON_BN_CLICKED(IDC_BUTTON_ALL_ETH, &CFC_ConfigerDlg::OnBnClickedAllEth)
	ON_BN_CLICKED(IDC_BUTTON_WRT_PRIORITY, &CFC_ConfigerDlg::OnWriteEthPriority)
	ON_BN_CLICKED(IDC_BTN_WR, &CFC_ConfigerDlg::OnBnClickedWriteCfg)
	ON_BN_CLICKED(IDC_BTN_LOAD_CFG, &CFC_ConfigerDlg::OnBnClickedBtnLoadCfg)
	ON_BN_CLICKED(IDC_BTN_CAN_RD, &CFC_ConfigerDlg::OnBnClickedBtnCanRd)
	ON_BN_CLICKED(IDC_BTN_CAN_WR, &CFC_ConfigerDlg::OnBnClickedBtnCanWr)
	ON_BN_CLICKED(IDC_BTN_WR_SWITCHID, &CFC_ConfigerDlg::OnBnClickedBtnWrSwitchid)
	ON_BN_CLICKED(IDC_BTN_RD_SWITCHID, &CFC_ConfigerDlg::OnBnClickedBtnRdSwitchid)
	ON_BN_CLICKED(IDC_BTN_WR_SourceID, &CFC_ConfigerDlg::OnBnClickedBtnWrSourceID)
	ON_BN_CLICKED(IDC_BTN_RD_SourceID, &CFC_ConfigerDlg::OnBnClickedBtnRdSourceID)
	ON_BN_CLICKED(IDC_BTN_RD_PRIORITY, &CFC_ConfigerDlg::OnBnClickedBtnRdPriority)
	ON_BN_CLICKED(IDC_BTN_RD, &CFC_ConfigerDlg::OnBnClickedBtnRd)
	ON_BN_CLICKED(IDC_BTN_CLR_STAT, &CFC_ConfigerDlg::OnBnClickedBtnClrStat)
	ON_BN_CLICKED(IDC_BTN_RFESH_STAT, &CFC_ConfigerDlg::OnBnClickedBtnRfeshStat)
	ON_BN_CLICKED(IDC_BTN_INSERT_IPDID, &CFC_ConfigerDlg::OnBnClickedBtnInsertIpdid)
	ON_BN_CLICKED(IDC_BTN_ONEKEY_WR, &CFC_ConfigerDlg::OnBnClickedOneKeyWrite)
	ON_BN_CLICKED(IDC_BTN_ONEKEY_Rd, &CFC_ConfigerDlg::OnBnClickedBtnOnekeyRd)
	ON_BN_CLICKED(IDC_CHECK_AUTO_READ_STA, &CFC_ConfigerDlg::OnBnClickedCheckAutoReadSta)
	ON_BN_CLICKED(IDC_BUTTON_TREAD, &CFC_ConfigerDlg::OnBnClickedButtonTread)
	ON_BN_CLICKED(IDC_BUTTON_TWRITE, &CFC_ConfigerDlg::OnBnClickedButtonTwrite)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, &CFC_ConfigerDlg::OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BroadIPInc, &CFC_ConfigerDlg::OnBroadIPChanged)
END_MESSAGE_MAP()


// CFC_ConfigerDlg 消息处理程序

BOOL CFC_ConfigerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
//	CMenu* menu =new CMenu;
//	menu->LoadMenu(MAKEINTRESOURCE(IDR_MENU1));      //红色部分为对应自己添加的菜单资源的名称//
//	this->SetMenu(menu);
	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);



	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

   LOGFONT lf;                        // Used to create the CFont.


   memset(&lf, 0, sizeof(LOGFONT));   // Clear out structure.
   lf.lfHeight = 20;                  // Request a 20-pixel-high font
   strcpy_s(lf.lfFaceName, "黑体");    //    with face name "Arial".
   m_font.CreateFontIndirect(&lf);    // Create the font.

   // Use the font to paint a control. This code assumes
   // a control named IDC_TEXT1 in the dialog box.
   //GetDlgItem(IDC_EDIT6)->SetFont(&m_font);

	//显示logo
	h_logo=::LoadBitmap(AfxGetResourceHandle(),MAKEINTRESOURCE(IDB_BITMAP1));
	m_Ctl_Logo.ModifyStyle(0x0F,SS_BITMAP|SS_CENTERIMAGE);
	m_Ctl_Logo.SetBitmap(h_logo);
	//ini 文件读写
	CString strCOM = _T(""); 
	CFileFind finder;   //查找是否存在ini文件，若不存在，则生成一个新的默认设置的ini文件，这样就保证了我们更改后的设置每次都可用  
	BOOL ifFind = finder.FindFile(_T(".\\FC_AE_Config.ini"));  
	if( !ifFind )  
	{  		
		::WritePrivateProfileString(_T("FC_AE"),_T("COMPORT"),_T("COM0"),_T(".\\FC_AE_Config.ini"));  
		OnComPara();
	}  
	else
	{
		::GetPrivateProfileString("FC_AE","COMPORT","", strCOM.GetBuffer(MAX_PATH), MAX_PATH,".\\FC_AE_Config.ini"); 
		strCOM.ReleaseBuffer();
		strCOM.Delete(0,3);
		m_comport_portnr=atoi(strCOM);
		::GetPrivateProfileString("RETRY","RD_CYCLES","", strCOM.GetBuffer(MAX_PATH), MAX_PATH,".\\FC_AE_Config.ini"); 
		m_nWaitCycles=atoi(strCOM);
		if ((m_nWaitCycles < 1) || (m_nWaitCycles>10000))
		{
			m_nWaitCycles = 50;
			MessageBox(_T("超出范围[1,10000]"), _T("RD_CYCLES值超出范围"), MB_ICONSTOP);
		}
			
		::GetPrivateProfileString("TIME","GAP","", strCOM.GetBuffer(MAX_PATH), MAX_PATH,".\\FC_AE_Config.ini"); 
		m_nTimeGap=atoi(strCOM);
		if (m_nTimeGap < 1 || m_nTimeGap > 1000)
		{
			m_nTimeGap = 10;
			MessageBox(_T("超出范围[1,1000]"), _T("GAP值超出范围"), MB_ICONSTOP);
		}			

		strCOM.ReleaseBuffer();
	}

	m_comport_baud = 115200;			// baudrate

	BOOL bGridLines;
	COLORREF lineColor;
	m_UGGridCtrl.GetGridLines(bGridLines, lineColor);
	m_UGGridCtrl.SetGridLines(TRUE, lineColor);

	m_UGGridCtrl.InsertColumn(0, _T("序号"));
	m_UGGridCtrl.InsertColumn(1, _T("     IP地址    "));
	m_UGGridCtrl.InsertColumn(2, _T("DID"));

	m_UGGridCtrl.SetColumnWidth(0,40);
	CRect myrect;
	m_UGGridCtrl.GetWindowRect(&myrect);
	m_UGGridCtrl.SetColumnWidth(1,140);
	m_UGGridCtrl.SetColumnWidth(2,(myrect.Width()-180));
	//m_UGGridCtrl.SetAutoEdit(TRUE);
	m_UGGridCtrl.SetEditable(TRUE,1);
	m_UGGridCtrl.SetEditable(TRUE,2);
	m_UGGridCtrl.SetEditable(FALSE,0);
	m_UGGridCtrl.ShowWindow(SW_SHOW);
	m_UGGridCtrl.SetTextBkColor(RGB(100,160,100));
	m_UGGridCtrl.SetBkColor(RGB(100,160,100));
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT;
	lvc.fmt = LVCFMT_CENTER;
	for(int i=0;i<3;i++)
		m_UGGridCtrl.SetColumn(i, &lvc);

//增加广播
	//LVS_NOCOLUMNHEADER
	//m_UGGridCtrl_BroadCast.ModifyStyle(0,LVS_NOCOLUMNHEADER,0); 
	m_UGGridCtrl_BroadCast.GetGridLines(bGridLines, lineColor);
	m_UGGridCtrl_BroadCast.SetGridLines(TRUE, lineColor);

	m_UGGridCtrl_BroadCast.InsertColumn(0, _T("序号"));
	m_UGGridCtrl_BroadCast.InsertColumn(1, _T("     IP地址    "));
	m_UGGridCtrl_BroadCast.InsertColumn(2, _T("DID"));

	m_UGGridCtrl_BroadCast.SetColumnWidth(0,40);
	m_UGGridCtrl_BroadCast.GetWindowRect(&myrect);
	m_UGGridCtrl_BroadCast.SetColumnWidth(1,140);
	m_UGGridCtrl_BroadCast.SetColumnWidth(2,(myrect.Width()-180));
	m_UGGridCtrl_BroadCast.SetEditable(TRUE,1);
	m_UGGridCtrl_BroadCast.SetEditable(TRUE,2);
	m_UGGridCtrl_BroadCast.SetEditable(FALSE,0);
	m_UGGridCtrl_BroadCast.ShowWindow(SW_SHOW);
	m_UGGridCtrl_BroadCast.SetTextBkColor(RGB(10,154,220));
	m_UGGridCtrl_BroadCast.SetBkColor(RGB(10,154,220));
	for(int i=0;i<3;i++)
		m_UGGridCtrl_BroadCast.SetColumn(i, &lvc);

	//增加组播IP与网络端口的映射
	//LVS_NOCOLUMNHEADER
	m_Grid_BroadIPPort.GetGridLines(bGridLines, lineColor);
	m_Grid_BroadIPPort.SetGridLines(TRUE, lineColor);
	m_Grid_BroadIPPort.InsertColumn(0, _T("序号"));
	m_Grid_BroadIPPort.InsertColumn(1, _T(" 组播IP "));
	m_Grid_BroadIPPort.InsertColumn(2, _T("网络端口"));	

	m_Grid_BroadIPPort.SetColumnWidth(0, 40);	
	m_Grid_BroadIPPort.GetWindowRect(&myrect);
	m_Grid_BroadIPPort.SetColumnWidth(1, (myrect.Width() - 120));
	m_Grid_BroadIPPort.SetColumnWidth(2, 70);	
	m_Grid_BroadIPPort.SetEditable(TRUE, 1);
	m_Grid_BroadIPPort.SetEditable(TRUE, 2);
	m_Grid_BroadIPPort.SetEditable(FALSE, 0);
	m_Grid_BroadIPPort.ShowWindow(SW_SHOW);
	m_Grid_BroadIPPort.SetTextBkColor(RGB(10, 154, 220));
	m_Grid_BroadIPPort.SetBkColor(RGB(10, 154, 220));
	for (int i = 0; i<3; i++)
		m_Grid_BroadIPPort.SetColumn(i, &lvc);
//end 新增广播
	m_UGGridDID_Eth.GetGridLines(bGridLines, lineColor);
	m_UGGridDID_Eth.SetGridLines(TRUE, lineColor);
	m_UGGridDID_Eth.InsertColumn(0, _T("序号"));
	m_UGGridDID_Eth.InsertColumn(1, _T("单播IP/CAN ID"));
	m_UGGridDID_Eth.InsertColumn(2, _T("网络端口"));
	m_UGGridDID_Eth.InsertColumn(3, _T("CAN端口"));
	m_UGGridDID_Eth.SetColumnWidth(0,40);	
	m_UGGridDID_Eth.SetColumnWidth(2,60);
	m_UGGridDID_Eth.SetColumnWidth(3,70);
	m_UGGridDID_Eth.GetWindowRect(&myrect);
	m_UGGridDID_Eth.SetColumnWidth(1, (myrect.Width() - 170));
	//m_UGGridCtrl.SetAutoEdit(TRUE);
	m_UGGridDID_Eth.SetEditable(TRUE,1);
	m_UGGridDID_Eth.SetEditable(TRUE,2);
	m_UGGridDID_Eth.SetEditable(TRUE,3);
	m_UGGridDID_Eth.SetEditable(FALSE,0);
	for(int i=0;i<4;i++)
		m_UGGridDID_Eth.SetColumn(i, &lvc);
	m_UGGridDID_Eth.ShowWindow(SW_SHOW);
	m_UGGridDID_Eth.SetGridLines(TRUE, lineColor);

	m_UGGridDID_Eth.SetTextBkColor( ::GetSysColor(COLOR_INFOBK));
	m_UGGridDID_Eth.SetBkColor(::GetSysColor(COLOR_INFOBK));
	CString FCLable[6] = {"光纤端口1","光纤端口2","CRC错误帧","发送帧","总字节","CRC错误帧"};
	for(int i=0;i<6;i++)
	{
		CString stitle;
		if(i<4)
		{
			stitle.Format(_T("网口%d"),i+1);
			m_GridEthState.InsertColumn(i,stitle);
			m_GridEthState.SetColumnWidth(i,80);

			stitle.Format(_T("CAN%d"),i+1);
			m_GridCanState.InsertColumn(i,stitle);
			m_GridCanState.SetColumnWidth(i,80);
		}
		if(i<2)
		{
			m_GridFCState.InsertColumn(i,FCLable[i]);
			m_GridFCState.SetColumnWidth(i,100);
		}
	}
	m_GridEthState.SetGridLines(TRUE, lineColor);
	m_GridCanState.SetGridLines(TRUE, lineColor);
	m_GridFCState.SetGridLines(TRUE, lineColor);
	m_GridFCState.SetTextColor(RGB(0,0xf,0xff));
	m_GridEthState.ShowWindow(SW_SHOW);
	m_GridCanState.ShowWindow(SW_SHOW);
	InitStatisticsShow();
	UpdateStatistics();
	m_strlSpinIPMAP.SetRange(0,255);
	m_strlSpinDIDMAP.SetRange(0,255);
	m_Spin_BroadIPPort.SetRange(0, 255);

	for(int i=0;i<4;i++)
		m_SpinEth[i].SetRange(0,255);
	m_SpinDIDETH.SetRange(0,255);
	m_strIP_HighPart_Uni = _T("192.168.1");
	m_strIP_HighPart_Broad = _T("240.1.1");
	//系统串行端口获取
	CUIntArray ports;
	CString strTempSerialName;
	if (UsingQueryDosDevice(ports))
	{
		int num = ports.GetSize(),i;
		if(num>0)
		{
			for (i=0; i<num; i++)
			{
				if( m_comport_portnr == ports[i])
				{
					OpenSerialPort();
					break;
				}
			}
			if(i>=num)
				OnComPara();
		}
		else
			MessageBox( _T("未发现串行通信端口"), _T("系统无可用串口"), MB_ICONINFORMATION);;
	}
	//禁止按键操作
	if(m_ComPort.IsOpen() == FALSE)
		EnableButtons(FALSE);
	COLORREF bkgrd = GetSysColor(COLOR_3DFACE);
	for(int i=0;i<6;i++)
	{
		m_csLinkStatus[i].m_bOn = true;
		m_csLinkStatus[i].SetBKColor(bkgrd);
		m_csLinkStatus[i].SetOnFgColor(RGB(0,255,0));
		m_csLinkStatus[i].SetOffFgColor(RGB(255,0,0));
	}

	COleDateTime FileNamePre;
	FileNamePre = COleDateTime::GetCurrentTime();
	CString Fname = FileNamePre.Format(_T("串口收发数据%y年%m月%d日%H点%M分.txt"));

	if ( mFile.Open(Fname, CFile::modeReadWrite|CFile::modeNoTruncate|CFile::modeCreate))
	{
		mFile.SeekToEnd();
	}
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CFC_ConfigerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}


// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFC_ConfigerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);

	}
	else
	{
		m_UGGridCtrl.ShowScrollBar(SB_HORZ, FALSE);	
		//广播
		m_UGGridCtrl_BroadCast.ShowScrollBar(SB_HORZ, FALSE);	
		//
		m_UGGridDID_Eth.ShowScrollBar(SB_HORZ, FALSE);	
		m_Grid_BroadIPPort.ShowScrollBar(SB_HORZ, FALSE);
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CFC_ConfigerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CFC_ConfigerDlg::ShowAboutDlg()
{
	CAboutDlg dlg;
	dlg.DoModal();
}


void CFC_ConfigerDlg::OnComPara()
{	
	CComportPara dlg;
	dlg.m_comport_portnr = m_comport_portnr;
	dlg.m_baudrate = m_comport_baud;
	if(TRUE == dlg.DoModal())
	{
		m_comport_portnr = dlg.m_comport_portnr;
		m_comport_baud = dlg.m_baudrate;
		if(m_comport_portnr>0)
		{
			CString strCOM;
			strCOM.Format(_T("COM%d"),m_comport_portnr);
			::WritePrivateProfileString(_T("FC_AE"),_T("COMPORT"),strCOM,_T(".\\FC_AE_Config.ini"));  
			OpenSerialPort();
			if(m_ComPort.IsOpen() == TRUE)
				EnableButtons(TRUE);
			else
				EnableButtons(FALSE);
		}
		else
			MessageBox(_T("系统无串口或未能选择正确！"));
	}
}


BOOL CFC_ConfigerDlg::SendDataFrame(void)
{
	//清除接收
	char rcvStr[10];
	m_RcvIndex = 0;
	memset(RcvBuff,32,0);
	COleDateTime dtime;
	CString Fname;
	if(m_ComPort.IsOpen())
	{
		m_ComPort.WriteToPort(SendBuff,m_SendLength);
		//
		mFile.SeekToEnd();
		dtime = COleDateTime::GetCurrentTime();
		Fname = dtime.Format(_T("%H:%M:%S "));
		for(int i=0;i<9;i++)
			rcvStr[i] = Fname.GetAt(i);
		mFile.Write(rcvStr, 9);
		sprintf_s(rcvStr,_T("SEND: "));
		mFile.Write(rcvStr, 6);
		for(int i=0;i<m_SendLength;i++)
		{
			sprintf_s(rcvStr,_T("%02X "),SendBuff[i]);
			mFile.Write(rcvStr, 3);
		}
		sprintf_s(rcvStr,_T("\r\n"));
		mFile.Write(rcvStr, 2);
		//
	}
	if(m_SendTimesLeft>0)
		m_SendTimesLeft--;
	//设置超时定时器
	//if(bResponseReq)
		SetTimer(1,2,NULL);
	//else
	//	bStopTransfer = TRUE;
	m_nRcvTimer = 0;
	return TRUE;
}

void CFC_ConfigerDlg::OnTimer(UINT nTimerID)
{	
	char rcvStr[10];
	COleDateTime dtime;
	CString Fname;
	int recvCount = 0;
	int writeLen = 0;
	switch(nTimerID)
	{
	case 1:
		/*
		if(( 0<m_SendTimesLeft)&&(bStopTransfer == FALSE))
			SendDataFrame();
		else
		{
			KillTimer(nTimerID);
			ShowMessgeInfor(FALSE,_T("-->应答超时！"));
			TransferComplete();
		}*/

		m_ComPort.ReceiveData(RcvBuff + m_RcvIndex,(DWORD*)&m_RcvIndex);
		
		writeLen = m_ComPort.GetWriteSize();
		if (writeLen == 11)
		{
			if (m_RcvIndex < 3)
			{
				recvCount = m_RcvIndex;
				m_RcvIndex = 0;
				Sleep(1);
				m_ComPort.ReceiveData(RcvBuff + recvCount, (DWORD*)&m_RcvIndex);
				m_RcvIndex += recvCount;
			}
		}
		else if (writeLen == 7)
		{
			if (m_RcvIndex < 7)
			{
				recvCount = m_RcvIndex;
				m_RcvIndex = 0;
				Sleep(1);
				m_ComPort.ReceiveData(RcvBuff + recvCount, (DWORD*)&m_RcvIndex);
				m_RcvIndex += recvCount;
			}
		}		
		
		//数据记录
		if( m_RcvIndex>0)
		{
			dtime = COleDateTime::GetCurrentTime();
			Fname = dtime.Format(_T("%H:%M:%S "));
			for(int i=0;i<9;i++)
				rcvStr[i] = Fname.GetAt(i);
			mFile.Write(rcvStr, 9);
			sprintf_s(rcvStr,_T("RCV: "));
			mFile.Write(rcvStr, 5);
			for(int i=0;i<m_RcvIndex;i++)
			{
				sprintf_s(rcvStr,_T("%02X "),RcvBuff[i]);
				mFile.Write(rcvStr, 3);
			}
			sprintf_s(rcvStr,_T("\r\n"));
			mFile.Write(rcvStr, 2);
		}
		//end
		ReceiveDataAnalyze();
		m_nRcvTimer++;
		if(m_nRcvTimer>20)
		{
			m_nRcvTimer = 0;
			KillTimer(nTimerID);
			ShowMessgeInfor(FALSE,_T("-->应答超时！"));
			TransferComplete();
		}
		break;
	case 2:
		OnBnClickedBtnRfeshStat();
		break;
	}
	//m_Status = OFF_LINE;
	//::SendMessage(this->GetOwner()->m_hWnd, WM_GPSBDDEV_TIMEOUT,0, 0);
}


void CFC_ConfigerDlg::ShowMessgeInfor(BOOL bNewMsg,CString strText)
{
	if(bNewMsg)
		str_msg_text = strText;
	else
		str_msg_text += strText;
	CFont *p_Font = new CFont;
	p_Font->CreateFont(
		18,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		DEFAULT_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("黑体"));                 // lpszFacename
	SetFont( p_Font );
	m_MessageShow.SetFont(p_Font,1);
	p_Font->DeleteObject();
	//m_MessageShow.SetBackgroundColor(false,RGB(155,100,120));
	m_MessageShow.SetSel(0,-1);


	CHARFORMAT cFmt;
	cFmt.cbSize = sizeof(CHARFORMAT);
	cFmt.crTextColor = RGB(255,0,0);
	cFmt.dwEffects = 0;
	cFmt.dwMask = CFM_COLOR;
	m_MessageShow.SetSelectionCharFormat(cFmt);
	m_MessageShow.ReplaceSel(str_msg_text);
	delete p_Font;
}
/*设备程序版本号寄存器28-31：1代表以太网卡，2代表CAN卡；
24-27：硬件版本号，依次为A，B，C等
0-23：每一个字节代表一个版本号，例如0x020104，版本为，2.1.4*/
void CFC_ConfigerDlg::ShowVersion(UINT data)
{
	CString strinf = _T("卡类型:");
	UINT temp;
	temp = (data>>28)&0x0f;
	if(1==temp)
		strinf += _T("CAN");
	else
		strinf += _T("以太网");
	temp = (data>>24)&0x0f;
	CString hwver;
	hwver.Format(_T("  硬件版本：%c"),temp+'A');
	strinf += hwver;

	temp = (data>>16)&0xff;
	hwver.Format(_T("  固件版本：%d.%d.%d"),(data>>16)&0xff,(data>>8)&0xff,(data)&0xff);
	strinf += hwver;
	GetDlgItem(IDC_STATIC_VER)->SetWindowText(strinf);
}

//板卡信息
void CFC_ConfigerDlg::OnBnClickedGetDeviceInfo()
{
	int num = PackageCardInfor(0);
	StartupCmdlistSend(num);
}

//板卡复位
void CFC_ConfigerDlg::OnBnClickedResetDevice()
{
	cmd_item_list[0] = Get_CMD_Item(PC_WRITE_CMD,device_soft_reset);
	StartupCmdlistSend(1);
}

//返回值0--成功，数据正确，1-数据无效
int CFC_ConfigerDlg::ProcessRcv(UINT rcvdata)
{
	//当前命令信息
	PC_CMD_ITEM cmd = cmd_item_list[cmd_item_index],cmd_last; 
	UINT reg = cmd.nRegAddr,add,temp;
	UINT *ptr = cmd.pReadData;
	*ptr = rcvdata;
	switch(reg)
	{
	case device_version:
		ShowVersion(rcvdata);
		break;
	case Can_speed:
		UpDateCanSpd(rcvdata);
		break;
	case Flash_cfg_data_rd:

		if(cmd_item_index>0)
			cmd_last = cmd_item_list[cmd_item_index-1];
		add = cmd_last.WriteData;
		if((add>=0x201)&&(add<=0x204))
		{	
			if( 0x100==(rcvdata&0x100))
			{
				m_nPriorityEth[add-0x201] = rcvdata&0xff;
				UpdateData(false);
			}
		}
		if((add>=0x205)&&(add<=0x208))
		{	
			if( 0x100==(rcvdata&0x100))
			{
				m_nCAN_Send_DID[add-0x205] = rcvdata&0xff;
				UpdateData(false);
			}
		}
		else if(add == 0x200)
		{	
			if( 0x100==(rcvdata&0x100))
			{
				m_nSwitchID = rcvdata&0xff;
				UpdateData(false);
			}
		}
		//receive m_Map_BroadIPPort
		else if (add == 0x49f)
		{
			int address = 0x400;
			m_Map_BroadIPPort.RemoveAll();
			while (true)
			{
				DWORD ipValue = 0;
				for (size_t j = 0; j < 4; j++)
				{
					UINT partValue = Card_Flash_Table[address];					
					ipValue += ((partValue & 0xff) << (3 - j)*8);
					address++;
				}
				if (ipValue == 0xFFFFFFFF || address > 0x49f)
				{
					break;
				}
				int portValue = Card_Flash_Table[address];				
				address++;
				m_Map_BroadIPPort.Add(CPoint(ipValue, portValue & 0xff));
			}
			ShowBroadIP_Port_Map();
			UpdateData(false);
		}
		else if (add == 0x229)
		{
			//SourceID
			int address = 0x227;
			int sourceId = 0;
			for (int i = 0; i < 3; i++)
			{				
				sourceId += (Card_Flash_Table[address] & 0xff) << (2 - i)*8;
				address++;
			}
			m_SourceID = sourceId;
			m_SourceIDStr.Format(_T("0x%x"), m_SourceID);
			UpdateData(false);
		}
		else if(add == 0x3ff)
		{
			//配置表读取完成
			FC_IP_DID_Map_UniCast.RemoveAll();
			for(int i=0;i<256;i++)
			{
				if ((Card_Flash_Table[i] & 0xff) != 0xff)
					FC_IP_DID_Map_UniCast.Add(CPoint(i,Card_Flash_Table[i]&0xff));
			}
			//广播
			FC_IP_DID_Map_BroadCast.RemoveAll();
			for(int i=0;i<256;i++)
			{
				if ((Card_Flash_Table[i + 0x300] & 0xff) != 0xff)
					FC_IP_DID_Map_BroadCast.Add(CPoint(i,Card_Flash_Table[i+0x300]&0xff));
			}

			//光口到网口
			FC_DID_ETH_Map.RemoveAll();
			for (int i = 0; i < 256; i++)
			{
				if ((Card_Flash_Table[i + 0x100] & 0xff) != 0xff)
					FC_DID_ETH_Map.Add(CPoint(i, Card_Flash_Table[i + 0x100] & 0xff));
			}
			int count=FC_DID_ETH_Map.GetCount();
			CString baseIPStr = m_strIP_HighPart_Uni + ".0";
			int baseIP = htonl(inet_addr(baseIPStr));
			for (int i = 0; i < count; i++)
			{
				CPoint point=FC_DID_ETH_Map.GetAt(i);
				int highPort = point.y & 0xf0;
				if (highPort == 0)
				{
					point.x = baseIP + point.x;
					FC_DID_ETH_Map.RemoveAt(i);
					FC_DID_ETH_Map.InsertAt(i, point);
				}				
			}			
		}		
		else if(add==0x21f)
		{
			//CAN的端口ID和发送DID
			for(int i=0;i<4;i++)
			{
				temp = 0;
				for(int j=0;j<4;j++)
				{
					temp <<= 8;					
					temp += (m_nCAN_Port_ID_Part[i*4+j]&0xff);
				}
				m_nCAN_Port_ID[i] = temp;
			}
			UpdateData(false);
		}
		else if(add==0x225)
		{	
			for(int i=0;i<3;i++)
			{
				Uni_IP_H[i] = Card_Flash_Table[i+0x220]&0xff;
				Broad_IP_H[i] = Card_Flash_Table[i+0x223]&0xff;
			}

			m_strIP_HighPart_Uni.Format(_T("%d.%d.%d"),Uni_IP_H[0],Uni_IP_H[1],Uni_IP_H[2]);
			m_strIP_HighPart_Broad.Format(_T("%d.%d.%d"),Broad_IP_H[0],Broad_IP_H[1],Broad_IP_H[2]);
			ShowIP_DID_Map();
			ShowID_EthPort_Map();
			UpdateData(false);
		}
		else if (add == 0x226)
		{
			UpDateCanSpd(rcvdata);
		}

		if(bFlashTest)
		{
			bFlashTest = FALSE;
			m_strTestFlashRead.Format(_T("0X%X"),m_nFlashReadValue);
			UpdateData(false);
		}
		break;
	case FcB_send_byte_H:
		UpdateStatistics();
		break;
	case Sw_id:
		UpdateData(false);
	default:
		;
		break;
	}

	//接收到的数据是否满足继续条件
	UINT condition = cmd.nNextCmd;
	BOOL bRetry = false;
	switch(condition)
	{
	case 1:
		bRetry = (1!=(rcvdata&0x01));
		break;
	case 2:
		bRetry = (0x100!=(rcvdata&0x100));
		break;
	case -1:
		bRetry = false;
		break;
	default:
		;
	}

	//重新读取，
	if(bRetry)
	{
		nPollCounter ++ ;
		if(nPollCounter>m_nWaitCycles)
		{
			TransferComplete();
			MessageBox( _T("等待设备超时"), _T("等待设备超时"), MB_ICONSTOP);
			return 1;
		}
		else
		{
			Sleep(m_nTimeGap);		
			Cmd_Item_Send(cmd_queue[cmd_item_index],1);
		}
	}
	else
	{
		nPollCounter = 0;		
		if(cmd_item_num>0)
		{
			cmd_item_num --;
			cmd_item_index++;
			if(cmd_item_num!=0)
			{
				Sleep(m_nTimeGap);		
				Cmd_Item_Send(cmd_queue[cmd_item_index],1);
			}
			else
			{			//发送结束
			TransferComplete();
			return 0;
			}
		}
		else
		{

		}
	}
	return 0;
}
void CFC_ConfigerDlg::ProcessACK(void)
{
	//仅有一条命令，发送结束
	if(cmd_item_num<=1)
	{
		TransferComplete();
		return;
	}
	cmd_item_num --;
	cmd_item_index++;
	if(cmd_item_num>0)
	{
		Sleep(m_nTimeGap);
		Cmd_Item_Send(cmd_queue[cmd_item_index],1);
	}
}


void CFC_ConfigerDlg::UpDateCanSpd(UINT data)
{
	UINT temp = data;
	for(int i=0;i<4;i++)
		m_CAN_SPD[i] = (temp>>(i*2))&0x03;
	UpdateData(FALSE);
}

LRESULT CFC_ConfigerDlg::OnGetDefID(WPARAM wp, LPARAM lp) 
{
  return MAKELONG(0,DC_HASDEFID); 
}


void CFC_ConfigerDlg::InitStatisticsShow(void)
{
	LV_ITEM lvi;
	lvi.mask       = LVIF_TEXT | LVIF_IMAGE;
	lvi.cchTextMax = 0;
	lvi.lParam     = 0;

	m_GridEthState.DeleteAllItems();
	m_GridCanState.DeleteAllItems();
	TCHAR sBuffer[20];
	for( int j = 0;j<2;j++)
	{
		lvi.iItem      = j;
		lvi.iSubItem   = 0;
		lvi.iImage	  = 1;		
		wsprintf(sBuffer, _T("%u"),0);
		lvi.pszText = sBuffer;		
		int nRtnValue = m_GridEthState.InsertItem(&lvi);
	
		wsprintf(sBuffer, _T("%u"),0);
		lvi.pszText = sBuffer;		
		nRtnValue = m_GridCanState.InsertItem(&lvi);

	}
	for( int j = 0;j<5;j++)
	{
		lvi.iItem      = j;
		lvi.iSubItem   = 0;
		lvi.iImage	  = 1;
		
		wsprintf(sBuffer, _T("%u"),0);
		lvi.pszText = sBuffer;		
		int nRtnValue = m_GridFCState.InsertItem(&lvi);
	}
}
void CFC_ConfigerDlg::UpdateStatistics(void)
{
	TCHAR sBuffer[20];

	//链路状态
	/*
0：FCA link状态
1：FCB link状态
8：eth0 link状态
9：eth1 link状态
10：eth2 link状态
11：eth3 link状态*/
	UINT lk = m_nLinkStatus;
	for(int i=0;i<2;i++)
	{
		m_csLinkStatus[4+i].m_bOn = (lk>>i)&0x1;
		m_csLinkStatus[4+i].Invalidate();
	}
	for(int i=0;i<4;i++)
	{
		m_csLinkStatus[i].m_bOn = (lk>>(i+8))&0x1;
		m_csLinkStatus[i].Invalidate();
	}
	for( int j = 0;j<2;j++)
	{		
		for(int i=0;i<4;i++)
		{
			wsprintf(sBuffer, _T("%u"),StatisData[j*4+i]);
			m_GridEthState.SetItemText(j, i, sBuffer);
			wsprintf(sBuffer, _T("%u"),StatisData[j*4+i+8]);
			m_GridCanState.SetItemText(j, i, sBuffer);
		}
	}
	int index = 16;
	for(int i=0;i<2;i++)
	for( int j = 0;j<5;j++)
	{	
		if((1==j)||(4==j))
		{	
			CString strLongToCstr;
			long long total;
			total = StatisData[index++];
			total += total+StatisData[index++]*(0x100000000L);
			strLongToCstr.Format("%I64d", total);
			m_GridFCState.SetItemText(j, i, strLongToCstr);
		}
		else
		{
			wsprintf(sBuffer, _T("%u"),StatisData[index++]);
			m_GridFCState.SetItemText(j, i, sBuffer);
		}
	}
}

void CFC_ConfigerDlg::OnBnClickedAllEth()
{
	UpdateData(TRUE);	
	for(int i=0;i<4;i++)
		m_bEthPort[i] = TRUE;
	UpdateData(FALSE);
}


void CFC_ConfigerDlg::OnWriteEthPriority()
{
	UpdateData(TRUE);	
	int num = PackageEthPriority(PC_WRITE_CMD,0);
	StartupCmdlistSend(num);
}
LRESULT CFC_ConfigerDlg::OnMessageDeal(WPARAM data, LPARAM len)
{
	if((CFCGrid*)data==&m_UGGridCtrl)
		OnBnClickedButtonDelIpfc();
	else if ((CFCGrid*)data==&m_UGGridDID_Eth)
		OnBnClickedButtonDelDid();
	else if ((CFCGrid*)data==&m_UGGridCtrl_BroadCast)
		OnBnClickedButtonDelIpfc_Broad();
	else if ((CFCGrid*)data == &m_Grid_BroadIPPort)
	{
		CFCGrid* pGrid = &m_Grid_BroadIPPort;
		int nIdx = GridDeleteItems(pGrid);
		if (nIdx >= 0)
			UpDateGridView(pGrid, nIdx);
	}
	return 0;
}


//下载转发表
void CFC_ConfigerDlg::OnBnClickedWriteCfg()
{
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
	int num =  PackageTransferTable(0);	
	StartupCmdlistSend(num);
}

//发送一条指令
BOOL CFC_ConfigerDlg::Cmd_Item_Send(PC_CMD_ITEM cmd,int retrytimes)
{
	int index = 0;
	//组帧
	SendBuff[index++] = FC_FAE_SOF;
	if(cmd.bReadWrite)
		SendBuff[index++] = PC2CARD_WRITE;
	else
		SendBuff[index++] = PC2CARD_READ;
	for(int i=0;i<4;i++,index++)
		SendBuff[index] = (cmd.nRegAddr>>((3-i)*8))&0xff; 
	if(cmd.bReadWrite)
	{
		for(int i=0;i<4;i++,index++)
			SendBuff[index] = (cmd.WriteData>>((3-i)*8))&0xff; 
	}
	SendBuff[index++] = FC_FAE_EOF;
	m_SendLength = index;
	m_SendTimesLeft = retrytimes;

	bResponseReq = (cmd.nNextCmd>=0);
	SendDataFrame();

	if((cmd.nRegAddr == Flash_cfg_addr_wr)||(cmd.nRegAddr ==Flash_cfg_addr_rd))
		bHideReplyMsg = TRUE;
	else
		bHideReplyMsg = FALSE;
	CString str_cmd_infor = Get_CMD_Description(cmd);
	ShowMessgeInfor(TRUE,str_cmd_infor);
	m_Ctl_Prgress.SetPos(cmd_item_index+1);
	return true;
}

void CFC_ConfigerDlg::OnBnClickedBtnLoadCfg()
{
	PC_CMD_ITEM cmd; 
	int index = 0;
	cmd = Get_CMD_Item(PC_READ_CMD,Cfg_done);
	cmd_item_list[index++] = cmd;
	cmd = Get_CMD_Item(PC_WRITE_CMD,Cfg_update);
	cmd.WriteData = 1;
	cmd_item_list[index++] = cmd;
	StartupCmdlistSend(index);
}

PC_CMD_ITEM CFC_ConfigerDlg::Get_CMD_Item(BOOL bReadWrite,UINT nRegAddr)
{
	const PC_CMD_ITEM cmd_items_table[] = {
		{PC_WRITE_CMD,device_soft_reset,1,&m_nVoidV,-1},
		{PC_READ_CMD,device_version,1,&m_nVersion,0},
		{PC_READ_CMD,Cfg_done,1,&m_nCfgdone,1},
		{PC_WRITE_CMD,Can_speed,1,&m_nCANSpeed,0},
		{PC_READ_CMD,Can_speed,1,&m_nCANSpeed,0},
		{PC_READ_CMD,Link_status,1,&m_nLinkStatus,0},
		{PC_READ_CMD,Sw_id,1,&m_nSwitchID,0},
		{PC_WRITE_CMD,Sw_id,1,&m_nSwitchID,0},
		{PC_WRITE_CMD,Clear_cfg,1,&m_nVoidV,0},
		{PC_READ_CMD,Clear_done,0,&m_nVoidV,1},
		{PC_WRITE_CMD,Cfg_update,1,&m_nVoidV,0},
		{PC_WRITE_CMD,Flash_cfg_addr_wr,1,&m_nVoidV,0},
		{PC_WRITE_CMD,Flash_cfg_data_wr,1,&m_nVoidV,0},
		{PC_WRITE_CMD,Flash_cfg_addr_rd,1,&m_nVoidV,0},
		{PC_READ_CMD,Flash_cfg_data_rd,1,&m_nCfgReadValue,2},
		{PC_WRITE_CMD,Mib_clear,1,&m_nVoidV,0},

	};	
	for(int i=0;i<sizeof(cmd_items_table)/sizeof(PC_CMD_ITEM);i++)
	{
		if((cmd_items_table[i].bReadWrite == bReadWrite)&&
			(cmd_items_table[i].nRegAddr == nRegAddr))
			return(cmd_items_table[i]);
	}
	return(cmd_items_table[0]);
}

//返回总条数
int CFC_ConfigerDlg::PackageTransferTable(int start_index)
{
	PC_CMD_ITEM cmd_temp_a,cmd_temp_b; 
	int index = start_index;
	cmd_item_list[index++] = Get_CMD_Item(PC_WRITE_CMD,Clear_cfg);
	cmd_item_list[index++] = Get_CMD_Item(PC_READ_CMD,Clear_done);
	cmd_temp_a = Get_CMD_Item(PC_WRITE_CMD,Flash_cfg_addr_wr);
	cmd_temp_b = Get_CMD_Item(PC_WRITE_CMD,Flash_cfg_data_wr);
	for(int i=0;i<FC_IP_DID_Map_UniCast.GetCount();i++)
	{
		cmd_temp_a.WriteData = FC_IP_DID_Map_UniCast.GetAt(i).x;
		cmd_item_list[index++] = cmd_temp_a;
		cmd_temp_b.WriteData = FC_IP_DID_Map_UniCast.GetAt(i).y;	
		cmd_item_list[index++] = cmd_temp_b;
	}
	for (int i = 0; i < FC_DID_ETH_Map.GetCount(); i++)
	{
		int x = FC_DID_ETH_Map.GetAt(i).x;
		int y = FC_DID_ETH_Map.GetAt(i).y;
		cmd_temp_a.WriteData = (FC_DID_ETH_Map.GetAt(i).x & 0xff) + 0x100;
		cmd_item_list[index++] = cmd_temp_a;
		cmd_temp_b.WriteData = FC_DID_ETH_Map.GetAt(i).y;
		cmd_item_list[index++] = cmd_temp_b;
	}
	for(int i=0;i<FC_IP_DID_Map_BroadCast.GetCount();i++)
	{
		cmd_temp_a.WriteData = FC_IP_DID_Map_BroadCast.GetAt(i).x+0x300;
		cmd_item_list[index++] = cmd_temp_a;
		cmd_temp_b.WriteData = FC_IP_DID_Map_BroadCast.GetAt(i).y;	
		cmd_item_list[index++] = cmd_temp_b;
	}
	//m_Map_BroadIPPort
	int address = 0x400;
	for (int i = 0; i<m_Map_BroadIPPort.GetCount(); i++)
	{		
		DWORD ipValue = m_Map_BroadIPPort.GetAt(i).x;
		DWORD portValue = m_Map_BroadIPPort.GetAt(i).y;
		
		for (size_t j = 0; j < 4; j++)
		{
			cmd_temp_a.WriteData = address;
			cmd_item_list[index++] = cmd_temp_a;
			cmd_temp_b.WriteData = (ipValue >> (3 - j)*8) & 0xff;
			cmd_item_list[index++] = cmd_temp_b;
			address++;
		}
		cmd_temp_a.WriteData = address;
		cmd_item_list[index++] = cmd_temp_a;
		cmd_temp_b.WriteData = portValue;
		cmd_item_list[index++] = cmd_temp_b;
		address++;
	}
	while (address <= 0x49f)
	{
		cmd_temp_a.WriteData = address++;
		cmd_item_list[index++] = cmd_temp_a;
		cmd_temp_b.WriteData = 0;
		cmd_item_list[index++] = cmd_temp_b;
	}

	//以太网发送到fc的单播地址高字节
	for(int i=0;i<3;i++)
	{
		cmd_temp_a.WriteData = 0x220+i;
		cmd_item_list[index++] = cmd_temp_a;
		cmd_temp_b.WriteData = Uni_IP_H[i];
		cmd_item_list[index++] = cmd_temp_b;
	}
	//以太网发送到fc的组播地址高字节
	for(int i=0;i<3;i++)
	{
		cmd_temp_a.WriteData = 0x223+i;
		cmd_item_list[index++] = cmd_temp_a;
		cmd_temp_b.WriteData = Broad_IP_H[i];
		cmd_item_list[index++] = cmd_temp_b;
	}
	return (index-start_index);
}

//返回总条数,CAN速率的读写
int CFC_ConfigerDlg::PackageCANSpeed(BOOL bReadWrite,int start_index)
{

	PC_CMD_ITEM cmd_temp_a, cmd_temp_b;
	int index = start_index;
	if (bReadWrite)
	{
		cmd_temp_a = Get_CMD_Item(PC_WRITE_CMD, Flash_cfg_addr_wr);
		cmd_temp_b = Get_CMD_Item(PC_WRITE_CMD, Flash_cfg_data_wr);
	}
	else
	{
		cmd_temp_a = Get_CMD_Item(PC_WRITE_CMD, Flash_cfg_addr_rd);
		cmd_temp_b = Get_CMD_Item(PC_READ_CMD, Flash_cfg_data_rd);
	}
	cmd_temp_a.WriteData = 0x226;
	cmd_item_list[index++] = cmd_temp_a;
	if (bReadWrite)
	{
		UINT temp = 0;
		for (int i = 0; i<4; i++)
			temp += m_CAN_SPD[i] << (i * 2);
		m_nCANSpeed = temp;
		cmd_temp_b.WriteData = m_nCANSpeed;
	}		
	else
	{
		cmd_temp_b.pReadData = &m_nCANSpeed;
	}
		
	cmd_item_list[index++] = cmd_temp_b;	
	//CAN端口ID

	//CAN的发送DID
	return (index-start_index);
}

void CFC_ConfigerDlg::StartupCmdlistSend(int num)
{
	if( !bStopTransfer)
	{
		MessageBox( _T("上次通信尚未结束"), _T("请等待。。"), MB_ICONSTOP);
		return;
	}
	bStopTransfer = FALSE;
	cmd_item_index = 0;
	nPollCounter = 0;	
	cmd_item_num = num;
	for (int i = 0; i < cmd_item_num; i++)
	{
		cmd_queue[i] = cmd_item_list[i];
		/*if (cmd_queue[i].WriteData != 0)
		{
			Cmd_Item_Send(cmd_queue[i], 1);
		}	*/	
	}
		
	Cmd_Item_Send(cmd_queue[0],1);
	m_Ctl_Prgress.SetRange(0,cmd_item_num);
	m_Ctl_Prgress.SetPos(0);
}
//can速率读取
void CFC_ConfigerDlg::OnBnClickedBtnCanRd()
{
	int num = PackageCANSpeed(PC_READ_CMD,0);
	num += PackageCAN_PortID(PC_READ_CMD,num);
	num += PackageCAN_SendDID(PC_READ_CMD,num);
	StartupCmdlistSend(num);
}
//can速率写入
void CFC_ConfigerDlg::OnBnClickedBtnCanWr()
{
	UpdateData(TRUE);
	int num = PackageCANSpeed(PC_WRITE_CMD,0);
	num += PackageCAN_PortID(PC_WRITE_CMD,num);
	num += PackageCAN_SendDID(PC_WRITE_CMD,num);
	StartupCmdlistSend(num);
}

//修改交换机ID
void CFC_ConfigerDlg::OnBnClickedBtnWrSwitchid()
{
	UpdateData(TRUE);
	int num = PackageSwitchID(PC_WRITE_CMD,0);
	StartupCmdlistSend(num);
}

//交换机的ID
int CFC_ConfigerDlg::PackageSwitchID(BOOL bReadWrite,int start_index)
{
	PC_CMD_ITEM cmd_temp_a,cmd_temp_b; 
	int index = start_index;
	if(bReadWrite)
	{
		cmd_temp_a = Get_CMD_Item(PC_WRITE_CMD,Flash_cfg_addr_wr);			
		cmd_temp_b = Get_CMD_Item(PC_WRITE_CMD,Flash_cfg_data_wr);
	}
	else
	{
		cmd_temp_a = Get_CMD_Item(PC_WRITE_CMD,Flash_cfg_addr_rd);		
		cmd_temp_b = Get_CMD_Item(PC_READ_CMD,Flash_cfg_data_rd);
	}
	cmd_temp_a.WriteData = 0x200;
	cmd_item_list[index++] = cmd_temp_a;
	if(bReadWrite)
		cmd_temp_b.WriteData = m_nSwitchID;
	else
		cmd_temp_b.pReadData = &m_nSwitchID;
	cmd_item_list[index++] = cmd_temp_b;
	return (index-start_index);
}

//修改SourceID
void CFC_ConfigerDlg::OnBnClickedBtnWrSourceID()
{
	UpdateData(TRUE);
	if (!Verify_SourceID())
	{
		return;
	}
	int num = PackageSourceID(PC_WRITE_CMD, 0);
	StartupCmdlistSend(num);
}

//PackageSourceID
int CFC_ConfigerDlg::PackageSourceID(BOOL bReadWrite, int start_index)
{
	PC_CMD_ITEM cmd_temp_a, cmd_temp_b;
	int index = start_index;
	int address = 0x227;
	if (bReadWrite)
	{
		cmd_temp_a = Get_CMD_Item(PC_WRITE_CMD, Flash_cfg_addr_wr);
		cmd_temp_b = Get_CMD_Item(PC_WRITE_CMD, Flash_cfg_data_wr);
		int sourID = m_SourceID & 0xffffff;
		for (int i = 0; i < 3; i++)
		{
			cmd_temp_a.WriteData = address;
			cmd_item_list[index++] = cmd_temp_a;
			cmd_temp_b.WriteData = (sourID >> ((2 - i) * 8)) & 0xff;
			cmd_item_list[index++] = cmd_temp_b;
			address++;
		}
	}
	else
	{
		cmd_temp_a = Get_CMD_Item(PC_WRITE_CMD, Flash_cfg_addr_rd);
		cmd_temp_b = Get_CMD_Item(PC_READ_CMD, Flash_cfg_data_rd);		
		for (int i = 0; i < 3; i++)
		{
			cmd_temp_a.WriteData = address;
			cmd_item_list[index++] = cmd_temp_a;
			cmd_temp_b.pReadData = Card_Flash_Table + address;
			cmd_item_list[index++] = cmd_temp_b;
			address++;
		}
	}
	return (index - start_index);
}

//板卡的信息
int CFC_ConfigerDlg::PackageCardInfor(int start_index)
{
	PC_CMD_ITEM cmd_temp_a; 
	int index = start_index;
	cmd_temp_a = Get_CMD_Item(PC_READ_CMD,device_version);
	cmd_item_list[index++] = cmd_temp_a;
	return (index-start_index);
}
//读取ID
void CFC_ConfigerDlg::OnBnClickedBtnRdSwitchid()
{
	int num = PackageSwitchID(PC_READ_CMD,0);
	StartupCmdlistSend(num);
}
//读取SourceID
void CFC_ConfigerDlg::OnBnClickedBtnRdSourceID()
{
	int num = PackageSourceID(PC_READ_CMD, 0);
	StartupCmdlistSend(num);
}

void CFC_ConfigerDlg::OnBnClickedBtnRdPriority()
{
	int num = PackageEthPriority(PC_READ_CMD,0);
	StartupCmdlistSend(num);
}

//读取/写入以太网口的优先级
int CFC_ConfigerDlg::PackageEthPriority(BOOL bReadWrite,int start_index)
{
	PC_CMD_ITEM cmd_temp_a,cmd_temp_b; 
	int index = start_index;


	if(bReadWrite)
	{
		cmd_temp_a = Get_CMD_Item(PC_WRITE_CMD,Flash_cfg_addr_wr);			
		cmd_temp_b = Get_CMD_Item(PC_WRITE_CMD,Flash_cfg_data_wr);
	}
	else
	{
		cmd_temp_a = Get_CMD_Item(PC_WRITE_CMD,Flash_cfg_addr_rd);		
		cmd_temp_b = Get_CMD_Item(PC_READ_CMD,Flash_cfg_data_rd);
	}
	for(int i=0;i<4;i++)
	{
		cmd_temp_a.WriteData = 0x201+i;
		cmd_item_list[index++] = cmd_temp_a;
		if(bReadWrite)
			cmd_temp_b.WriteData = m_nPriorityEth[i];
		else
			cmd_temp_b.pReadData = m_nPriorityEth+i;
		cmd_item_list[index++] = cmd_temp_b;
	}
	return (index-start_index);
}

//获取命令的描述
CString CFC_ConfigerDlg::Get_CMD_Description(PC_CMD_ITEM cmd)
{
	CMD_DESCRP strCMDText[] = {
		device_soft_reset,_T("软件复位"),
		device_version,_T("设备程序版本"),
		Cfg_done,_T("设备上电初始化状态"),
		Can_speed,_T("设备can工作速度"),
		Link_status,_T("链路状态"),
		Sw_id,_T("交换机ID号"),
		Clear_cfg,_T("清除板卡配置(擦除flash)"),
		Clear_done,_T("板卡擦除flash状态"),
		Cfg_update,_T("配置信息加载寄存器"),
		Mib_clear,_T("清除全部mib信息"),
		net_A_recv_num,_T("以太网物理端口优先级"),
		net_A_send_num,_T("网口收到的以太网帧数目"),
		net_A_send_num,_T("网口发送的以太网帧数目"),
		can_A_recv_num,_T("can口A收到的can帧数目"),
		can_A_send_num,_T("can口A发送的can帧数目"),
		FcA_recv_num,_T("FC A收到的帧数目"),
	};
	CString strPre,strFlash;
	UINT reg = cmd.nRegAddr;
	if(cmd.bReadWrite)
		strPre = _T("写入_");
	else
		strPre = _T("读取_");
	for(int i=0;i<sizeof(strCMDText)/sizeof(CMD_DESCRP);i++)
	{
		if(reg == strCMDText[i].nRegAddr)
			strPre += strCMDText[i].strDes;
	}
	if((reg==Flash_cfg_addr_wr)||(reg==Flash_cfg_addr_rd))
	{
		UINT data = cmd.WriteData;
		if(data <= 0xff)
		{
			strFlash.Format(_T(".%d"),data);
			strFlash =_T("IP地址")+ m_strIP_HighPart_Uni+strFlash;
		}
		else if(data<0x200)
		{
			strFlash.Format(_T("DID:%d优先级"),data-0x100);
		}
		else if(data==0x200)
		{
			strFlash = _T("交换机ID号");
		}
		else if (data == 0x227)
		{
			strFlash = _T("SourceID号");
		}
		else if(data<=0x204)
			strFlash.Format(_T("以太网物理端口%d优先级"),data-0x200);
		strPre += strFlash;
	}
	if((reg==Flash_cfg_data_wr)||(reg==Flash_cfg_data_rd))
		strPre = str_msg_text;
	return strPre;

}
void CFC_ConfigerDlg::ReceiveDataAnalyze(void)
{
	//解析
	//未收到帧头或已经暂停操作
	if(( FC_FAE_SOF != RcvBuff[0])||(bStopTransfer == TRUE))
	{
		m_RcvIndex = 0;
		return;
	}
	if( m_RcvIndex<2)
		return ;
	if( CARD2PC_DATA == RcvBuff[1])
	{
		if(m_RcvIndex>=7)
		{
			if( FC_FAE_EOF == RcvBuff[6])
			{
				//正确接收到板卡的数
				//UINT RcvData = *(UINT*)(RcvBuff+2);
				UINT RcvData = 0;
				for(int i=0;i<4;i++)
				{
					RcvData <<= 8;					
					RcvData += RcvBuff[2+i];
				}
				KillTimer(1);
				m_SendTimesLeft = 0;
				if( 0== ProcessRcv(RcvData))
				{
					if(!bHideReplyMsg)
					ShowMessgeInfor(FALSE, _T("-->成功读取！"));
				}
				else
				{
					int start_w = str_msg_text.Find(_T("--等待设备..--等待设备.."));
					if( (start_w)<0)
						ShowMessgeInfor(TRUE, str_msg_text+_T("--等待设备.."));
					else
					{
						str_msg_text = str_msg_text.Mid(0,start_w);
						ShowMessgeInfor(TRUE, str_msg_text);
					}
				}

			}
			m_RcvIndex = 0;
		}
	}
	else if( CARD2PC_ACK == RcvBuff[1])
	{
		if(m_RcvIndex>=3)
		{
			if( FC_FAE_EOF == RcvBuff[2])
			{
				//板卡应答
				KillTimer(1);
				m_SendTimesLeft = 0;
				if(!bHideReplyMsg)
				ShowMessgeInfor(FALSE, _T("-->应答OK！"));
				ProcessACK();
			}
			m_RcvIndex = 0;
		}
	}
	else
	{
		m_RcvIndex = 0;
		return;
	}
}
//配置表读出
void CFC_ConfigerDlg::OnBnClickedBtnRd()
{
	int num =  PackageTransferTable_RD(0);	
	StartupCmdlistSend(num);
}

//返回总条数
int CFC_ConfigerDlg::PackageTransferTable_RD(int start_index)
{
	PC_CMD_ITEM cmd_temp_a,cmd_temp_b; 
	int index = start_index;
	cmd_temp_a = Get_CMD_Item(PC_WRITE_CMD,Flash_cfg_addr_rd);
	cmd_temp_b = Get_CMD_Item(PC_READ_CMD,Flash_cfg_data_rd);
	for(int i=0;i<256;i++)
	{
		cmd_temp_a.WriteData = i;
		cmd_item_list[index++] = cmd_temp_a;
		cmd_temp_b.pReadData = Card_Flash_Table+i;	
		cmd_item_list[index++] = cmd_temp_b;
	}
	for(int i=0;i<256;i++)
	{
		cmd_temp_a.WriteData = i+0x100;
		cmd_item_list[index++] = cmd_temp_a;
		cmd_temp_b.pReadData = Card_Flash_Table + i + 0x100;
		cmd_item_list[index++] = cmd_temp_b;
	}
	//以太网发送到fc的单播地址高字节
	for (int i = 0; i<3; i++)
	{
		cmd_temp_a.WriteData = 0x220 + i;
		cmd_item_list[index++] = cmd_temp_a;
		cmd_temp_b.pReadData = Card_Flash_Table + 0x220 + i;
		cmd_item_list[index++] = cmd_temp_b;
	}
	//以太网发送到fc的组播地址高字节
	for (int i = 0; i<3; i++)
	{
		cmd_temp_a.WriteData = 0x223 + i;
		cmd_item_list[index++] = cmd_temp_a;
		cmd_temp_b.pReadData = Card_Flash_Table + 0x223 + i;
		cmd_item_list[index++] = cmd_temp_b;
	}

	for(int i=0;i<256;i++)
	{
		cmd_temp_a.WriteData = i+0x300;
		cmd_item_list[index++] = cmd_temp_a;
		cmd_temp_b.pReadData = Card_Flash_Table+i+0x300;	
		cmd_item_list[index++] = cmd_temp_b;
	}
	//m_Map_BroadIPPort
	int address = 0x400;
	for (int i = 0; i<m_Map_BroadIPPort.GetCount(); i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			cmd_temp_a.WriteData = address;
			cmd_item_list[index++] = cmd_temp_a;
			cmd_temp_b.pReadData = Card_Flash_Table + address;
			cmd_item_list[index++] = cmd_temp_b;
			address++;
		}
		cmd_temp_a.WriteData = address;
		cmd_item_list[index++] = cmd_temp_a;
		cmd_temp_b.pReadData = Card_Flash_Table + address;
		cmd_item_list[index++] = cmd_temp_b;
		address++;
	}
	while (address <= 0x49f)
	{
		cmd_temp_a.WriteData = address;
		cmd_item_list[index++] = cmd_temp_a;
		Card_Flash_Table[address] = 0;
		cmd_temp_b.pReadData = Card_Flash_Table + address;
		cmd_item_list[index++] = cmd_temp_b;
		address++;
	}		
	return (index-start_index);
}

void CFC_ConfigerDlg::OnBnClickedBtnClrStat()
{
	PC_CMD_ITEM cmd_temp_a; 
	cmd_temp_a = Get_CMD_Item(PC_WRITE_CMD,Mib_clear);
	cmd_temp_a.WriteData = 1;
	cmd_item_list[0] = cmd_temp_a;
	StartupCmdlistSend(1);
}


void CFC_ConfigerDlg::OnBnClickedBtnRfeshStat()
{
	PC_CMD_ITEM cmd; 
	int index = 0;
	//链路
	cmd = Get_CMD_Item(PC_READ_CMD,Link_status);
	cmd.bReadWrite = PC_READ_CMD;
	cmd.nNextCmd = 0;
	cmd.WriteData = 0;
	cmd_item_list[index++] = cmd;
	for(int i=0;i<30;i++)
	{
		UINT t =  net_A_recv_num+i*4;
		if( t>FcA_send_byte_H)
			t += 4;
		cmd.nRegAddr = t;
		cmd.pReadData = StatisData+i;
		cmd_item_list[index++] = cmd;
	}
	StartupCmdlistSend(index);
}

void CFC_ConfigerDlg::OpenSerialPort()
{	
	if( TRUE == m_ComPort.InitPort(this->m_hWnd,m_comport_portnr,m_comport_baud,'N',8,ONESTOPBIT,EV_RXCHAR))
	{
		m_ComPort.StartMonitoring();
	}
}
void CFC_ConfigerDlg::TransferComplete()
{
	m_SendTimesLeft = 0;
	cmd_item_num = 0;
	cmd_item_index  = 0;
	bStopTransfer = TRUE;
}


void CFC_ConfigerDlg::OnBnClickedOneKeyWrite()
{
	int num = 0;
	UpdateData(TRUE);
	num = PackageCANSpeed(PC_WRITE_CMD,0);

	//
	num += PackageCAN_PortID(PC_WRITE_CMD,num);
	num += PackageCAN_SendDID(PC_WRITE_CMD,num);
	//

	num += PackageEthPriority(PC_WRITE_CMD,num);
	num += PackageSwitchID(PC_WRITE_CMD,num);	
	num += PackageSourceID(PC_WRITE_CMD, num);

	IP_DID_TableToArray();
	if( !Verify_Ethernet_FC())
		return;
	DID_Port_TableToArray();
	if( !Verify_FC_Ethernet())
		return;	
	IP_DID_TableToArray_Broad();
	if( !Verify_Ethernet_FC_Broad())
		return;
	TableToArray_BroadIPPort();
	if (!Verify_BroadIPPort())
		return;	
	num +=  PackageTransferTable(num);	
	StartupCmdlistSend(num);
}


void CFC_ConfigerDlg::OnBnClickedBtnOnekeyRd()
{
	int num = 0;
	num += PackageCardInfor(num);
	num += PackageCANSpeed(PC_READ_CMD,num);

	num += PackageCAN_PortID(PC_READ_CMD,num);
	num += PackageCAN_SendDID(PC_READ_CMD,num);

	num += PackageEthPriority(PC_READ_CMD,num);
	num += PackageSwitchID(PC_READ_CMD,num);
	num += PackageSourceID(PC_READ_CMD, num);
	num += PackageTransferTable_RD(num);

	StartupCmdlistSend(num);
}


void CFC_ConfigerDlg::OnBnClickedCheckAutoReadSta()
{
	m_bAutoReadStat = (!m_bAutoReadStat);
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_AUTO_READ_STA);
	pBtn->SetCheck(m_bAutoReadStat);
	if(m_bAutoReadStat)
		SetTimer(2,2000,NULL);
	else
		KillTimer(2);
}

void CFC_ConfigerDlg::EnableButtons(BOOL bEn)
{
	CWnd* pWnd;
	for(int i=0;i<15;i++)
	{
		pWnd = (CWnd*)GetDlgItem(IDC_BTN_ONEKEY_WR+i);
		pWnd->EnableWindow(bEn);
	}
	pWnd = (CWnd*)GetDlgItem(IDC_CHECK_AUTO_READ_STA);
	pWnd->EnableWindow(bEn);
	pWnd = (CWnd*)GetDlgItem(IDC_BTN_WR_SourceID);
	pWnd->EnableWindow(bEn);
	pWnd = (CWnd*)GetDlgItem(IDC_BTN_RD_SourceID);
	pWnd->EnableWindow(bEn);
	//禁止菜单对应项的操作
	if(bEn)
		GetMenu()->GetSubMenu(0)->EnableMenuItem(2,MF_BYPOSITION | MF_ENABLED);
	else
		GetMenu()->GetSubMenu(0)->EnableMenuItem(2,MF_BYPOSITION | MF_DISABLED);
}

//CAN的发送DID
int CFC_ConfigerDlg::PackageCAN_SendDID(BOOL bReadWrite,int start_index)
{
	PC_CMD_ITEM cmd_temp_a,cmd_temp_b; 
	int index = start_index;
	if(bReadWrite)
	{
		cmd_temp_a = Get_CMD_Item(PC_WRITE_CMD,Flash_cfg_addr_wr);			
		cmd_temp_b = Get_CMD_Item(PC_WRITE_CMD,Flash_cfg_data_wr);
	}
	else
	{
		cmd_temp_a = Get_CMD_Item(PC_WRITE_CMD,Flash_cfg_addr_rd);		
		cmd_temp_b = Get_CMD_Item(PC_READ_CMD,Flash_cfg_data_rd);
	}
	for(int i=0;i<4;i++)
	{
		cmd_temp_a.WriteData = 0x205+i;
		cmd_item_list[index++] = cmd_temp_a;
		if(bReadWrite)
			cmd_temp_b.WriteData = m_nCAN_Send_DID[i];
		else
			cmd_temp_b.pReadData =m_nCAN_Send_DID+i;
		cmd_item_list[index++] = cmd_temp_b;
	}
	return (index-start_index);
}

//CAN的端口ID
int CFC_ConfigerDlg::PackageCAN_PortID(BOOL bReadWrite,int start_index)
{
	PC_CMD_ITEM cmd_temp_a,cmd_temp_b; 
	int index = start_index;
	if(bReadWrite)
	{
		cmd_temp_a = Get_CMD_Item(PC_WRITE_CMD,Flash_cfg_addr_wr);			
		cmd_temp_b = Get_CMD_Item(PC_WRITE_CMD,Flash_cfg_data_wr);
	}
	else
	{
		cmd_temp_a = Get_CMD_Item(PC_WRITE_CMD,Flash_cfg_addr_rd);		
		cmd_temp_b = Get_CMD_Item(PC_READ_CMD,Flash_cfg_data_rd);
	}
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<4;j++)
		{
			cmd_temp_a.WriteData = 0x210+i*4+j;
			cmd_item_list[index++] = cmd_temp_a;
			if(bReadWrite)
				cmd_temp_b.WriteData = (m_nCAN_Port_ID[i]>>((3-j)*8))&0xff;
			else
				cmd_temp_b.pReadData = m_nCAN_Port_ID_Part+i*4+j;
			cmd_item_list[index++] = cmd_temp_b;
		}
	}
	return (index-start_index);
}

void CFC_ConfigerDlg::OnBnClickedButtonTread()
{
	UpdateData(TRUE);
	bFlashTest = TRUE;
	m_nTestFlashAdd = _tcstol(m_strTestFlashAdd, NULL, 16);//_tstoi(m_strTestFlashAdd);
	PackageFlashTest(FALSE);
	StartupCmdlistSend(2);
}
//Flash
int CFC_ConfigerDlg::PackageFlashTest(BOOL bReadWrite)
{
	PC_CMD_ITEM cmd_temp_a,cmd_temp_b; 
	int index = 0;
	if(bReadWrite)
	{
		cmd_temp_a = Get_CMD_Item(PC_WRITE_CMD,Flash_cfg_addr_wr);			
		cmd_temp_b = Get_CMD_Item(PC_WRITE_CMD,Flash_cfg_data_wr);
	}
	else
	{
		cmd_temp_a = Get_CMD_Item(PC_WRITE_CMD,Flash_cfg_addr_rd);		
		cmd_temp_b = Get_CMD_Item(PC_READ_CMD,Flash_cfg_data_rd);
	}
	cmd_temp_a.WriteData = m_nTestFlashAdd;
	cmd_item_list[index++] = cmd_temp_a;
	if(bReadWrite)
		cmd_temp_b.WriteData = m_nFlashWriteValue;
	else
		cmd_temp_b.pReadData = &m_nFlashReadValue;
	cmd_temp_b.nNextCmd = 0;
	cmd_item_list[index++] = cmd_temp_b;
	return (index);
}

void CFC_ConfigerDlg::OnBnClickedButtonTwrite()
{	
	UpdateData(TRUE);
	bFlashTest = TRUE;
	m_nTestFlashAdd = _tcstol(m_strTestFlashAdd, NULL, 16);
	m_nFlashWriteValue  = _tcstol(m_strTestFlashWrite, NULL, 16);
	PackageFlashTest(TRUE);
	StartupCmdlistSend(2);
}


void CFC_ConfigerDlg::OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	UpdateData(TRUE);
	m_nIP_Input +=  pNMUpDown->iDelta;
	UpdateData(FALSE);
	*pResult = 0;
}


void CFC_ConfigerDlg::OnBroadIPChanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	UpdateData(TRUE);
	m_BroadIP += pNMUpDown->iDelta;
	UpdateData(FALSE);
	*pResult = 0;
}

