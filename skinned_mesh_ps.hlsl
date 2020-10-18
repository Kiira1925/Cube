#include "skinned_mesh.hlsli"

Texture2D diffuseMap : register(t0);
SamplerState diffuseMapSamplerState : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    //return pin.color;

    // pin.color�ɂ��Ċ��Ⴂ���Ă����\������
    // pin.color�͑f�ނ̍ގ��J���[�ł�����lightcolor�ł͂Ȃ��B
    // �����Ĉꉞlight�ɂ̓J���[�͂��邪�[���Ƃ��\���ł��邾����
    // ���ʂ̓J���[��e�N�X�`���[������Ƃ�����Ƃ炵������悤�ɂ��Ă���
    return diffuseMap.Sample(diffuseMapSamplerState, pin.texcoord) * pin.color;
}