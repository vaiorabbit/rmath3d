/* -*- C -*- */
#ifndef RMATHVEC2_H_INCLUDED
#define RMATHVEC2_H_INCLUDED

#include "RType.h"

struct RMtx2;

typedef struct RVec2
{
    union
    {
        struct
        {
            rmReal x, y;
        };
        rmReal e[2];
    };
} RVec2;

#ifdef __cplusplus
extern "C" {
#endif

void    RVec2SetElements( RVec2* out, rmReal x, rmReal y );
void    RVec2SetElement( RVec2* out, int at, rmReal f );
void    RVec2SetX( RVec2* out, rmReal x );
void    RVec2SetY( RVec2* out, rmReal y );

rmReal  RVec2GetElement( const RVec2* in, int at );
rmReal  RVec2GetX( const RVec2* in );
rmReal  RVec2GetY( const RVec2* in );

int     RVec2Equal( const RVec2* v1, const RVec2* v2 );

void    RVec2Add( RVec2* out, const RVec2* v1, const RVec2* v2 );
void    RVec2Sub( RVec2* out, const RVec2* v1, const RVec2* v2 );
void    RVec2Scale( RVec2* out, const RVec2* in, rmReal f );
rmReal  RVec2Cross( const RVec2* v1, const RVec2* v2 );

rmReal  RVec2Length( const RVec2* in );
rmReal  RVec2LengthSq( const RVec2* in );

rmReal  RVec2Dot( const RVec2* v1, const RVec2* v2 );

void    RVec2Copy( RVec2* out, const RVec2* in );
void    RVec2Normalize( RVec2* out, const RVec2* in );

void    RVec2Transform( struct RVec2* out, const struct RMtx2* m, const RVec2* in );


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
