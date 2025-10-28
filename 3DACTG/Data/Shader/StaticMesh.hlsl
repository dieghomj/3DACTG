/*********************************************************************
*	スタティックメッシュクラス用シェーダファイル.
**/
//グローバル変数.
//テクスチャは レジスタ t(n).
Texture2D		g_Texture	: register( t0 );
//サンプラは レジスタ s(n).
SamplerState	g_SamLinear	: register( s0 );

//コンスタントバッファ.
//メッシュ単位.
cbuffer per_mesh	: register( b0 )
{
	matrix	g_mW;			//ワールド行列.
	matrix	g_mWVP;			//ワールド,ビュー,プロジェクションの合成行列.
};
//マテリアル単位.
cbuffer per_material: register( b1 )
{
	float4	g_Diffuse;		//ディフューズ色(拡散反射色).
	float4	g_Ambient;		//アンビエント色(環境色).
	float4	g_Specular;		//スペキュラ色(鏡面反射色).
};
//フレーム単位.
cbuffer per_frame	: register( b2 )
{
	float4	g_CameraPos;	//カメラ位置(視点位置).
	float4	g_vLightDir;	//ライトの方向ベクトル.
    float4 g_FogColor;		//フォグの色.
    float4 g_FogParams;		//フォグのパラメータ(x:開始距離, y:終了距離, z:未使用, w:未使用).
};

//頂点シェーダの出力パラメータ.
struct VS_OUTPUT
{
	float4	Pos			: SV_Position;
	float3	Normal		: TEXCOORD0;
	float2	UV			: TEXCOORD1;
	float3	Light		: TEXCOORD2;
	float3	EyeVector	: TEXCOORD3;
	float4	PosWorld	: TEXCOORD4;
	float4	Color		: COLOR;
};

//-------------------------------------------------
//	頂点(バーテックス)シェーダ.
//-------------------------------------------------
VS_OUTPUT VS_Main(
	float4 Pos	: POSITION,
	float4 Norm	: NORMAL,
	float2 UV	: TEXCOORD)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	//プロジェクション変換(ワールド,ビュー,プロジェクション).
	output.Pos = mul( Pos, g_mWVP );

	//法線をモデルの姿勢に合わせる.
	// (モデルが回転すれば法線も回転させる必要があるため).
	output.Normal = mul( Norm, (float3x3)g_mW );
	output.Normal = normalize( output.Normal );

	//ライト方向:
	// ディレクショナルライトは、どこでも同じ方向.位置は無関係.
	output.Light = normalize( g_vLightDir );

	output.PosWorld = mul( Pos, g_mW );

	//視線ベクトル:
	// ワールド空間上での頂点から頂点へ向かうベクトル.
	output.EyeVector = normalize( g_CameraPos - output.PosWorld );

	//テクスチャ座標.
	output.UV = UV;

	return output;
}

// ヘルパー: ガンマ補正（簡易 sRGB 変換）
static float3 ToSRGB(float3 col )
{
	return pow( saturate(col), 1.0 / 2.2 );
}

//-------------------------------------------------
//	ピクセルシェーダ.
//	改良点: Blinn-Phong, ピクセルでの再正規化, AO(テクスチャalpha), リムライト, ガンマ補正
//-------------------------------------------------
float4 PS_Main(VS_OUTPUT input) : SV_Target
{
	//テクスチャカラー.
    float4 texColor = g_Texture.Sample(g_SamLinear, input.UV);

	//ピクセルで再正規化（補間により長さが変わるため）
    float3 N = normalize(input.Normal);
    float3 L = normalize(input.Light);
    float3 V = normalize(input.EyeVector);

	//Blinn-Phong ハーフベクタ
    float3 H = normalize(L + V);

	//簡易AO: テクスチャのアルファをAOとして利用（無ければ1）
    float ao = texColor.a;
    if (ao <= 0.001f)
        ao = 1.0f;

	//環境光
    float3 ambient = texColor.rgb * g_Ambient.rgb * ao;

	//ディフューズ: テクスチャ * マテリアル を基本に
    float NdotL = saturate(dot(N, L));
    float3 diffuse = texColor.rgb * g_Diffuse.rgb * NdotL * ao;

	//スペキュラ: g_Specular.a を shininess の目安に（1..128）
    float shininess = max(1.0f, g_Specular.a * 128.0f);
    float specFactor = pow(saturate(dot(N, H)), shininess);
    float3 specular = g_Specular.rgb * specFactor;

	//リムライトで輪郭を強調
    float rim = pow(1.0f - saturate(dot(N, V)), 2.0f);
    float3 rimColor = g_Specular.rgb * rim * 0.12f;

	//合成（シンプルなエネルギー保存考慮）
    float3 colorLinear = ambient + diffuse + specular + rimColor;

	//出力アルファはテクスチャのアルファ
    float alpha = texColor.a;

    float dist = length(input.PosWorld.xyz - g_CameraPos.xyz);
    float fogFactor = 1.0f;
    int mode = (int) g_FogParams.w;
    if (mode == 0) // linear
    {
        float start = g_FogParams.x;
        float end = max(g_FogParams.y, start + 0.001f);
        fogFactor = saturate((end - dist) / (end - start));
    }
    else if (mode == 1) // exp
    {
        float density = g_FogParams.z;
        fogFactor = exp(-density * dist);
    }
    else // exp2
    {
        float density = g_FogParams.z;
        fogFactor = exp(-density * density * dist * dist);
    }
    float3 outLinear = lerp(g_FogColor.rgb, colorLinear, fogFactor);
	
    //ガンマ補正（線形->sRGB）
    float3 finalColor = ToSRGB(outLinear);


    return float4(finalColor, alpha);
}

//========= テクスチャ無し用 ========================================
//-------------------------------------------------
//	頂点(バーテックス)シェーダ.
//-------------------------------------------------
VS_OUTPUT VS_NoTex(
	float4 Pos : POSITION,
	float4 Norm : NORMAL)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;

    output.Pos = mul(Pos, g_mWVP);

    output.Normal = mul(Norm, (float3x3) g_mW);
    output.Normal = normalize(output.Normal);

    output.Light = normalize(g_vLightDir);

    output.PosWorld = mul(Pos, g_mW);

    output.EyeVector = normalize(g_CameraPos - output.PosWorld);

    return output;
}

//-------------------------------------------------
//	ピクセルシェーダ (テクスチャ無し).
//-------------------------------------------------
float4 PS_NoTex(VS_OUTPUT input) : SV_Target
{
    float3 N = normalize(input.Normal);
    float3 L = normalize(input.Light);
    float3 V = normalize(input.EyeVector);
    float3 H = normalize(L + V);

    float NdotL = saturate(dot(N, L));
    float3 diffuse = g_Diffuse.rgb * NdotL;

    float3 ambient = g_Ambient.rgb;

    float shininess = max(1.0f, g_Specular.a * 128.0f);
    float specFactor = pow(saturate(dot(N, H)), shininess);
    float3 specular = g_Specular.rgb * specFactor;

    float rim = pow(1.0f - saturate(dot(N, V)), 2.0f);
    float3 rimColor = g_Specular.rgb * rim * 0.12f;

    float3 colorLinear = ambient + diffuse + specular + rimColor;

    float3 finalColor = ToSRGB(colorLinear);

    return float4(finalColor, 1.0f);
}
