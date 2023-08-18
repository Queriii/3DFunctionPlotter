struct PS_IN
{
    float4 f4HomoPosition   : SV_Position;
    float3 f3Color          : Color;
};



float4 main(PS_IN Input) : SV_TARGET
{
    return float4(Input.f3Color, 1.0f);
}