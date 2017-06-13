/*
**	FILENAME			CSerialPort.cpp
**
*/

#include "stdafx.h"
#include "SerialPort.h"

#include <assert.h>

int m_nComArray[20];//��Ż�Ծ�Ĵ��ں�
//
// Constructor
//
CSerialPort::CSerialPort()
{
	m_hComm = NULL;

	// initialize overlapped structure members to zero
	///��ʼ���첽�ṹ��
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

	//�����̹߳����жϣ���������̹߳����´��ڹر����������� add by itas109 2016-06-29
	if(IsThreadSuspend(m_Thread))
	{
		ResumeThread(m_Thread);
	}

	//���ھ����Ч  add by itas109 2016-07-29
	if(m_hComm == INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hComm);
		m_hComm = NULL;
		return;
	}

	//do
	//{
	//	SetEvent(m_hShutdownEvent);
	//	//add by liquanhai  ��ֹ����  2011-11-06
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
///��ʼ�����ڡ�ֻ����1-MaxSerialPortNum
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

	//�����̹߳����жϣ���������̹߳����´��ڹر����������� add by itas109 2016-06-29
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
	//		//add by liquanhai  ��ֹ����  2011-11-06
	//		if(::PeekMessage(&message,m_pOwner,0,0,PM_REMOVE))
	//		{
	//			::TranslateMessage(&message);
	//			::DispatchMessage(&message);
	//		}
	//	} while (m_bThreadAlive);
	//	//TRACE("Thread ended\n");
	//	//�˴�����ʱ����Ҫ����Ϊ������ڿ��ţ����͹ر�ָ����׹ر���Ҫһ����ʱ�䣬�����ʱӦ�ø����Ե��������
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
	///�¼������ʼ�����趨���ȼ���
	m_hEventArray[0] = m_hShutdownEvent;	// highest priority
	//Ϊ������Щ�����豸���������룬��һֱ���ض��¼���ʹ�����߳�������
	//���Խ���д���������߳��У������޸Ķ�д�¼����ȼ�
	//�޸����ȼ�������������
	//����һΪ�����߳���WaitCommEvent()���������������䣺
	//if (WAIT_OBJECT_O == WaitForSingleObject(port->m_hWriteEvent, 0))
	//	ResetEvent(port->m_ov.hEvent);
	//������Ϊ��ʼ��ʱ���޸ģ�������������䣺
	m_hEventArray[1] = m_hWriteEvent;
	m_hEventArray[2] = m_ov.hEvent;


	// initialize critical section
	///��ʼ���ٽ���Դ
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
	����̲߳�����ͬ������ʱ��һ������Ҫ��˳����ʵģ�������������ݴ��ң�
	�޷��������ݣ�������������Ϊ���������⣬����Ҫ���뻥���������ÿ
	���̶߳���˳��ط��ʱ�������������Ҫʹ��EnterCriticalSection��
	LeaveCriticalSection������
	*/
	// now it critical!
	EnterCriticalSection(&m_csCommunicationSync);

	// if the port is already opened: close it
	///�����Ѵ򿪾͹ص�
	if (m_hComm != NULL)
	{
		CloseHandle(m_hComm);
		m_hComm = NULL;
	}

	// prepare port strings
	sprintf_s(szPort,50, "\\\\.\\COM%d", portnr);///������ʾCOM10���϶˿�//add by itas109 2014-01-09


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
		//����Ĭ���������Ϊstopbits=1.5ʱ��SetCommState�ᱨ��
		//һ��ĵ��Դ��ڲ�֧��1.5ֹͣλ�����1.5ֹͣλ�ƺ����ں��⴫���ϵġ�
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
		//����Ĭ�������
		//by itas109 20160506
	default:
		myparity = 0;
		break;
	}
	sprintf_s(szBaud,50, "baud=%d parity=%c data=%d stop=%d", baud, parity, databits, mystop);

	// get a handle to the port
	/*
	ͨ�ų�����CreateFile��ָ�������豸����صĲ������ԣ��ٷ���һ�������
	�þ���������ں�����ͨ�Ų��������ᴩ����ͨ�Ź��̴��ڴ򿪺�������
	������ΪĬ��ֵ�����ݾ�����Ҫ��ͨ������GetCommState(hComm,&&dcb)��ȡ
	��ǰ�����豸���ƿ�DCB���ã��޸ĺ�ͨ��SetCommState(hComm,&&dcb)����д
	�롣����ReadFile()��WriteFile()������API����ʵ�ִ��ڶ�д��������Ϊ��
	��ͨ�ŷ�ʽ�������������һ������Ϊָ��OVERLAPPED�ṹ�ķǿ�ָ�룬�ڶ�
	д��������ֵΪFALSE������£�����GetLastError()����������ֵΪERROR_IO_PENDING��
	����I/O�������ң�������ת���̨����ִ�С���ʱ��������WaitForSingleObject()
	���ȴ������źŲ�������ȴ�ʱ��
	*/
	m_hComm = CreateFile(LPCSTR(szPort),GENERIC_READ|GENERIC_WRITE, 0, NULL,OPEN_EXISTING, 0, NULL);
	///����ʧ��
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		//add by itas109 2016-08-02
		//���ڴ�ʧ�ܣ�������ʾ��Ϣ
		switch(GetLastError())
		{
			//���ڲ�����
		case ERROR_FILE_NOT_FOUND:
			{
				CString str;
				str.Format(_T("COM%d ���ڲ�����,Error Code:%d"),portnr,GetLastError());
				AfxMessageBox(str);
				break;
			}
			//���ھܾ�����
		case ERROR_ACCESS_DENIED:
			{
				CString str;
				str.Format(_T("COM%d ���ھܾ�����,Error Code:%d"),portnr,GetLastError());
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
	///���ó�ʱ

	m_CommTimeouts.ReadIntervalTimeout=MAXDWORD;
	m_CommTimeouts.ReadTotalTimeoutMultiplier=0;
	m_CommTimeouts.ReadTotalTimeoutConstant=50;
	m_CommTimeouts.WriteTotalTimeoutMultiplier=50;
	m_CommTimeouts.WriteTotalTimeoutConstant=2000;

	// configure
	///����
	///�ֱ����Windows API���ô��ڲ���
	if (SetCommTimeouts(m_hComm, &m_CommTimeouts))///���ó�ʱ
	{						   
		/*
		���Զ˿����ݵ���Ӧʱ��Ҫ����ϸ񣬿ɲ����¼�������ʽ��
		�¼�������ʽͨ�������¼�֪ͨ������ϣ�����¼�����ʱ��Windows
		�������¼��ѷ�����֪ͨ������DOS�����µ��жϷ�ʽ�����ơ�Windows
		������9�ִ���ͨ���¼����ϳ��õ����������֣�
		EV_RXCHAR:���յ�һ���ֽڣ����������뻺������
		EV_TXEMPTY:����������е����һ���ַ������ͳ�ȥ��
		EV_RXFLAG:���յ��¼��ַ�(DCB�ṹ��EvtChar��Ա)���������뻺����
		����SetCommMask()ָ�������õ��¼���Ӧ�ó���ɵ���WaitCommEvent()���ȴ���
		���ķ�����SetCommMask(hComm,0)��ʹWaitCommEvent()��ֹ
		*/
		if (SetCommMask(m_hComm, dwCommEvents))///����ͨ���¼�
		{

			if (GetCommState(m_hComm, &m_dcb))///��ȡ��ǰDCB����
			{
				m_dcb.EvtChar = 'q';
				m_dcb.fRtsControl = RTS_CONTROL_ENABLE;		// set RTS bit high!
				m_dcb.BaudRate = baud;  // add by mrlong
				m_dcb.Parity   = myparity;
				m_dcb.ByteSize = databits;
				m_dcb.StopBits = mystop;

				//if (BuildCommDCB(szBaud &m_dcb))///��дDCB�ṹ
				//{
				if (SetCommState(m_hComm, &m_dcb))///����DCB
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
	///��ֹ��д����ս��պͷ���
	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	// release critical section
	///�ͷ��ٽ���Դ
	LeaveCriticalSection(&m_csCommunicationSync);

	//TRACE("Initialisation for communicationport %d completed.\nUse Startmonitor to communicate.\n", portnr);

	return TRUE;
}

//
//  The CommThread Function.
///�̺߳���
///�����̵߳Ĵ������̣�
///��鴮��-->����ѭ��{WaitCommEvent(������ѯ��)ѯ���¼�-->������¼�����-->����Ӧ����(�ر�\��\д)}
//
DWORD WINAPI CSerialPort::CommThread(LPVOID pParam)
{
	// Cast the void pointer passed to the thread back to
	// a pointer of CSerialPort class
	CSerialPort *port = (CSerialPort*)pParam;

	// Set the status variable in the dialog class to
	// TRUE to indicate the thread is running.
	///TRUE��ʾ�߳���������
	port->m_bThreadAlive = TRUE;	

	// Misc. variables
	DWORD BytesTransfered = 0; 
	DWORD Event = 0;
	DWORD CommEvent = 0;
	DWORD dwError = 0;
//	COMSTAT comstat;

	BOOL  bResult = TRUE;

	// Clear comm buffers at startup
	///��ʼʱ������ڻ���
	if (port->m_hComm)		// check if the port is opened
		PurgeComm(port->m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	// begin forever loop.  This loop will run as long as the thread is alive.
	///ֻҪ�̴߳��ھͲ��϶�ȡ����
	for (;;) 
	{ 

		ReceiveChar(port);//���ַ�����

	} // close forever loop

	return 0;
}

//
// start comm watching
///���������߳�
//
BOOL CSerialPort::StartMonitoring()
{
	//if (!(m_Thread = ::CreateThread (NULL, 0, CommThread, this, 0, NULL )))
	//	return FALSE;
	return TRUE;
}

//
// Restart the comm thread
///�ӹ���ָ������߳�
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
///��������߳�
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
///����д��󣬸�����ʾ
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

			///���ڶ������������������ֽ�
			bResult = ReadFile(port->m_hComm,		// Handle to COMM port 
				&RXBuff,				// RX Buffer Pointer
				1,					// Read one byte
				&BytesRead,			// Stores number of bytes read
				&port->m_ov);		// pointer to the m_ov structure
			// deal with the error code 
			///�����ش��󣬴�����
			if (!bResult)  
			{ 
				switch (dwError = GetLastError()) 
				{ 
				case ERROR_IO_PENDING: 	
					{ 
						// asynchronous i/o is still in progress 
						// Proceed on to GetOverlappedResults();
						///�첽IO���ڽ���
						bRead = FALSE;
						break;
					}
				case ERROR_ACCESS_DENIED:///�ܾ����� erroe code:5
					{
						port->m_hComm = INVALID_HANDLE_VALUE;
						CString str;
						str.Format(_T("COM%d ERROR_ACCESS_DENIED��ReadFile() Error Code:%d"),port->m_nPortNr,GetLastError());
						AfxMessageBox(str);
						break;
					}
				case ERROR_INVALID_HANDLE:///�򿪴���ʧ�� erroe code:6
					{
						port->m_hComm = INVALID_HANDLE_VALUE;
						break;
					}
				case ERROR_BAD_COMMAND:///���ӹ����зǷ��Ͽ� erroe code:22
					{
						port->m_hComm = INVALID_HANDLE_VALUE;
						CString str;
						str.Format(_T("COM%d ERROR_BAD_COMMAND��ReadFile() Error Code:%d"),port->m_nPortNr,GetLastError());
						AfxMessageBox(str);
						break;
					}
				default:
					{
						// Another error has occured.  Process this error.
						port->ProcessErrorMessage(_T("ReadFile()"));
						break;
						//return;///��ֹ��д����ʱ�����ڷ������Ͽ�������ѭ��һֱִ�С�add by itas109 2014-01-09 ������liquanhai��ӷ������Ĵ�����
					} 
				}
			}
			else///ReadFile����TRUE
			{
				// ReadFile() returned complete. It is not necessary to call GetOverlappedResults()
				bRead = TRUE;
				if(BytesRead>0)
				SendMessage(port->m_pOwner, WM_COMM_RXCHAR, (WPARAM)RXBuff, (LPARAM) port->m_nPortNr);
			}
		}  // close if (bRead)


		// notify parent that a byte was received
		//�����̻߳���ȴ�������������ʹ��PostMessage()����SendMessage()

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
	return m_hComm != NULL && m_hComm!= INVALID_HANDLE_VALUE;//m_hComm����INVALID_HANDLE_VALUE����� add by itas109 2016-07-29
}

void CSerialPort::ClosePort()
{
	MSG message;

	//�����̹߳����жϣ���������̹߳����´��ڹر����������� add by itas109 2016-06-29
	if(IsThreadSuspend(m_Thread))
	{
		ResumeThread(m_Thread);
	}

	//���ھ����Ч  add by itas109 2016-07-29
	if(m_hComm == INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hComm);
		m_hComm = NULL;
		return;
	}

	do
	{
		SetEvent(m_hShutdownEvent);
		//add by liquanhai  ��ֹ����  2011-11-06
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
		//CloseHandle(m_hWriteEvent);//�����߷�ӳ������ᵼ�¶�����ڹ���ʱ�����´򿪴����쳣
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
	///���ڶ������������������ֽ�
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