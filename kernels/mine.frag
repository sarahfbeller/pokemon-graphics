// mine.frag

/*
    Techniques: 
        bump mapping (normal vector)
        use a uniform value 
        change rendering style? maybe?
        suggestive contours?
*/

varying vec3 ambientColor;
varying vec3 diffuseColor;
varying vec3 specularColor;
varying vec3 lightColor;

varying vec3 eyeSpaceNormal;
uniform vec3 eyeSpaceViewDir;
uniform vec3 lightDir;
varying vec4 vertices;
varying vec4 target_position;

vec4 blinnPhongModified(vec3 N, vec3 L, vec3 V, vec4 vertices) {
    vec3 H = normalize(L+V);
    vec3 amb = ambientColor * lightColor;
    vec3 diff = diffuseColor * dot(L, N) * lightColor;
    vec3 spec = specularColor * pow(dot(H, N), gl_FrontMaterial.shininess) * lightColor;
    spec = max(spec, vec3(0,0,0));
    vec4 colors = vec4(amb + diff + spec, 1.0);
    float x_dist  = vertices.x - target_position.x;
    // float y_dist  = vertices.y - target_position.y;
    float z_dist  = vertices.z - target_position.z;
    float overall_dist  = sqrt(x_dist * x_dist + z_dist * z_dist);
    // colors.r *= (vertices.x + 1.0);
    colors.r = (overall_dist/30.0 * 1.0/256.0);
    return normalize(colors);
}

void main(){
    vec3 N = normalize(eyeSpaceNormal);
    vec3 L = gl_LightSource[0].diffuse.rgb;
    vec3 V = normalize(gl_LightSource[0].spotDirection);
    gl_FragColor        = blinnPhongModified(N, L, V, vertices);
}


