struct VS_IN
{
    float3 pos : Position;
    float3 color : Color;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float3 color : Color;
};

VS_OUT main(VS_IN input)
{
    VS_OUT vso = (VS_OUT) 0;
    
    vso.pos = float4(input.pos, 1.0f);
    vso.color = input.color;
    
    return vso;
}