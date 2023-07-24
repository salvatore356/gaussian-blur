import glsl from 'glslify';

const fragmentShader = glsl`
precision highp float;
uniform vec2 iResolution;
uniform float iTime;

uniform sampler2D iChannel0;

uniform float radius;

#define PI 3.1415926538
varying vec2 vUv;

vec4 texture(sampler2D sampler, vec2 coord, float x){
    return texture2D(sampler,  coord);
}

void  mainImage( out vec4,  vec2 fragCoord );

void main () {
    vec4 outfrag;
    vec2 fragCoord = iResolution * vUv;
    mainImage(outfrag, fragCoord);
    gl_FragColor =  outfrag;
}

float gaussianWeight(float squaredL2Norm, float sigma) {
    float sigmaSquared = sigma * sigma;
    float factor = 1.0 / (sqrt(2.0 * PI) * sigmaSquared);
    return factor * exp(-squaredL2Norm / (2.0 * sigmaSquared));
}

void mainImage(out vec4 fragColor, vec2 fragCoord){
    float r = radius, xs = 1024.0, ys = 1024.0;
    float step = 1.0 / ys;
    float x, y, xx, yy, rr = r*r;
    vec2 p;
    vec4 col = vec4(0.0, 0.0, 0.0, 0.0);

    for ( x = -r, p.x= (vUv.x)+(x*step); x <= r; x++, p.x += step ){ 
        xx = x*x;
        for ( y = -r, p.y =(vUv.y)+(y*step); y <= r; y++, p.y += step ){ 
            yy = y*y;
            col += texture2D(iChannel0, p) * gaussianWeight(xx+yy, r);
        }
    }
    fragColor = col;
}

`

export default fragmentShader