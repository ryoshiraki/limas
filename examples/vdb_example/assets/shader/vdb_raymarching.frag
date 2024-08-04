#version 400

in vec3 v_texcoord;
in vec3 v_position;
out vec4 f_color;

uniform sampler3D albedoSampler;

vec4 getColorAlongRayStep(vec3 normalizedVolumeCoord, float stepLength, float extinctionCoefficient, RayContext rayContext)
{
	vec4 albedoSample = texturePcf2(albedoSampler, normalizedVolumeCoord, oneOnVolumeTextureSize / 2.0f);
	float alpha = albedoSample.r * opacityMultiplier;

	if (alpha > 0.001)
	{
		float deltaTransmissivity = exp(-extinctionCoefficient * stepLength * alpha);
		
		vec3 shadowTexcoord = calcShadowTexCoord(shadowModelViewProj, normalizedVolumeCoord, volumeSize_modelSpace);
		float visibility = getVisibility_dsm(shadowSampler, shadowTexcoord);
		
		return vec4(diffuseColor * visibility, 1.0 - deltaTransmissivity);
	}
	else
	{
		return vec4(0);
	}
}


void main() {
//  f_color = texture(u_tex, v_texcoord) * v_color;
  f_color = v_color;
}

