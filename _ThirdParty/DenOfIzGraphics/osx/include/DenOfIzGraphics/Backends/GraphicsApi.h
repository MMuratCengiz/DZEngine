#pragma once

#include "Interface/ILogicalDevice.h"

namespace DenOfIz
{
    enum class APIPreferenceWindows
    {
        DirectX12,
        Vulkan,
        WebGPUNative
    };

    enum class APIPreferenceOSX
    {
        Metal,
        Vulkan,
        WebGPUNative
    };

    enum class APIPreferenceLinux
    {
        Vulkan,
        WebGPUNative
    };

    enum class APIPreferenceWeb
    {
        WebGPU
    };

    struct DZ_API APIPreference
    {
        APIPreferenceWindows Windows = APIPreferenceWindows::DirectX12;
        APIPreferenceOSX     OSX     = APIPreferenceOSX::Metal;
        APIPreferenceLinux   Linux   = APIPreferenceLinux::Vulkan;
        APIPreferenceWeb     Web     = APIPreferenceWeb::WebGPU;
    };

    /// <summary>
    /// A class that provides a factory for creating API agnostic structures
    /// Currently creates a logical device and a shader program
    /// </summary>
    class DZ_API GraphicsApi
    {
        APIPreference m_apiPreference;

    public:
        explicit GraphicsApi( const APIPreference &preference );
        ~GraphicsApi( );

        [[nodiscard]] ILogicalDevice *CreateLogicalDevice( const LogicalDeviceDesc &desc = {} ) const;
        void                          LogDeviceCapabilities( PhysicalDevice gpuDesc ) const;
        [[nodiscard]] ILogicalDevice *CreateAndLoadOptimalLogicalDevice( const LogicalDeviceDesc &desc = {} ) const;
        [[nodiscard]] InteropString   ActiveAPI( ) const;

        static void ReportLiveObjects( );

    private:
        [[nodiscard]] bool IsVulkanPreferred( ) const;
        [[nodiscard]] bool IsDX12Preferred( ) const;
        [[nodiscard]] bool IsMetalPreferred( ) const;
        [[nodiscard]] bool IsWebGPUPreferred( ) const;
        [[nodiscard]] bool IsWebGPUNativePreferred( ) const;
    };

} // namespace DenOfIz
