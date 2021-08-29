#version 330 core
uniform vec3 u_color;
out vec4 FragColor;
in vec2 UV;
uniform sampler2D texture_diffuse;
in vec4 vertexColor;
in vec3 o_normal;

void main()
{
	//FragColor = vec4(o_normal, 1.0);
	//FragColor = vertexColor;
	FragColor = texture(texture_diffuse, UV);
	//FragColor = vertexColor * texture(texture_diffuse, UV);
}