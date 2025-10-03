#include "MetadataNHImpl/NHMetadata.h"

namespace Iris
{
	namespace Engine
	{
		template<typename T>
		void IMetadata::Write(std::string _valueName, const T& _value)
		{
			Cast()->Write(_valueName, _value);
		}

		template<typename T>
		void IMetadata::Write(std::vector<std::string> _valuePath, const T& _value)
		{
			Cast()->Write(_valuePath, _value);
		}

		template<typename T>
		void IMetadata::AddValue(std::string _valueName, const T& _value)
		{
			Cast()->AddValue(_valueName, _value);
		}
		
		template<typename T>
		void IMetadata::AddValue(std::vector<std::string> _valuePath, const T& _value)
		{
			Cast()->AddValue(_valuePath, _value);
		}
	}
}



