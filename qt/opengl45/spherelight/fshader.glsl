#version 450

uniform mat4 model; // Only the rotation matrix, NOT including the projection matrix

// Don't know how to pass
uniform struct Light {
   vec3 position;
   vec3 intensities; // a.k.a the color of the light
} light;

//in vec2 fragTexCoord;

in VERTEX
{
    vec4 position;
    vec4 normal;
    vec4 color;
} vertex;

out vec4 finalColor;

void main() {
    // calculate normal in world coordinates
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 fragNormal = vertex.normal.xyz;
    vec3 normal = normalize(normalMatrix * fragNormal);

    //calculate the location of this fragment (pixel) in world coordinates
    vec3 fragPosition = vec3(model * vertex.position);

    //calculate the vector from this pixels surface to the light source
    vec3 surfaceToLight = light.position - fragPosition;

    //calculate the cosine of the angle of incidence
    float brightness = dot(normal, surfaceToLight) / (length(surfaceToLight) * length(normal));
    brightness = clamp(brightness, 0, 1);

    // calculate final color of the pixel, based on:
    // 1. The angle of incidence: brightness
    // 2. The color/intensities of the light: light.intensities
    // 3. The texture and texture coord: texture(tex, fragTexCoord)
    //vec4 surfaceColor = texture(tex, fragTexCoord); // commented out; no texture
    finalColor = vec4(brightness * light.intensities * vertex.color.rgb, vertex.color.a);
}
