struct PS_IN
{
    float4 f4HomoPosition   : SV_Position;
    float3 f3WorldPosition  : WorldPosition;
    float3 f3Color          : Color;
};



cbuffer GridType : register(b0)
{
    int     GridTypeID;         //0 = XZ, 1 = XY, 2 = ZY, 3 = Don't clip
    float3  Pad;
}



float4 main(PS_IN Input) : SV_TARGET
{
    switch (GridTypeID)
    {
        case 0:
        {
            if (Input.f3WorldPosition.x == 0.0f || Input.f3WorldPosition.z == 0.0f)
            {
                clip(-1.0f);
            }
            break;
        }
        
        case 1:
        {
            if (Input.f3WorldPosition.x == 0.0f || Input.f3WorldPosition.y == 0.0f)
            {
                clip(-1.0f);
            }
            break;
        }
        
        case 2:
        {
            if (Input.f3WorldPosition.z == 0.0f || Input.f3WorldPosition.y == 0.0f)
            {
                clip(-1.0f);
            }
            break;
        }
        
        case 3:
        {
            break;
        }
        
        default:
        {
            clip(-1.0f);
            break;
        }
    }

    
	return float4(Input.f3Color, 1.0f);
}