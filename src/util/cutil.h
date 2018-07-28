#ifndef CUTIL_H
#define CUTIL_H

#define GC(x) \
    class xGC{\
        ~xGC()\
        {\
            if(x::instance){\
                delete x::instance;\
                x::instance = NULL;\
            }\
        }\
    };\
    static xGC gc;

/**
 * 单例模式.h文件引入，示例 SINGLETON_H(MapManager)
 */
#define SINGLETON_H(x) \
    private:\
    static x * instance; \
    x(); \
    public:\
    static x * getInstance(); \
    private:\
    class xGC{\
        ~xGC()\
        {\
            if(x::instance){\
                delete x::instance;\
                x::instance = NULL;\
            }\
        }\
    };\
    static xGC gc;

/**
 * 单例模式.cpp文件引入，示例 SINGLETON_CPP(MapManager)
 */
#define SINGLETON_CPP(x)\
    x* x::instance = NULL;\
    x::x(){}\
    x* x::getInstance(){\
        if(instance == NULL){\
            instance = new x();\
        }\
        return instance;\
    } \

#endif // CUTIL_H
