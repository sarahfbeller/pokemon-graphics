// // red.vert

// /*
//   This simple GLSL vertex shader just sets a color (red)
//   to use in the fragment shader and computes the position
//   of the vertex (to be interpolated automatically, and used
//   in the fragment shader)
// */

// // This 'varying' vertex output can be read as an input
// // by a fragment shader that makes the same declaration.
// varying vec4 sky_color;

// void main(){
//     // we want to render the shape using standard OpenGL position transforms.
//     gl_Position = ftransform();

//     // go cardinal!
//     sky_color = vec4(0.137, 0.31, 0.698, 1.);
// }


// phong.vert


varying vec3 ambientColor;
varying vec3 diffuseColor;
varying vec3 specularColor;
varying vec3 lightColor;
varying vec3 eyeSpaceNormal;

void main(){
    // we want to render the shape using standard OpenGL position transforms.
    gl_Position = ftransform();

    // define ambient, diffuse, specular, light color
    lightColor      = vec3(40./256., 140./256., 255./256.);
    ambientColor    = gl_LightSource[0].ambient.rgb;
    diffuseColor    = gl_LightSource[0].diffuse.rgb;
    specularColor   = gl_LightSource[0].specular.rgb;
    eyeSpaceNormal  = gl_Normal;
}
