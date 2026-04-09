#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D u_inputTexture;
uniform int u_colorSpace;          // 0=SDR, 1=scRGB, 2=PQ
uniform float u_displayMaxNits;    // ui:min=80 max=2000 step=10
uniform float u_sdrReferenceWhite; // ui:min=40 max=200 step=1

// Color matrices (D65)
const mat3 REC709_to_REC2020 = mat3(
    0.627404, 0.329283, 0.043313,
    0.069097, 0.919540, 0.011363,
    0.016391, 0.088013, 0.895596
);

vec3 srgb_to_linear(vec3 c) {
    return mix(c / 12.92, pow((c + 0.055) / 1.055, vec3(2.4)), step(0.04045, c));
}

vec3 linear_to_srgb(vec3 c) {
    return mix(c * 12.92, 1.055 * pow(c, vec3(1.0/2.4)) - 0.055, step(0.0031308, c));
}

vec3 tone_map_aces(vec3 x) {
    const float a = 2.51, b = 0.03, c = 2.43, d = 0.59, e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main() {
    vec4 color = texture(u_inputTexture, TexCoord);
    vec3 rgb = max(color.rgb, 0.0);
    
    vec3 linear = srgb_to_linear(rgb);
    linear = REC709_to_REC2020 * linear;
    linear = max(linear, 0.0);
    
    vec3 normalized = linear * (u_sdrReferenceWhite / u_displayMaxNits);
    vec3 mapped = tone_map_aces(normalized);
    vec3 final = linear_to_srgb(mapped);
    
    FragColor = vec4(clamp(final, 0.0, 1.0), color.a);
}
