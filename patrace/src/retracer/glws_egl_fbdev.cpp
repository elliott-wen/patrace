#include "retracer/glws_egl_fbdev.hpp"
#include "retracer/retracer.hpp"

#include "dispatch/eglproc_auto.hpp"

namespace retracer
{

class FbdevWindow : public NativeWindow
{
public:
    FbdevWindow(int width, int height, const std::string& title)
        : NativeWindow(width, height, title)
        , mFbdevWindowStruct(width, height)
    {
        mHandle = reinterpret_cast<EGLNativeWindowType>(&mFbdevWindowStruct);
    }

    ~FbdevWindow()
    {
    }

    bool resize(int w, int h)
    {
        if (NativeWindow::resize(w, h))
        {
            mFbdevWindowStruct.setWidth(w);
            mFbdevWindowStruct.setHeight(h);
            return true;
        }

        return false;
    }

private:
    struct FbdevWindowStruct
    {
        FbdevWindowStruct(int width_, int height_)
            : width(static_cast<unsigned short>(width_))
            , height(static_cast<unsigned short>(height_))
        {}

        void setWidth(int width_)
        {
            width = static_cast<unsigned short>(width_);
        }

        void setHeight(int height_)
        {
            height = static_cast<unsigned short>(height_);
        }

        unsigned short width;
        unsigned short height;
    };

    FbdevWindowStruct mFbdevWindowStruct;
};


Drawable* GlwsEglFbdev::CreateDrawable(int width, int height, int /*win*/, EGLint const* attribList)
{
    // TODO: Delete
    // fbdev only supports one single native window. Therefor, map everything to 0.
    Drawable* handler = NULL;
    gWinNameToNativeWindowMap[0] = new FbdevWindow(width, height, "0");
    handler = new EglDrawable(width, height, mEglDisplay, mEglConfig, gWinNameToNativeWindowMap[0], attribList);
    return handler;
}

static void usage()
{
    printf("Cmd usage:\n");
    printf("    D: render one draw call.\n");
    printf("    N: render one frame.\n");
    printf("    M: render 10 frames.\n");
    printf("    L: render 100 frames.\n");
    printf("    Q: exit.\n");
    printf("    H: show this usage.\n");
}

GlwsEglFbdev::GlwsEglFbdev()
    : GlwsEgl()
{
    if (gRetracer.mOptions.mStepMode) usage();
}

GlwsEglFbdev::~GlwsEglFbdev()
{
}

EGLImageKHR GlwsEglFbdev::createImageKHR(Context* context, EGLenum target, uintptr_t buffer, const EGLint* attrib_list)
{
    EGLContext eglContext = EGL_NO_CONTEXT;
    if (context)
    {
        eglContext = static_cast<EglContext*>(context)->mContext;
    }

    uintptr_t uintBuffer = static_cast<uintptr_t>(buffer);
    EGLClientBuffer eglBuffer = reinterpret_cast<EGLClientBuffer>(uintBuffer);
    return _eglCreateImageKHR(mEglDisplay, eglContext, target, eglBuffer, attrib_list);
}

GLWS& GLWS::instance()
{
    static GlwsEglFbdev g;
    return g;
}

void GlwsEglFbdev::processStepEvent()
{
    const char cmd = getchar();

    if (cmd == 'N')
    {
        gRetracer.frameBudget++;
    }
    else if (cmd == 'M')
    {
        gRetracer.frameBudget += 10;
    }
    else if (cmd == 'L')
    {
        gRetracer.frameBudget += 100;
    }
    else if (cmd == 'D')
    {
        gRetracer.drawBudget++;
    }
    else if (cmd == 'Q')
    {
        gRetracer.mFinish = true;
    }
    else if (cmd == 'H')
    {
        usage();
    }
}

} // namespace retracer
