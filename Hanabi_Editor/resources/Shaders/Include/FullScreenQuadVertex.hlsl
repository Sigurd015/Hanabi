#pragma once
// --------------------------
// Full Screen Quad Vertex Shader
// --------------------------

// (-1, 1)________ (3, 1)
//        |   |  /
// (-1,-1)|___|/ (1, -1)   
//        |  /
// (-1,-3)|/      
float4 main(uint VertexID : SV_VertexID) : SV_Position
{
    float2 grid = float2((VertexID << 1) & 2, VertexID & 2);
    float2 xy = grid * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
    return float4(xy, 1.0f, 1.0f);
}