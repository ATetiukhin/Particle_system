#version 120

uniform sampler2D WaterHeightMap, WaterNormalMap;

uniform mat4x4 PhotonsWorldToTextureMatrices[6];
uniform vec3 LightPosition, CubeMapNormals[6];

vec3 IntersectCubeMap(vec3 Position, vec3 Direction)
{
	vec3 Point;

	for(int i = 0; i < 6; i++)
	{
		float NdotR = -dot(CubeMapNormals[i], Direction);

		if(NdotR > 0.0)
		{
			float Distance = (dot(CubeMapNormals[i], Position) + 1.0) / NdotR;

			if(Distance > -0.03)
			{
				Point = Direction * Distance + Position;

				if(Point.x > -1.001 && Point.x < 1.001 && Point.y > -1.001 && Point.y < 1.001 && Point.z > -1.001 && Point.z < 1.001)
				{
					break;
				}
			}
		}
	}

	return Point;
}

void main()
{
	gl_TexCoord[0].st = vec2(gl_Vertex.x * 0.5 + 0.5, 0.5 - gl_Vertex.z * 0.5);

	vec3 Position = gl_Vertex.xyz;

	Position.y += texture2D(WaterHeightMap, gl_TexCoord[0].st).g;

	vec3 Normal = normalize(texture2D(WaterNormalMap, gl_TexCoord[0].st).rgb);

	vec3 LightDirection = normalize(Position - LightPosition);

	vec3 LightDirectionRefracted = refract(LightDirection, Normal, 0.750395);

	vec3 IntersetionPoint = IntersectCubeMap(Position, LightDirectionRefracted);

	int MaxAxis = 0;

	float Axes[6] = float[](IntersetionPoint.x, -IntersetionPoint.x, IntersetionPoint.y, -IntersetionPoint.y, IntersetionPoint.z, -IntersetionPoint.z);

	for(int i = 1; i < 6; i++)
	{
		if(Axes[i] > Axes[MaxAxis])
		{
			MaxAxis = i;
		}
	}

	gl_TexCoord[0] = PhotonsWorldToTextureMatrices[MaxAxis] * vec4(IntersetionPoint, 1.0);

	gl_Position = gl_TexCoord[0] * 2.0 - 1.0;
}
