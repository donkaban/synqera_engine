#ifndef _THE_METHOD_
#define _THE_METHOD_

#include "TheEngine/comtypes.h"
#include "TheEngine/logger.h"
template<int...>          struct seq {};
template<int N, int ...M> struct gen : gen<N-1, N-1, M...> { };
template<int ...N>        struct gen<0, N...> {typedef seq<N...> type;};

enum mstate {DIRECT, PAUSED, DELETED};
 
template <typename T> class method;
template <typename R, typename... A> class method<R(A...)> 
{
public:    
    using type = std::function<R(A...)>;  
    using P = std::tuple<A...>; 
    using L = std::function<void(R)>;
    method() {}
    method(const type &f) : _func(f) {}
    R operator() (A && ... args)  {return _func(args ...);}    
    
    method & operator() (A && ... args, const L &l)  
    {
        _lambda = l;
        _params = std::make_tuple(args...);
        return *this;
    }    
    method & param(A && ... args) {_params = std::make_tuple(args...);return *this;}
    method & lambda(const L &l)   {_lambda=l;return *this;}
    R call() 
    {
        _result = _apply(typename gen<sizeof...(A)>::type());
        if(_lambda) _lambda(_result);
        return _result;
    }
    mstate state() const   {return _state;}
    void   state(mstate s) {_state = s;}
    R & get() const {return _result;}

  private:
    template<int ...N> R _apply(seq<N...>) {return _func(std::get<N>(_params)... );}
    type _func   {};
    P _params {};
    L _lambda {};
    R _result;
    mstate _state = DIRECT;
}; 

template <typename... A> class method<void(A...)> 
{
public:    
    using type = std::function<void(A...)>;  
    using P = std::tuple<A...>; 
    using L = std::function<void()>;
    method() {}
    method(const type &f) : _func(f) {}
    void operator() (A && ... args)  { _func(args ...);}    
    
    method & operator() (A && ... args, const L &l)  
    {
        _lambda = l;
        _params = std::make_tuple(args...);
        return *this;
    }    
    method & param(A... args) {_params = std::make_tuple(args...);return *this;}
    method & lambda(const L &l)   {_lambda=l;return *this;}
    void call() 
    {
        _apply(typename gen<sizeof...(A)>::type());
        if(_lambda) _lambda();
    }
    mstate state() const   {return _state;}
    void   state(mstate s) {_state = s;}
  private:
    template<int ...N> void _apply(seq<N...>) {return _func(std::get<N>(_params)... );}
    type _func   {};
    P _params {};
    L _lambda {};
    mstate _state = DIRECT;
}; 

#endif