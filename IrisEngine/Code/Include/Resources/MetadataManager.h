#pragma once
#include "Resources/Metadata.h"
#include <map>

namespace Iris
{
	namespace Engine
	{
		class MetaDataManager
		{
		public:
			MetaDataManager();
			~MetaDataManager();

			/// <summary>
			/// Load all resources
			/// </summary>
			void Init();

			/// <summary>
			/// Creates a metadata and store it
			/// </summary>
			/// <param name="_path">: path to creates file</param>
			/// <param name="_name">: name of the wanted file (without .meta)</param>
			IRIS_ENGINE_API IMetadata* CreateMetadata(const std::filesystem::path& _path, const std::string& _name);

			/// <summary>
			/// Creates a metadata that will be used to give data to another metada
			/// <para>This metadata isn't stored</para>
			/// </summary>
			IRIS_ENGINE_API IMetadata* CreateMetadata();

			/// <returns>The wanted metadata or nullptr if is not stored</returns>
			IRIS_ENGINE_API IMetadata* GetMetadata(std::string _name);
		private:
			std::map<std::string, IMetadata*> m_MetaData;
		};
	}
}