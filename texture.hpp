#ifndef TEXTURE_HPP
#define TEXTURE_HPP

//// Since GLFW 3, glfwLoadTexture2D() has been removed. You have to use another texture loading library, 
//// or do it yourself (just like loadBMP_custom and loadDDS)

// Load a .DDS file using GLFW's own loader
GLuint loadDDS(const char * imagepath);


#endif