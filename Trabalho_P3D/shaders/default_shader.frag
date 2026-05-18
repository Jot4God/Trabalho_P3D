#version 330 core

layout (location = 0) out vec4 fColor; // Cor final do fragmento

void main()
{
	// Definindo a cor do fragmento como 'magenta'
	fColor = vec4(1.0f, 0.0f, 1.0f, 1.0f);
}
