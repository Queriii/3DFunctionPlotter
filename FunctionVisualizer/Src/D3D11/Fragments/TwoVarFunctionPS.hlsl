struct PS_IN
{
    float4 f4HomoPosition : SV_Position;
    float3 f3WorldPosition : WorldPosition;
    float3 f3Color : Color;
};



cbuffer CurrentYRange : register(b0) 
{
    float   fYRange;
    float3  Pad0;
};



float4 main(PS_IN Input) : SV_TARGET
{
    if (Input.f3WorldPosition.y > fYRange || Input.f3WorldPosition.y < (-fYRange))
    {
        clip(-1.0f);
    }
    
    return (float4(Input.f3Color, 1.0f));
}