#pragma once
#include "Core/Tools/FilePath.h"
#include <string>
#include <vector>
#include "../Externals/json/single_include/nlohmann/json.hpp"

#include <mutex>

/// <summary>
/// no time to find such a modular equivalent to use es
/// </summary>
using MetaDataType = nlohmann::json;

namespace Iris
{
	namespace Engine
	{
		enum class MetadataResourceType : int
		{
			IE_NO_TYPE = -1,
			IE_SCENE_DATA = 0,
			IE_MATERIAL_DATA = 1
		};

		class NHMetadata;
		class IMetadata
		{
		public:
			IMetadata() = default;

			virtual ~IMetadata() = default;

			virtual void Load() = 0;
			virtual void Update() = 0;

			virtual NHMetadata* Cast() { return nullptr; }
			
			virtual void WriteMeta(std::string _valueName, IMetadata* _value) = 0;
			virtual void WriteMeta(std::vector<std::string> _valuePath, IMetadata* _value) = 0;

			template<typename T>
			IE_FORCE_INLINE void Write(std::string _valueName, const T& _value);

			template<typename T>
			IE_FORCE_INLINE void Write(std::vector<std::string> _valuePath, const T& _value);

			template<typename T>
			IE_FORCE_INLINE void AddValue(std::string _valueName, const T& _value);

			template<typename T>
			IE_FORCE_INLINE void AddValue(std::vector<std::string> _valuePath, const T& _value);

			virtual MetaDataType GetData() = 0;
			virtual MetaDataType GetData(std::string _valueName) = 0;
			virtual MetaDataType GetData(std::vector<std::string> _valuePath) = 0;

			virtual void RemoveAt(std::string _valueName) = 0;
			virtual void RemoveAt(std::vector<std::string> _valuePath) = 0;

			virtual void RemoveAtInList(std::string _valueName, size_t index) = 0;
			virtual void RemoveAtInList(std::vector<std::string> _valuePath, size_t index) = 0;

		protected:
			std::mutex p_Mutex;
			std::filesystem::path p_Path;
			std::string p_Name = "";
		};
	}
}

#include "Resources/Metadata.inl"