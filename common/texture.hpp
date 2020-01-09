#ifndef TEXTURE_HPP
#define TEXTURE_HPP

// Load a .BMP file using our custom loader
GLuint loadBMPFile(const char *imagepath);

// Load a .DDS file using GLFW's own loader
GLuint loadDDS(const char *imagepath);

#endif