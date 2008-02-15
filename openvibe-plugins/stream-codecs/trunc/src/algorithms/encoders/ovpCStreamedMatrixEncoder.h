#ifndef __SamplePlugin_Algorithms_CStreamedMatrixEncoder_H__
#define __SamplePlugin_Algorithms_CStreamedMatrixEncoder_H__

#include "ovpCEBMLBaseEncoder.h"

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CStreamedMatrixEncoder : virtual public OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoder
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processHeader(void);
			virtual OpenViBE::boolean processBuffer(void);

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoder, OVP_ClassId_Algorithm_StreamedMatrixEncoder);

		protected:

			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > m_oMatrixHandle;

		private:

			OpenViBE::uint64 m_ui64MatrixBufferSize;
		};

		class CStreamedMatrixEncoderDesc : virtual public OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoderDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Streamed matrix stream encoder"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Algorithm/Stream codecs/Encoders"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Algorithm_StreamedMatrixEncoder; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::StreamCodecs::CStreamedMatrixEncoder(); }

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				rAlgorithmPrototype.addInputParameter(OVP_StreamedMatrixEncoder_Matrix_InParameterId, "Matrix", OpenViBE::Kernel::ParameterType_Matrix);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoderDesc, OVP_ClassId_Algorithm_StreamedMatrixEncoderDesc);
		};
	};
};

#endif // __SamplePlugin_Algorithms_CStreamedMatrixEncoder_H__
