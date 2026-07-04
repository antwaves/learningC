struct VSInput {
    float2 in_position;
    float3 in_color;
    float2 uv : TEXCOORD0;
};

struct VSOutput {
    float4 pos : SV_Position;
    float3 color;
    float2 uv : TEXCOORD0;
};

struct TransformationSSBO {
    float2 transformation;
};
StructuredBuffer<TransformationSSBO> transformations;

[shader("vertex")]
VSOutput vert_main(VSInput input, uint vid: SV_VertexID) {
    VSOutput output;
    output.pos = float4(input.in_position + transformations[vid].transformation, 0.0, 1.0);
    output.color = input.in_color;
    output.uv = input.uv;
    return output;
}

[shader("fragment")]
float4 frag_main(VSOutput vert_in) : SV_Target {
    float2 center = vert_in.uv * 2.0 - 1.0;
    float dist = length(center);
    float radius = 1;
    float edgeSoftness = 0.01;
    float alpha = smoothstep(radius, radius - edgeSoftness, dist);
    return float4(vert_in.color, alpha);
}
