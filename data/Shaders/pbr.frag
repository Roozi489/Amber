#version 420 core

in vec3 position_Worldspace;
in vec2 uv;
in vec3 normal_Worldspace;

in vec3 position_Modelspace;
in vec3 normal_Modelspace;

out vec3 color;

uniform sampler2D texSampler;
uniform samplerCube cubeTexture;

uniform mat4 M;
uniform mat4 V;
uniform float roughness = 0.0;

float PI = 3.14159265358979323846264;
float diffuseCoeficient = 0.5;
float specularIBLCoeficient = 0.5;


float clamp01(float value)
{
	return clamp(value, 0.0, 1.0);
}

// Taken from http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
float radicalInverse_VdC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley(uint i, uint N)
{
	return vec2(float(i)/float(N), radicalInverse_VdC(i));
}

float G_1(float k, float dotProduct)
{
	return dotProduct / (dotProduct * (1 - k) + k);
}

// Taken from http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
float G_Smith(float Roughness, float NoV, float NoL)
{
	float k = Roughness;
	// Remap roughness when not using image based lighting
	//k = ((Roughness + 1) * (Roughness + 1)) / 8;
	return G_1(k, NoV) * G_1(k, NoL);
}

// Taken from http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
vec3 ImportanceSampleGGX( vec2 Xi, float Roughness, vec3 N )
{
	float a = Roughness * Roughness;
	float Phi = 2 * PI * Xi.x;
	float CosTheta = sqrt( (1 - Xi.y) / ( 1 + (a*a - 1) * Xi.y ) );
	float SinTheta = sqrt( 1 - CosTheta * CosTheta );
	vec3 H;
	H.x = SinTheta * cos( Phi );
	H.y = SinTheta * sin( Phi );
	H.z = CosTheta;
	vec3 UpVector = abs(N.z) < 0.999 ? vec3(0,0,1) : vec3(1,0,0);
	vec3 TangentX = normalize( cross( UpVector, N ) );
	vec3 TangentY = cross( N, TangentX );
	// Tangent to world space
	return vec3(TangentX * H.x + TangentY * H.y + N * H.z);
}

vec3 SpecularIBL( vec3 SpecularColor, float Roughness, vec3 N, vec3 V )
{
	vec3 SpecularLighting = vec3(0.0);
	const uint NumSamples = 512;
	for( uint i = 0; i < NumSamples; i++ )
	{
		vec2 Xi = Hammersley( i, NumSamples );
		vec3 H = ImportanceSampleGGX( Xi, Roughness, N );
		vec3 L = 2 * dot( V, H ) * H - V;
		float NoV = clamp01( dot( N, V ) );
		float NoL = clamp01( dot( N, L ) );
		float NoH = clamp01( dot( N, H ) );
		float VoH = clamp01( dot( V, H ) );
		if( NoL > 0 )
		{
			vec3 SampleColor = texture(cubeTexture, L).rgb;
			float G = G_Smith( Roughness, NoV, NoL );
			float Fc = pow( 1 - VoH, 5 );
			vec3 F = (1 - Fc) * SpecularColor + Fc;
			// Incident light = SampleColor * NoL
			// Microfacet specular = D*G*F / (4*NoL*NoV)
			// pdf = D * NoH / (4 * VoH)
			SpecularLighting += SampleColor * F * G * VoH / (NoH * NoV);
		}
	}
	return SpecularLighting / NumSamples;
}

void main()
{
    //vec3 diffuseColor = texture2D(texSampler, uv).rgb;
	//vec3 ambientColor = vec3(0.2, 0.2, 0.2) * diffuseColor;
	//vec3 specularColor = vec3(0.2, 0.2, 0.2);
	
	//color = ambientColor + colorLight0 + colorLight1 + colorLight2;
	vec3 whiteColor = vec3(1.0);
	color = SpecularIBL(whiteColor, roughness, position_Modelspace, normal_Modelspace);
}