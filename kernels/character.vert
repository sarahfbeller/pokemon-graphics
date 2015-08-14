varying vec3 ambientColor;
varying vec3 diffuseColor;
varying vec3 specularColor;
varying vec3 lightColor;
varying vec4 vertices;

varying vec3 eyeSpaceNormal;
uniform vec3 eyeSpaceViewDir;
uniform vec3 lightDir;

uniform float x_position;
uniform float z_position;
varying vec4 target_position;

void main()
{
    // we want to render the shape using standard OpenGL position transforms.
    gl_Position = ftransform();
    
    // define ambient, diffuse, specular, light color
    ambientColor    = gl_LightSource[0].ambient.rgb;
    diffuseColor    = gl_LightSource[0].diffuse.rgb;
    specularColor   = gl_LightSource[0].specular.rgb;
    lightColor      = vec3(1.0, 1.0, 0.5);

    vertices       = gl_Vertex;
    vertices.x     = vertices.x + x_position;
    vertices.z     = vertices.z + z_position;
    gl_Position    = gl_ModelViewProjectionMatrix * vertices;
    eyeSpaceNormal  = gl_Normal;
}
