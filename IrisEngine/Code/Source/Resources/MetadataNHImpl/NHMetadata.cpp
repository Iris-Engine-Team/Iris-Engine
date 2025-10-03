#include "Resources/MetadataNHImpl/NHMetadata.h"

namespace Iris
{
	namespace Engine
	{
		NHMetadata::NHMetadata(std::filesystem::path _path, std::string _name)
		{
			p_Name = _name;
			p_Path = _path;

			std::ifstream fileTryOpen(p_Path);

			if (!fileTryOpen.is_open())
			{
				std::ofstream file(p_Path);

				if (!file.is_open())
				{
					throw std::runtime_error("File cannot be created\n" + p_Name + ".meta\n");
				}

				m_Data["Name"] = _name;
				m_Data["Type"] = MetadataResourceType::IE_NO_TYPE;
				file << m_Data.dump(4);
				file.close();
				return;
			}
			fileTryOpen.close();

			Load();

			m_Data["Name"] = _name;
			m_Data["Type"] = MetadataResourceType::IE_NO_TYPE;
			Update();
		}

		void NHMetadata::Load()
		{
			std::ifstream file(p_Path);

			if (!file.is_open())
			{
				throw std::runtime_error("File cannot be opened\n" + p_Name + "\n");
			}

			m_Data = MetaDataType::parse(file);

			file.close();
		}

		void NHMetadata::Update()
		{
			if (p_Path == "")
				return;

			std::ofstream file(p_Path);

			if (!file.is_open())
			{
				throw std::runtime_error("File cannot be created\n" + p_Name + "\n");
			}

			file << m_Data.dump(4);
			file.close();
		}

		void NHMetadata::WriteMeta(std::string _valueName, IMetadata* _value)
		{
			Write(_valueName, _value->Cast()->m_Data);
		}

		void NHMetadata::WriteMeta(std::vector<std::string> _valuePath, IMetadata* _value)
		{
			Write(_valuePath, _value->Cast()->m_Data);
		}

		MetaDataType NHMetadata::GetData(std::string _valueName)
		{
			return m_Data[_valueName];
		}

		MetaDataType NHMetadata::GetData(std::vector<std::string> _valuePath)
		{
			if (_valuePath.size() == 1)
				return GetData(_valuePath[0]);
			else if (_valuePath.size() == 0)
				return NULL;

			MetaDataType& result = GetElement(m_Data, _valuePath);
			return result[_valuePath[0]];
		}

		void NHMetadata::RemoveAt(std::string _valueName)
		{
			std::lock_guard<std::mutex> lock(p_Mutex);

			auto it = m_Data.find(_valueName);
			if (it == m_Data.end())
				return;

			m_Data.erase(it);
		}

		void NHMetadata::RemoveAt(std::vector<std::string> _valuePath)
		{
			if (_valuePath.size() == 1)
			{
				RemoveAt(_valuePath[0]);
				return;
			}
			else if (_valuePath.size() == 0)
				return;

			std::lock_guard<std::mutex> lock(p_Mutex);
			MetaDataType& result = GetElement(m_Data, _valuePath);
			result.erase(_valuePath[0]);
		}

		void NHMetadata::RemoveAtInList(std::string _valueName, size_t index)
		{
			std::lock_guard<std::mutex> lock(p_Mutex);
			m_Data[_valueName].erase(index);
		}

		void NHMetadata::RemoveAtInList(std::vector<std::string> _valuePath, size_t index)
		{
			if (_valuePath.size() == 1)
			{
				RemoveAt(_valuePath[0]);
				return;
			}
			else if (_valuePath.size() == 0)
				return;

			std::lock_guard<std::mutex> lock(p_Mutex);
			MetaDataType& result = GetElement(m_Data, _valuePath);
			result[_valuePath[0]].erase(index);
		}

		MetaDataType& NHMetadata::GetElement(MetaDataType& element, std::vector<std::string>& _valuePath)
		{
			if (_valuePath.size() == 0)
				return element;

			std::string valueName = _valuePath[0];

			if (element[valueName].size() == 0)
				element[valueName] = {};

			if (_valuePath.size() == 1)
				return element;

			_valuePath.erase(_valuePath.begin());

			return GetElement(element[valueName], _valuePath);
		}
	}
}