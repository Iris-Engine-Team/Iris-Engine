#pragma once
#include "Core/Tools/Macros.h"
#include "Core/Input/InputEnum.h"
#include "Core/Input/IInput.h"

#include <array>

namespace Iris
{
	namespace Engine
	{
		class GLFWInput : public IInput
		{
		public:

			unsigned int ConvertToIris(unsigned int _key) override;

			unsigned int ConvertToGLFW() override;
			
		private:
			std::array<unsigned int, 128> m_GlfwArray = {
				//mouse
				0,1,2,3,4,5,6,7,

				//letters
				65,66,67,68,69,70,71,72,73,74,75,76,79,78,79,80,81,82,83,84,85,86,87,88,89,90,
				
				//special
				32,262,263,264,265,259,256,257,258,280,340,341,342,344,345,346,

				//numbers
				48,49,50,51,52,53,54,55,56,57,

				//key pad
				321,322,323,324,325,326,327,328,329,330,331,332,333,334,335,336,

				//specail symbole
				96,45,61,91,93,92,59,39,44,46,47,

				//F
				290,291,292,293,294,295,296,297,298,299,300,301,302,303,304,305,306,307,308,309,310,311,312,313,314,

				//other
				260,261,266,267,268,269,281,282,283,284,348,161,162,343,347
			};
		};
	}
}