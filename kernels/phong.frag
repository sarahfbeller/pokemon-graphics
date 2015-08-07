// phong.frag


varying vec3 ambientColor;
varying vec3 diffuseColor;
varying vec3 specularColor;
varying vec3 lightColor;
varying vec3 eyeSpaceNormal;


vec4 blinnPhong(vec3 N, vec3 L, vec3 V) {
    vec3 H = normalize(L+V);
    vec3 amb = ambientColor * lightColor;
    vec3 diff = diffuseColor * dot(L, N) * lightColor;
    vec3 spec = specularColor * pow(dot(H, N), gl_FrontMaterial.shininess) * lightColor;
    spec = max(spec, vec3(0,0,0));
    return vec4(amb + diff + spec, 1.0);
}


void main()
{
    vec3 N = normalize(eyeSpaceNormal);
    vec3 L = vec3(gl_LightSource[0].diffuse.r,gl_LightSource[0].diffuse.g,gl_LightSource[0].diffuse.b);
    vec3 V = normalize(gl_LightSource[0].spotDirection);
    gl_FragColor = blinnPhong(N, L, V);
}


