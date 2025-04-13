#ifndef DP_IMAPP_APP_IMPL_INTERFACE_HPP
#define DP_IMAPP_APP_IMPL_INTERFACE_HPP

struct GLFWwindow;

namespace dpImApp::detail
{
    class AppImplInterface
    {
    public:
        virtual ~AppImplInterface() = default;

        virtual void InitBeforeCreateMainWindow() = 0;
        virtual void InitBeforeMainLoop(GLFWwindow* main_window) = 0;
        virtual void Update() = 0;
    };
}

#endif
