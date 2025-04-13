#ifndef DP_IMAPP_CONFIG_HPP
#define DP_IMAPP_CONFIG_HPP

#ifdef DP_IMAPP_SHARED
    #ifdef _WIN32
        #ifdef dpImApp_EXPORTS
            #define DP_IMAPP_API __declspec(dllexport)
        #else
            #define DP_IMAPP_API __declspec(dllimport)
        #endif
    #else
        #define DP_IMAPP_API __attribute__((visibility("default")))
    #endif
#else
    #define DP_IMAPP_API
#endif

#endif
