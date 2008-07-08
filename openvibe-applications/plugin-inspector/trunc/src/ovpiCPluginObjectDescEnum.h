#ifndef __OpenViBEPluginInspector_CPluginObjectDescEnum_H__
#define __OpenViBEPluginInspector_CPluginObjectDescEnum_H__

#include "ovpi_base.h"

class CPluginObjectDescEnum
{
public:

	CPluginObjectDescEnum(const OpenViBE::Kernel::IKernelContext& rKernelContext);
	virtual ~CPluginObjectDescEnum(void);

	virtual OpenViBE::boolean enumeratePluginObjectDesc(void);
	virtual OpenViBE::boolean enumeratePluginObjectDesc(
		const OpenViBE::CIdentifier& rParentClassIdentifier);

	virtual OpenViBE::boolean callback(
		const OpenViBE::Plugins::IPluginObjectDesc& rPluginObjectDesc)=0;

	static OpenViBE::CString transform(const OpenViBE::CString& sInput);

protected:

	const OpenViBE::Kernel::IKernelContext& m_rKernelContext;
};

#endif // __OpenViBEPluginInspector_CPluginObjectDescEnum_H__
