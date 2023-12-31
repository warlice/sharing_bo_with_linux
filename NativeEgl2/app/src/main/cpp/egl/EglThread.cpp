//
// Created by baidu on 2021/5/25.
//

#include "EglThread.h"

EglThread::EglThread() {
    pthread_mutex_init(&pthread_mutex, NULL);
    pthread_cond_init(&pthread_cond, NULL);
}


EglThread::~EglThread() {
    pthread_mutex_destroy(&pthread_mutex);
    pthread_cond_destroy(&pthread_cond);
}

void *eglThreadImpl(void *context){
    EglThread *eglThread = static_cast<EglThread *>(context);
    if(!eglThread){
        LOGE("eglThreadImpl eglThread is null");
        return 0;
    }

    EglHelper *eglHelper = new EglHelper();

    if(eglHelper->initEgl(eglThread->mANativeWindow) != 0){
        LOGE("eglHelper initEgl error");
        return 0;
    }
    eglThread->isExit = false;
    while (!eglThread->isExit){

        if(eglThread->isCreate){
            eglThread->isCreate = false;
            eglThread->onCreate();
        }

        if(eglThread->isChange){
            eglThread->isChange = false;
            eglThread->isStart = true;
            eglThread->onChange(eglThread->surfaceWidth, eglThread->surfaceHeight);
        }

        if(eglThread->isStart){
            eglHelper->rotate_data();
            glBindTexture(GL_TEXTURE_2D,eglHelper->texture);
            glTexSubImage2D(GL_TEXTURE_2D,0,0,0,eglHelper->TEXTURE_DATA_WIDTH,eglHelper->TEXTURE_DATA_HEIGHT,GL_RGBA,GL_UNSIGNED_BYTE,eglHelper->texture_data);
            eglHelper->gl_draw_scene();
            eglThread->onDraw();
            //切换缓冲区，显示
            eglHelper->swapBuffers();
            LOGE("swap buffers");

            if(eglThread->mRenderType == RENDER_MODULE_AUTO){
                // sleep 1/60秒，近似1秒绘制60帧
                usleep(1000000 / 60);
            } else {
                //&(eglThread->pthread_mutex)是地址；* 取值；& 取地址
                pthread_mutex_lock(&eglThread->pthread_mutex);
                pthread_cond_wait(&eglThread->pthread_cond, &eglThread->pthread_mutex);
                pthread_mutex_unlock(&eglThread->pthread_mutex);
            }
        }
    }

    eglHelper->destroyEgl();
    delete eglHelper;
    return 0;
}

void EglThread::onSurfaceCreate(EGLNativeWindowType window) {
    if(mEglThread == -1){
        isCreate = true;
        mANativeWindow = window;
        pthread_create(&mEglThread, NULL, eglThreadImpl, this);
    }
}

void EglThread::onSurfaceChange(int width, int height) {
    if(mEglThread != -1){
        surfaceWidth = width;
        surfaceHeight = height;
        isChange = true;
        notifyRender();
    }
}

void EglThread::setRenderModule(int renderModule) {
    mRenderType = renderModule;
    notifyRender();
}

void EglThread::notifyRender() {
    pthread_mutex_lock(&pthread_mutex);
    pthread_cond_signal(&pthread_cond);
    pthread_mutex_unlock(&pthread_mutex);
}

void EglThread::callBackOnCreate(EglThread::OnCreate onCreate) {
    this->onCreate = onCreate;
}

void EglThread::callBackOnChange(EglThread::OnChange onChange) {
    this->onChange = onChange;
}

void EglThread::callBackOnDraw(EglThread::OnDraw onDraw) {
    this->onDraw = onDraw;
}

