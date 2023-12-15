//
// Created by baidu on 2021/5/25.
//

#ifndef NATIVEEGL_EGLHELPER_H
#define NATIVEEGL_EGLHELPER_H

#include "EGL/egl.h"
#include "../log/JniLog.h"
#include <stddef.h>
#include "GLES/gl.h"
#include "GLES/glext.h"
#include "GLES2/gl2.h"
#include "GLES3/gl3.h"
#include "GLES2/gl2ext.h"
#include "EGL/eglext.h"













class EglHelper {

public:
    EGLDisplay mEglDisplay;
    EGLContext mEglContext;
    EGLSurface mEglSurface;
    int * texture_data;
    const size_t TEXTURE_DATA_WIDTH = 256;
    const size_t TEXTURE_DATA_HEIGHT = TEXTURE_DATA_WIDTH;
    const size_t TEXTURE_DATA_SIZE = TEXTURE_DATA_WIDTH * TEXTURE_DATA_HEIGHT;
    const char *SERVER_FILE= "/data/test_server" ;
    const char *CLIENT_FILE= "/data/test_client";
    GLuint texture;

public:
    EglHelper();
    ~EglHelper();
    void rotate_data();
    void gl_setup_scene();
    void gl_draw_scene();
    int initEgl(EGLNativeWindowType surface);
    int swapBuffers();
    int* create_data(size_t size);

    int create_socket(const char *path);
    void read_fd(int sock, int *fd, void *data, size_t data_len);
    void write_fd(int sock, int fd, void *data, size_t data_len);
    int connect_socket(int sock, const char *path);
    void destroyEgl();
};


#endif //NATIVEEGL_EGLHELPER_H
