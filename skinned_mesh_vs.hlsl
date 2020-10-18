#include "skinned_mesh.hlsli"

VS_OUT main(float4 position : POSITION, float4 normal : NORMAL, float2 texcoord : TEXCOORD, float4 boneWeights : WEIGHTS, uint4 boneIndices : BONES)
{
    VS_OUT vout;

    float3 p = { 0, 0, 0 };
    float3 n = { 0, 0, 0 };
    int i = 0;
    for (i = 0; i < 4; i++)
    {
        p += (boneWeights[i] * mul(position, boneTransforms[boneIndices[i]])).xyz;
        n += (boneWeights[i] * mul(float4(normal.xyz, 0), boneTransforms[boneIndices[i]])).xyz;

    }
    position = float4(p, 1.0f);
    normal = float4(n, 0.0f);

    vout.position = mul(position, world_view_projection);

    normal.w = 0;
    float4 N = normalize(mul(normal, world));

    float4 L = normalize(-light_direction);
    vout.color = material_color * max(0, dot(L, N));
    vout.color.a = material_color.a;

    //float4 influence = { 0, 0, 0, 1 };
    //for (int i = 0; i < 4;i++)
    //{
    //    float weight = boneWeights[i];
    //    if (weight > 0.0f)
    //    {
    //        switch (boneIndices[i])
    //        {
    //            case 0:
    //                influence.r = weight;
    //                break;
    //            case 1:
    //                influence.g = weight;
    //                break;
    //            case 2:
    //                influence.b = weight;
    //                break;
    //        }

    //    }
    //}

    //vout.color = influence;
    vout.texcoord = texcoord;

    return vout;

}