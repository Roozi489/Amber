void main()
{
	vec3 normal = normalize( vec3(input.InterpolatedPosition.xy, 1) );
    if(cubeFace==2)
        normal = normalize( vec3(input.InterpolatedPosition.x,  1, -input.InterpolatedPosition.y) );
    else if(cubeFace==3)
        normal = normalize( vec3(input.InterpolatedPosition.x, -1,  input.InterpolatedPosition.y) );
    else if(cubeFace==0)
        normal = normalize( vec3(  1, input.InterpolatedPosition.y,-input.InterpolatedPosition.x) );
    else if(cubeFace==1)
        normal = normalize( vec3( -1, input.InterpolatedPosition.y, input.InterpolatedPosition.x) );
    else if(cubeFace==5)
        normal = normalize( vec3(-input.InterpolatedPosition.x, input.InterpolatedPosition.y, -1) );

    vec3 up = vec3(0,1,0);
    vec3 right = normalize(cross(up,normal));
    up = cross(normal,right);

    vec3 sampledColour = vec3(0,0,0);
    float index = 0;
    for(float phi = 0; phi < 6.283; phi += 0.025)
    {
        for(float theta = 0; theta < 1.57; theta += 0.1)
        {
            vec3 temp = cos(phi) * right + sin(phi) * up;
            vec3 sampleVector = cos(theta) * normal + sin(theta) * temp;
            sampledColour += texCUBE( diffuseCubemap_Sampler, sampleVector ).rgb * cos(theta) * sin(theta);
            index ++;
        }
    }

    return vec4( PI * sampledColour / index), 1 );
}