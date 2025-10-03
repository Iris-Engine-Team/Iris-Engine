#include "Resources/MetadataManager.h"

namespace Iris
{
	namespace Engine
	{
		MetaDataManager::MetaDataManager()
		{

		}

		MetaDataManager::~MetaDataManager()
		{
			for (auto it = m_MetaData.begin(); it != m_MetaData.end(); ++it)
				delete it->second;
			
			m_MetaData.clear();
		}

		IMetadata* MetaDataManager::CreateMetadata(const std::filesystem::path& _path, const std::string& _name)
		{
			auto meta = m_MetaData.find(_name);
			if (meta != m_MetaData.end())
				return meta->second;

			NHMetadata* newData = new NHMetadata(_path, _name);
			newData->Load();

			m_MetaData[_name] = newData;
			return newData;
		}

		IMetadata* MetaDataManager::CreateMetadata()
		{
			return new NHMetadata();
		}

		IMetadata* MetaDataManager::GetMetadata(std::string _name)
		{
			auto meta = m_MetaData.find(_name);
			if (meta != m_MetaData.end())
				return meta->second;
			return nullptr;
		}
	}
}