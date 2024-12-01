// uniform vec2 pan;
// uniform float zoom;
// uniform float aspect;
// uniform float uTime;
attribute vec4 position;
varying vec2 texCoord;
void main()
{
    gl_Position = vec4(position.xyz, 1.0);
    // gl_Position.xy += pan;
    // gl_Position.xy *= zoom;
    texCoord = vec2(gl_Position.x, -gl_Position.y);
    // gl_Position.y *= aspect;
}
