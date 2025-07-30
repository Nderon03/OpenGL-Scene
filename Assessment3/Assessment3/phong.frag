#version 330 core

layout (location = 0) out vec4 fColour;

in vec2 TexCoords;
in vec3 nor;
in vec3 FragPosWorldSpace;
in vec4 FragPosProjectedLightSpace;

uniform sampler2D Texture;
uniform sampler2D shadowMap;

uniform vec3 lightDirection;
uniform vec3 lightColour;
uniform vec3 camPos;
uniform vec3 lightPos;


float shadowOnFragment(vec4 FragPosProjectedLightSpace)
{
    vec3 ndc = FragPosProjectedLightSpace.xyz / FragPosProjectedLightSpace.w;
    vec3 ss = (ndc + 1) * 0.5;
     
    float fragDepth = ss.z;
    
    float bias = max(0.05 * (1.0 - dot(normalize(nor), normalize(-lightDirection))), 0.005);
    
    float shadow = 0.0;
    
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float litDepth = texture(shadowMap, ss.xy + vec2(x, y) * texelSize).r;
            shadow += fragDepth > (litDepth + bias) ? 1.0 : 0.0;
        }
    }
    
    shadow /= 9.0; // Average of samples
    if(fragDepth > 1)
        shadow = 0.f;
    return shadow;
}


float CalculateDirectionalIllumination()
{
    float ambient = 0.1;
    vec3 Nnor = normalize(nor);
    vec3 Nto_light = normalize(-lightDirection);
    float diffuse = max(dot(Nnor, Nto_light), 0.0);

    vec3 Nfrom_light = normalize(lightDirection);
    vec3 NrefLight = reflect(-Nfrom_light, Nnor);
    vec3 camDirection = normalize(camPos - FragPosWorldSpace);
    float specular = pow(max(dot(NrefLight, camDirection), 0.0), 128);

    float shadow = shadowOnFragment(FragPosProjectedLightSpace);
    
    float phong = ambient + ((1.f-shadow)*(diffuse + specular));
    return phong;
}

float CalculatePositionalIllumination(){

    float ambient = 0.1;
    vec3 Nnor = normalize(nor);
    vec3 Nto_light = normalize(lightPos - FragPosWorldSpace);
    float diffuse = max(dot(Nnor, Nto_light), 0.0);

    vec3 Nfrom_light = -Nto_light;
    vec3 NrefLight = reflect(-Nfrom_light, Nnor);
    vec3 camDirection = normalize(camPos - FragPosWorldSpace);
    float specular = pow(max(dot(NrefLight, camDirection), 0.0), 128);

     
    float constant = 1.0;
    float linear = 0.05;
    float quadratic = 0.002;
    float distance = length(lightPos - FragPosWorldSpace);
    float attenuation = 1.0 / (constant + (linear * distance) + (quadratic * distance * distance));

    float shadow = shadowOnFragment(FragPosProjectedLightSpace);

    // Apply shadow to the final illumination
    float phong = (ambient + ((1.0 - shadow) * (diffuse + specular))) * attenuation;
    return phong;

}

float CalculateSpotIllumination(){

    float ambient = 0.1;
    vec3 Nnor = normalize(nor);
    vec3 Nto_light = normalize(lightPos - FragPosWorldSpace);
    float diffuse = max(dot(Nnor, Nto_light), 0.0);

    vec3 Nfrom_light = -Nto_light;
    vec3 NrefLight = reflect(-Nfrom_light, Nnor);
    vec3 camDirection = normalize(camPos - FragPosWorldSpace);
    float specular = pow(max(dot(NrefLight, camDirection), 0.0), 128);

     
    float constant = 1.0;
    float linear = 0.05;
    float quadratic = 0.002;
    float distance = length(lightPos - FragPosWorldSpace);
    float attenuation = 1.0 / (constant + (linear * distance) + (quadratic * distance * distance));

    float phi = cos(radians(15.0));
    vec3 NSpotDir = normalize(lightDirection);
    float theta = dot(Nfrom_light,NSpotDir);

    float shadow = shadowOnFragment(FragPosProjectedLightSpace);

    float phong; 

    if(theta > phi)
    {
            phong = (ambient + ((1.0 - shadow) * (diffuse + specular))) * attenuation;
    }else
    {
            phong = ambient * attenuation;
    }
    return phong;

}

uniform bool spotlightActive;
uniform bool positionalActive;

void main()
{
    vec3 color = texture(Texture, TexCoords).rgb;
    float phong;

    // Check if spotlight is active
    if (spotlightActive) {
        phong = CalculateSpotIllumination();
    } 
    else if (positionalActive){
        phong = CalculatePositionalIllumination();
    }
    else {
        phong = CalculateDirectionalIllumination();
    }

    /*
    if(color.r == 1.0 && color.g == 1.0 && color.b == 1.0 && texture(Texture, TexCoords).a == 1.0)
        fColour = vec4(phong * color * lightColour, 1.f);
    else
        fColour = vec4(phong * color, texture(Texture, TexCoords).a);
    */
    fColour = vec4(phong * color * lightColour, texture(Texture, TexCoords).a);
}