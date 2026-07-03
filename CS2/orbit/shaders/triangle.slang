struct VSInput {
    float2 in_position;
    float3 in_color;
};

struct VSOutput {
    float4 pos : SV_Position;
    float3 color;
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
    return output;
}

[shader("fragment")]
float4 frag_main(VSOutput vert_in) : SV_Target {
    return float4(vert_in.color, 1.0);
}
