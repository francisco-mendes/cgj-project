#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform Shadows {
    mat4 ShadowMatrices[6];
    float FarPlane;
};

out vec4 ex_Pos; // FragPos from GS (output per emitvertex)

void main()
{
    for (int face = 0; face < 6; ++face)
    {
        gl_Layer = face; // built-in variable that specifies to which face we render.
        for(int i = 0; i < 3; ++i) // for each triangle vertex
        {
            ex_Pos = gl_in[i].gl_Position;
            gl_Position = ShadowMatrices[face] * ex_Pos;
            EmitVertex();
        }    
        EndPrimitive();
    }
}  