#version 330 core

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 Tex;
    vec4 FragPosLightSpace;
} fs;

struct Material {
    vec3 diffuse;
    vec3 specular;
    float shininess;
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
};

uniform Material material;
uniform sampler2D floorTexture;
uniform bool useModelTexture;
uniform bool useFloorTiling;
uniform float floorTileScale;
uniform vec3 viewPos;

struct DirLight {
    vec3 direction;
    vec3 color;
    float intensity;
};

struct PointLight{
    vec3 position;
    vec3 color;
    float intensity;
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
    float cutOff;
    float outerCutOff;
    float constant;
    float linear;
    float quadratic;
};

uniform DirLight dirLight;
uniform PointLight pointLights[2];
uniform SpotLight spotLight;
uniform sampler2D shadowMap;
uniform bool usePCF;
uniform float shadowBias;
uniform float materialAlpha;

uniform sampler2D bayerTexture;
uniform bool enableDithering;
uniform float ditherStrength;
uniform int ditherLevels;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    if(projCoords.z > 1.0)
    return 0.0;
    if(projCoords.x < 0.0 || projCoords.x > 1.0 ||
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

vec3 getDiffuseColor() {
    if (useFloorTiling) {
        vec2 tiled = fs.FragPos.xz / max(floorTileScale, 0.0001);
        return texture(floorTexture, tiled).rgb;
    }

    if (useModelTexture) {
        return texture(material.texture_diffuse1, fs.Tex).rgb;
    }

    return material.diffuse;
}

vec3 getSpecularColor() {
    if (useModelTexture) {
        return material.specular * texture(material.texture_specular1, fs.Tex).rgb;
    }

    return material.specular;
}

vec3 blinnPhong(vec3 Ldir, vec3 Vdir, vec3 N, vec3 Lrgb, float Li) {
    vec3 kd = getDiffuseColor();
    vec3 ks = getSpecularColor();
    float NdotL = max(dot(N, Ldir), 0.0);
    vec3 ambient = 0.03 * kd * Lrgb;
    vec3 diffuse = NdotL * kd * Lrgb;
    vec3 H = normalize(Ldir + Vdir);
    float NdotH = max(dot(N, H), 0.0);
    vec3 spec = pow(NdotH, max(material.shininess, 1.0)) * ks * Lrgb;
    return Li * (ambient + diffuse + spec);
}

vec3 CalcDirLight(DirLight L, vec3 N, vec3 V) {
    return blinnPhong(normalize(-L.direction), V, N, L.color, L.intensity);
}

vec3 CalcPointLight(PointLight L, vec3 N, vec3 V, vec3 P) {
    vec3 toL = L.position - P;
    float d = length(toL);
    vec3 Ldir= toL / max(d, 1e-6);
    float att = 1.0 / (L.constant + L.linear*d + L.quadratic*d*d);
    return att * blinnPhong(Ldir, V, N, L.color, L.intensity);
}

vec3 CalcSpotLight(SpotLight L, vec3 N, vec3 V, vec3 P) {
    vec3 toL = L.position - P;
    float d = length(toL);
    vec3 Ldir= toL / max(d, 1e-6);
    float theta = dot(Ldir, normalize(-L.direction));
    float eps = max(L.cutOff - L.outerCutOff, 1e-4);
    float smoothiness = clamp((theta - L.outerCutOff) / eps, 0.0, 1.0);
    float att = 1.0 / (L.constant + L.linear*d + L.quadratic*d*d);
    return smoothiness * att * blinnPhong(Ldir, V, N, L.color, L.intensity);
}

vec3 applyDithering(vec3 color) {
    vec2 bayerCoord = gl_FragCoord.xy / 8.0;
    float bayerValue = texture(bayerTexture, bayerCoord).r;

    float threshold = (bayerValue / 64.0 - 0.5) * ditherStrength;

    float levels = float(ditherLevels);
    vec3 quantized = floor(color * levels + threshold) / levels;

    return clamp(quantized, 0.0, 1.0);
}

void main() {
    vec3 N = normalize(fs.Normal);
    vec3 V = normalize(viewPos - fs.FragPos);
    vec3 color = vec3(0.0);

    float shadow = ShadowCalculation(fs.FragPosLightSpace, N, -dirLight.direction);
    color += CalcDirLight(dirLight, N, V) * (1.0 - shadow);
    color += CalcPointLight(pointLights[0], N, V, fs.FragPos);
    color += CalcPointLight(pointLights[1], N, V, fs.FragPos);
    color += CalcSpotLight(spotLight, N, V, fs.FragPos);

    color = clamp(color, 0.0, 1.0);

    if (enableDithering) {
        color = applyDithering(color);
    }

    FragColor = vec4(color, materialAlpha);
}
