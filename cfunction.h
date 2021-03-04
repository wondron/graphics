/*!
 ***************************************************************************
 * Copyright (C) 2017 The Sunny Company Std.
 *
 * \file       cfunction.h
 * \brief      函数封装
 * \details
 *
 * \author     wengzhuohuang  <hzweng@sunnyoptical.com>
 * \version    v1.0.0
 * \date       2017-05-17
 ***************************************************************************
 * revision:
 *
 ***************************************************************************
 */

#pragma once
#include <QDebug>
#include <QAtomicInt>

namespace Utils {

template <bool B, typename T = void> struct EnableIf;
template <typename T> struct EnableIf<true, T>
{
    typedef T Type;
};

template <bool B, typename T, typename F> struct Conditional
{
    typedef T Type;
};
template <typename T, typename F> struct Conditional<false, T, F>
{
    typedef F Type;
};

template <typename...> struct List {};
template <typename Head, typename... Tail> struct List<Head, Tail...>
{
    typedef Head Car;
    typedef List<Tail...> Cdr;
};
template <typename, typename> struct List_Append;
template <typename... L1, typename...L2> struct List_Append<List<L1...>, List<L2...>>
{
    typedef List<L1..., L2...> Value;
};
template <typename L, int N> struct List_Left
{
    typedef typename List_Append < List<typename L::Car>, typename List_Left < typename L::Cdr, N - 1 >::Value >::Value Value;
};
template <typename L> struct List_Left<L, 0>
{
    typedef List<> Value;
};
template <typename L, int N> struct List_Select
{
    typedef typename List_Select < typename L::Cdr, N - 1 >::Value Value;
};
template <typename L> struct List_Select<L, 0>
{
    typedef typename L::Car Value;
};

template<typename Func> struct CFunctionSpec
{
    enum {ArgumentCount = -1, IsPointerToMemberFunction = false};
};

template<class Obj, typename Ret, typename... Args> struct CFunctionSpec<Ret (Obj::*) (Args...)>
{
    using Object        = Obj;
    using Arguments     = List<Args...>;
    using ReturnType    = Ret;
    using Function      = Ret (Obj::*) (Args...);
    enum {ArgumentCount = sizeof...(Args), IsPointerToMemberFunction = true};
};

template<class Obj, typename Ret, typename... Args> struct CFunctionSpec<Ret (Obj::*) (Args...) const>
{
    using Object        = Obj;
    using Arguments     = List<Args...>;
    using ReturnType    = Ret;
    using Function      = Ret (Obj::*) (Args...) const;
    enum {ArgumentCount = sizeof...(Args), IsPointerToMemberFunction = true};
};

template<typename Ret, typename... Args> struct CFunctionSpec<Ret (*) (Args...)>
{
    using Arguments     = List<Args...>;
    using ReturnType    = Ret;
    using Function      = Ret (*) (Args...);
    enum {ArgumentCount = sizeof...(Args), IsPointerToMemberFunction = false};
};


template<class T> class CFunction {};

template<typename Ret, typename... Args>
class CFunction<Ret (Args...)>
{
    struct AbstractWrapper
    {
        AbstractWrapper() {}
        virtual ~AbstractWrapper() {}
        virtual Ret call(Args... args) = 0;
    };

    template<typename F>
    class Wrapper: public AbstractWrapper
    {
    public:
        Wrapper(F func): func(func) {}
        Ret call(Args... args) override {return func(args...);}
        F func;
    };

    template<typename R, typename... A>
    class Wrapper<R (*)(A...)>: public AbstractWrapper
    {
    public:
        using F = R (*)(A...);
        Wrapper(F func): func(func) {}
        R call(A... args) override {return func(args...);}
        F func;
    };

    template<class O, typename R, typename... A>
    class Wrapper <R (O::*)(A...)>: public AbstractWrapper
    {
    public:
        using Object = O ;
        using F = R (O::*)(A...);
        Wrapper(O* obj, F func): obj(obj), func(func) {}
        R call(A... args) override {; return (obj->*func)(args...);}
        O* obj;
        F func;
    };

    template<class O, typename R, typename... A>
    class Wrapper <R (O::*)(A...) const>: public AbstractWrapper
    {
    public:
        using Object = O ;
        using F = R (O::*)(A...) const;
        Wrapper(O* obj, F func): obj(obj), func(func) {}
        R call(A... args) override {return (obj->*func)(args...);}
        O* obj;
        F func;
    };

    QSharedPointer<AbstractWrapper> m_wrapper;
public :
    CFunction() {}
    CFunction(const CFunction& others) {*this = others;}

    template<typename Func>
    CFunction(Func func): m_wrapper(new Wrapper<Func>(func))
    {

    }

    template<typename Func>
    CFunction(typename Wrapper<Func>::Object* obj, Func func): m_wrapper(new Wrapper<Func>(obj, func))
    {

    }

    ~CFunction() { }

    inline bool isVaild() const {return !m_wrapper.isNull();}
    inline operator bool() const {return !m_wrapper.isNull();}

    inline Ret operator()(Args... args) {return m_wrapper->call(args...);}
    inline Ret operator()(Args... args) const {return m_wrapper->call(args...);}

    template<typename Func>
    static inline CFunction bind(Func func) {
        return CFunction(func);
    }

    template<typename Func>
    static inline CFunction bind(typename Wrapper<Func>::Object* obj, Func func) {
        return CFunction(obj, func);
    }
};

template<class T>
class CFunctionPointerType {};

template<typename Ret, typename... Args>
class CFunctionPointerType<Ret (*)(Args...)>
{
public:
    using  Arguments    =   List<Args...> ;
    using  ReturnType   =   Ret ;
    using  Function     =   Ret (*)(Args...);
    enum {ArgumentCount =   sizeof...(Args)};

    class CAbstractFunctionPointer
    {
    public:
        virtual ~CAbstractFunctionPointer() {}
        virtual Ret call(Args... args) = 0;
    };

    template<typename _Func>
    class CFunctionPointer: public CAbstractFunctionPointer
    {
    public:
        CFunctionPointer(_Func func): func(func) {}
        Ret call(Args... args)
        {
            return func(args...);
        }
        _Func func;
    };

    template<typename _Ret, typename... _Args>
    class CFunctionPointer<_Ret (*)(_Args...)>: public CAbstractFunctionPointer
    {
    public:
        using NormalFunction = _Ret (*)(_Args...);
        CFunctionPointer(NormalFunction func): func(func) {}
        _Ret call(_Args... args)
        {
            return func(args...);
        }
        NormalFunction func;
    };

    template<class _Obj, typename _Ret, typename... _Args>
    class CFunctionPointer <_Ret (_Obj::*)(_Args...)>: public CAbstractFunctionPointer
    {
    public:
        using Object            =   _Obj ;
        using MemberFunction    =   _Ret (_Obj::*)(_Args...);
        CFunctionPointer(Object* obj, MemberFunction func): obj(obj), func(func) {}
        _Ret call(Args... args)
        {
            return (obj->*func)(args...);
        }
        Object* obj;
        MemberFunction func;
    };

    template<class _Obj, typename _Ret, typename... _Args>
    class CFunctionPointer <_Ret (_Obj::*)(_Args...)const>: public CAbstractFunctionPointer
    {
    public:
        using Object            =   _Obj ;
        using MemberFunction    =   _Ret (_Obj::*)(_Args...)const;
        CFunctionPointer(Object* obj, MemberFunction func): obj(obj), func(func) {}
        _Ret call(_Args... args)
        {
            return (obj->*func)(args...);
        }
        Object* obj;
        MemberFunction func;
    };

    template<typename Func>
    static inline CAbstractFunctionPointer*  create(Func func)
    {
        return new CFunctionPointer<Func>(func);
    }

    template<typename Func>
    static inline CAbstractFunctionPointer*  create(typename CFunctionPointer<Func>::Object* obj, Func func)
    {
        return new CFunctionPointer<Func>(obj, func);
    }
};

}
