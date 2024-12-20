precision mediump float;                            
precision mediump int;

varying vec2 texCoord;                              
uniform float aspect;
uniform float time_mil;

uniform float camPos[3];
vec3 gCamPos = vec3(camPos[0], camPos[1], camPos[2]);

uniform float mouseDelta[2];
vec2 gMouseDelta = vec2(mouseDelta[0], mouseDelta[1]);

float time_sec = time_mil/1000.0;
// uniform sampler2D texSampler;                       

mat2 rot2D(float angle){
  float s = sin(angle);
  float c = cos(angle);
  return mat2(c, -s, s, c);
}

float sdBox( vec3 p, vec3 b )
{
    vec3 q = abs(p) - b;
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

float sdSphere( vec3 p, float s )
{
  return length(p)-s;
}

float smin( float a, float b, float k )
{
    float res = exp2( -k*a ) + exp2( -k*b );
    return -log2( res )/k;
}

float map(vec3 p)
{
  
  
    vec3 spherePos = vec3(sin(time_sec)*3.0, 0.5, cos(time_sec)*3.0);
    float sphere = sdSphere(p - spherePos, 1.0);
  
    vec3 q = p;
  
    // q.x += sin(time_sec);

    q.y -= time_sec * 0.2;
    
    q = fract(q) - 0.5;
  
    float box1 = sdBox(q, vec3(0.1));

    float ground = p.y + 0.75;

    float dist = smin(ground, smin(sphere, box1, 10.0), 5.0);
    // float dist = smin(ground, box1, 5.0);
    return dist;
}

vec3 palette(float t){
  vec3 a = vec3(0.5, 0.5, 0.5);
  vec3 b = vec3(0.5, 0.5, 0.5);
  vec3 c = vec3(1.0, 1.0, 1.0);
  vec3 d = vec3(0.263, 0.416, 0.557);
  return a + b * cos(6.28318 * (c * t + d));
}

void main()                                         
{                                                   
    // vec2 uv = (texCoord * 2.0 * -vec2(aspect, 1.0)) / 1.0; 
    vec2 uv = texCoord * 2.0 * -vec2(aspect, 1.0); 
      
    vec3 ro = vec3(0, 0, -5);         // ray origin
    vec3 rd = normalize(vec3(uv*0.5, 1)); // ray direction, adjusting FOV with miltiplier
    vec3 col = vec3(1.0);            // pixel color

    ro += gCamPos;

    rd.yz *= rot2D(gMouseDelta.y);
    rd.xz *= rot2D(-gMouseDelta.x);

    float t = 0.0;                   // travelled distance of ray

    // Raymarching
    int i;
    i = 0;
    for (int j = 0; j < 50; j++) {
        vec3 p = ro + rd * t;           // position along the ray 
      
        float d = map(p);               // current distance to the scene

        t += d;                         // march the ray
    
        i = j;
        if(d < 0.001 || t > 80.0) break;
    }
      
    col = vec3(t* 0.04 + float(i)*0.004);
    col = palette(t * 0.04 + float(i)*0.005);

    // col = vec3(t * 0.2);
    
      
    gl_FragColor = vec4(col, 1.0);
}                                                   
