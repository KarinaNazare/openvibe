#ifndef __SamplePlugin_Algorithms_CSpectrumEncoder_H__
#define __SamplePlugin_Algorithms_CSpectrumEncoder_H__

#include "ovpCStreamedMatrixEncoder.h"

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CSpectrumEncoder : virtual public OpenViBEPlugins::StreamCodecs::CStreamedMatrixEncoder
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processHeader(void);

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixEncoder, OVP_ClassId_Algorithm_SpectrumEncoder);

		protected:

			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > m_oFrequencyBandMinMaxHandle;
		};

		class CSpectrumEncoderDesc : virtual public OpenViBEPlugins::StreamCodecs::CStreamedMatrixEncoderDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Spectrum stream encoder"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Algorithm/Stream codecs/Encoders"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Algorithm_SpectrumEncoder; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::StreamCodecs::CSpectrumEncoder(); }

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				OpenViBEPlugins::StreamCodecs::CStreamedMatrixEncoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				rAlgorithmPrototype.addInputParameter(OVP_SpectrumEncoder_FrequencyBandMinMax_InParameterId, "Min/Max frenquency bands", OpenViBE::Kernel::ParameterType_Matrix);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixEncoderDesc, OVP_ClassId_Algorithm_SpectrumEncoderDesc);
		};
	};
};

#endif // __SamplePlugin_Algorithms_CSpectrumEncoder_H__
