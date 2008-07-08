#include "ovkCPlayer.h"
#include "ovkCSimulatedBox.h"
#include "ovkCScheduler.h"

#include <system/Time.h>

#include <xml/IReader.h>

#include <string>
#include <iostream>
#include <fstream>

//___________________________________________________________________//
//                                                                   //

using namespace std;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

#define _Scheduler_Frequency_ 128
#define _Scheduler_Maximum_Loops_Duration_ (100LL << 22) /* 100/1024 second = approx 100ms */

//___________________________________________________________________//
//                                                                   //

CPlayer::CPlayer(const IKernelContext& rKernelContext)
	:TKernelObject<IPlayer>(rKernelContext)
	,m_oScheduler(rKernelContext)
	,m_ui64CurrentTimeToReach(0)
	,m_ui64Lateness(0)
	,m_eStatus(PlayerStatus_Stop)
	,m_bIsInitialized(false)
{
	m_oScheduler.setFrequency(_Scheduler_Frequency_);
}

CPlayer::~CPlayer(void)
{
	if(m_bIsInitialized)
	{
		this->uninitialize();
	}
}

//___________________________________________________________________//
//                                                                   //

boolean CPlayer::setScenario(
	const CIdentifier& rScenarioIdentifier)
{
	log() << LogLevel_Trace << "Player setScenario\n";

	if(m_bIsInitialized)
	{
		log() << LogLevel_Warning << "Trying to configure an initialized player !\n";
		return false;
	}

	return m_oScheduler.setScenario(rScenarioIdentifier);;
}

boolean CPlayer::initialize(void)
{
	log() << LogLevel_Trace << "Player initialize\n";

	if(m_bIsInitialized)
	{
		log() << LogLevel_Warning << "Trying to initialize an initialized player !\n";
		return false;
	}

	m_oScheduler.initialize();
	m_oBenchmarkChrono.reset(_Scheduler_Frequency_);

	m_ui64CurrentTimeToReach=0;
	m_ui64Lateness=0;
	m_eStatus=PlayerStatus_Stop;
	m_bIsInitialized=true;

	return true;

}

boolean CPlayer::uninitialize(void)
{
	log() << LogLevel_Trace << "Player uninitialize\n";

	if(!m_bIsInitialized)
	{
		log() << LogLevel_Warning << "Trying to uninitialize an uninitialized player !\n";
		return false;
	}

	m_oScheduler.uninitialize();

	m_bIsInitialized=false;
	return true;
}

boolean CPlayer::stop(void)
{
	log() << LogLevel_Trace << "Player stop\n";

	if(m_bIsInitialized)
	{
		this->uninitialize();
	}
	m_eStatus=PlayerStatus_Stop;

	return true;
}

boolean CPlayer::pause(void)
{
	log() << LogLevel_Trace << "Player pause\n";

	if(!m_bIsInitialized)
	{
		this->initialize();
	}
	m_eStatus=PlayerStatus_Pause;

	return true;
}

boolean CPlayer::step(void)
{
	log() << LogLevel_Trace << "Player step\n";

	if(!m_bIsInitialized)
	{
		this->initialize();
	}
	m_eStatus=PlayerStatus_Step;

	return true;
}

boolean CPlayer::play(void)
{
	log() << LogLevel_Trace << "Player play\n";

	if(!m_bIsInitialized)
	{
		this->initialize();
	}
	m_eStatus=PlayerStatus_Play;

	return true;
}

boolean CPlayer::forward(void)
{
	log() << LogLevel_Trace << "Player forward\n";

	if(!m_bIsInitialized)
	{
		this->initialize();
	}
	m_eStatus=PlayerStatus_Forward;

	return true;
}

EPlayerStatus CPlayer::getStatus(void) const
{
	return m_eStatus;
}

boolean CPlayer::loop(
	const uint64 ui64ElapsedTime)
{
	if(!m_bIsInitialized)
	{
		return false;
	}

	switch(m_eStatus)
	{
		// Calls a single controller loop and goes back to pause state
		case PlayerStatus_Step:
			m_ui64CurrentTimeToReach+=(1LL<<32)/_Scheduler_Frequency_;
			m_eStatus=PlayerStatus_Pause;
			break;

		// Calls multiple controller loops
		case PlayerStatus_Forward:
			// We can't know what m_ui64CurrentTimeToReach should be in advance
			// We will try to do as many scheduler loops as possible until
			// _Scheduler_Maximum_Loops_Duration_ seconds elapsed
			break;

		// Simply updates time according to delta time
		case PlayerStatus_Play:
			m_ui64CurrentTimeToReach+=ui64ElapsedTime;
			break;

		default:
			return true;
			break;
	}

	uint64 l_ui64StartTime=System::Time::zgetTime();
	boolean l_bFinished=false;
	while(!l_bFinished)
	{
		if(m_eStatus!=PlayerStatus_Forward && m_oScheduler.getCurrentTime() > m_ui64CurrentTimeToReach)
		{
			l_bFinished=true;
		}
		else
		{
			m_oScheduler.loop();
		}
		if(System::Time::zgetTime() > l_ui64StartTime+_Scheduler_Maximum_Loops_Duration_)
		{
			l_bFinished=true;
		}
	}

	if(m_eStatus==PlayerStatus_Forward)
	{
		m_ui64CurrentTimeToReach=m_oScheduler.getCurrentTime();
	}

	uint64 l_ui64Lateness;
	if(m_ui64CurrentTimeToReach>m_oScheduler.getCurrentTime())
	{
		l_ui64Lateness=(m_ui64CurrentTimeToReach-m_oScheduler.getCurrentTime())>>32;
	}
	else
	{
		l_ui64Lateness=0;
	}

	if(l_ui64Lateness!=m_ui64Lateness)
	{
		log() << (l_ui64Lateness==0?LogLevel_Info:(l_ui64Lateness>=10?LogLevel_ImportantWarning:LogLevel_Warning))
			<< "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Player" << LogColor_PopStateBit
			<< "::" << LogColor_PushStateBit << LogColor_ForegroundBlue << "can not reach realtime" << LogColor_PopStateBit << "> "
			<< l_ui64Lateness << " second(s) late...\n";
		m_ui64Lateness=l_ui64Lateness;
	}

	return true;
}

uint64 CPlayer::getCurrentSimulatedTime(void) const
{
	return m_oScheduler.getCurrentTime();
}
