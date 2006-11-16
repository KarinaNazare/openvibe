#ifndef __OpenViBEKernel_Plugins_CBoxAlgorithmContext_H__
#define __OpenViBEKernel_Plugins_CBoxAlgorithmContext_H__

#include "ovkTPluginObjectContext.h"
#include "ovkCStaticBoxContext.h"
#include "ovkCDynamicBoxContext.h"
#include "ovkCPlayerContext.h"
#include "simulated-objects/ovkPsSimulatedBox.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class CBoxAlgorithmContext : virtual public OpenViBE::Kernel::TPluginObjectContext<OpenViBE::Plugins::IBoxAlgorithmContext>
		{
		public:

			CBoxAlgorithmContext(::PsSimulatedBox* pSimulatedBox, const OpenViBE::Kernel::IBox* pBox);

			virtual OpenViBE::Plugins::IStaticBoxContext* getStaticBoxContext(void);
			virtual const OpenViBE::Plugins::IStaticBoxContext* getStaticBoxContext(void) const;
			virtual OpenViBE::Plugins::IDynamicBoxContext* getDynamicBoxContext(void);
			virtual const OpenViBE::Plugins::IDynamicBoxContext* getDynamicBoxContext(void) const;
			virtual OpenViBE::Plugins::IPlayerContext* getPlayerContext(void);
			virtual const OpenViBE::Plugins::IPlayerContext* getPlayerContext(void) const;
			virtual OpenViBE::boolean markAlgorithmAsReadyToProcess(void);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TPluginObjectContext<OpenViBE::Plugins::IBoxAlgorithmContext>, OVK_ClassId_Kernel_BoxAlgorithmContext)

			virtual OpenViBE::boolean isAlgorithmReadyToProcess(void);

		protected:

			OpenViBE::Kernel::CStaticBoxContext m_oStaticBoxContext;
			OpenViBE::Kernel::CDynamicBoxContext m_oDynamicBoxContext;
			OpenViBE::Kernel::CPlayerContext m_oPlayerContext;
			OpenViBE::boolean m_bReadyToProcess;
		};
	};
};

#endif // __OpenViBEKernel_Plugins_CBoxAlgorithmContext_H__
