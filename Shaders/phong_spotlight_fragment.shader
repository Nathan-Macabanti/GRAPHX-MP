#version 330 core
uniform vec3 u_color;
out vec4 FragColor;
in vec3 FragPos;
in vec2 UV;
in vec3 Normal;
uniform sampler2D texture_diffuse;

uniform vec3 u_light_pos;
uniform vec3 u_light_dir;
uniform vec3 u_camera_pos;
uniform vec3 u_ambient_color;

float attenuate(float value, float minimum, float maximum) {
	return 1.0f - (clamp(value, minimum, maximum) - minimum / (maximum - minimum));
}

float simple_attenuate(float value, float maximum) {
	if (value > maximum) {
		value = 0.0;
	}
	else {
		value = 1.0;
	}
	return value;
}

void main()
{
	vec3 lightToSurface = normalize(u_light_pos - FragPos);
	vec3 lightDir = normalize(-u_light_dir);

	float distance = length(u_light_pos - FragPos);

	vec3 lightColor = vec3(1.0, 1.0, 1.0);

	float specularStrength = 0.3;
	vec3 viewDir = normalize(u_camera_pos - FragPos);
	vec3 reflectDir = reflect(-lightDir, Normal);

	float spec = pow(max(dot(reflectDir, viewDir), 0.0), 4);

	vec3 specular = specularStrength * spec * lightColor;

	vec3 diffuse = vec3(max(dot(Normal, lightDir), 0.0f)) * lightColor;
	vec3 ambient = u_ambient_color * lightColor;

	float angle = abs(acos(dot(lightDir, lightToSurface)));
	float gradient = attenuate(angle, 0.2 ,0.3);

	FragColor = vec4(ambient + (diffuse + specular) * gradient, 1.0);
}