#version 440 core

uniform mat4 Model;			// Uniforms não utilizados podem ser removidos pelo compilador. Também se verifica o mesmo para atributos. Ligar um atributo do VS a uma variável que não é utilizada no FS, faz com que o compilador remova a ligação.
uniform mat4 View;
uniform mat4 ModelView;		// View * Model
uniform mat4 Projection;
uniform mat3 NormalMatrix;

in vec3 vPosition;			// Coordenadas locais do vértice
in vec2 vTexture;			// Coordenadas de textura
in vec3 vNormal;			// Normal do vértice

out vec3 vPositionEyeSpace;
out vec2 textureCoord;
out vec3 vNormalEyeSpace;

void main()
{ 
	// Posição do vértice em coordenadas do olho.
	vPositionEyeSpace = (ModelView * vec4(vPosition, 1.0)).xyz;

	// Transformar a normal do vértice.
	vNormalEyeSpace = normalize(NormalMatrix * vNormal);

	textureCoord = vTexture;

	// Posição final do vértice (em coordenadas de clip)
	gl_Position = Projection * ModelView * vec4(vPosition, 1.0f);
}