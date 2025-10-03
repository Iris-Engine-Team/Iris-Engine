#pragma once

#include "Core/Graphics/RHI/IBuffer.h"

#include <vulkan/vulkan.h>

namespace Iris
{
	namespace Engine
	{
		class VulkanDevice;
		class VulkanCommandAllocator;

		class VulkanBuffer : public IBuffer
		{
		public:
			/// <summary>
			/// Default VulkanBuffer's destructor
			/// </summary>
			IRIS_ENGINE_API ~VulkanBuffer() override = default;

			/// <summary>
			/// Casts an IBuffer into a Vulkan buffer
			/// </summary>
			/// <returns>The Vulkan buffer or nullptr</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE VulkanBuffer* CastToVulkan() override { return this; }

			/// <summary>
			/// Gets the raw type of the Vulkan buffer
			/// </summary>
			/// <returns>Raw Vulkan buffer object</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE VkBuffer GetBuffer() const { return m_Buffer; }

			/// <summary>
			/// Creates a new buffer
			/// </summary>
			/// <param name="_Device">: Device for creation</param>
			/// <param name="_CommandAllocator">: Command Allocator for creation and allocation</param>
			/// <param name="_BufferType">: Type of buffer (UNIFORM, VERTEX, INDEX)</param>
			/// <param name="_BufferSize">: Size of the buffer</param>
			/// <param name="_Data">: Data of the buffer</param>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			IRIS_ENGINE_API RHIResult CreateBuffer(IDevice* _device, ICommandAllocator* _commandAllocator, RHIBufferType _bufferType, size_t _bufferSize, void* _data) override;
			
			/// <summary>
			/// Destroys the buffer
			/// </summary>
			/// <param name="_Device">: Device for deletion</param>
			/// <returns>An int corresponding to the operation result (0 = SUCCESS)</returns>
			IRIS_ENGINE_API RHIResult DestroyBuffer(IDevice* _device) override;

			/// <summary>
			/// Updates the data of an uniform buffer
			/// </summary>
			/// <param name="_Device">: Device</param>
			/// <param name="_Data">: New data for the buffer</param>
			/// <param name="_DataSize">: Size of the new data</param>
			IRIS_ENGINE_API	void UpdateUniformBuffer(IDevice* _device, void* _data, size_t _dataSize) override;

			/// <summary>
			/// Creates an internal Vulkan buffer
			/// </summary>
			/// <param name="_device"></param>
			/// <param name="_size"></param>
			/// <param name="_usage"></param>
			/// <param name="_properties"></param>
			/// <param name="_buffer"></param>
			/// <param name="_bufferMemory"></param>
			/// <returns></returns>
			IRIS_ENGINE_API RHIResult CreateVulkanBuffer(VulkanDevice* _device, VkDeviceSize _size, VkBufferUsageFlags _usage, VkMemoryPropertyFlags _properties, VkBuffer& _buffer, VkDeviceMemory& _bufferMemory);

			/// <summary>
			/// Finds the memory
			/// </summary>
			/// <param name="_device"></param>
			/// <param name="_typeFilter"></param>
			/// <param name="_properties"></param>
			/// <returns></returns>
			IRIS_ENGINE_API static unsigned int FindMemoryType(VulkanDevice* _device, uint32_t _typeFilter, VkMemoryPropertyFlags _properties);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="_buffer"></param>
			/// <param name="_imageType"></param>
			/// <param name="_image"></param>
			/// <param name="_width"></param>
			/// <param name="_height"></param>
			/// <returns></returns>
			IRIS_ENGINE_API static RHIResult CopyBufferToImage(VulkanDevice* _device, VulkanCommandAllocator* _commandAllocator, VkBuffer _buffer, RHITextureType _imageType, VkImage _image, unsigned int _width, unsigned int _height);

		private:
			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <param name="_size"></param>
			/// <param name="_data"></param>
			/// <returns></returns>
			RHIResult CreateVulkanUniformBuffer(VulkanDevice* _device, VkDeviceSize _size, void* _data);
			
			/// <summary>
			/// 
			/// </summary>
			/// <param name="_device"></param>
			/// <param name="_commandAllocator"></param>
			/// <param name="_srcBuffer"></param>
			/// <param name="_dstBuffer"></param>
			/// <param name="_size"></param>
			/// <returns></returns>
			RHIResult CopyBuffer(VulkanDevice* _device, VulkanCommandAllocator* _commandAllocator, VkBuffer _srcBuffer, VkBuffer _dstBuffer, VkDeviceSize _size);

		private:
			VkBuffer m_Buffer = VK_NULL_HANDLE;
			VkDeviceMemory m_BufferMemory = VK_NULL_HANDLE;
		};
	}
}