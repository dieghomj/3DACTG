/***************************************************************************************************
*	SkinMeshCode Version 3.00
*	LastUpdate	: 2025/10/28.
**/
//------------------------------------------------.
//	定数.
//------------------------------------------------.
static const int MAX_BONE_MATRICES = 255;

//------------------------------------------------.
//	リソース.
//------------------------------------------------.
Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

//------------------------------------------------.
//	定数バッファ.
//------------------------------------------------.
cbuffer per_mesh : register(b0)
{
    float4x4 g_mW;
    float4x4 g_mWVP;
};
cbuffer per_material : register(b1)
{
    float4 g_Ambient;
    float4 g_Diffuse;
    float4 g_Specular;
};
cbuffer per_frame : register(b2)
{
    float4 g_CameraPos;
    float4 g_vLightDir;
};
cbuffer per_bones : register(b3)
{
    float4x4 g_mConstBoneWorld[MAX_BONE_MATRICES];
};

// ヘルパー: ガンマ補正（簡易 sRGB 変換）
static float3 ToSRGB(float3 col )
{
	return pow( saturate(col), 1.0 / 2.2 );
}

// スキニング構造体等（既存）
struct Skin
{
    float4 Pos;
    float4 Norm;
};
struct VSSkinIn
{
    float3 Pos : POSITION;
    float3 Norm : NORMAL;
    float2 UV : TEXCOORD;
    uint4 Bones : BONE_INDEX;
    float4 Weights : BONE_WEIGHT;
};
struct PSSkinIn
{
    float4 Pos : SV_Position;
    float3 Norm : TEXCOORD0;
    float2 UV : TEXCOORD1;
    float3 Light : TEXCOORD2;
    float3 EyeVector : TEXCOORD3;
    float4 PosWorld : TEXCOORD4;
    float4 Color : COLOR;
};

// FetchBoneMatrix 等は既存ロジックを使用
matrix FetchBoneMatrix(uint Bone)
{
    return g_mConstBoneWorld[Bone];
}

Skin SkinVert(VSSkinIn Input)
{
    Skin Output = (Skin) 0;

    float4 Pos = float4(Input.Pos, 1.f);
    float4 Norm = float4(Input.Norm, 0.f);

    uint Bone = Input.Bones.x;
    float Weight = Input.Weights.x;
    float4x4 m = FetchBoneMatrix(Bone);
    Output.Pos += Weight * mul(Pos, m);
    Output.Norm += Weight * mul(Norm, m);

    Bone = Input.Bones.y;
    Weight = Input.Weights.y;
    m = FetchBoneMatrix(Bone);
    Output.Pos += Weight * mul(Pos, m);
    Output.Norm += Weight * mul(Norm, m);

    Bone = Input.Bones.z;
    Weight = Input.Weights.z;
    m = FetchBoneMatrix(Bone);
    Output.Pos += Weight * mul(Pos, m);
    Output.Norm += Weight * mul(Norm, m);

    Bone = Input.Bones.w;
    Weight = Input.Weights.w;
    m = FetchBoneMatrix(Bone);
    Output.Pos += Weight * mul(Pos, m);
    Output.Norm += Weight * mul(Norm, m);

    return Output;
}

PSSkinIn VS_Main(VSSkinIn input)
{
    PSSkinIn output = (PSSkinIn) 0;

    Skin vSkinned = SkinVert(input);

    output.Pos = mul(vSkinned.Pos, g_mWVP);

    output.Norm = normalize((float3) mul(vSkinned.Norm, g_mW));

    output.Light = normalize(g_vLightDir.xyz);

    output.PosWorld = mul(float4(input.Pos, 1.f), g_mW);

    output.EyeVector = normalize(g_CameraPos.xyz - output.PosWorld.xyz);

    output.UV = input.UV;

    return output;
}

//-------------------------------------------------
//	ピクセルシェーダ.
//	改良点: Blinn-Phong, 再正規化, AO(alpha), リムライト, ガンマ補正
//-------------------------------------------------
float4 PS_Main(PSSkinIn input) : SV_Target
{
	// テクスチャ取得
    float4 texColor = g_Texture.Sample(g_Sampler, input.UV);

	// 再正規化
    float3 N = normalize(input.Norm);
    float3 L = normalize(input.Light);
    float3 V = normalize(input.EyeVector);
    float3 H = normalize(L + V);

	// AO: テクスチャアルファを利用（無ければ1）
    float ao = texColor.a;
    if (ao <= 0.001f)
        ao = 1.0f;

	// 環境光
    float3 ambient = texColor.rgb * g_Ambient.rgb * ao;

	// ディフューズ
    float NdotL = saturate(dot(N, L));
    float3 diffuse = texColor.rgb * g_Diffuse.rgb * NdotL * ao;

	// スペキュラ
    float shininess = max(1.0f, g_Specular.a * 128.0f);
    float specFactor = pow(saturate(dot(N, H)), shininess);
    float3 specular = g_Specular.rgb * specFactor;

	// リムライト
    float rim = pow(1.0f - saturate(dot(N, V)), 2.0f);
    float3 rimColor = g_Specular.rgb * rim * 0.12f;

	// 合成
    float3 colorLinear = ambient + diffuse + specular + rimColor;

	// 出力アルファはテクスチャのアルファ
    float alpha = texColor.a;

	// ガンマ補正
    float3 finalColor = ToSRGB(colorLinear);

    return float4(finalColor, alpha);
}