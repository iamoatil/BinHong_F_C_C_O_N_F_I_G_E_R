/*
**	FILENAME			CSerialPort.cpp
**
*/

#include "stdafx.h"
#include "SerialPort.h"

#include <assert.h>

int m_nComArray[20];//存放活跃的串口号
//
// Constructor
//
CSerialPort::CSerialPort()
{
	m_hComm = NULL;

	// initialize overlapped structure members to zero
	///初始化异步结构体
	m_ov.Offset = 0;
	m_ov.OffsetHigh = 0;

	// create events
	m_ov.hEvent = NULL;
	m_hWriteEvent = NULL;
	m_hShutdownEvent = NULL;

	m_szWriteBuffer = NULL;

	m_bThreadAlive = FALSE;
	m_nWriteSize = 1;
	m_bIsSuspened = FALSE;
}

//
// Delete dynamic memory
//
CSerialPort::~CSerialPort()
{
//	MSG message;

	//增加线程挂起判断，解决由于线程挂起导致串口关闭死锁的问题 add by itas109 2016-06-29
	if(IsThreadSuspend(m_Thread))
	{
		ResumeThread(m_Thread);
	}

	//串口句柄无效  add by itas109 2016-07-29
	if(m_hComm == INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hComm);
		m_hComm = NULL;
		return;
	}

	//do
	//{
	//	SetEvent(m_hShutdownEvent);
	//	//add by liquanhai  防止死锁  2011-11-06
	//	if(::PeekMessage(&message,m_pOwner,0,0,PM_REMOVE))
	//	{
	//		::TranslateMessage(&message);
	//		::DispatchMessage(&message);
	//	}
	//} while (m_bThreadAlive);

	// if the port is still opened: close it 
	if (m_hComm != NULL)
	{
		CloseHandle(m_hComm);
		m_hComm = NULL;
	}
	// Close Handles  
	if(m_hShutdownEvent!=NULL)
		CloseHandle( m_hShutdownEvent); 
	if(m_ov.hEvent!=NULL)
		CloseHandle( m_ov.hEvent ); 
	if(m_hWriteEvent!=NULL)
		CloseHandle( m_hWriteEvent ); 

	//TRACE("Thread ended\n");

	if(m_szWriteBuffer != NULL)
	{
		delete [] m_szWriteBuffer;
		m_szWriteBuffer = NULL;
	}
}

//
// Initialize the port. This can be port 1 to MaxSerialPortNum.
///初始化串口。只能是1-MaxSerialPortNum
//
//parity:
//  n=none
//  e=even
//  o=odd
//  m=mark
//  s=space
//data:
//  5,6,7,8
//stop:
//  1,1.5,2 
//
BOOL CSerialPort::InitPort(HWND pPortOwner,	// the owner (CWnd) of the port (receives message)
	UINT  portnr,		// portnumber (1..MaxSerialPortNum)
	UINT  baud,			// baudrate
	char  parity,		// parity 
	UINT  databits,		// databits 
	UINT  stopbits,		// stopbits 
	DWORD dwCommEvents,	// EV_RXCHAR, EV_CTS etc
	UINT  writebuffersize,// size to the writebuffer

	DWORD   ReadIntervalTimeout,
	DWORD   ReadTotalTimeoutMultiplier,
	DWORD   ReadTotalTimeoutConstant,
	DWORD   WriteTotalTimeoutMultiplier,
	DWORD   WriteTotalTimeoutConstant )	

{
	assert(portnr > 0 && portnr < MaxSerialPortNum);
	assert(pPortOwner != NULL);

//	MSG message;

	//增加线程挂起判断，解决由于线程挂起导致串口关闭死锁的问题 add by itas109 2016-06-29
	if(IsThreadSuspend(m_Thread))
	{
		ResumeThread(m_Thread);
	}
	SuspendMonitoring();
	// if the thread is alive: Kill
	//if (m_bThreadAlive)
	//{
	//	do
	//	{
	//		SetEvent(m_hShutdownEvent);
	//		//add by liquanhai  防止死锁  2011-11-06
	//		if(::PeekMessage(&message,m_pOwner,0,0,PM_REMOVE))
	//		{
	//			::TranslateMessage(&message);
	//			::DispatchMessage(&message);
	//		}
	//	} while (m_bThreadAlive);
	//	//TRACE("Thread ended\n");
	//	//此处的延时很重要，因为如果串口开着，发送关闭指令到彻底关闭需要一定的时间，这个延时应该跟电脑的性能相关
	//	Sleep(50);//add by itas109 2016-08-02
	//}

	// create events
	if (m_ov.hEvent != NULL)
		ResetEvent(m_ov.hEvent);
	else
		m_ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (m_hWriteEvent != NULL)
		ResetEvent(m_hWriteEvent);
	else
		m_hWriteEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (m_hShutdownEvent != NULL)
		ResetEvent(m_hShutdownEvent);
	else
		m_hShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// initialize the event objects
	///事件数组初始化，设定优先级别
	m_hEventArray[0] = m_hShutdownEvent;	// highest priority
	//为避免有些串口设备无数据输入，但一直返回读事件，使监听线程阻塞，
	//可以将读写放在两个线程中，或者修改读写事件优先级
	//修改优先级有两个方案：
	//方案一为监听线程中WaitCommEvent()后，添加如下两条语句：
	//if (WAIT_OBJECT_O == WaitForSingleObject(port->m_hWriteEvent, 0))
	//	ResetEvent(port->m_ov.hEvent);
	//方案二为初始化时即修改，即下面两条语句：
	m_hEventArray[1] = m_hWriteEvent;
	m_hEventArray[2] = m_ov.hEvent;


	// initialize critical section
	///初始化临界资源
	InitializeCriticalSection(&m_csCommunicationSync);

	// set buffersize for writing and save the owner
	m_pOwner = pPortOwner;

	if(m_szWriteBuffer != NULL)
	{
		delete [] m_szWriteBuffer;
		m_szWriteBuffer = NULL;
	}
	m_szWriteBuffer = new char[writebuffersize];

	m_nPortNr = portnr;

	m_nWriteBufferSize = writebuffersize;
	m_dwCommEvents = dwCommEvents;

	BOOL bResult = FALSE;
	char *szPort = new char[50];
	char *szBaud = new char[50];

	/*
	多个线程操作相同的数据时，一般是需要按顺序访问的，否则会引导数据错乱，
	无法控制数据，变成随机变量。为解决这个问题，就需要引入互斥变量，让每
	个线程都按顺序地访问变量。这样就需要使用EnterCriticalSection和
	LeaveCriticalSection函数。
	*/
	// now it critical!
	EnterCriticalSection(&m_csCommunicationSync);

	// if the port is already opened: close it
	///串口已打开就关掉
	if (m_hComm != NULL)
	{
		CloseHandle(m_hComm);
		m_hComm = NULL;
	}

	// prepare port strings
	sprintf_s(szPort,50, "\\\\.\\COM%d", portnr);///可以显示COM10以上端口//add by itas109 2014-01-09


	// stop is index 0 = 1 1=1.5 2=2
	int mystop;
	int myparity;
	switch(stopbits)
	{
	case 0:
		mystop = ONESTOPBIT;
		break;
	case 1:
		mystop = ONE5STOPBITS;
		break;
	case 2:
		mystop = TWOSTOPBITS;
		break;
		//增加默认情况，因为stopbits=1.5时，SetCommState会报错。
		//一般的电脑串口不支持1.5停止位，这个1.5停止位似乎用在红外传输上的。
		//by itas109 20160506
	default:
		mystop = ONESTOPBIT;
		break;
	}
	myparity = 0;
	parity = toupper(parity);
	switch(parity)
	{
	case 'N':
		myparity = 0;
		break;
	case 'O':
		myparity = 1;
		break;
	case 'E':
		myparity = 2;
		break;
	case 'M':
		myparity = 3;
		break;
	case 'S':
		myparity = 4;
		break;
		//增加默认情况。
		//by itas109 20160506
	default:
		myparity = 0;
		break;
	}
	sprintf_s(szBaud,50, "baud=%d parity=%c data=%d stop=%d", baud, parity, databits, mystop);

	// get a handle to the port
	/*
	通信程序在CreateFile处指定串口设备及相关的操作属性，再返回一个句柄，
	该句柄将被用于后续的通信操作，并贯穿整个通信过程串口打开后，其属性
	被设置为默认值，根据具体需要，通过调用GetCommState(hComm,&&dcb)读取
	当前串口设备控制块DCB设置，修改后通过SetCommState(hComm,&&dcb)将其写
	入。运用ReadFile()与WriteFile()这两个API函数实现串口读写操作，若为异
	步通信方式，两函数中最后一个参数为指向OVERLAPPED结构的非空指针，在读
	写函数返回值为FALSE的情况下，调用GetLastError()函数，返回值为ERROR_IO_PENDING，
	表明I/O操作悬挂，即操作转入后台继续执行。此时，可以用WaitForSingleObject()
	来等待结束信号并设置最长等待时间
	*/
	m_hComm = CreateFile(LPCSTR(szPort),GENERIC_READ|GENERIC_WRITE, 0, NULL,OPEN_EXISTING, 0, NULL);
	///创建失败
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		//add by itas109 2016-08-02
		//串口打开失败，增加提示信息
		switch(GetLastError())
		{
			//串口不存在
		case ERROR_FILE_NOT_FOUND:
			{
				CString str;
				str.Format(_T("COM%d 串口不存在,Error Code:%d"),portnr,GetLastError());
				AfxMessageBox(str);
				break;
			}
			//串口拒绝访问
		case ERROR_ACCESS_DENIED:
			{
				CString str;
				str.Format(_T("COM%d 串口拒绝访问,Error Code:%d"),portnr,GetLastError());
				AfxMessageBox(str);
				break;
			}
		default:
			break;
		}
		// port not found
		delete [] szPort;
		delete [] szBaud;

		return FALSE;
	}

	// set the timeout values
	///设置超时

	m_CommTimeouts.ReadIntervalTimeout=MAXDWORD;
	m_CommTimeouts.ReadTotalTimeoutMultiplier=0;
	m_CommTimeouts.ReadTotalTimeoutConstant=50;
	m_CommTimeouts.WriteTotalTimeoutMultiplier=50;
	m_CommTimeouts.WriteTotalTimeoutConstant=2000;

	// configure
	///配置
	///分别调用Windows API设置串口参数
	if (SetCommTimeouts(m_hComm, &m_CommTimeouts))///设置超时
	{						   
		/*
		若对端口数据的响应时间要求较严格，可采用事件驱动方式。
		事件驱动方式通过设置事件通知，当所希望的事件发生时，Windows
		发出该事件已发生的通知，这与DOS环境下的中断方式很相似。Windows
		定义了9种串口通信事件，较常用的有以下三种：
		EV_RXCHAR:接收到一个字节，并放入输入缓冲区；
		EV_TXEMPTY:输出缓冲区中的最后一个字符，发送出去；
		EV_RXFLAG:接收到事件字符(DCB结构中EvtChar成员)，放入输入缓冲区
		在用SetCommMask()指定了有用的事件后，应用程序可调用WaitCommEvent()来等待事
		件的发生。SetCommMask(hComm,0)可使WaitCommEvent()中止
		*/
		if (SetCommMask(m_hComm, dwCommEvents))///设置通信事件
		{

			if (GetCommState(m_hComm, &m_dcb))///获取当前DCB参数
			{
				m_dcb.EvtChar = 'q';
				m_dcb.fRtsControl = RTS_CONTROL_ENABLE;		// set RTS bit high!
				m_dcb.BaudRate = baud;  // add by mrlong
				m_dcb.Parity   = myparity;
				m_dcb.ByteSize = databits;
				m_dcb.StopBits = mystop;

				//if (BuildCommDCB(szBaud &m_dcb))///填写DCB结构
				//{
				if (SetCommState(m_hComm, &m_dcb))///配置DCB
					; // normal operation... continue
				else
					ProcessErrorMessage(_T("SetCommState()"));
				//}
				//else
				//	ProcessErrorMessage("BuildCommDCB()");
			}
			else
				ProcessErrorMessage(_T("GetCommState()"));
		}
		else
			ProcessErrorMessage(_T("SetCommMask()"));
	}
	else
		ProcessErrorMessage(_T("SetCommTimeouts()"));

	delete [] szPort;
	delete [] szBaud;

	// flush the port
	///终止读写并清空接收和发送
	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	// release critical section
	///释放临界资源
	LeaveCriticalSection(&m_csCommunicationSync);

	//TRACE("Initialisation for communicationport %d completed.\nUse Startmonitor to communicate.\n", portnr);

	return TRUE;
}

//
//  The CommThread Function.
///线程函数
///监视线程的大致流程：
///检查串口-->进入循环{WaitCommEvent(不阻塞询问)询问事件-->如果有事件来到-->到相应处理(关闭\读\写)}
//
DWORD WINAPI CSerialPort::CommThread(LPVOID pParam)
{
	// Cast the void pointer passed to the thread back to
	// a pointer of CSerialPort class
	CSerialPort *port = (CSerialPort*)pParam;

	// Set the status variable in the dialog class to
	// TRUE to indicate the thread is running.
	///TRUE表示线程正在运行
	port->m_bThreadAlive = TRUE;	

	// Misc. variables
	DWORD BytesTransfered = 0; 
	DWORD Event = 0;
	DWORD CommEvent = 0;
	DWORD dwError = 0;
//	COMSTAT comstat;

	BOOL  bResult = TRUE;

	// Clear comm buffers at startup
	///开始时清除串口缓冲
	if (port->m_hComm)		// check if the port is opened
		PurgeComm(port->m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	// begin forever loop.  This loop will run as long as the thread is alive.
	///只要线程存在就不断读取数据
	for (;;) 
	{ 

		ReceiveChar(port);//单字符接收

	} // close forever loop

	return 0;
}

//
// start comm watching
///开启监视线程
//
BOOL CSerialPort::StartMonitoring()
{
	//if (!(m_Thread = ::CreateThread (NULL, 0, CommThread, this, 0, NULL )))
	//	return FALSE;
	return TRUE;
}

//
// Restart the comm thread
///从挂起恢复监视线程
//
BOOL CSerialPort::ResumeMonitoring()
{
	//TRACE("Thread resumed\n");
	//m_Thread->ResumeThread();
	::ResumeThread(m_Thread);
	return TRUE;
}

//
// Suspend the comm thread
///挂起监视线程
//
BOOL CSerialPort::SuspendMonitoring()
{
	//TRACE("Thread suspended\n");
	//m_Thread->SuspendThread();
	::SuspendThread(m_Thread);
	return TRUE;
}

BOOL CSerialPort::IsThreadSuspend(HANDLE hThread)
{
	DWORD   count = SuspendThread(hThread);
	if (count == -1)
	{
		return FALSE;
	}
	ResumeThread(hThread);
	return (count != 0);
}

//
// If there is a error, give the right message
///如果有错误，给出提示
//
void CSerialPort::ProcessErrorMessage(LPCTSTR ErrorText)
{
	char *Temp = new char[200];

	LPVOID lpMsgBuf;

	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
		);

	sprintf_s(Temp,200,"WARNING:  %s Failed with the following error: \n%s\nPort: %d\n", (char*)ErrorText, lpMsgBuf, m_nPortNr); 
	MessageBox(NULL, (LPCSTR)Temp, _T("Application Error"), MB_ICONSTOP);

	LocalFree(lpMsgBuf);
	delete[] Temp;
}

//
// Character received. Inform the owner
//
void CSerialPort::ReceiveChar(CSerialPort* port)
{
	BOOL  bRead = TRUE; 
	BOOL  bResult = TRUE;
	DWORD dwError = 0;
	DWORD BytesRead = 0;
//	COMSTAT comstat;
	unsigned char RXBuff;

	for (;;) 
	{ 

			///串口读出，读出缓冲区中字节
			bResult = ReadFile(port->m_hComm,		// Handle to COMM port 
				&RXBuff,				// RX Buffer Pointer
				1,					// Read one byte
				&BytesRead,			// Stores number of bytes read
				&port->m_ov);		// pointer to the m_ov structure
			// deal with the error code 
			///若返回错误，错误处理
			if (!bResult)  
			{ 
				switch (dwError = GetLastError()) 
				{ 
				case ERROR_IO_PENDING: 	
					{ 
						// asynchronous i/o is still in progress 
						// Proceed on to GetOverlappedResults();
						///异步IO仍在进行
						bRead = FALSE;
						break;
					}
				case ERROR_ACCESS_DENIED:///拒绝访问 erroe code:5
					{
						port->m_hComm = INVALID_HANDLE_VALUE;
						CString str;
						str.Format(_T("COM%d ERROR_ACCESS_DENIED，ReadFile() Error Code:%d"),port->m_nPortNr,GetLastError());
						AfxMessageBox(str);
						break;
					}
				case ERROR_INVALID_HANDLE:///打开串口失败 erroe code:6
					{
						port->m_hComm = INVALID_HANDLE_VALUE;
						break;
					}
				case ERROR_BAD_COMMAND:///连接过程中非法断开 erroe code:22
					{
						port->m_hComm = INVALID_HANDLE_VALUE;
						CString str;
						str.Format(_T("COM%d ERROR_BAD_COMMAND，ReadFile() Error Code:%d"),port->m_nPortNr,GetLastError());
						AfxMessageBox(str);
						break;
					}
				default:
					{
						// Another error has occured.  Process this error.
						port->ProcessErrorMessage(_T("ReadFile()"));
						break;
						//return;///防止读写数据时，串口非正常断开导致死循环一直执行。add by itas109 2014-01-09 与上面liquanhai添加防死锁的代码差不多
					} 
				}
			}
			else///ReadFile返回TRUE
			{
				// ReadFile() returned complete. It is not necessary to call GetOverlappedResults()
				bRead = TRUE;
				if(BytesRead>0)
				SendMessage(port->m_pOwner, WM_COMM_RXCHAR, (WPARAM)RXBuff, (LPARAM) port->m_nPortNr);
			}
		}  // close if (bRead)


		// notify parent that a byte was received
		//避免线程互相等待，产生死锁，使用PostMessage()代替SendMessage()

		//::SendMessage((port->m_pOwner), WM_COMM_RXCHAR, (WPARAM) RXBuff, (LPARAM) port->m_nPortNr);
}


//
// Return the device control block
//
DCB CSerialPort::GetDCB()
{
	return m_dcb;
}

//
// Return the communication event masks
//
DWORD CSerialPort::GetCommEvents()
{
	return m_dwCommEvents;
}

//
// Return the output buffer size
//
DWORD CSerialPort::GetWriteBufferSize()
{
	return m_nWriteBufferSize;
}

BOOL CSerialPort::IsOpen()
{
	return m_hComm != NULL && m_hComm!= INVALID_HANDLE_VALUE;//m_hComm增加INVALID_HANDLE_VALUE的情况 add by itas109 2016-07-29
}

void CSerialPort::ClosePort()
{
	MSG message;

	//增加线程挂起判断，解决由于线程挂起导致串口关闭死锁的问题 add by itas109 2016-06-29
	if(IsThreadSuspend(m_Thread))
	{
		ResumeThread(m_Thread);
	}

	//串口句柄无效  add by itas109 2016-07-29
	if(m_hComm == INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hComm);
		m_hComm = NULL;
		return;
	}

	do
	{
		SetEvent(m_hShutdownEvent);
		//add by liquanhai  防止死锁  2011-11-06
		if(::PeekMessage(&message,m_pOwner,0,0,PM_REMOVE))
		{
			::TranslateMessage(&message);
			::DispatchMessage(&message);
		}
	} while (m_bThreadAlive);

	// if the port is still opened: close it 
	if (m_hComm != NULL)
	{
		CloseHandle(m_hComm);
		m_hComm = NULL;
	}

	// Close Handles  
	if(m_hShutdownEvent!=NULL)
	{
		ResetEvent(m_hShutdownEvent);
	}
	if(m_ov.hEvent!=NULL)
	{
		ResetEvent(m_ov.hEvent);
	}
	if(m_hWriteEvent!=NULL)
	{
		ResetEvent(m_hWriteEvent);
		//CloseHandle(m_hWriteEvent);//开发者反映，这里会导致多个串口工作时，重新打开串口异常
	}

	if(m_szWriteBuffer != NULL)
	{
		delete [] m_szWriteBuffer;
		m_szWriteBuffer = NULL;
	}
}



void CSerialPort::WriteToPort(BYTE* Buffer, int n)
{
	assert(m_hComm != 0);
	memset(m_szWriteBuffer, 0, sizeof(m_szWriteBuffer));
	int i;
	for(i=0; i<n; i++)
		m_szWriteBuffer[i] = Buffer[i];
	m_nWriteSize=n;
	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
	WriteFile(m_hComm,m_szWriteBuffer,m_nWriteSize,(LPDWORD)&m_nWriteSize,NULL);
}


BOOL CSerialPort::ReceiveData(BYTE* RXBuff,DWORD* BytesRead)
{
	BOOL bResult;
	///串口读出，读出缓冲区中字节
	bResult = ReadFile(m_hComm,		// Handle to COMM port 
		RXBuff,				// RX Buffer Pointer
		32,					// Read one byte
		BytesRead,			// Stores number of bytes read
		&m_ov);		// pointer to the m_ov structure
	return bResult;
}
int CSerialPort::GetWriteSize()
{
	return m_nWriteSize;
}