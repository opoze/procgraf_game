#ifndef _GL_UTILS_H_
#define _GL_UTILS_H_

#include <GL/glew.h>		// include GLEW and new version of GL on Windows
#include <stdarg.h>

#define GL_LOG_FILE "gl.log"


bool restart_gl_log();

bool gl_log( const char *message, ... );

bool gl_log_err( const char *message, ... );

void print_shader_info_log( GLuint shader_index );

void print_programme_info_log( GLuint sp );

bool parse_file_into_str( const char *file_name, char *shader_str, int max_len );

#endif
