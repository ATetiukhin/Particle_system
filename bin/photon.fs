#version 120

uniform sampler2D PhotonsTexture;

void main()
{
	gl_FragColor = vec4(texture2D(PhotonsTexture, gl_TexCoord[0].st).rgb + 0.0625, 1.0);
}
