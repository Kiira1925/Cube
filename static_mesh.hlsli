struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

cbuffer CONSTANT_BUFFER : register(b0) // ������`���邱�Ƃ��ł��� 
    // ������ɂ���ăX���b�g�ԍ���0�Œ�ɂ��Ă���
    // �����Ă������邪�A�X���b�g�Ǘ����ʓ|�ɂȂ邽�ߕt���邱�Ƃ𐄏�����
{
    row_major float4x4 world_view_projection;
    row_major float4x4 world;
    float4 material_color;
    float4 light_direction;
};