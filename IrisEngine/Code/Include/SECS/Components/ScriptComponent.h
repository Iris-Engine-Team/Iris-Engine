#pragma once
#include "SECS/Components/IComponent.h"

namespace Iris
{
	namespace Engine
	{
		class IMetadata;
		class RigidBody;

		class ScriptSystem;

		class ScriptComponent : public IComponent
		{
		public:

			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
			//							Component DEF							 //
			DEFINE_COMPONENT(ScriptComponent);
			static IE_FORCE_INLINE std::string GetName() { return "Script"; }
			static IE_FORCE_INLINE ComponentID GetID() { return ComponentID::IE_SCRIPT; }
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//


			/// <summary>
			/// Writes data in the given metadata
			/// </summary>
			/// <param name="_metadata"></param>
			void Serialize(IMetadata* _metadata) final override;

			/// <summary>
			/// Init the components with the given data
			/// </summary>
			void Deserialization(IMetadata* _metadata, InitEditorContext& _initializer) final override;

			IRIS_ENGINE_API void Init(ScriptSystem* _system, std::string _className = "Template");

			/// <summary>
			/// Init function called from the editor
			/// </summary>
			IRIS_ENGINE_API void Init(InitEditorContext& _initializer) final override;

			IRIS_ENGINE_API IE_FORCE_INLINE void SetInstance(void* _instance) { m_ScriptInstance = _instance; }

			void Copy(IComponent* _componentToCopy) final override;

			void Duplicate(IComponent* _componentToDuplicate, InitEditorContext* _initializer) final override;
		
			void Destroy() final override;

			void OnCollisionEnter(RigidBody* _rb);
			void OnCollisionStay(RigidBody* _rb);
			void OnCollisionExit(RigidBody* _rb);

			void OnTriggerEnter(RigidBody* _rb);
			void OnTriggerStay(RigidBody* _rb);
			void OnTriggerExit(RigidBody* _rb);


			std::string GetScriptName() const { return m_ScriptName; }
			void* GetScriptInstance() const { return m_ScriptInstance; }
		private:
			std::string m_ScriptName = "";
			void* m_ScriptInstance = nullptr;
			
			ScriptSystem* m_ScriptSystem = nullptr;
		};
	}
}