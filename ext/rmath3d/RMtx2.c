#include <math.h>
#include <string.h>

#include "RVec2.h"
#include "RMtx2.h"

/* NOTE : column-major */
#define SET_ELEMENT(out, row, col, val) (out)->e[(col)*2+(row)] = (val)
#define GET_ELEMENT(in, row, col) ((in)->e[(col)*2+(row)])

void
RMtx2SetElement( RMtx2* out, int row, int col, rmReal e )
{
    SET_ELEMENT( out, row, col, e );
}

rmReal
RMtx2GetElement( const RMtx2* in, int row, int col )
{
    return GET_ELEMENT( in, row, col );
}

void
RMtx2GetRow( RVec2* out, const RMtx2* in, int row )
{
    RVec2SetX( out, GET_ELEMENT( in, row, 0 ) );
    RVec2SetY( out, GET_ELEMENT( in, row, 1 ) );
}

void
RMtx2GetColumn( RVec2* out, const RMtx2* in, int col )
{
    RVec2SetX( out, GET_ELEMENT( in, 0, col ) );
    RVec2SetY( out, GET_ELEMENT( in, 1, col ) );
}

void
RMtx2SetRow( RMtx2* out, const RVec2* in, int row )
{
    SET_ELEMENT( out, row, 0, RVec2GetElement( in, 0 ) );
    SET_ELEMENT( out, row, 1, RVec2GetElement( in, 1 ) );
}

void
RMtx2SetColumn( RMtx2* out, const RVec2* in, int col )
{
    SET_ELEMENT( out, 0, col, RVec2GetElement( in, 0 ) );
    SET_ELEMENT( out, 1, col, RVec2GetElement( in, 1 ) );
}

void
RMtx2Copy( RMtx2* out, const RMtx2* in )
{
    memmove( out, in, sizeof(RMtx2) );
}

void
RMtx2SetElements( RMtx2* out,
                  rmReal e00, rmReal e01,
                  rmReal e10, rmReal e11
    )
{
    SET_ELEMENT( out, 0, 0, e00 );
    SET_ELEMENT( out, 0, 1, e01 );

    SET_ELEMENT( out, 1, 0, e10 );
    SET_ELEMENT( out, 1, 1, e11 );
}

void
RMtx2Zero( RMtx2* out )
{
    int row, col;
    for ( row = 0; row < 2; ++row )
        for ( col = 0; col < 2; ++col )
            SET_ELEMENT( out, row, col, 0.0f );
}

void
RMtx2Identity( RMtx2* out )
{
    int row, col;
    for ( row = 0; row < 2; ++row )
        for ( col = 0; col < 2; ++col )
            SET_ELEMENT( out, row, col, (row==col) ? 1.0f : 0.0f );
}

rmReal
RMtx2Determinant( const RMtx2* in )
{
#define I( r, c ) GET_ELEMENT( in, (r), (c) )
#define D( e00, e01, e10, e11 ) ((e00)*(e11)-(e01)*(e10))

    return D( I(0,0), I(0,1), I(1,0), I(1,1) );

#undef I
#undef D
}

void
RMtx2Transpose( RMtx2* out, const RMtx2* in )
{
    int row, col;
    RMtx2 tmp;
    for ( row = 0; row < 2; ++row )
        for ( col = 0; col < 2; ++col )
            SET_ELEMENT( &tmp, row, col, GET_ELEMENT( in, col, row ) );

    RMtx2Copy( out, &tmp );
}

rmReal
RMtx2Inverse( RMtx2* out, const RMtx2* in )
{
#define I( r, c ) GET_ELEMENT( in, (r), (c) )
#define R( r, c ) GET_ELEMENT( &result, (r), (c) )
#define D( e00, e01, e10, e11 ) ((e00)*(e11)-(e01)*(e10))

    RMtx2 result;
    rmReal det;

    det = D( I(0,0), I(0,1), I(1,0), I(1,1) );

    if ( rmFabs(det) < RMATH_TOLERANCE )
        return det;

    SET_ELEMENT( &result, 0, 0,  I(1,1) );
    SET_ELEMENT( &result, 0, 1, -I(0,1) );

    SET_ELEMENT( &result, 1, 0, -I(1,0) );
    SET_ELEMENT( &result, 1, 1,  I(0,0) );

    RMtx2Scale( out, &result, 1.0f / det );

    return det;

#undef I
#undef R
#undef D
}


/* http://en.wikipedia.org/wiki/Rotation_representation_%28mathematics%29
   http://en.wikipedia.org/wiki/Rotation_matrix
*/
void
RMtx2Rotation( RMtx2* out, rmReal radian )
{
    rmReal s = rmSin( radian );
    rmReal c = rmCos( radian );

    RMtx2Identity( out );
    SET_ELEMENT( out, 0, 0,  c );
    SET_ELEMENT( out, 0, 1, -s );
    SET_ELEMENT( out, 1, 0,  s );
    SET_ELEMENT( out, 1, 1,  c );
}

void
RMtx2Scaling( RMtx2* out, rmReal sx, rmReal sy )
{
    RMtx2Identity( out );
    SET_ELEMENT( out, 0, 0, sx );
    SET_ELEMENT( out, 1, 1, sy );
}


int
RMtx2Equal( const RMtx2* m1, const RMtx2* m2 )
{
    if ( 0 == memcmp( m1, m2, sizeof(RMtx2) ) )
        return !0;
    else
        return 0;
}


void
RMtx2Add( RMtx2* out, const RMtx2* m1, const RMtx2* m2 )
{
    int row, col;
    RMtx2 tmp;
    for ( row = 0; row < 2; ++row )
        for ( col = 0; col < 2; ++col )
            SET_ELEMENT( &tmp, row, col,
                         GET_ELEMENT( m1, row, col ) + GET_ELEMENT( m2, row, col ) );

    RMtx2Copy( out, &tmp );
}

void
RMtx2Sub( RMtx2* out, const RMtx2* m1, const RMtx2* m2 )
{
    int row, col;
    for ( row = 0; row < 2; ++row )
        for ( col = 0; col < 2; ++col )
            SET_ELEMENT( out, row, col,
                         GET_ELEMENT( m1, row, col ) - GET_ELEMENT( m2, row, col ) );
}

void
RMtx2Mul( RMtx2* out, const RMtx2* m1, const RMtx2* m2 )
{
    int row, col;
    RMtx2 tmp;
    for ( row = 0; row < 2; ++row )
    {
        for ( col = 0; col < 2; ++col )
        {
            int i;
            rmReal sum = 0.0f;
            for ( i = 0; i < 2; ++i )
            {
                sum += GET_ELEMENT( m1, row, i ) * GET_ELEMENT( m2, i, col );
            }
            SET_ELEMENT( &tmp, row, col, sum );
        }
    }

    RMtx2Copy( out, &tmp );
}

void
RMtx2Scale( RMtx2* out, const RMtx2* m, rmReal f )
{
    int row, col;
    for ( row = 0; row < 2; ++row )
        for ( col = 0; col < 2; ++col )
            SET_ELEMENT( out, row, col,
                         GET_ELEMENT( m, row, col ) * f );
}

/*
RMath : Ruby math module for 3D Applications
Copyright (c) 2008-2017 vaiorabbit  <http://twitter.com/vaiorabbit>

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
