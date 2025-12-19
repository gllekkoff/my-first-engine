#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform int tonemapAlgorithm;
uniform float exposure;
uniform float gamma;
uniform bool fxaaEnabled;
uniform vec2 screenSize;

vec3 noTonemap(vec3 color) {
    return color;
}

vec3 reinhardTonemap(vec3 color) {
    return color / (color + vec3(1.0));
}

vec3 reinhardLuma(vec3 color) {
    float luma = dot(color, vec3(0.2126, 0.7152, 0.0722));
    float toneMappedLuma = luma / (1.0 + luma);
    return color * (toneMappedLuma / luma);
}

vec3 acesTonemap(vec3 color) {
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((color * (a * color + b)) / (color * (c * color + d) + e), 0.0, 1.0);
}

vec3 uncharted2Tonemap(vec3 x) {
    const float A = 0.15;
    const float B = 0.50;
    const float C = 0.10;
    const float D = 0.20;
    const float E = 0.02;
    const float F = 0.30;
    return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

vec3 filmicTonemap(vec3 color) {
    color = max(vec3(0.0), color - vec3(0.004));
    color = (color * (6.2 * color + 0.5)) / (color * (6.2 * color + 1.7) + 0.06);
    return color;
}

vec3 applyTonemapping(vec3 hdrColor) {
    vec3 mapped;

    if (tonemapAlgorithm == 0) {
        mapped = noTonemap(hdrColor);
    } else if (tonemapAlgorithm == 1) {
        mapped = reinhardTonemap(hdrColor * exposure);
    } else if (tonemapAlgorithm == 2) {
        mapped = acesTonemap(hdrColor * exposure);
    } else if (tonemapAlgorithm == 3) {
        hdrColor *= exposure;
        vec3 curr = uncharted2Tonemap(hdrColor);
        vec3 whiteScale = vec3(1.0) / uncharted2Tonemap(vec3(11.2));
        mapped = curr * whiteScale;
    } else if (tonemapAlgorithm == 4) {
        mapped = filmicTonemap(hdrColor * exposure);
    } else if (tonemapAlgorithm == 5) {
        mapped = reinhardTonemap(hdrColor * exposure);
    } else {
        mapped = reinhardLuma(hdrColor * exposure);
    }

    return pow(mapped, vec3(1.0 / gamma));
}

vec3 fxaa(sampler2D tex, vec2 fragCoord, vec2 resolution) {
    vec2 inverseVP = 1.0 / resolution;
    vec3 rgbNW = texture(tex, (fragCoord + vec2(-1.0, -1.0)) * inverseVP).rgb;
    vec3 rgbNE = texture(tex, (fragCoord + vec2(1.0, -1.0)) * inverseVP).rgb;
    vec3 rgbSW = texture(tex, (fragCoord + vec2(-1.0, 1.0)) * inverseVP).rgb;
    vec3 rgbSE = texture(tex, (fragCoord + vec2(1.0, 1.0)) * inverseVP).rgb;
    vec3 rgbM  = texture(tex, fragCoord * inverseVP).rgb;

    vec3 luma = vec3(0.299, 0.587, 0.114);
    float lumaNW = dot(rgbNW, luma);
    float lumaNE = dot(rgbNE, luma);
    float lumaSW = dot(rgbSW, luma);
    float lumaSE = dot(rgbSE, luma);
    float lumaM  = dot(rgbM,  luma);

    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

    vec2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));

    float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * 0.15), 0.0078125);
    float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);

    dir = min(vec2(8.0, 8.0), max(vec2(-8.0, -8.0), dir * rcpDirMin)) * inverseVP;

    vec3 rgbA = 0.5 * (
    texture(tex, fragCoord * inverseVP + dir * (1.0 / 3.0 - 0.5)).rgb +
    texture(tex, fragCoord * inverseVP + dir * (2.0 / 3.0 - 0.5)).rgb);

    vec3 rgbB = rgbA * 0.5 + 0.25 * (
    texture(tex, fragCoord * inverseVP + dir * -0.5).rgb +
    texture(tex, fragCoord * inverseVP + dir * 0.5).rgb);

    float lumaB = dot(rgbB, luma);

    if ((lumaB < lumaMin) || (lumaB > lumaMax)) {
        return rgbA;
    } else {
        return rgbB;
    }
}

void main() {
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
    vec3 color = applyTonemapping(hdrColor);

    if (fxaaEnabled) {
        color = fxaa(hdrBuffer, gl_FragCoord.xy, screenSize);
        color = applyTonemapping(color);
    }

    FragColor = vec4(color, 1.0);
}
