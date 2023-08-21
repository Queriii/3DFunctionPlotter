#pragma pack_matrix(row_major) 




struct VS_IN
{
    float3 f3LocalPosition          : LocalPosition;
    float3 f3Color                  : Color;
};

struct VS_OUT
{
    float4 f4HomoPosition           : SV_Position;
    float3 f3WorldPosition          : WorldPosition;
    float3 f3Color                  : Color;
};



cbuffer VertexTransformation : register(b0)
{
    float4x4 WorldMatrix;
    float4x4 ViewMatrix;
    float4x4 ProjectionMatrix;
}




VS_OUT main(VS_IN Input)
{
    VS_OUT Output;

    Output.f4HomoPosition   = mul(float4(Input.f3LocalPosition, 1.0f), mul(mul(WorldMatrix, ViewMatrix), ProjectionMatrix));
    Output.f3WorldPosition  = mul(float4(Input.f3LocalPosition, 1.0f), WorldMatrix).xyz;
    Output.f3Color          = Input.f3Color;
    
    return Output;
}