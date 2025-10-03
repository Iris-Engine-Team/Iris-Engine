#pragma once

#include <shaderc/shaderc.hpp>

namespace Iris
{
	namespace Engine
	{
		class ShadercIncluder : public shaderc::CompileOptions::IncluderInterface
		{
		public:
			shaderc_include_result* GetInclude(const char* _requestedSource, shaderc_include_type _type, const char* _requestingSource, size_t _includeDepth) override;

			void ReleaseInclude(shaderc_include_result* _result) override;

		private:
			shaderc_include_result* CreateData(std::array<std::string, 2>* _container);
			std::string ReadFile(const std::string& _name, std::string& _debugMsg);
		};
	}
}