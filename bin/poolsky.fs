#version 120

uniform samplerCube PoolSkyCubeMap, PhotonsCubeMap;

void main()
{
	gl_FragColor = textureCube(PoolSkyCubeMap, gl_TexCoord[0].stp) + textureCube(PhotonsCubeMap, gl_TexCoord[0].stp);
}
