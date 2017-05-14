#version 120

uniform samplerCube CubeMap;

uniform vec3 Offset;

void main()
{
	float Color = 0.0;

	for(float i = -3.0; i <= 3.0; i += 1.0)
	{
		Color += textureCube(CubeMap, Offset * i + gl_TexCoord[0].stp).r * (4.0 - abs(i));
	}

	gl_FragColor = vec4(Color / 16.0, 0.0, 0.0, 1.0);
}
