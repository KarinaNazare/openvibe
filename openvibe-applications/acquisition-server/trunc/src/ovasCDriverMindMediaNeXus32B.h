#ifndef __OpenViBE_AcquisitionServer_CDriverMindMediaNeXus32B_H__
#define __OpenViBE_AcquisitionServer_CDriverMindMediaNeXus32B_H__

#include "ovasIDriver.h"

namespace OpenViBEAcquisitionServer
{
	class CDriverMindMediaNeXus32B : virtual public OpenViBEAcquisitionServer::IDriver
	{
	public:

		CDriverMindMediaNeXus32B(void);
		virtual ~CDriverMindMediaNeXus32B(void);
		virtual void release(void);
		virtual const char* getName(void);

		virtual OpenViBEAcquisitionServer::boolean initialize(
			const OpenViBEAcquisitionServer::uint32 ui32SampleCountPerSentBlock,
			OpenViBEAcquisitionServer::IDriverCallback& rCallback);
		virtual OpenViBEAcquisitionServer::boolean uninitialize(void);

		virtual OpenViBEAcquisitionServer::boolean start(void);
		virtual OpenViBEAcquisitionServer::boolean stop(void);
		virtual OpenViBEAcquisitionServer::boolean loop(void);

		virtual OpenViBEAcquisitionServer::boolean isConfigurable(void);
		virtual OpenViBEAcquisitionServer::boolean configure(void);
		virtual const OpenViBEAcquisitionServer::IHeader* getHeader(void) { return m_pHeader; }

	protected:

		OpenViBEAcquisitionServer::IDriverCallback* m_pCallback;
		OpenViBEAcquisitionServer::IHeader* m_pHeader;

		OpenViBEAcquisitionServer::boolean m_bInitialized;
		OpenViBEAcquisitionServer::boolean m_bStarted;
		OpenViBEAcquisitionServer::uint32 m_ui32SampleCountPerSentBlock;
		OpenViBEAcquisitionServer::float32* m_pSample;

		OpenViBEAcquisitionServer::uint32 m_ui32SampleIndex;
	};
};

#endif // __OpenViBE_AcquisitionServer_CDriverMindMediaNeXus32B_H__
