#version 410

in vec3 color;
in vec3 center;
out vec4 frag_color;
in vec2 uv;

void main () {

    float border = 0.05;
    float radius = 0.5;
    vec4 color0 = vec4(0.0, 0.0, 0.0, 0.0); // border
    vec4 color1 = vec4(color, .5);

    vec2 m = uv - vec2(0.5, 0.5);
    float dist = radius - sqrt(m.x * m.x + m.y * m.y);

    float t = 0.0;
    if (dist > border)
        t = 1.0;
    else if (dist > 0.0)
        t = dist / border;
    
    frag_color = mix(color0, color1, t);


    // plain ol square
//    frag_color = vec4(color, 1.0);
}
