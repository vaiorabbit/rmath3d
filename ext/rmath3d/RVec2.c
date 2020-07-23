#include <math.h>
#include <string.h>

#include "RVec2.h"
#include "RMtx2.h"

#define SET_ELEMENT( out, at, f )

void
RVec2SetElement( RVec2* out, int at, rmReal f )
{
    out->e[at] = f;
}

void
RVec2SetX( RVec2* out, rmReal x )
{
    out->x = x;
}

void
RVec2SetY( RVec2* out, rmReal y )
{
    out->y = y;
}

void
RVec2SetElements( RVec2* out, rmReal x, rmReal y )
{
    RVec2SetX( out, x );
    RVec2SetY( out, y );
}

rmReal
RVec2GetElement( const RVec2* in, int at )
{
    return in->e[at];
}

rmReal
RVec2GetX( const RVec2* in )
{
    return in->x;
}

rmReal
RVec2GetY( const RVec2* in )
{
    return in->y;
}

int
RVec2Equal( const RVec2* v1, const RVec2* v2 )
{
    if ( 0 == memcmp( v1, v2, sizeof(RVec2) ) )
        return !0;
    else
        return 0;
}

void
RVec2Add( RVec2* out, const RVec2* v1, const RVec2* v2 )
{
    out->x = v1->x + v2->x;
    out->y = v1->y + v2->y;
}

void
RVec2Sub( RVec2* out, const RVec2* v1, const RVec2* v2 )
{
    out->x = v1->x - v2->x;
    out->y = v1->y - v2->y;
}

void
RVec2Scale( RVec2* out, const RVec2* in, rmReal f )
{
    out->x = in->x * f;
    out->y = in->y * f;
}

rmReal
RVec2Cross( const RVec2* v1, const RVec2* v2 )
{
    rmReal v1x, v1y, v2x, v2y;

    v1x = v1->x;
    v1y = v1->y;
    v2x = v2->x;
    v2y = v2->y;

    return v1x*v2y - v1y*v2x;
}

rmReal
RVec2LengthSq( const RVec2* in )
{
    return in->x*in->x + in->y*in->y;
}

rmReal
RVec2Length( const RVec2* in )
{
    return rmSqrt( in->x*in->x + in->y*in->y );
}

rmReal
RVec2Dot( const RVec2* v1, const RVec2* v2 )
{
    return v1->x*v2->x + v1->y*v2->y;
}

void
RVec2Copy( RVec2* out, const RVec2* in )
{
    out->x = in->x;
    out->y = in->y;
}

void
RVec2Normalize( RVec2* out, const RVec2* in )
{
    rmReal length = RVec2Length( in );
    RVec2Scale( out, in, 1.0f/length );
}

void
RVec2Transform( RVec2* out, const RMtx2* m, const RVec2* in )
{
    RVec2 result, in_vec2;
    int row;

    RVec2Copy( &in_vec2, in );
    for ( row = 0; row < 2; ++row )
    {
        RVec2 row_vector;
        RMtx2GetRow( &row_vector, m, row );
        RVec2SetElement( &result, row, RVec2Dot( &row_vector, &in_vec2 ) );
    }

    out->x = RVec2GetX( &result );
    out->y = RVec2GetY( &result );
}

/*
RMath : Ruby math module for 3D Applications
Copyright (c) 2008-2020 vaiorabbit  <http://twitter.com/vaiorabbit>.

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.
 */
