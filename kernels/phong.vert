// phong.vert


varying vec3 ambientColor;
varying vec3 diffuseColor;
varying vec3 specularColor;
varying vec3 lightColor;
varying vec3 eyeSpaceNormal;

void main()
{
    // we want to render the shape using standard OpenGL position transforms.
    gl_Position = ftransform();

    // define ambient, diffuse, specular, light color
    lightColor      = vec3(.65, 0.61, .85);
    ambientColor    = gl_LightSource[0].ambient.rgb;
    ambientColor.r  = .40;
    diffuseColor    = gl_LightSource[0].diffuse.rgb;
    diffuseColor.g  = 1.0;
    specularColor   = gl_LightSource[0].specular.rgb;
    eyeSpaceNormal  = gl_Normal;
}
