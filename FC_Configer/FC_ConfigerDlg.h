
// FC_ConfigerDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "serialport.h"
#include "afxcmn.h"
#include "OxGridList.h"
#include "OXGridEdit.h"
#include "FCGrid.h"
#include "DataFrame.h"

#include "mscomm1.h"
#include "LED.h "
// CFC_ConfigerDlg 对话框

class CFC_ConfigerDlg : public CDialogEx
{
// 构造
public:
	CFC_ConfigerDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CFC_ConfigerDlg(void);	
// 对话框数据
	enum { IDD = IDD_FC_CONFIGER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	CFont m_font;
	CImageList m_ImageSmall;
	COXGridEdit m_gridEdit;
	CImageList m_stateImages;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	afx_msg void OnTimer(UINT nTimerID);
	DECLARE_MESSAGE_MAP()
public:
	CStatic m_Ctl_Logo;
	HBITMAP h_logo;
	CSerialPort m_ComPort;
	UINT  m_comport_portnr,m_comport_baud;		// portnumber (1..MaxSerialPortNum)
	int m_SendTimesLeft; //数据帧重传次数
	int m_SendLength; //数据帧长度
	int m_RcvIndex;
	BOOL bResponseReq; //是否需要应答
	BYTE SendBuff[32],RcvBuff[32];
	//等待设备完成操作的重新读循环次数，两次读写之间的间隔
	UINT m_nWaitCycles,m_nTimeGap;

	CRichEditCtrl m_MessageShow;

	int m_CAN_SPD[4];
	UINT m_nPriorityEth[4];
	BOOL m_b_IP_DID_IP_AutoInc,m_bEthPort[4];
	UINT m_nSwitchID;
	PC_CMD_ITEM cmd_item_list[2048],cmd_queue[2048]; 
	int cmd_item_num,cmd_item_index;
	UINT m_nVoidV,m_nVersion,m_nCfgdone,m_nReadFlashAddr,m_nCANSpeed,m_nLinkStatus,m_nCfgReadValue;
	CString str_msg_text;
	BOOL bHideReplyMsg;
	UINT Card_Flash_Table[1024+5],StatisData[32];
	UINT nPollCounter;
	BOOL bStopTransfer,m_bAutoReadStat;
	CProgressCtrl m_Ctl_Prgress;
	CFCGrid m_UGGridCtrl,m_UGGridCtrl_BroadCast,m_UGGridDID_Eth;
	BYTE m_nIP_map,m_nDID_map,m_nEthPortSel,m_nDID_ETH;
	CSpinButtonCtrl m_SpinEth[4],m_SpinDIDETH,m_strlSpinIPMAP,m_strlSpinDIDMAP;

	CArray<CPoint,CPoint> FC_IP_DID_Map_UniCast,FC_IP_DID_Map_BroadCast,FC_DID_ETH_Map;
	CLED m_csLinkStatus[6];
	int m_nRcvTimer;
	void ShowID_EthPort_Map(void);
	COXGridList m_GridCanState,m_GridFCState,m_GridEthState;
	afx_msg void ShowAboutDlg();
	afx_msg void OnComPara();
	afx_msg void OnBnClickedGetDeviceInfo();
	afx_msg void OnBnClickedResetDevice();
	afx_msg void OnBnClickedAllEth();
	afx_msg void OnBnClickedBtnCanRd();
	afx_msg void OnBnClickedBtnCanWr();
	afx_msg void OnBnClickedBtnWrSwitchid();
	afx_msg void OnBnClickedBtnRdSwitchid();
	afx_msg void OnBnClickedBtnRdPriority();
	afx_msg void OnBnClickedBtnRd();
	afx_msg void OnBnClickedBtnClrStat();
	afx_msg void OnBnClickedBtnRfeshStat();
	afx_msg void OnBnClickedBtnInsertIpdid();
	afx_msg void OnBnClickedBtnLoadCfg();
	afx_msg void OnBnClickedWriteCfg();
	afx_msg void OnWriteEthPriority();
	afx_msg void OnBnClickedButtonDelDid();
	afx_msg LRESULT OnGetDefID(WPARAM wp, LPARAM lp);
	afx_msg void OnBnClickedButtonDelIpfc();
	afx_msg void OnBnClickedButtonDelIpfc_Broad();
	afx_msg LRESULT OnMessageDeal(WPARAM data, LPARAM len);
	afx_msg void SaveFile();
	afx_msg void OPenFile();
	afx_msg void OnBnClickedButtonInsertDideth();


	BOOL SendDataFrame(void);
	void ShowMessgeInfor(BOOL bNewMsg,CString strText);
	void UpDateCanSpd(UINT data);
	void ShowIP_DID_Map(void);
	void UpdateStatistics(void);
	CString m_strIP_HighPart_Uni,m_strIP_HighPart_Broad;
	/*0x220-0x222	以太网发送到fc的单播地址高字节	最高字节放0x220地址	例：192.168.1.x
0x223-0x225	以太网发送到fc的组播地址高字节	最高字节放0x220地址	例：224.0.0.x
	*/
	BYTE Uni_IP_H[4],Broad_IP_H[4];
	BOOL Verify_FC_Ethernet(void);
	BOOL Verify_Ethernet_FC(void);
	BOOL Verify_Ethernet_FC_Broad(void);
	void IP_DID_TableToArray(void);
	void IP_DID_TableToArray_Broad(void);
	void DID_Port_TableToArray(void);
	int Read_IP_DID_Map(LPARAM lp);
	int UpDateGridView(CFCGrid* pGrid,int cursel);
	int GridInsertItem(CFCGrid* pGrid);
	int GridDeleteItems(CFCGrid* pGrid);
	CString EthPortToString(BYTE ports_sel);
	int SetGridItemText(CFCGrid* pGrid,int row_number,int x,int y);
	void MapDataToGridView(CFCGrid* pGrid);
	BOOL Cmd_Item_Send(PC_CMD_ITEM cmd,int retrytimes);
	void ProcessACK(void);
	PC_CMD_ITEM Get_CMD_Item(BOOL bReadWrite,UINT nRegAddr);
	int PackageTransferTable(int start_index);
	int PackageCANSpeed(BOOL bReadWrite,int start_index);
	int PackageEthPriority(BOOL bReadWrite,int start_index);
	void StartupCmdlistSend(int num);
	int PackageSwitchID(BOOL bReadWrite,int start_index);
	int PackageCardInfor(int start_index);
	int PackageTransferTable_RD(int start_index);
	void TransferComplete(void);
	void InitStatisticsShow(void);
	void OpenSerialPort(void);
	void ReceiveDataAnalyze(void);
	void ShowVersion(UINT data);
	int ProcessRcv(UINT rcvdata);
	BOOL VerifyDID_CAN_PORT(void);
	BOOL VerifyDID_CAN_IP_FC(void);
	CString Get_CMD_Description(PC_CMD_ITEM cmd);
	void EnableButtons(BOOL bEn);
	int PackageCAN_PortID(BOOL bReadWrite,int start_index);
	int PackageCAN_SendDID(BOOL bReadWrite,int start_index);
	afx_msg void OnBnClickedOneKeyWrite();
	afx_msg void OnBnClickedBtnOnekeyRd();
	afx_msg void OnBnClickedCheckAutoReadSta();
	int m_nEthCan;
	UINT m_nCAN_Send_DID[4];
	UINT m_nCAN_Port_ID[4],m_nCAN_Port_ID_Part[16];
	UINT m_nTestFlashAdd, m_nFlashReadValue,m_nFlashWriteValue;
	CString m_strTestFlashAdd,m_strTestFlashRead,m_strTestFlashWrite;
	afx_msg void OnBnClickedButtonTread();
	int PackageFlashTest(BOOL bReadWrite);
	BOOL bFlashTest;
	afx_msg void OnBnClickedButtonTwrite();
	CFile mFile;

	BOOL m_bUniBroad;
	DWORD m_nIP_Input;
	afx_msg void OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult);

	private:
		int FindItemById(BYTE id);
		BYTE UpdateEthAndCanValue(int row, byte ethCanPortValue);
		BYTE StringToEthAndCanValue(CString ethStr, CString canStr);
};
