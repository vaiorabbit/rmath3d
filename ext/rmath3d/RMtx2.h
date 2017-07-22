/* -*- C -*- */
#ifndef RMATHMTX2_H_INCLUDED
#define RMATHMTX2_H_INCLUDED

#include "RType.h"

struct RVec2;

typedef struct RMtx2
{
    union
    {
        /* NOTE : column-major */
        struct
        {
            rmReal e00, e10;
            rmReal e01, e11;
        };
        rmReal e[4];
    };
} RMtx2;

#ifdef __cplusplus
extern "C" {
#endif

void RMtx2SetElements( RMtx2* out,
                       rmReal e00, rmReal e01,
                       rmReal e10, rmReal e11
    );

void    RMtx2SetElement( RMtx2* out, int row, int col, rmReal e );
rmReal  RMtx2GetElement( const RMtx2* in, int row, int col );

void    RMtx2GetRow( struct RVec2* out, const RMtx2* in, int row );
void    RMtx2GetColumn( struct RVec2* out, const RMtx2* in, int col );

void    RMtx2SetRow( struct RMtx2* out, const RVec2* in, int row );
void    RMtx2SetColumn( struct RMtx2* out, const RVec2* in, int col );

void    RMtx2Copy( RMtx2* out, const RMtx2* in );

void    RMtx2Zero( RMtx2* out );
void    RMtx2Identity( RMtx2* out );
rmReal  RMtx2Determinant( const RMtx2* in );
void    RMtx2Transpose( RMtx2* out, const RMtx2* in );
rmReal  RMtx2Inverse( RMtx2* out, const RMtx2* in );

void    RMtx2Rotation( RMtx2* out, rmReal radian );
void    RMtx2Scaling( RMtx2* out, rmReal sx, rmReal sy );

int     RMtx2Equal( const RMtx2* m1, const RMtx2* m2 );

void    RMtx2Add( RMtx2* out, const RMtx2* m1, const RMtx2* m2 );
void    RMtx2Sub( RMtx2* out, const RMtx2* m1, const RMtx2* m2 );
void    RMtx2Mul( RMtx2* out, const RMtx2* m1, const RMtx2* m2 );
void    RMtx2Scale( RMtx2* out, const RMtx2* m, rmReal f );

#ifdef __cplusplus
}
#endif


#endif

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
