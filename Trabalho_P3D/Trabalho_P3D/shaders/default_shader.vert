#version 330 core

uniform mat4 Model;
uniform mat4 View;
uniform mat4 ModelView;		// View * Model
uniform mat4 Projection;
uniform mat3 NormalMatrix;

layout( location = 0 ) in vec3 vPosition;

void main()
{
    //gl_Position = vec4(vPosition, 1.0f);

    // Posição final do vértice (em coordenadas de clip)
    gl_Position = Projection * View * Model * vec4(vPosition, 1.0f);
}
