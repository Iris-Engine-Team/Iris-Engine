#pragma once
#include "Resources/Metadata.h"

namespace Iris
{
	namespace Engine
	{
		class NHMetadata : public IMetadata
		{
		public:
			NHMetadata() = default;
			NHMetadata(std::filesystem::path _path, std::string _name);
			IE_FORCE_INLINE ~NHMetadata() override { Update(); }

			void Load() override;
			void Update() override;

			NHMetadata* Cast() final override { return this; }

			void WriteMeta(std::string _valueName, IMetadata* _value) override;
			void WriteMeta(std::vector<std::string> _valuePath, IMetadata* _value) final override;

			template<typename T>
			IE_FORCE_INLINE void Write(std::string _valueName, const T& _value)
			{
				std::lock_guard<std::mutex> lock(p_Mutex);
				m_Data[_valueName] = _value;
			}

			template<typename T>
			IE_FORCE_INLINE void Write(std::vector<std::string> _valuePath, const T& _value)
			{

				if (_valuePath.size() == 1)
				{
					Write(_valuePath[0], _value);
					return;
				}
				else if (_valuePath.size() == 0)
					return;

				std::lock_guard<std::mutex> lock(p_Mutex);
				MetaDataType& result = GetElement(m_Data, _valuePath);

				if (result[_valuePath[0]] == _value)
					return;

				result[_valuePath[0]] = _value;
			}

			template<typename T>
			IE_FORCE_INLINE void AddValue(std::string _valueName, const T& _value)
			{
				std::lock_guard<std::mutex> lock(p_Mutex);
				m_Data[_valueName].push_back(_value);
			}

			template<typename T>
			IE_FORCE_INLINE void AddValue(std::vector<std::string> _valuePath, const T& _value)
			{
				if (_valuePath.size() == 1)
				{
					AddValue(_valuePath[0], _value);
					return;
				}
				else if (_valuePath.size() == 0)
					return;

				std::lock_guard<std::mutex> lock(p_Mutex);
				MetaDataType& result = GetElement(m_Data, _valuePath);

				result[_valuePath[0]].push_back(_value);
			}


			IE_FORCE_INLINE MetaDataType GetData() final override { return m_Data; }
			MetaDataType GetData(std::string _valueName) final override;
			MetaDataType GetData(std::vector<std::string> _valuePath) final override;

			void RemoveAt(std::string _valueName) final override;
			void RemoveAt(std::vector<std::string> _valuePath) final override;

			void RemoveAtInList(std::string _valueName, size_t index) final override;
			void RemoveAtInList(std::vector<std::string> _valuePath, size_t index) final override;

		private:
			IRIS_ENGINE_API MetaDataType& GetElement(MetaDataType& element, std::vector<std::string>& _valuePath);
		
		private:
			MetaDataType m_Data;
		};
	}
}