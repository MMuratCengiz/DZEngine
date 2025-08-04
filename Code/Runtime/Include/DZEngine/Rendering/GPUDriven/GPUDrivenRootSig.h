#pragma once

#include <DenOfIzGraphics/Backends/Interface/ILogicalDevice.h>
#include <DenOfIzGraphics/Backends/Interface/IRootSignature.h>
#include <memory>

using namespace DenOfIz;

namespace DZEngine
{
    class GPUDrivenRootSig
    {
        std::vector<ResourceBindingDesc> m_resourceBindings;
        RootSignatureDesc                m_desc;
        std::unique_ptr<IRootSignature>  m_rootSignature;

    public:
        explicit GPUDrivenRootSig( ILogicalDevice *device );
        RootSignatureDesc GetDesc( ) const;
        IRootSignature   *GetRootSignature( ) const;
    };
} // namespace DZEngine
