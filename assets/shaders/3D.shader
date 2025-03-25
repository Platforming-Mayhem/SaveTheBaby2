//Vertex

#version 460

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 textureCoord;
layout(location = 2) in vec3 normal;

out vec2 TexCoord;
out vec3 Normal;

//Projection Matrix
uniform mat4 projectionMatrix = mat4(0.0);
//Transformation Matrix
uniform mat4 modelMatrix = mat4(1.0);
//Camera Transformation Matrix
uniform mat4 viewMatrix = mat4(1.0);

void main()
{
	vec4 newPos = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
	gl_Position = newPos;
	TexCoord = textureCoord;
	Normal = normal;
}

//Fragment

#version 460

layout(location = 0) out vec4 colour;

in vec2 TexCoord;
in vec3 Normal;

uniform sampler2DArray texture0;
uniform sampler2D texture1;

uniform bool hasTexture = false;
uniform bool hasNormal = false;
uniform bool canChromaKey = false;
uniform bool canDepth = true;
uniform vec3 lightDirection = vec3(0.0, 0.0, 1.0);
uniform vec3 colorTint = vec3(1.0, 1.0, 1.0);
uniform vec3 fogColour = vec3(1.0, 1.0, 1.0);
uniform vec3 chromaKey = vec3(0.0, 0.0, 0.0);
uniform int frame = 0;

float LinearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0; // back to NDC 
	return (2.0 * 0.1 * 1000) / (1000 + 0.1 - z * (1000 - 0.1));
}

void main()
{
	if (hasTexture) 
	{
		colour = texture(texture0, vec3(TexCoord, frame));
	}
	else 
	{
		colour = vec4(1.0, 1.0, 1.0, 1.0);
	}
	if (canChromaKey) 
	{
		float r = abs(colour.r - chromaKey.r);
		float g = abs(colour.g - chromaKey.g);
		float b = abs(colour.b - chromaKey.b);
		if (r < 0.1 && g < 0.1 && b < 0.1) 
		{
			discard;
		}
	}
	colour.rgb *= colorTint;
	if(hasNormal)
	{
		float lighting = dot(normalize(texture(texture1, TexCoord).rgb * 2.0 - 1.0), normalize(lightDirection));
		colour.rgb *= lighting;
	}
	if(canDepth)
	{
		float depthRaw = gl_FragCoord.z * 2.0 - 1.0;
		colour.rgb = mix(colour.rgb, fogColour,LinearizeDepth(depthRaw) / 2.0);
	}
}