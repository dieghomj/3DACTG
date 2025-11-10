/*********************************************************************
*	スキンメッシュクラス用シェーダファイル.
**/

// Textures and Samplers
Texture2D g_Texture : register(t0);
SamplerState g_SamPoint : register(s0); // Use Point Sampler for PSX effect

// Constant Buffers
cbuffer per_mesh : register(b0)
{
    matrix g_mWVP; // World-View-Projection Matrix
};

cbuffer per_material : register(b1)
{
    float4 g_Diffuse; // Diffuse color
};

cbuffer per_frame : register(b2)
{
    float4 g_LightColor; // Light Color
    float4 g_LightDir; // Light Direction
    float4 g_AmbientColor; // Ambient Color
    float g_AffineIntensity; // PSX Affine Texture Mapping Intensity
    float g_VertexSnapping; // PSX Vertex Snapping Intensity
};

cbuffer per_skin : register(b3)
{
    matrix g_mBone[256];
};

// Vertex Shader Input
struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD0;
    float4 Indices : BLENDINDICES0;
    float4 Weights : BLENDWEIGHT0;
};

// Pixel Shader Input
struct PS_INPUT
{
    float4 Pos : SV_Position;
    float2 UV : TEXCOORD0;
    float3 Normal : TEXCOORD1;
};

//-------------------------------------------------
//	Vertex Shader
//-------------------------------------------------
PS_INPUT VS_Main(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;

	// Skinning
    float4 originalPos = float4(0, 0, 0, 1);
    float3 originalNormal = float3(0, 0, 0);

    float weights[4] = { input.Weights.x, input.Weights.y, input.Weights.z, input.Weights.w };
    uint indices[4] = { input.Indices.x, input.Indices.y, input.Indices.z, input.Indices.w };

    float totalWeight = 0.0f;
    for (int i = 0; i < 4; ++i)
    {
        if (weights[i] > 0)
        {
            totalWeight += weights[i];
            originalPos += weights[i] * mul(input.Pos, g_mBone[indices[i]]);
            originalNormal += weights[i] * mul(input.Normal, (float3x3) g_mBone[indices[i]]);
        }
    }
    originalPos /= totalWeight;

	// Projection
    output.Pos = mul(originalPos, g_mWVP);

    // --- PSX Effects Start ---
    output.Pos.xyw *= output.Pos.w;
    output.Pos.w *= g_AffineIntensity;

    if (g_VertexSnapping > 0.0f)
    {
        output.Pos.xy = floor(g_VertexSnapping * output.Pos.xy) / g_VertexSnapping;
    }
    // --- PSX Effects End ---

	// Pass data to pixel shader
    output.Normal = normalize(originalNormal);
    output.UV = input.UV;

    return output;
}

//-------------------------------------------------
//	Pixel Shader
//-------------------------------------------------
float4 PS_Main(PS_INPUT input) : SV_Target
{
	// Sample texture
    float4 texColor = g_Texture.Sample(g_SamPoint, input.UV);

	// Lighting
    float3 N = normalize(input.Normal);
    float3 L = normalize(g_LightDir.xyz);
    float NdotL = saturate(dot(N, -L));

	// Final Color Calculation
    float3 diffuse = g_Diffuse.rgb * texColor.rgb * g_LightColor.rgb * NdotL;
    float3 ambient = g_AmbientColor.rgb * texColor.rgb;
    float3 finalColor = ambient + diffuse;

    return float4(finalColor, texColor.a);
}