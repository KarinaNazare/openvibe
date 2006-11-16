#ifndef __OpenViBEKernel_Kernel_CPluginManager_H__
#define __OpenViBEKernel_Kernel_CPluginManager_H__

#include "ovkTKernelObject.h"

#include <vector>
#include <map>

namespace OpenViBE
{
	namespace Kernel
	{
		class CPluginManager : virtual public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IPluginManager>
		{
		public:

			CPluginManager(const OpenViBE::Kernel::IKernelContext& rKernelContext);

			virtual OpenViBE::boolean addPluginsFromFiles(
				const OpenViBE::CString& rFileNameWildCard);

			virtual OpenViBE::boolean enumeratePluginObjectDesc(
				OpenViBE::Kernel::IPluginManager::IPluginObjectDescEnum& rCallback) const;
			virtual OpenViBE::boolean enumeratePluginObjectDesc(
				OpenViBE::Kernel::IPluginManager::IPluginObjectDescEnum& rCallback,
				const OpenViBE::CIdentifier& rBaseClassIdentifier) const;

			virtual const OpenViBE::Plugins::IPluginObjectDesc* getPluginObjectDescCreating(
				const OpenViBE::CIdentifier& rClassIdentifier) const;
			virtual OpenViBE::Plugins::IPluginObject* createPluginObject(
				const OpenViBE::CIdentifier& rClassIdentifier);
			virtual OpenViBE::boolean releasePluginObject(
				OpenViBE::Plugins::IPluginObject* pPluginObject);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::IPluginManager, OVK_ClassId_Kernel_PluginManager)

		protected:

			std::vector<OpenViBE::Kernel::IPluginModule*> m_vPluginModule;
			std::map<OpenViBE::Kernel::IPluginModule*, std::vector<OpenViBE::Plugins::IPluginObjectDesc*> > m_vPluginObjectDesc;
			std::map<OpenViBE::Kernel::IPluginModule*, std::vector<OpenViBE::Plugins::IPluginObject*> > m_vPluginObject;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_CPluginManager_H__
