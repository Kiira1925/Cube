#include "skinned_mesh.hlsli"

Texture2D diffuseMap : register(t0);
SamplerState diffuseMapSamplerState : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    //return pin.color;

    // pin.colorについて勘違いしていた可能性が大
    // pin.colorは素材の材質カラーであってlightcolorではない。
    // そして一応lightにはカラーはあるが夕暮れとか表現できるだけで
    // 普通はカラーやテクスチャーがあるところを照らし見えるようにしている
    return diffuseMap.Sample(diffuseMapSamplerState, pin.texcoord) * pin.color;
}