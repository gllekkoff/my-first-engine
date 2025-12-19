#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
    mat3 TBN;
} vs_out;

const float PI = 3.14159265359;

struct PBRMaterial {
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;

    sampler2D albedoMap;
    sampler2D normalMap;
    sampler2D metallicMap;
    sampler2D roughnessMap;
    sampler2D aoMap;

    bool hasAlbedoMap;
    bool hasNormalMap;
    bool hasMetallicMap;
    bool hasRoughnessMap;
    bool hasAOMap;
};

struct DirLight {
    vec3 direction;
    vec3 color;
    float intensity;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float constant;
    float linear;
    float quadratic;
};

uniform PBRMaterial pbrMaterial;
uniform DirLight dirLight;
uniform PointLight pointLights[2];
uniform vec3 viewPos;
uniform sampler2D shadowMap;
uniform bool usePCF;
uniform float shadowBias;

vec3 getNormalFromMap() {
    if (!pbrMaterial.hasNormalMap) {
        return normalize(vs_out.Normal);
    }

    vec3 tangentNormal = texture(pbrMaterial.normalMap, vs_out.TexCoords).xyz * 2.0 - 1.0;
    return normalize(vs_out.TBN * tangentNormal);
}

vec3 getAlbedo() {
    if (pbrMaterial.hasAlbedoMap) {
        vec3 texColor = texture(pbrMaterial.albedoMap, vs_out.TexCoords).rgb;
        return texColor * 3.0;
    }
    return pbrMaterial.albedo;
}

float getMetallic() {
    if (pbrMaterial.hasMetallicMap) {
        return texture(pbrMaterial.metallicMap, vs_out.TexCoords).r;
    }
    return pbrMaterial.metallic;
}

float getRoughness() {
    if (pbrMaterial.hasRoughnessMap) {
        return texture(pbrMaterial.roughnessMap, vs_out.TexCoords).r;
    }
    return pbrMaterial.roughness;
}

float getAO() {
    if (pbrMaterial.hasAOMap) {
        return texture(pbrMaterial.aoMap, vs_out.TexCoords).r;
    }
    return pbrMaterial.ao;
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / max(denom, 0.0000001);
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / max(denom, 0.0000001);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 ||
    projCoords.y < 0.0 || projCoords.y > 1.0)
    return 0.0;

    float currentDepth = projCoords.z;
    float bias = max(shadowBias * (1.0 - dot(normal, lightDir)), shadowBias * 0.1);

    if (usePCF) {
        float shadow = 0.0;
        vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
        for(int x = -1; x <= 1; ++x) {
            for(int y = -1; y <= 1; ++y) {
                float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
                shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
            }
        }
        return shadow / 9.0;
    } else {
        float closestDepth = texture(shadowMap, projCoords.xy).r;
        return currentDepth - bias > closestDepth ? 1.0 : 0.0;
    }
}

vec3 calculatePBR(vec3 N, vec3 V, vec3 L, vec3 radiance, float shadow, vec3 albedo, float metallic, float roughness) {
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    vec3 H = normalize(V + L);

    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    float NdotL = max(dot(N, L), 0.0);
    return (kD * albedo / PI + specular) * radiance * NdotL * (1.0 - shadow);
}

void main() {
    vec3 N = getNormalFromMap();
    vec3 V = normalize(viewPos - vs_out.FragPos);

    vec3 albedo = getAlbedo();
    float metallic = getMetallic();
    float roughness = max(getRoughness(), 0.05);
    float ao = getAO();

    vec3 Lo = vec3(0.0);

    vec3 dirLightDir = normalize(-dirLight.direction);
    float shadow = ShadowCalculation(vs_out.FragPosLightSpace, N, dirLightDir);
    vec3 dirRadiance = dirLight.color * dirLight.intensity;
    Lo += calculatePBR(N, V, dirLightDir, dirRadiance, shadow, albedo, metallic, roughness);

    for(int i = 0; i < 2; ++i) {
        vec3 L = normalize(pointLights[i].position - vs_out.FragPos);
        float distance = length(pointLights[i].position - vs_out.FragPos);
        float attenuation = 1.0 / (pointLights[i].constant + pointLights[i].linear * distance +
        pointLights[i].quadratic * (distance * distance));
        vec3 radiance = pointLights[i].color * pointLights[i].intensity * attenuation;
        Lo += calculatePBR(N, V, L, radiance, 0.0, albedo, metallic, roughness);
    }

    vec3 ambient = vec3(0.3) * albedo * ao;
    vec3 color = ambient + Lo;

    FragColor = vec4(color, 1.0);
}
