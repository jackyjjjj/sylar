#pragma once

#include <memory>
using namespace std;
namespace jyl
{
    namespace
    {
        template <class T, class X, int N>
        T &getInstanceX()
        {
            static T v;
            return v;
        }

        template <class T, class X, int N>
        shared_ptr<T> getInstancePtr()
        {
            static shared_ptr<T> v(new T);
            return v;
        }
    }

    /*单例模式封装类*/
    template <class T, class X = void, int N = 0>
    class Singleton
    {
    public:
        /*返回单例裸指针*/
        static T *getInstance()
        {
            static T v;
            return &v;
        }
    };

    template <class T, class X = void, int N = 0>
    class SingletonPtr
    {
    public:
        /*返回单例模式的指针*/
        static shared_ptr<T> getInstance()
        {
            static shared_ptr<T> v(new T);
            return v;
            // return getInstancePtr<T,X,N>();
        }
    };
}