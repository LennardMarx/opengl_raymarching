precision mediump float;

// Screen-space UV coordinates (from vertex shader)
varying vec2 texCoord;

// Signed distance function for a cube
float sdBox(vec3 p, vec3 b) {
    vec3 q = abs(p) - b;                  // Distance from the surface of the cube
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

// Raymarching function
float rayMarch(vec3 ro, vec3 rd, out vec3 hitPos) {
    const int MAX_STEPS = 100;
    const float MAX_DIST = 50.0;
    const float SURFACE_DIST = 0.001;    // Distance threshold to consider "hit"
    
    float t = 0.0;                       // Total distance traveled along the ray
    for (int i = 0; i < MAX_STEPS; i++) {
        vec3 p = ro + rd * t;            // Current point on the ray
        float d = sdBox(p, vec3(1.0));   // Cube size (1 unit in each direction)
        
        if (d < SURFACE_DIST) {          // Close enough to the surface
            hitPos = p;
            return t;
        }
        if (t > MAX_DIST) break;         // Ray has traveled too far
        t += d;                          // Advance the ray
    }
    return -1.0;                         // Return -1 if no hit
}

// Calculate the normal vector at a point on the surface
vec3 estimateNormal(vec3 p) {
    const vec3 eps = vec3(0.001, 0.0, 0.0); // Small offset for numerical gradient
    float dx = sdBox(p + eps.xyy, vec3(1.0)) - sdBox(p - eps.xyy, vec3(1.0));
    float dy = sdBox(p + eps.yxy, vec3(1.0)) - sdBox(p - eps.yxy, vec3(1.0));
    float dz = sdBox(p + eps.yyx, vec3(1.0)) - sdBox(p - eps.yyx, vec3(1.0));
    return normalize(vec3(dx, dy, dz));
}

// Soft shadow computation
float softShadow(vec3 ro, vec3 rd) {
    float res = 1.0;
    float t = 0.1;
    for (int i = 0; i < 30; i++) {
        vec3 p = ro + rd * t;
        float d = sdBox(p, vec3(1.0));
        if (d < 0.001) return 0.0;
        res = min(res, 10.0 * d / t);
        t += d;
    }
    return res;
}

// Basic lighting (Lambertian + shadow)
vec3 computeLighting(vec3 p, vec3 normal, vec3 lightDir, vec3 viewDir) {
    float ambient = 0.1;                    // Ambient light
    float diffuse = max(dot(normal, lightDir), 0.0); // Diffuse light
    float shadow = softShadow(p + normal * 0.01, lightDir); // Soft shadow
    vec3 lighting = vec3(ambient + diffuse * shadow);       // Combine
    return lighting;
}

void main() {
    // Convert texCoord to normalized device coordinates (NDC)
    vec2 uv = texCoord * 2.0 - 1.0;      // Map [0, 1] to [-1, 1]
    uv.x *= 800.0 / 600.0;               // Adjust for aspect ratio (width / height)

    // Define the ray origin and direction
    vec3 ro = vec3(0.0, 0.0, -5.0);      // Camera position (backward along Z-axis)
    vec3 rd = normalize(vec3(uv, 1.0)); // Ray direction (into the screen)

    // Perform raymarching
    vec3 hitPos;                         // Position of the ray hit
    float t = rayMarch(ro, rd, hitPos);

    // Initialize the fragment color
    vec3 color = vec3(0.0);              // Default to black (no hit)

    if (t > 0.0) {                       // If the ray hit the cube
        vec3 normal = estimateNormal(hitPos);   // Compute the surface normal
        vec3 lightDir = normalize(vec3(-0.5, 0.8, -1.0)); // Light direction
        vec3 viewDir = normalize(-rd);          // View direction
        color = computeLighting(hitPos, normal, lightDir, viewDir) * vec3(1.0, 0.5, 0.2); // Orange-ish
    } else {
        // Background gradient
        color = mix(vec3(0.2, 0.4, 0.6), vec3(0.8, 0.9, 1.0), uv.y * 0.5 + 0.5); // Sky color
    }

    gl_FragColor = vec4(color, 1.0);     // Output the fragment color
}






// precision mediump float;
//
// // Screen-space UV coordinates (from vertex shader)
// varying vec2 texCoord;
//
// // Signed distance function for a cube
// float sdBox(vec3 p, vec3 b) {
//     vec3 q = abs(p) - b;                  // Distance from the surface of the cube
//     return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
// }
//
// // Raymarching function
// float rayMarch(vec3 ro, vec3 rd, out vec3 hitPos) {
//     const int MAX_STEPS = 100;
//     const float MAX_DIST = 50.0;
//     const float SURFACE_DIST = 0.001;    // Distance threshold to consider "hit"
//     
//     float t = 0.0;                       // Total distance traveled along the ray
//     for (int i = 0; i < MAX_STEPS; i++) {
//         vec3 p = ro + rd * t;            // Current point on the ray
//         float d = sdBox(p, vec3(1.0));   // Cube size (1 unit in each direction)
//         
//         if (d < SURFACE_DIST) {          // Close enough to the surface
//             hitPos = p;
//             return t;
//         }
//         if (t > MAX_DIST) break;         // Ray has traveled too far
//         t += d;                          // Advance the ray
//     }
//     return -1.0;                         // Return -1 if no hit
// }
//
// // Basic lighting (Lambertian)
// vec3 computeLighting(vec3 p, vec3 normal, vec3 lightDir) {
//     float diffuse = max(dot(normal, lightDir), 0.0);
//     return vec3(1.0, 0.5, 0.2) * diffuse; // Orange-ish color
// }
//
// // Calculate the normal vector at a point on the surface
// vec3 estimateNormal(vec3 p) {
//     const vec3 eps = vec3(0.001, 0.0, 0.0); // Small offset for numerical gradient
//     float dx = sdBox(p + eps.xyy, vec3(1.0)) - sdBox(p - eps.xyy, vec3(1.0));
//     float dy = sdBox(p + eps.yxy, vec3(1.0)) - sdBox(p - eps.yxy, vec3(1.0));
//     float dz = sdBox(p + eps.yyx, vec3(1.0)) - sdBox(p - eps.yyx, vec3(1.0));
//     return normalize(vec3(dx, dy, dz));
// }
//
// void main() {
//     // Convert texCoord to normalized device coordinates (NDC)
//     vec2 uv = texCoord * 2.0 - 1.0;      // Map [0, 1] to [-1, 1]
//     uv.x *= 800.0 / 600.0;               // Adjust for aspect ratio (width / height)
//
//     // Define the ray origin and direction
//     vec3 ro = vec3(0.0, 0.0, -5.0);      // Camera position (backward along Z-axis)
//     vec3 rd = normalize(vec3(uv, 1.0)); // Ray direction (into the screen)
//
//     // Perform raymarching
//     vec3 hitPos;                         // Position of the ray hit
//     float t = rayMarch(ro, rd, hitPos);
//
//     // Initialize the fragment color
//     vec3 color = vec3(0.0);              // Default to black (no hit)
//
//     if (t > 0.0) {                       // If the ray hit the cube
//         vec3 normal = estimateNormal(hitPos); // Compute the surface normal
//         vec3 lightDir = normalize(vec3(-0.5, 0.8, -1.0)); // Light direction
//         color = computeLighting(hitPos, normal, lightDir); // Apply lighting
//     }
//
//     gl_FragColor = vec4(color, 1.0);     // Output the fragment color
// }





// precision mediump float;                            
// varying vec2 texCoord;                              
// uniform sampler2D texSampler;                       
//
// float sdBox( vec3 p, vec3 b )
// {
//   vec3 q = abs(p) - b;
//   return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
// }
//
// float smin( float a, float b, float k )
// {
//     float res = exp2( -k*a ) + exp2( -k*b );
//     return -log2( res )/k;
// }
//
// float map(vec3 p){
//   
//   vec3 q = p;
//   // q.x += sin(gTime);
//   q.x += sin(0);
//   float box1 = sdBox(q, vec3(1.0f));
//   
//   float ground = p.y + 1.5f;
//
//   float dist = smin(ground, box1, 5.0f);
//   return dist;
// }
//
// void main()                                         
// {                                                   
//     // gl_FragColor = texture2D(texSampler, texCoord); 
//     // gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
//   vec2 uv = (texCoord * 2.0f * 800) / 1200;
//     
//   vec3 ro = vec3(0, 0, -3);         // ray origin
//   vec3 rd = normalize(vec3(uv*0.5f, 1)); // ray direction, adjusting FOV with miltiplier
//   vec3 col = vec3(1.0f);            // pixel color
//
//   // ro += gCamPos;
//
//   // rd.yz *= rot2D(gMouseDelta.y);
//   // rd.xz *= rot2D(-gMouseDelta.x);
//
//   float t = 0.0f;                   // travelled distance of ray
//
//   // Raymarching
//   int i;
//   for(i = 0; i < 80; i++){
//     vec3 p = ro + rd * t;           // position along the ray 
//     
//     float d = map(p);               // current distance to the scene
//
//     t += d;                         // march the ray
//
//
//     if(d < 0.001f || t > 100.0f) break;
//   }
//     
//   col = vec3(t* 0.04f + float(i)*0.004f);
//   // col = palette(t * 0.04f + float(i)*0.005f);
//     //red
//     col = vec3(1.0f, 0.0f, 0.0f);
//   
//   color = vec4(col, 1.0f);
//     
//     gl_FragColor = vec4(col, 1.0);
// }                                                   
