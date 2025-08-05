
#include "DZEngine/Math/MathConverter.h"

using namespace DenOfIz;
using namespace DZEngine;

DirectX::XMFLOAT4X4 MathConverter::Float4x4ToXMFLOAT4X4( const Float4x4 &matrix )
{
    DirectX::XMFLOAT4X4 result;
    result._11 = matrix._11;
    result._12 = matrix._12;
    result._13 = matrix._13;
    result._14 = matrix._14;
    result._21 = matrix._21;
    result._22 = matrix._22;
    result._23 = matrix._23;
    result._24 = matrix._24;
    result._31 = matrix._31;
    result._32 = matrix._32;
    result._33 = matrix._33;
    result._34 = matrix._34;
    result._41 = matrix._41;
    result._42 = matrix._42;
    result._43 = matrix._43;
    result._44 = matrix._44;
    return result;
}

Float4x4 MathConverter::Float4x4FromXMFLOAT4X4( const DirectX::XMFLOAT4X4 &matrix )
{
    Float4x4 result;
    result._11 = matrix._11;
    result._12 = matrix._12;
    result._13 = matrix._13;
    result._14 = matrix._14;
    result._21 = matrix._21;
    result._22 = matrix._22;
    result._23 = matrix._23;
    result._24 = matrix._24;
    result._31 = matrix._31;
    result._32 = matrix._32;
    result._33 = matrix._33;
    result._34 = matrix._34;
    result._41 = matrix._41;
    result._42 = matrix._42;
    result._43 = matrix._43;
    result._44 = matrix._44;
    return result;
}

Float4x4 MathConverter::Float4X4FromXMMATRIX( const DirectX::XMMATRIX &matrix )
{
    Float4x4 result;
    result._11 = DirectX::XMVectorGetX( matrix.r[ 0 ] );
    result._12 = DirectX::XMVectorGetY( matrix.r[ 0 ] );
    result._13 = DirectX::XMVectorGetZ( matrix.r[ 0 ] );
    result._14 = DirectX::XMVectorGetW( matrix.r[ 0 ] );
    result._21 = DirectX::XMVectorGetX( matrix.r[ 1 ] );
    result._22 = DirectX::XMVectorGetY( matrix.r[ 1 ] );
    result._23 = DirectX::XMVectorGetZ( matrix.r[ 1 ] );
    result._24 = DirectX::XMVectorGetW( matrix.r[ 1 ] );
    result._31 = DirectX::XMVectorGetX( matrix.r[ 2 ] );
    result._32 = DirectX::XMVectorGetY( matrix.r[ 2 ] );
    result._33 = DirectX::XMVectorGetZ( matrix.r[ 2 ] );
    result._34 = DirectX::XMVectorGetW( matrix.r[ 2 ] );
    result._41 = DirectX::XMVectorGetX( matrix.r[ 3 ] );
    result._42 = DirectX::XMVectorGetY( matrix.r[ 3 ] );
    result._43 = DirectX::XMVectorGetZ( matrix.r[ 3 ] );
    result._44 = DirectX::XMVectorGetW( matrix.r[ 3 ] );
    return result;
}

DirectX::XMMATRIX MathConverter::Float4X4ToXMMATRIX( const Float4x4 &matrix )
{
    // clang-format off
        return DirectX::XMMatrixSet(
            matrix._11, matrix._12, matrix._13, matrix._14,
            matrix._21, matrix._22, matrix._23, matrix._24,
            matrix._31, matrix._32, matrix._33, matrix._34,
            matrix._41, matrix._42, matrix._43, matrix._44
        );
    // clang-format on
}

Float4 MathConverter::Float4FromXMVECTOR( const DirectX::XMVECTOR &vector )
{
    return Float4{ DirectX::XMVectorGetX( vector ), DirectX::XMVectorGetY( vector ), DirectX::XMVectorGetZ( vector ), DirectX::XMVectorGetW( vector ) };
}

Float4 MathConverter::Float4FromXMFLOAT4( const DirectX::XMFLOAT4 &vector )
{
    return Float4{ vector.x, vector.y, vector.z, vector.w };
}

DirectX::XMVECTOR MathConverter::Float4ToXMVECTOR( const Float4 &vector )
{
    return DirectX::XMVectorSet( vector.X, vector.Y, vector.Z, vector.W );
}

Float3 MathConverter::Float3FromXMVECTOR( const DirectX::XMVECTOR &vector )
{
    return Float3{ DirectX::XMVectorGetX( vector ), DirectX::XMVectorGetY( vector ), DirectX::XMVectorGetZ( vector ) };
}

Float3 MathConverter::Float3FromXMFLOAT3( const DirectX::XMFLOAT3 &vector )
{
    return Float3{ vector.x, vector.y, vector.z };
}

DirectX::XMVECTOR MathConverter::Float3ToXMVECTOR( const Float3 &vector )
{
    return DirectX::XMVectorSet( vector.X, vector.Y, vector.Z, 0.0f );
}

Float2 MathConverter::Float2FromXMVECTOR( const DirectX::XMVECTOR &vector )
{
    return Float2{ DirectX::XMVectorGetX( vector ), DirectX::XMVectorGetY( vector ) };
}

Float2 MathConverter::Float2FromXMFLOAT2( const DirectX::XMFLOAT2 &vector )
{
    return Float2{ vector.x, vector.y };
}

DirectX::XMVECTOR MathConverter::Float2ToXMVECTOR( const Float2 &vector )
{
    return DirectX::XMVectorSet( vector.X, vector.Y, 0.0f, 0.0f );
}
