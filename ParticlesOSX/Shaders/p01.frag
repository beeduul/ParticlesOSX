#version 410

in vec3 color;
in vec3 center;
out vec4 frag_color;
in vec2 uv;

void main () {

    float border = 0.1;
    float radius = 0.5;
    
    // vec4 color0 = vec4(1-color.x, 1-color.y, 1-color.z, 0.0); // COMPLEMENT COLOR BORDER
    vec4 color0 = vec4(0.0, 0.0, 0.0, 0.0); // border

    vec3 c = color * (1.5 - uv[0]);
    vec4 color1 = vec4(c, 1);
    
    vec2 m = uv - vec2(0.5, 0.5);

    // float dist = (m.x * m.x + m.y * m.y); // HOLES IN THE CENTER OF SQUARES
    float dist = radius - sqrt(m.x * m.x + m.y * m.y);

    float t = 0.0;
    if (dist > border)
        t = 1.0;
    else if (dist > 0.0)
        t = dist / border;
    else
        discard;
    
    frag_color = mix(color0, color1, t);


    // plain ol square
//    frag_color = vec4(color, 1.0);
}
