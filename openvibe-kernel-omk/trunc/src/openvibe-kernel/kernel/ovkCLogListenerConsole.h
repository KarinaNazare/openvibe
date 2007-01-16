#ifndef __OpenViBEKernel_Kernel_CLogListenerConsole_H__
#define __OpenViBEKernel_Kernel_CLogListenerConsole_H__

#include "ovkTKernelObject.h"

#include <stack>

namespace OpenViBE
{
	namespace Kernel
	{
		class CLogListenerConsole : virtual public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::ILogListener>
		{
		public:

			CLogListenerConsole(const OpenViBE::Kernel::IKernelContext& rKernelContext);

			virtual void log(const OpenViBE::uint64 ui64Value);
			virtual void log(const OpenViBE::uint32 ui32Value);
			virtual void log(const OpenViBE::uint16 ui16Value);
			virtual void log(const OpenViBE::uint8 ui8Value);

			virtual void log(const OpenViBE::int64 i64Value);
			virtual void log(const OpenViBE::int32 i32Value);
			virtual void log(const OpenViBE::int16 i16Value);
			virtual void log(const OpenViBE::int8 i8Value);

			virtual void log(const OpenViBE::float64 f64Value);
			virtual void log(const OpenViBE::float32 f32Value);

			virtual void log(const OpenViBE::CIdentifier& rValue);
			virtual void log(const OpenViBE::CString& rValue);
			virtual void log(const char* pValue);

			virtual void log(const OpenViBE::Kernel::ELogLevel eLogLevel);
			virtual void log(const OpenViBE::Kernel::ELogColor eLogColor);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::ILogListener>, OVK_ClassId_Kernel_LogListenerConsole);

		protected:

			virtual void applyColor(void);

		protected:

			std::stack<OpenViBE::Kernel::ELogColor> m_vLogColor;
			OpenViBE::Kernel::ELogColor m_eLogColor;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_CLogListenerConsole_H__
