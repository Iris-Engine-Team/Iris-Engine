#pragma once

#include "Core/Tools/Macros.h"
#include "Core/Tools/Log.h"
#include "Core/Graphics/RHI/CommonTypes.h"

namespace Iris
{
	namespace Engine
	{
		class IDevice;
		class ICommandAllocator;
		class VulkanBuffer;

		class IBuffer
		{
		public:
			/// <summary>
			/// Default IBuffer's destructor
			/// </summary>
			virtual ~IBuffer() = default;

			/// <summary>
			/// Casts an IBuffer into a Vulkan buffer
			/// </summary>
			/// <returns>The Vulkan buffer or nullptr</returns>
			virtual IE_FORCE_INLINE VulkanBuffer* CastToVulkan() { return nullptr; }

			/// <summary>
			/// Creates a new buffer
			/// </summary>
			/// <param name="_Device">: Device for creation</param>
			/// <param name="_CommandAllocator">: Command Allocator for creation and allocation</param>
			/// <param name="_BufferType">: Type of buffer (UNIFORM, VERTEX, INDEX)</param>
			/// <param name="_BufferSize">: Size of the buffer</param>
			/// <param name="_Data">: Data of the buffer</param>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			virtual RHIResult CreateBuffer(IDevice* _device, ICommandAllocator* _commandAllocator, RHIBufferType _bufferType, size_t _bufferSize, void* _data) = 0;

			/// <summary>
			/// Destroys the buffer
			/// </summary>
			/// <param name="_Device">: Device for deletion</param>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			virtual RHIResult DestroyBuffer(IDevice* _device) = 0;

			/// <summary>
			/// Updates the data of an uniform buffer
			/// </summary>
			/// <param name="_Device">: Device</param>
			/// <param name="_Data">: New data for the buffer</param>
			/// <param name="_DataSize">: Size of the new data</param>
			virtual void UpdateUniformBuffer(IDevice* _device, void* _data, size_t _dataSize) = 0;
		};
	}
}