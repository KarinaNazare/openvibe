#include "ovasCDriverOpenEEGModularEEG.h"
#include "ovasCConfigurationOpenEEGModularEEG.h"

#include <openvibe-toolkit/ovtk_all.h>

#include <system/Time.h>
#include <system/Memory.h>
#include <cmath>
#include <iostream>

#if defined OVAS_OS_Windows
 #include <windows.h>
 #include <winbase.h>
 #include <cstdio>
 #include <cstdlib>
 #include <commctrl.h>
 #define TERM_SPEED 57600
#elif defined OVAS_OS_Linux
 #include <cstdio>
 #include <unistd.h>
 #include <fcntl.h>
 #include <termios.h>
 #include <sys/select.h>
 #define TERM_SPEED B57600
#else
#endif

#define boolean OpenViBE::boolean
using namespace OpenViBEAcquisitionServer;
using namespace OpenViBE;

//___________________________________________________________________//
//                                                                   //

CDriverOpenEEGModularEEG::CDriverOpenEEGModularEEG(IDriverContext& rDriverContext)
	:IDriver(rDriverContext)
	,m_pCallback(NULL)
	,m_bInitialized(false)
	,m_bStarted(false)
	,m_ui32SampleCountPerSentBlock(0)
	,m_ui32DeviceIdentifier(uint32(-1))
	,m_pSample(NULL)
{
	m_oHeader.setSamplingFrequency(256);
	m_oHeader.setChannelCount(6);
}

void CDriverOpenEEGModularEEG::release(void)
{
	delete this;
}

const char* CDriverOpenEEGModularEEG::getName(void)
{
	return "OpenEEG Modular EEG P2";
}

//___________________________________________________________________//
//                                                                   //

boolean CDriverOpenEEGModularEEG::initialize(
	const uint32 ui32SampleCountPerSentBlock,
	IDriverCallback& rCallback)
{
	if(m_bInitialized)
	{
		return false;
	}

	if(!m_oHeader.isChannelCountSet()
	 ||!m_oHeader.isSamplingFrequencySet())
	{
		return false;
	}

	m_ui16Readstate=0;
	m_ui16ExtractPosition=0;

	if(!this->initTTY(&m_i32FileDescriptor, m_ui32DeviceIdentifier!=uint32(-1)?m_ui32DeviceIdentifier:1))
	{
		return false;
	}

	m_pSample=new float32[m_oHeader.getChannelCount()*ui32SampleCountPerSentBlock];
	m_pChannelBuffer=new int32[m_oHeader.getChannelCount()];
	if(!m_pSample || !m_pChannelBuffer)
	{
		delete [] m_pSample;
		m_pSample=NULL;
		delete [] m_pChannelBuffer;
		m_pChannelBuffer=NULL;
		return false;
	}
	System::Memory::set(m_pChannelBuffer, 0, m_oHeader.getChannelCount()*sizeof(int32));

	m_pCallback=&rCallback;
	m_bInitialized=true;
	m_ui32SampleCountPerSentBlock=ui32SampleCountPerSentBlock;
	m_ui8LastPacketNumber=0;

	// printf("ModulerEEG Device Driver initialized.\n");
	return true;
}

boolean CDriverOpenEEGModularEEG::start(void)
{
	if(!m_bInitialized)
	{
		return false;
	}

	if(m_bStarted)
	{
		return false;
	}

	// printf("ModulerEEG Device Driver started.\n");
	m_bStarted=true;
	return m_bStarted;
}

boolean CDriverOpenEEGModularEEG::loop(void)
{
	if(!m_bInitialized)
	{
		return false;
	}

	if(!m_bStarted)
	{
		return false;
	}

	uint32 l_i32ReceivedSamples=0;
	uint32 l_ui32StartTime=System::Time::getTime();
	while(l_i32ReceivedSamples < m_ui32SampleCountPerSentBlock && System::Time::getTime()-l_ui32StartTime < 1000)
	{
		switch(this->readPacketFromTTY(m_i32FileDescriptor))
		{
			case 0: // No Packet finished
				break;

			case 1: // Packet with Samples arrived
				// m_ui8LastPacketNumber++;
				m_ui8LastPacketNumber=m_ui8PacketNumber;
				for(uint32 j=0; j<m_oHeader.getChannelCount(); j++)
				{
					m_pSample[j*m_ui32SampleCountPerSentBlock+l_i32ReceivedSamples]=(float32)m_pChannelBuffer[j]-512.f;
				}
				l_i32ReceivedSamples++;
				break;

			case -1: // Timeout, could inidicate communication error
				return false;
		}
	}

	if(l_i32ReceivedSamples!=m_ui32SampleCountPerSentBlock)
	{
		// Timeout
		return false;
	}

	m_pCallback->setSamples(m_pSample);
	return true;
}

boolean CDriverOpenEEGModularEEG::stop(void)
{
	if(!m_bInitialized)
	{
		return false;
	}

	if(!m_bStarted)
	{
		return false;
	}

	// printf("ModulerEEG Device Driver stopped.\n");
	m_bStarted=false;
	return !m_bStarted;
}

boolean CDriverOpenEEGModularEEG::uninitialize(void)
{
	if(!m_bInitialized)
	{
		return false;
	}

	if(m_bStarted)
	{
		return false;
	}

	m_bInitialized=false;

	this->closeTTY(m_i32FileDescriptor);
	// printf("ModulerEEG Device Driver closed.\n");

	delete [] m_pSample;
	m_pSample=NULL;
	m_pCallback=NULL;

	return true;
}

//___________________________________________________________________//
//                                                                   //

boolean CDriverOpenEEGModularEEG::isConfigurable(void)
{
	return true;
}

boolean CDriverOpenEEGModularEEG::configure(void)
{
	CConfigurationOpenEEGModularEEG m_oConfiguration("../share/openvibe-applications/acquisition-server/interface-OpenEEG-ModularEEG.glade", m_ui32DeviceIdentifier);
	return m_oConfiguration.configure(m_oHeader);
}

//___________________________________________________________________//
//                                                                   //

/*
void CDriverOpenEEGModularEEG::logPacket(void)
{
	printf("\nPacket %d received:\n", m_ui8PacketNumber);
	for(uint32 i=0; i<m_oHeader.getChannelCount(); i++)
	{
		printf("Channel %d:%d\n", i+1, m_pChannelBuffer[i]);
	}
}
*/

boolean CDriverOpenEEGModularEEG::parseByteP2(uint8 ui8Actbyte)
{
	uint32 m_ui32ChannelCount=m_oHeader.getChannelCount();
	switch(m_ui16Readstate)
	{
		case 0:
			if(ui8Actbyte==165)
			{
				m_ui16Readstate++;
			}
			break;

		case 1:
			if(ui8Actbyte==90)
			{
				m_ui16Readstate++;
			}
			else
			{
				m_ui16Readstate=0;
			}
			break;

		case 2:
			m_ui16Readstate++;
			break;

		case 3:
			m_ui8PacketNumber=ui8Actbyte;
			m_ui16ExtractPosition=0;
			m_ui16Readstate++;
			break;

		case 4:
			if(m_ui16ExtractPosition < 12)
			{
				if((m_ui16ExtractPosition & 1) == 0)
				{
					if((uint32)(m_ui16ExtractPosition>>1)<m_ui32ChannelCount)
					{
						m_pChannelBuffer[m_ui16ExtractPosition>>1]= ((int32)ui8Actbyte)<<8;
					}
				}
				else
				{
					if((uint32)(m_ui16ExtractPosition>>1)<m_ui32ChannelCount)
					{
						m_pChannelBuffer[m_ui16ExtractPosition>>1]+=ui8Actbyte;
					}
				}
				m_ui16ExtractPosition++;
			}
			else
			{
				m_ui16Switches=ui8Actbyte;
				m_ui16Readstate=0;
				return true;
			}
			break;

		default:
			m_ui16Readstate=0;
			break;
	}
	return false;
}

boolean CDriverOpenEEGModularEEG::initTTY(::FD_TYPE* pFileDescriptor, uint32 ui32TTYNumber)
{
	char l_sTTYName[1024];

#if defined OVAS_OS_Windows

	sprintf(l_sTTYName, "\\\\.\\COM%d", ui32TTYNumber);
	DCB dcb = {0};
	*pFileDescriptor=::CreateFile(
		(LPCSTR)l_sTTYName,
		GENERIC_READ|GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0);

	if(*pFileDescriptor == INVALID_HANDLE_VALUE)
	{
		printf("Could not open Communication Port %s for ModulerEEG Driver.\n", l_sTTYName);
		return false;
	}

	if(!::GetCommState(*pFileDescriptor, &dcb))
	{
		return false;
	}

	// update DCB rate, byte size, parity, and stop bits size
	dcb.DCBlength = sizeof(dcb);
	dcb.BaudRate  = CBR_56000;
	dcb.ByteSize  = 8;
	dcb.Parity    = NOPARITY;
	dcb.StopBits  = ONESTOPBIT;
	dcb.EvtChar   = '\0';

	// update flow control settings
	dcb.fDtrControl       = DTR_CONTROL_ENABLE;
	dcb.fRtsControl       = RTS_CONTROL_ENABLE;
	dcb.fOutxCtsFlow      = FALSE;
	dcb.fOutxDsrFlow      = FALSE;
	dcb.fDsrSensitivity   = FALSE;;
	dcb.fOutX             = FALSE;
	dcb.fInX              = FALSE;
	dcb.fTXContinueOnXoff = FALSE;
	dcb.XonChar           = 0;
	dcb.XoffChar          = 0;
	dcb.XonLim            = 0;
	dcb.XoffLim           = 0;
	dcb.fParity           = FALSE;

	::SetCommState(*pFileDescriptor, &dcb);
	::SetupComm(*pFileDescriptor, 1024, 1024);
	::EscapeCommFunction(*pFileDescriptor, SETDTR);
	::SetCommMask (*pFileDescriptor, EV_RXCHAR | EV_CTS | EV_DSR | EV_RLSD | EV_RING);

#elif defined OVAS_OS_Linux

	struct termios l_oTerminalAttributes;

	// open ttyS<i> for i < 10, else open ttyUSB<i-10>
	if(ui32TTYNumber<10)
	{
		sprintf(l_sTTYName, "/dev/ttyS%d", ui32TTYNumber);
	}
	else
	{
		sprintf(l_sTTYName, "/dev/ttyUSB%d", ui32TTYNumber-10);
	}

	if((*pFileDescriptor=::open(l_sTTYName, O_RDWR))==-1)
	{
		printf("Could not open Communication Port %s for ModulerEEG Driver.\n", l_sTTYName);
		return false;
	}

	if(tcgetattr(*pFileDescriptor, &l_oTerminalAttributes)!=0)
	{
		::close(*pFileDescriptor);
		*pFileDescriptor=-1;
		printf("terminal: tcgetattr() failed\n");
		return false;
	}

	/* l_oTerminalAttributes.c_cflag = TERM_SPEED | CS8 | CRTSCTS | CLOCAL | CREAD; */
	l_oTerminalAttributes.c_cflag = TERM_SPEED | CS8 | CLOCAL | CREAD;
	l_oTerminalAttributes.c_iflag = 0;
	l_oTerminalAttributes.c_oflag = OPOST | ONLCR;
	l_oTerminalAttributes.c_lflag = 0;
	if(::tcsetattr(*pFileDescriptor, TCSAFLUSH, &l_oTerminalAttributes)!=0)
	{
		::close(*pFileDescriptor);
		*pFileDescriptor=-1;
		printf("terminal: tcsetattr() failed\n");
		return false;
	}

#else

	return false;

#endif

	return true;
 }

void CDriverOpenEEGModularEEG::closeTTY(::FD_TYPE i32FileDescriptor)
{
#if defined OVAS_OS_Windows
	::CloseHandle(i32FileDescriptor);
#elif defined OVAS_OS_Linux
	::close(i32FileDescriptor);
#else
#endif
}

int32 CDriverOpenEEGModularEEG::readPacketFromTTY(::FD_TYPE i32FileDescriptor)
{
	uint8  l_ui8ReadBuffer[100];
	uint32 l_ui32BytesProcessed=0;
	int32  l_i32PacketsProcessed=0;

#if defined OVAS_OS_Windows

	uint32 l_ui32ReadLength=0;
	uint32 l_ui32ReadOk=0;
	struct _COMSTAT l_oStatus;
	::DWORD l_dwState;

	if(::ClearCommError(i32FileDescriptor, &l_dwState, &l_oStatus))
	{
		l_ui32ReadLength=l_oStatus.cbInQue;
	}

	for(l_ui32BytesProcessed=0; l_ui32BytesProcessed<l_ui32ReadLength; l_ui32BytesProcessed++)
	{
		// Read the data from the serial port.
		::ReadFile(i32FileDescriptor, l_ui8ReadBuffer, 1, (LPDWORD)&l_ui32ReadOk, 0);
		if(l_ui32ReadOk==1)
		{
			if(this->parseByteP2(l_ui8ReadBuffer[0]))
			{
				l_i32PacketsProcessed++;
			}
		}
	}

#elif defined OVAS_OS_Linux

	fd_set  l_inputFileDescriptorSet;
	struct timeval l_timeout;
	ssize_t l_ui32ReadLength=0;

	l_timeout.tv_sec=1;
	l_timeout.tv_usec=0;

	FD_ZERO(&l_inputFileDescriptorSet);
	FD_SET(i32FileDescriptor, &l_inputFileDescriptorSet);

	switch(::select(i32FileDescriptor+1, &l_inputFileDescriptorSet, NULL, NULL, &l_timeout))
	{
		case -1: // error or timeout
		case  0:
			return (-1);

		default:
			if(FD_ISSET(i32FileDescriptor, &l_inputFileDescriptorSet))
			{
				if((l_ui32ReadLength=::read(i32FileDescriptor, l_ui8ReadBuffer, 1)) > 0)
				{
					for(l_ui32BytesProcessed=0; l_ui32BytesProcessed<l_ui32ReadLength; l_ui32BytesProcessed++)
					{
						if(this->parseByteP2(l_ui8ReadBuffer[l_ui32BytesProcessed]))
						{
							l_i32PacketsProcessed++;
						}
					}
				}
			}
			break;
	}

#else

#endif

	return l_i32PacketsProcessed;
 }

