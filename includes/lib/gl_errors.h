#ifndef GL_ERRORS_H
#define GL_ERRORS_H

#include <glad/glad.h>

GLenum glCheckError_(const char *file, int line);

#define glCheckError() glCheckError_(__FILE__, __LINE__)
#endif
