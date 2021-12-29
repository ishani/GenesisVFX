/*
     __   ___  __   ___       ___  __     __
    |__) |__  / _` |__  |\ | |__  /__` | /__`
    |  \ |___ \__> |___ | \| |___ .__/ | .__/

        Remix of GenesisVFX by Harry Denholm, ishani.org 2018-2021
            Original code by Andrew Cross
                Photoshop version by Richard Wilson

*/

#ifndef H_RG_COMMON
#define H_RG_COMMON

#define _USE_MATH_DEFINES

#include <inttypes.h>
#include <stdint.h>
#include <cmath>
#include <utility>
#include <algorithm>
#include <vector>
#include <array>
#include <memory>
#include <string>
#include <functional>
#include <atomic>
#include <chrono>
#include <float.h>
#include <cstring>
#include <omp.h>

#ifdef MSC_VER
#define rginline    __forceinline
#else
#define rginline    inline
#endif

//----------------------------------------------------------------------------------------------------------------------

#define rg_macro_join(_lhs, _rhs)     rg_macro_do_join(_lhs, _rhs)
#define rg_macro_do_join(_lhs, _rhs)  rg_macro_do_join2(_lhs, _rhs)
#define rg_macro_do_join2(_lhs, _rhs) _lhs##_rhs

#define rg_xstr(a) #a
#define rg_stringify(a) rg_xstr(a)

//----------------------------------------------------------------------------------------------------------------------

#if defined(__INTEL_COMPILER)

#define AUTO_PARALLELIZE_LOOP   __pragma(omp parallel for)

#define OPT_VERSION             "icc"

#elif defined(RG_QPAR)

#define AUTO_PARALLELIZE_LOOP   __pragma( loop(hint_parallel(0)) ) \
                                __pragma( loop(ivdep) )

#define OPT_VERSION             "qpar"

#elif defined(RG_OPENMP)

#define AUTO_PARALLELIZE_LOOP   __pragma(omp parallel for)

#define OPT_VERSION             "omp" 

#else

#define AUTO_PARALLELIZE_LOOP

#define OPT_VERSION             "st"

#endif 

//----------------------------------------------------------------------------------------------------------------------

// allocate numElements of _T aligned to N bytes
template< typename _T >
inline static _T* allocateAlign( const size_t numElements, const size_t alignment )
{
#ifdef WIN32
    return reinterpret_cast<_T*>(_aligned_malloc( sizeof( _T ) * numElements, alignment ));
#else
    return reinterpret_cast<_T*>(aligned_alloc( alignment, sizeof( _T ) * numElements ));
#endif
}

// free memory allocated with allocateAlign
inline static void freeAlign( void* ptr )
{
#ifdef WIN32
    return _aligned_free( ptr );
#else
    return free( ptr );
#endif
}

//----------------------------------------------------------------------------------------------------------------------
enum class LogLevel
{
    Verbose,
    Perf,
    Default,
    Error
};

rginline const char* rgLogLevelText( const LogLevel ll )
{
    switch ( ll )
    {
    case LogLevel::Verbose:     return "[verbose] ";
    case LogLevel::Perf:        return "[ timed ] ";
    case LogLevel::Error:       return "[ ERROR ] ";
    default:
        break;
    }
    return "";
}

extern void rgLogOutput( const LogLevel ll, const char* fmt, ... );

extern void rgAssertHandler( const char* func, const uint64_t line, const char* failed_expr );

extern void rgProgressHandler( const char* task, const float current, const float total );


#define ensure( _expr )     { if (!(_expr)) { rgAssertHandler( __FUNCTION__, __LINE__, #_expr ); } }

#ifdef _DEBUG
#define dbg_ensure( _expr ) ensure( _expr )
#else
#define dbg_ensure( _expr ) 
#endif 



constexpr float c_pi        = 3.1415926535897932384626f;
constexpr float c_two_pi    = 6.2831853071795864769252f;
constexpr float c_half_pi   = 1.5707963267948966192313f;


//----------------------------------------------------------------------------------------------------------------------
template <typename _Storage>
class _Point3
{
public:
    _Storage x, y, z;

    rginline _Point3()
    {
    }

    rginline _Point3(
        _Storage _x,
        _Storage _y,
        _Storage _z )
        : x( _x )
        , y( _y )
        , z( _z )
    {}

    rginline _Point3 operator * ( const _Storage mul ) const
    {
        return _Point3( x * mul, y * mul, z * mul );
    };
};

using Point3f = _Point3<float>;
using Point3d = _Point3<double>;


//----------------------------------------------------------------------------------------------------------------------
template <typename _Storage>
class _Point2
{
    typedef _Point2<_Storage>   selftype;
public:
    _Storage x, y;

    rginline _Point2()
    {
    }

    rginline _Point2(
        _Storage _x,
        _Storage _y )
        : x( _x )
        , y( _y )
    {}

    rginline _Storage lengthSquared() const
    {
        return ( x * x ) + ( y * y );
    }

    rginline _Point2 operator * ( const _Storage mul ) const
    {
        return _Point2( x * mul, y * mul );
    }

    rginline _Point2 operator + ( const _Storage mul ) const
    {
        return _Point2( x + mul, y + mul );
    }

    rginline _Point2 operator - ( const _Storage mul ) const
    {
        return _Point2( x - mul, y - mul );
    }

    rginline _Point2 operator * (const _Point2& mul) const
    {
        return _Point2(x * mul.x, y * mul.y);
    }

    rginline _Point2 operator + (const _Point2& mul) const
    {
        return _Point2(x + mul.x, y + mul.y);
    }

    rginline _Point2 operator - (const _Point2& mul) const
    {
        return _Point2(x - mul.x, y - mul.y);
    }


    rginline void operator -= ( const selftype& opt )
    {
        x -= opt.x;
        y -= opt.y;
    }

    rginline void operator += ( const selftype& opt )
    {
        x += opt.x;
        y += opt.y;
    }

    rginline void operator *= ( const _Storage mul )
    {
        x *= mul;
        y *= mul;
    }

    rginline void operator /= ( const _Storage div )
    {
        x /= div;
        y /= div;
    }
};

using Point2f = _Point2<float>;
using Point2d = _Point2<double>;


//----------------------------------------------------------------------------------------------------------------------


rginline constexpr uint16_t   floatToUI16( const float a )
{
    return (uint16_t)( a * 65535.99f );
}

rginline constexpr float      UI16ToFloat( const uint16_t a )
{
    return (float)( (double)a * 1.52590218967E-5 );
}


rginline bool floatAlmostEqualRelative(
    float A,
    float B,
    float maxRelDiff = FLT_EPSILON )
{
    // calculate the difference
    float diff = std::fabs( A - B );
    A = std::fabs( A );
    B = std::fabs( B );

    // find the largest
    float largest = ( B > A ) ? B : A;

    if ( diff <= largest * maxRelDiff )
        return true;

    return false;
}

template<typename _T>
rginline constexpr _T rgLerp( const _T a, const _T b, const _T t )
{ 
    return a + t * (b - a); 
}

template<typename _T>
rginline constexpr _T rgLerpBiDir( const _T from, const _T toNeg, const _T toPos, const _T t ) 
{ 
    if ( t >= 0 )
        return from + t * (toPos - from); 
    else
        return from + (-t) * (toNeg - from);
}

rginline constexpr float saturate( const float in )
{
    if ( in < 0.0f )
        return 0.0f;
    if ( in > 1.0f )
        return 1.0f;
    return in;
}

rginline constexpr float clamp1n1( const float in )
{
    if ( in < -1.0f )
        return -1.0f;
    if ( in > 1.0f )
        return 1.0f;
    return in;
}

template< typename _type >
rginline constexpr _type clamp( const _type in, const _type low, const _type high )
{
    if ( in < low )
        return low;
    if ( in > high )
        return high;
    return in;
}

rginline float fractf( const float in )
{
    double ipart;
    return (float)modf( in, &ipart );
}


class Timer
{
public:

    rginline Timer()
        : m_start( hrClock::now() )
    {
    }

    rginline void reset()
    {
        m_start = hrClock::now();
    }

    rginline double secondsElapsed() const
    {
        return std::chrono::duration_cast<durationSecs>
            ( hrClock::now() - m_start ).count();
    }

    rginline double secondsElapsedAndReset()
    {
        double r = secondsElapsed();
        reset();
        return r;
    }

private:
    typedef std::chrono::high_resolution_clock              hrClock;
    typedef std::chrono::duration<double, std::ratio<1> >   durationSecs;

    std::chrono::time_point<hrClock>    m_start;
};

class ScopedTimer
{
public:

    rginline ScopedTimer( const char* tag, const LogLevel level = LogLevel::Perf )
        : m_tag( tag )
        , m_level( level )
    {
    }

    rginline ~ScopedTimer()
    {
        rgLogOutput( m_level, "Block {%s} took %lf seconds", m_tag.c_str(), m_timer.secondsElapsed() );
    }

private:

    Timer           m_timer;
    std::string     m_tag;
    LogLevel        m_level;
};





template <typename _lambda>
class rgScopeExit
{
public:

    rginline rgScopeExit( _lambda&& lFn )
        : mFn( std::forward<_lambda>( lFn ) )
    {
    }

    rginline ~rgScopeExit()
    {
        mFn();
    }

protected:

    _lambda mFn;
};

template <typename _lambda>
rginline rgScopeExit<_lambda> NewScopedCode( _lambda&& lFn )
{
    return rgScopeExit<_lambda>( std::forward<_lambda>( lFn ) );
};

#define RunAtScopeExit( _lambda_text ) \
    auto rg_macro_join( _scoped_code, __COUNTER__ ) = NewScopedCode( _lambda_text );



#endif // H_RG_COMMON