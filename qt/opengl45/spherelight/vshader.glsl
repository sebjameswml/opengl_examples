#version 450

// ProjMatrix * RotnMatrix operation carried out on CPU in this
// program, so only one mvp_matrix.
uniform mat4 mvp_matrix;

in vec4 position;
in vec4 normalin;
in vec4 color;
out VERTEX
{
    vec4 position;
    vec4 normal;
    vec4 color;
} vertex;

void main (void)
{
    vertex.position = (mvp_matrix * position);
    gl_Position = vertex.position;
    vertex.color = color;
    // Normals are all automatically computed, so there's no need for
    // this line and the cube program doesn't bother to pass in the
    // normals. Maybe required only for lighting?
    vertex.normal = mvp_matrix * normalin;
}
