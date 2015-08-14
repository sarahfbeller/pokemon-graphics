// sky.vert
// Doesn't actually do anything

void main(){
    // we want to render the shape using standard OpenGL position transforms.
    gl_Position = ftransform();
}
