/*
     __   ___  __   ___       ___  __     __
    |__) |__  / _` |__  |\ | |__  /__` | /__`
    |  \ |___ \__> |___ | \| |___ .__/ | .__/

        Remix of GenesisVFX by Harry Denholm, ishani.org 2018-2021
            Original code by Andrew Cross
                Photoshop version by Richard Wilson

*/

#pragma once

// http://easings.net/
// most of the below is from cocos2d-x

namespace tween
{
    rginline constexpr float smootherstep( const float x )
    {
        // evaluate polynomial
        return x * x * x * ( x * ( x * 6.0f - 15.0f ) + 10.0f );
    }

    rginline constexpr float smootherstep( const float edge0, const float edge1, const float x )
    {
        // scale, and clamp x to 0..1 range
        const float _x = clamp( ( x - edge0 ) / ( edge1 - edge0 ), 0.0f, 1.0f );

        // evaluate polynomial
        return smootherstep( _x );
    }

    // Sine Ease
    rginline float sineEaseIn( float time )
    {
        return -1 * cosf( time * c_half_pi ) + 1;
    }

    rginline float sineEaseOut( float time )
    {
        return sinf( time * c_half_pi );
    }

    rginline float sineEaseInOut( float time )
    {
        return -0.5f * ( cosf( c_pi * time ) - 1 );
    }


    // Quad Ease
    rginline float quadEaseIn( float time )
    {
        return time * time;
    }

    rginline float quadEaseOut( float time )
    {
        return -1 * time * ( time - 2 );
    }

    rginline float quadEaseInOut( float time )
    {
        time = time * 2;
        if ( time < 1 )
            return 0.5f * time * time;
        --time;
        return -0.5f * ( time * ( time - 2 ) - 1 );
    }



    // Cubic Ease
    rginline float cubicEaseIn( float time )
    {
        return time * time * time;
    }
    rginline float cubicEaseOut( float time )
    {
        time -= 1;
        return ( time * time * time + 1 );
    }
    rginline float cubicEaseInOut( float time )
    {
        time = time * 2;
        if ( time < 1 )
            return 0.5f * time * time * time;
        time -= 2;
        return 0.5f * ( time * time * time + 2 );
    }


    // Quart Ease
    rginline float quartEaseIn( float time )
    {
        return time * time * time * time;
    }

    rginline float quartEaseOut( float time )
    {
        time -= 1;
        return -( time * time * time * time - 1 );
    }

    rginline float quartEaseInOut( float time )
    {
        time = time * 2;
        if ( time < 1 )
            return 0.5f * time * time * time * time;
        time -= 2;
        return -0.5f * ( time * time * time * time - 2 );
    }


    // Quint Ease
    rginline float quintEaseIn( float time )
    {
        return time * time * time * time * time;
    }

    rginline float quintEaseOut( float time )
    {
        time -= 1;
        return ( time * time * time * time * time + 1 );
    }

    rginline float quintEaseInOut( float time )
    {
        time = time * 2;
        if ( time < 1 )
            return 0.5f * time * time * time * time * time;
        time -= 2;
        return 0.5f * ( time * time * time * time * time + 2 );
    }


    // Expo Ease
    rginline float expoEaseIn( float time )
    {
        return time == 0 ? 0 : powf( 2, 10 * ( time / 1 - 1 ) ) - 1 * 0.001f;
    }

    rginline float expoEaseOut( float time )
    {
        return time == 1 ? 1 : ( -powf( 2, -10 * time / 1 ) + 1 );
    }

    rginline float expoEaseInOut( float time )
    {
        if ( time == 0 || time == 1 )
            return time;

        if ( time < 0.5f )
            return 0.5f * powf( 2, 10 * ( time * 2 - 1 ) );

        return 0.5f * ( -powf( 2, -10 * ( time * 2 - 1 ) ) + 2 );
    }


    // Circ Ease
    rginline float circEaseIn( float time )
    {
        return -1 * ( sqrt( 1 - time * time ) - 1 );
    }

    rginline float circEaseOut( float time )
    {
        time = time - 1;
        return sqrt( 1 - time * time );
    }

    rginline float circEaseInOut( float time )
    {
        time = time * 2;
        if ( time < 1 )
            return -0.5f * ( sqrt( 1 - time * time ) - 1 );
        time -= 2;
        return 0.5f * ( sqrt( 1 - time * time ) + 1 );
    }


    // Elastic Ease
    rginline float elasticEaseIn( float time, float period )
    {

        float newT = 0;
        if ( time == 0 || time == 1 )
        {
            newT = time;
        }
        else
        {
            float s = period / 4;
            time = time - 1;
            newT = -powf( 2, 10 * time ) * sinf( ( time - s ) * c_two_pi / period );
        }

        return newT;
    }

    rginline float elasticEaseOut( float time, float period )
    {

        float newT = 0;
        if ( time == 0 || time == 1 )
        {
            newT = time;
        }
        else
        {
            float s = period / 4;
            newT = powf( 2, -10 * time ) * sinf( ( time - s ) * c_two_pi / period ) + 1;
        }

        return newT;
    }

    rginline float elasticEaseInOut( float time, float period )
    {

        float newT = 0;
        if ( time == 0 || time == 1 )
        {
            newT = time;
        }
        else
        {
            time = time * 2;
            if ( ! period )
            {
                period = 0.3f * 1.5f;
            }

            float s = period / 4;

            time = time - 1;
            if ( time < 0 )
            {
                newT = -0.5f * powf( 2, 10 * time ) * sinf( ( time - s ) * c_two_pi / period );
            }
            else
            {
                newT = powf( 2, -10 * time ) * sinf( ( time - s ) * c_two_pi / period ) * 0.5f + 1;
            }
        }
        return newT;
    }


    // Back Ease
    rginline float backEaseIn( float time )
    {
        float overshoot = 1.70158f;
        return time * time * ( ( overshoot + 1 ) * time - overshoot );
    }

    rginline float backEaseOut( float time )
    {
        float overshoot = 1.70158f;

        time = time - 1;
        return time * time * ( ( overshoot + 1 ) * time + overshoot ) + 1;
    }

    rginline float backEaseInOut( float time )
    {
        float overshoot = 1.70158f * 1.525f;

        time = time * 2;
        if ( time < 1 )
        {
            return ( time * time * ( ( overshoot + 1 ) * time - overshoot ) ) / 2;
        }
        else
        {
            time = time - 2;
            return ( time * time * ( ( overshoot + 1 ) * time + overshoot ) ) / 2 + 1;
        }
    }

    
    rginline float easeIn( float time, float rate )
    {
        return powf( time, rate );
    }

    rginline float easeOut( float time, float rate )
    {
        return powf( time, 1 / rate );
    }

    rginline float easeInOut( float time, float rate )
    {
        time *= 2;
        if ( time < 1 )
        {
            return 0.5f * powf( time, rate );
        }
        else
        {
            return ( 1.0f - 0.5f * powf( 2 - time, rate ) );
        }
    }

    rginline float quadraticIn( float time )
    {
        return   powf( time, 2 );
    }

    rginline float quadraticOut( float time )
    {
        return -time * ( time - 2 );
    }

    rginline float quadraticInOut( float time )
    {

        float resultTime = time;
        time = time * 2;
        if ( time < 1 )
        {
            resultTime = time * time * 0.5f;
        }
        else
        {
            --time;
            resultTime = -0.5f * ( time * ( time - 2 ) - 1 );
        }
        return resultTime;
    }

    rginline float bezieratFunction( float a, float b, float c, float d, float t )
    {
        return ( powf( 1 - t, 3 ) * a + 3 * t*( powf( 1 - t, 2 ) )*b + 3 * powf( t, 2 )*( 1 - t )*c + powf( t, 3 )*d );
    }

} // namespace tween
