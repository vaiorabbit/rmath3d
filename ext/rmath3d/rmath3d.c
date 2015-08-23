#include <ruby.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "RMath3D.h"

/* #define RMATH_ENABLE_ARGUMENT_CHECK */

/********************************************************************************
 *
 * Common Settings
 *
 ********************************************************************************/

VALUE rb_mRMath;
VALUE rb_cRVec2;
VALUE rb_cRVec3;
VALUE rb_cRVec4;
VALUE rb_cRQuat;
VALUE rb_cRMtx2;
VALUE rb_cRMtx3;
VALUE rb_cRMtx4;

/* RMATH_EXPORT
 * (for Init_RMath to avoid LoadError on +require+)
 */
#ifndef RMATH_EXPORT
#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__)
#   define RMATH_EXPORT __declspec(dllexport)
#else
#   define RMATH_EXPORT
#endif
#endif /* RMATH_EXPORT */

#define IsRVec2(v) rb_obj_is_kind_of( (v), rb_cRVec2 )
#define IsRVec3(v) rb_obj_is_kind_of( (v), rb_cRVec3 )
#define IsRVec4(v) rb_obj_is_kind_of( (v), rb_cRVec4 )
#define IsRQuat(v) rb_obj_is_kind_of( (v), rb_cRQuat )
#define IsRMtx2(v) rb_obj_is_kind_of( (v), rb_cRMtx2 )
#define IsRMtx3(v) rb_obj_is_kind_of( (v), rb_cRMtx3 )
#define IsRMtx4(v) rb_obj_is_kind_of( (v), rb_cRMtx4 )

static void RVec2_free( void* ptr );
static size_t RVec2_memsize( const void* ptr );

static void RVec3_free( void* ptr );
static size_t RVec3_memsize( const void* ptr );

static void RVec4_free( void* ptr );
static size_t RVec4_memsize( const void* ptr );

static void RQuat_free( void* ptr );
static size_t RQuat_memsize( const void* ptr );

static void RMtx2_free( void* ptr );
static size_t RMtx2_memsize( const void* ptr );

static void RMtx3_free( void* ptr );
static size_t RMtx3_memsize( const void* ptr );

static void RMtx4_free( void* ptr );
static size_t RMtx4_memsize( const void* ptr );

static const rb_data_type_t RVec2_type = {
    "RVec2",
    {0, RVec2_free, RVec2_memsize,},
    0, 0,
    RUBY_TYPED_FREE_IMMEDIATELY,
};

static const rb_data_type_t RVec3_type = {
    "RVec3",
    {0, RVec3_free, RVec3_memsize,},
    0, 0,
    RUBY_TYPED_FREE_IMMEDIATELY,
};

static const rb_data_type_t RVec4_type = {
    "RVec4",
    {0, RVec4_free, RVec4_memsize,},
    0, 0,
    RUBY_TYPED_FREE_IMMEDIATELY,
};

static const rb_data_type_t RQuat_type = {
    "RQuat",
    {0, RQuat_free, RQuat_memsize,},
    0, 0,
    RUBY_TYPED_FREE_IMMEDIATELY,
};

static const rb_data_type_t RMtx2_type = {
    "RMtx2",
    {0, RMtx2_free, RMtx2_memsize,},
    0, 0,
    RUBY_TYPED_FREE_IMMEDIATELY,
};

static const rb_data_type_t RMtx3_type = {
    "RMtx3",
    {0, RMtx3_free, RMtx3_memsize,},
    0, 0,
    RUBY_TYPED_FREE_IMMEDIATELY,
};

static const rb_data_type_t RMtx4_type = {
    "RMtx4",
    {0, RMtx4_free, RMtx4_memsize,},
    0, 0,
    RUBY_TYPED_FREE_IMMEDIATELY,
};


#ifdef RMATH_SINGLE_PRECISION
#   define RMATH_FVAL_FMT "%#.6g"
#else
#   define RMATH_FVAL_FMT "%#.15g"
#endif

static VALUE RMtx2_from_source( RMtx2* src );
static VALUE RMtx3_from_source( RMtx3* src );
static VALUE RMtx4_from_source( RMtx4* src );
static VALUE RQuat_from_source( RQuat* src );
static VALUE RVec2_from_source( RVec2* src );
static VALUE RVec3_from_source( RVec3* src );
static VALUE RVec4_from_source( RVec4* src );


/********************************************************************************
 *
 * RMtx2
 *
 ********************************************************************************/

/*
 * Document-class: RMath3D::RMtx2
 * provies 2x2 matrix arithmetic.
 *
 * <b>Notice</b>
 * * elements are stored in column-major order.
 */

static void
RMtx2_free( void* ptr )
{
    xfree( ptr );
}

static size_t
RMtx2_memsize( const void* ptr )
{
    const struct RMtx2* data = ptr;
    return data ? sizeof(*data) : 0;
}

static VALUE
RMtx2_from_source( RMtx2* src )
{
    RMtx2* v = ZALLOC( struct RMtx2 );
    RMtx2Copy( v, src );
    return TypedData_Wrap_Struct( rb_cRMtx2, &RMtx2_type, v );
}


static VALUE
RMtx2_allocate( VALUE klass )
{
    RMtx2* v = ZALLOC( RMtx2 );
    return TypedData_Wrap_Struct( klass, &RMtx2_type, v );
}


/*
 * call-seq:
 *   RMtx2.new -> ((1,0,0),(0,1,0),(0,0,1))
 *   RMtx2.new(e) -> ((e,e,e), (e,e,e), (e,e,e))
 *   RMtx2.new( other ) : Copy Constructor
 *   RMtx2.new( e0, e1, ..., e8 ) -> ((e0,e1,e2), (e3,e4,e5), (e6,e7,e8))
 *
 * Creates a new 2x2 matrix.
 */
static VALUE
RMtx2_initialize( int argc, VALUE* argv, VALUE self )
{
    RMtx2* v = NULL;

    TypedData_Get_Struct( self, RMtx2, &RMtx2_type, v );

    switch( argc )
    {
    case 0:
    {
        /* Default Constructor */
        RMtx2Zero( v );
        return self;
    }
    break;

    case 1:
    {
        VALUE arg = argv[0];
        switch ( TYPE( arg ) )
        {
        case T_FIXNUM:
        case T_FLOAT:
        {
            /* convert to float */
            rmReal f = NUM2DBL( arg );
            RMtx2SetElements( v,
                              f,f,
                              f,f );
            return self;
        }
        break;

        case T_DATA:
        {
            if ( IsRMtx2(arg) )
            {
                /* Copy Constructor */
                RMtx2* other;
                TypedData_Get_Struct( arg, RMtx2, &RMtx2_type, other );
                RMtx2Copy( v, other );
                return self;
            }
            else
                return Qnil;
        }
        break;

        default:
        {
            rb_raise( rb_eTypeError,
                      "RMtx2_new : Unknown type %s.",
                      rb_special_const_p( self ) ? RSTRING_PTR( rb_inspect( self ) ) : rb_obj_classname( self )
                );
            return Qnil;
        }
        break;
        } /* End : switch ( TYPE( arg ) ) */
    } /* End : case 1 */
    break;

    case 4:
    {
        /* Element-wise setter */
        int row, col;
        for ( row = 0; row < 2; ++row )
        {
            for ( col = 0; col < 2; ++col )
            {
                int i = 2*row + col;
                switch ( TYPE( argv[i] ) )
                {
                case T_FIXNUM:
                case T_FLOAT:
                {
                    rmReal f = NUM2DBL( argv[i] );
                    RMtx2SetElement( v, row, col, f );
                }
                break;

                default:
                {
                    rb_raise( rb_eTypeError,
                              "RMtx2_new : Unknown type %s. at arg %d",
                              rb_special_const_p( self ) ? RSTRING_PTR( rb_inspect( self ) ) : rb_obj_classname( self ),
                              i
                        );
                    return Qnil;
                }
                break;
                }
            } /* End : for ( col = 0; col < 2; ++col ) */
        } /* End : for ( row = 0; row < 2; ++row ) */

        return self;
    } /* End : case 4 */
    break;

    default:
    {
        rb_raise( rb_eRuntimeError, "RMtx2_initialize : wrong # of arguments (%d)", argc );
        return Qnil;
    }
    break;
    } /* End : switch( argc ) */
}

/*
 * call-seq: to_s
 *
 * Returns human-readable string.
 */
static VALUE
RMtx2_to_s( VALUE self )
{
    RMtx2* v;
    char dest[2*128], work[2][32], *p;
    int row, col, n;
    rmReal val;

    TypedData_Get_Struct( self, RMtx2, &RMtx2_type, v );

    p = dest;
    for ( row = 0; row < 2; ++row )
    {
        for ( col = 0; col < 2; ++col )
        {
            val = RMtx2GetElement( v, row, col );
            if ( isnan(val) )
                sprintf( work[col], "%s", val >= 0 ? "Inf" : "-Inf" );
            else if ( isinf(val) )
                sprintf( work[col], "%s", "NaN" );
            else
                sprintf( work[col], RMATH_FVAL_FMT, val );
        }
        n = sprintf( p, "(%s, %s)\n", work[0], work[1] );
        p += n;
    }

    return rb_str_new2( dest );
}

/*
 * call-seq: inspect
 *
 * Returns human-readable string.
 */
static VALUE
RMtx2_inspect( VALUE self )
{
    return RMtx2_to_s(self);
}

/*
 * call-seq: to_a
 *
 * Returns its elements as a new Array.
 */
static VALUE
RMtx2_to_a( VALUE self )
{
    int row, col;
    RMtx2* v = NULL;
    VALUE dbl[4];
    TypedData_Get_Struct( self, RMtx2, &RMtx2_type, v );

    /* column-major */
    for ( col = 0; col < 2; ++col )
    {
        for ( row = 0; row < 2; ++row )
        {
            int i = 2*col + row;
            dbl[i] = DBL2NUM(RMtx2GetElement(v,row,col));
        }
    }

    return rb_ary_new4( 4, dbl );
}

/*
 * call-seq: coerse(other)
 *
 * Resolves type mismatch.
 */
static VALUE
RMtx2_coerce( VALUE self, VALUE other )
{
    RMtx2* v = NULL;
    TypedData_Get_Struct( self, RMtx2, &RMtx2_type, v );

    switch( TYPE(other) )
    {
    case T_FLOAT:
    case T_FIXNUM:
    case T_BIGNUM:
    {
        /* 'other (op) RMtx2'
         * -> try again as 'RMtx2 (op) other'
         */
        return rb_ary_new3( 2,  self, other );
    }
    break;

    default:
    {
        rb_raise( rb_eTypeError,
                  "%s can't be coerced into %s",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other ),
                  rb_obj_classname( self )
            );
        return Qnil;
    }
    break;
    }
}

/*
 * call-seq: setElements( e0, e1, e2, e3 )
 *
 * Stores given 4 new values.
 */
static VALUE
RMtx2_setElements( int argc, VALUE* argv, VALUE self )
{
    RMtx2* m = NULL;
    int row, col;

#ifdef RMATH_ENABLE_ARGUMENT_CHECK
    if ( argc != 4 )
    {
        rb_raise( rb_eRuntimeError, "RMtx2_setElements : wrong # of arguments (%d)", argc );
        return Qnil;
    }
#endif

    TypedData_Get_Struct( self, RMtx2, &RMtx2_type, m );
    for ( row = 0; row < 2; ++row )
    {
        for ( col = 0; col < 2; ++col )
        {
            int i = 2*row + col;
            RMtx2SetElement( m, row, col, NUM2DBL(argv[i]) );
        }
    }

    return self;
}

/*
 * call-seq: [row,col]= value
 *
 * Stores +value+ at (+row+,+col+).
 */
static VALUE
RMtx2_setElement( VALUE self, VALUE r, VALUE c, VALUE f )
{
    RMtx2* m = NULL;
    int row, col;
    rmReal flt;

    TypedData_Get_Struct( self, RMtx2, &RMtx2_type, m );
    row = FIX2INT(r);
    col = FIX2INT(c);
    flt = NUM2DBL(f);

    RMtx2SetElement( m, row, col, flt );

    return self;
}

/*
 * call-seq: [row,col] -> value
 *
 * Returns the element at (+row+,+col+).
 */
static VALUE
RMtx2_getElement( VALUE self, VALUE r, VALUE c )
{
    RMtx2* m = NULL;
    int row, col;
    rmReal flt;

    TypedData_Get_Struct( self, RMtx2, &RMtx2_type, m );
    row = FIX2INT(r);
    col = FIX2INT(c);
    flt = RMtx2GetElement( m, row, col );

    return DBL2NUM( flt );
}

/* Returns the element at row 0 and column 0.
 */
static VALUE
RMtx2_get_e00( VALUE self )
{
    RMtx2* m;

    TypedData_Get_Struct( self, RMtx2, &RMtx2_type, m );

    return DBL2NUM( RMtx2GetElement( m, 0, 0 ) );
}

/* Returns the element at row 0 and column 1.
 */
static VALUE
RMtx2_get_e01( VALUE self )
{
    RMtx2* m;

    TypedData_Get_Struct( self, RMtx2, &RMtx2_type, m );

    return DBL2NUM( RMtx2GetElement( m, 0, 1 ) );
}

/* Returns the element at row 1 and column 0.
 */
static VALUE
RMtx2_get_e10( VALUE self )
{
    RMtx2* m;

    TypedData_Get_Struct( self, RMtx2, &RMtx2_type, m );

    return DBL2NUM( RMtx2GetElement( m, 1, 0 ) );
}

/* Returns the element at row 1 and column 1.
 */
static VALUE
RMtx2_get_e11( VALUE self )
{
    RMtx2* m;

    TypedData_Get_Struct( self, RMtx2, &RMtx2_type, m );

    return DBL2NUM( RMtx2GetElement( m, 1, 1 ) );
}

/* Replaces the element at row 0 and column 0 by +value+.
 */
static VALUE
RMtx2_set_e00( VALUE self, VALUE f )
{
    RMtx2* m;

    TypedData_Get_Struct( self, RMtx2, &RMtx2_type, m );

    RMtx2SetElement( m, 0, 0, NUM2DBL(f) );

    return self;
}

/* Replaces the element at row 0 and column 1 by +value+.
 */
static VALUE
RMtx2_set_e01( VALUE self, VALUE f )
{
    RMtx2* m;

    TypedData_Get_Struct( self, RMtx2, &RMtx2_type, m );

    RMtx2SetElement( m, 0, 1, NUM2DBL(f) );

    return self;
}

/* Replaces the element at row 1 and column 0 by +value+.
 */
static VALUE
RMtx2_set_e10( VALUE self, VALUE f )
{
    RMtx2* m;

    TypedData_Get_Struct( self, RMtx2, &RMtx2_type, m );

    RMtx2SetElement( m, 1, 0, NUM2DBL(f) );

    return self;
}

/* Replaces the element at row 1 and column 1 by +value+.
 */
static VALUE
RMtx2_set_e11( VALUE self, VALUE f )
{
    RMtx2* m;

    TypedData_Get_Struct( self, RMtx2, &RMtx2_type, m );

    RMtx2SetElement( m, 1, 1, NUM2DBL(f) );

    return self;
}


/*
 * call-seq: mtx2.getRow(r) -> RVec2
 *
 * Returns +r+-th row vector.
 */
static VALUE
RMtx2_getRow( VALUE self, VALUE row )
{
    RMtx2* m = NULL;
    int at;
    RVec2 out;

    TypedData_Get_Struct( self, RMtx2, &RMtx2_type, m );
    at = FIX2INT(row);
    RMtx2GetRow( &out, m, at );

    return RVec2_from_source( &out );
}

/*
 * call-seq: mtx2.getColumn(c) -> RVec2
 *
 * Returns +c+-th column vector.
 */
static VALUE
RMtx2_getColumn( VALUE self, VALUE column )
{
    RMtx2* m = NULL;
    int at;
    RVec2 out;

    TypedData_Get_Struct( self, RMtx2, &RMtx2_type, m );
    at = FIX2INT(column);
    RMtx2GetColumn( &out, m, at );

    return RVec2_from_source( &out );
}


/*
 * call-seq: mtx2.setRow(v,r)
 *
 * Returns sets +r+-th row by vector +v+.
 */
static VALUE
RMtx2_setRow( VALUE self, VALUE v, VALUE row )
{
    RMtx2* m;
    RVec2* in;
    int at;

    TypedData_Get_Struct( self, RMtx2, &RMtx2_type, m );
    TypedData_Get_Struct( v, RVec2, &RVec2_type, in );
    at = FIX2INT(row);
    RMtx2SetRow( m, in, at );

    return self;
}


/*
 * call-seq: mtx2.setColumn(v,c)
 *
 * Returns sets +c+-th column by vector +v+.
 */
static VALUE
RMtx2_setColumn( VALUE self, VALUE v, VALUE column )
{
    RMtx2* m;
    RVec2* in;
    int at;

    TypedData_Get_Struct( self, RMtx2, &RMtx2_type, m );
    TypedData_Get_Struct( v, RVec2, &RVec2_type, in );
    at = FIX2INT(column);
    RMtx2SetColumn( m, in, at );

    return self;
}



/*
 * call-seq: setZero
 *
 * Clears all elements by 0.0
 */
static VALUE
RMtx2_setZero( VALUE self )
{
    RMtx2* m = NULL;

    TypedData_Get_Struct( self, RMtx2, &RMtx2_type, m );
    RMtx2Zero( m );

    return self;
}

/*
 * call-seq: setIdentity
 *
 * Sets as identity matrix.
 */
static VALUE
RMtx2_setIdentity( VALUE self )
{
    RMtx2* m = NULL;

    TypedData_Get_Struct( self, RMtx2, &RMtx2_type, m );
    RMtx2Identity( m );

    return self;
}

/*
 * call-seq: getDeterminant -> determinant
 *
 * Calculates determinant.
 */
static VALUE
RMtx2_getDeterminant( VALUE self )
{
    RMtx2* m = NULL;
    rmReal f;

    TypedData_Get_Struct( self, RMtx2, &RMtx2_type, m );
    f = RMtx2Determinant( m );

    return DBL2NUM( f );
}

/*
 * call-seq: getTransposed
 *
 * Returns transposed matrix.
 */
static VALUE
RMtx2_transpose( VALUE self )
{
    RMtx2* m = NULL;
    RMtx2 out;

    TypedData_Get_Struct( self, RMtx2, &RMtx2_type, m );
    RMtx2Transpose( &out, m );

    return RMtx2_from_source( &out );
}

/*
 * call-seq: transpose!
 *
 * Transposeas its elements.
 */
static VALUE
RMtx2_transpose_intrusive( VALUE self )
{
    RMtx2* m = NULL;

    TypedData_Get_Struct( self, RMtx2, &RMtx2_type, m );
    RMtx2Transpose( m, m );

    return self;
}

/*
 * call-seq: getInverse -> inverse
 *
 * Returns the inverse.
 */
static VALUE
RMtx2_inverse( VALUE self )
{
    RMtx2* m = NULL;
    RMtx2 out;

    TypedData_Get_Struct( self, RMtx2, &RMtx2_type, m );
    RMtx2Inverse( &out, m );

    return RMtx2_from_source( &out );
}

/*
 * call-seq: invert! -> self
 *
 * Makes itself as the inverse of the original matrix.
 */
static VALUE
RMtx2_invert( VALUE self )
{
    RMtx2* m = NULL;

    TypedData_Get_Struct( self, RMtx2, &RMtx2_type, m );
    RMtx2Inverse( m, m );

    return self;
}

/*
 * call-seq: rotation(radian) -> self
 *
 * Makes a matrix that rotates around the z-axis.
 */
static VALUE
RMtx2_rotation( VALUE self, VALUE radian )
{
    RMtx2* m = NULL;
    rmReal angle_radian;

    TypedData_Get_Struct( self, RMtx2, &RMtx2_type, m );
    angle_radian = NUM2DBL(radian);
    RMtx2Rotation( m, angle_radian );

    return self;
}

/*
 * call-seq: scaling(sx,sy) -> self
 *
 * Makes itself as a scaling matrix.
 */
static VALUE
RMtx2_scaling( VALUE self, VALUE x, VALUE y )
{
    RMtx2* m = NULL;
    rmReal sx, sy;

    TypedData_Get_Struct( self, RMtx2, &RMtx2_type, m );
    sx = NUM2DBL(x);
    sy = NUM2DBL(y);
    RMtx2Scaling( m, sx, sy );

    return self;
}

/*
 * call-seq: +
 *
 * +mtx : Unary plus operator.
 */
static VALUE
RMtx2_op_unary_plus( VALUE self )
{
    return self;
}

/*
 * call-seq: -
 *
 * -mtx : Unary minus operator.
 */
static VALUE
RMtx2_op_unary_minus( VALUE self )
{
    RMtx2* m = NULL;
    RMtx2 out;

    TypedData_Get_Struct( self, RMtx2, &RMtx2_type, m );
    RMtx2Scale( &out, m, (rmReal)(-1) );

    return RMtx2_from_source( &out );
}

/*
 * call-seq: +
 *
 * mtx1 + mtx2 : Binary plus operator.
 */
static VALUE
RMtx2_op_binary_plus( VALUE self, VALUE other )
{
    RMtx2* m1 = NULL;
    RMtx2* m2 = NULL;
    RMtx2 result;

#ifdef RMATH_ENABLE_ARGUMENT_CHECK
    if ( !IsRMtx2(other) )
    {
        rb_raise( rb_eTypeError,
                  "RMtx2#+ : Unknown type %s.",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other )
            );
        return Qnil;
    }
#endif

    TypedData_Get_Struct( self,  RMtx2, &RMtx2_type, m1 );
    TypedData_Get_Struct( other, RMtx2, &RMtx2_type, m2 );
    RMtx2Add( &result, m1, m2 );

    return RMtx2_from_source( &result );
}

/*
 * call-seq: -
 *
 * mtx1 - mtx2 : Binary minus operator.
 */
static VALUE
RMtx2_op_binary_minus( VALUE self, VALUE other )
{
    RMtx2* m1 = NULL;
    RMtx2* m2 = NULL;
    RMtx2 result;

#ifdef RMATH_ENABLE_ARGUMENT_CHECK
    if ( !IsRMtx2(other) )
    {
        rb_raise( rb_eTypeError,
                  "RMtx2#- : Unknown type %s.",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other )
            );
        return Qnil;
    }
#endif

    TypedData_Get_Struct( self,  RMtx2, &RMtx2_type, m1 );
    TypedData_Get_Struct( other, RMtx2, &RMtx2_type, m2 );
    RMtx2Sub( &result, m1, m2 );

    return RMtx2_from_source( &result );
}

/*
 * call-seq: **
 *
 * mtx1 * mtx2 : Binary multiply operator.
 */
static VALUE
RMtx2_op_binary_mult( VALUE self, VALUE other )
{
    RMtx2* m1 = NULL;
    RMtx2* m2 = NULL;
    rmReal f;
    RMtx2 result;

    TypedData_Get_Struct( self,  RMtx2, &RMtx2_type, m1 );

    if ( IsRMtx2(other) )
    {
        TypedData_Get_Struct( other, RMtx2, &RMtx2_type, m2 );
        RMtx2Mul( &result, m1, m2 );
    }
    else
    {
        f = NUM2DBL( other );
        RMtx2Scale( &result, m1, f );
    }

    return RMtx2_from_source( &result );
}

/*
 * call-seq: ==
 *
 * mtx1 == mtx2 : evaluates equality.
 */
static VALUE
RMtx2_op_binary_eq( VALUE self, VALUE other )
{
    if ( IsRMtx2(other) )
    {
        RMtx2* m1 = NULL;
        RMtx2* m2 = NULL;

        TypedData_Get_Struct( self,  RMtx2, &RMtx2_type, m1 );
        TypedData_Get_Struct( other, RMtx2, &RMtx2_type, m2 );

        if ( !RMtx2Equal(m1,m2) )
            return Qfalse;
        else
            return Qtrue;

    }
    else
    {
        return Qfalse;
    }
}

/*
 * call-seq: mtx1.add!( mtx2 )
 *
 * mtx1 += mtx2 : appends the elements of +mtx2+ into corresponding +mtx1+ elements.
 */
static VALUE
RMtx2_op_assign_plus( VALUE self, VALUE other )
{
    RMtx2* m1 = NULL;
    RMtx2* m2 = NULL;

#ifdef RMATH_ENABLE_ARGUMENT_CHECK
    if ( !IsRMtx2(other) )
    {
        rb_raise( rb_eTypeError,
                  "RMtx2#+= : Unknown type %s.",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other )
            );
        return Qnil;
    }
#endif

    TypedData_Get_Struct( self,  RMtx2, &RMtx2_type, m1 );
    TypedData_Get_Struct( other, RMtx2, &RMtx2_type, m2 );
    RMtx2Add( m1, m1, m2 );

    return self;
}

/*
 * call-seq: mtx1.sub!( mtx2 )
 *
 * mtx1 -= mtx2 : subtracts the elements of +mtx2+ from corresponding +mtx1+ elements.
 */
static VALUE
RMtx2_op_assign_minus( VALUE self, VALUE other )
{
    RMtx2* m1 = NULL;
    RMtx2* m2 = NULL;

#ifdef RMATH_ENABLE_ARGUMENT_CHECK
    if ( !IsRMtx2(other) )
    {
        rb_raise( rb_eTypeError,
                  "RMtx2#-= : Unknown type %s.",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other )
            );
        return Qnil;
    }
#endif

    TypedData_Get_Struct( self,  RMtx2, &RMtx2_type, m1 );
    TypedData_Get_Struct( other, RMtx2, &RMtx2_type, m2 );
    RMtx2Sub( m1, m1, m2 );

    return self;
}

/*
 * call-seq: mtx1.mul!( mtx2 )
 *
 * mtx1 *= mtx2
 */
static VALUE
RMtx2_op_assign_mult( VALUE self, VALUE other )
{
    RMtx2* m1 = NULL;
    RMtx2* m2 = NULL;
    rmReal f;

    TypedData_Get_Struct( self,  RMtx2, &RMtx2_type, m1 );

    if ( IsRMtx2(other) )
    {
        TypedData_Get_Struct( other, RMtx2, &RMtx2_type, m2 );
        RMtx2Mul( m1, m1, m2 );
    }
    else
    {
        f = NUM2DBL( other );
        RMtx2Scale( m1, m1, f );
    }

    return self;
}


/********************************************************************************
 *
 * RMtx3
 *
 ********************************************************************************/

/*
 * Document-class: RMath3D::RMtx3
 * provies 3x3 matrix arithmetic.
 *
 * <b>Notice</b>
 * * elements are stored in column-major order.
 */

static void
RMtx3_free( void* ptr )
{
	xfree( ptr );
}

static size_t
RMtx3_memsize( const void* ptr )
{
    const struct RMtx3* data = ptr;
    return data ? sizeof(*data) : 0;
}

static VALUE
RMtx3_from_source( RMtx3* src )
{
    RMtx3* v = ZALLOC( struct RMtx3 );
    RMtx3Copy( v, src );
    return TypedData_Wrap_Struct( rb_cRMtx3, &RMtx3_type, v );
}


static VALUE
RMtx3_allocate( VALUE klass )
{
    RMtx3* v = ZALLOC( RMtx3 );
    return TypedData_Wrap_Struct( klass, &RMtx3_type, v );
}


/*
 * call-seq:
 *   RMtx3.new -> ((1,0,0),(0,1,0),(0,0,1))
 *   RMtx3.new(e) -> ((e,e,e), (e,e,e), (e,e,e))
 *   RMtx3.new( other ) : Copy Constructor
 *   RMtx3.new( e0, e1, ..., e8 ) -> ((e0,e1,e2), (e3,e4,e5), (e6,e7,e8))
 *
 * Creates a new 3x3 matrix.
 */
static VALUE
RMtx3_initialize( int argc, VALUE* argv, VALUE self )
{
    RMtx3* v = NULL;
    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, v );

    switch( argc )
    {
    case 0:
    {
        /* Default Constructor */
        RMtx3Zero( v );
        return self;
    }
    break;

    case 1:
    {
        VALUE arg = argv[0];
        switch ( TYPE( arg ) )
        {
        case T_FIXNUM:
        case T_FLOAT:
        {
            /* convert to float */
            rmReal f = NUM2DBL( arg );
            RMtx3SetElements( v,
                              f,f,f,
                              f,f,f,
                              f,f,f );
            return self;
        }
        break;

        case T_DATA:
        {
            if ( IsRMtx3(arg) )
            {
                /* Copy Constructor */
                RMtx3* other;
                TypedData_Get_Struct( arg , RMtx3, &RMtx3_type, other );
                RMtx3Copy( v, other );
                return self;
            }
            else
                return Qnil;
        }
        break;

        default:
        {
            rb_raise( rb_eTypeError,
                      "RMtx3_new : Unknown type %s.",
                      rb_special_const_p( self ) ? RSTRING_PTR( rb_inspect( self ) ) : rb_obj_classname( self )
                );
            return Qnil;
        }
        break;
        } /* End : switch ( TYPE( arg ) ) */
    } /* End : case 1 */
    break;

    case 9:
    {
        /* Element-wise setter */
        int row, col;
        for ( row = 0; row < 3; ++row )
        {
            for ( col = 0; col < 3; ++col )
            {
                int i = 3*row + col;
                switch ( TYPE( argv[i] ) )
                {
                case T_FIXNUM:
                case T_FLOAT:
                {
                    rmReal f = NUM2DBL( argv[i] );
                    RMtx3SetElement( v, row, col, f );
                }
                break;

                default:
                {
                    rb_raise( rb_eTypeError,
                              "RMtx3_new : Unknown type %s. at arg %d",
                              rb_special_const_p( self ) ? RSTRING_PTR( rb_inspect( self ) ) : rb_obj_classname( self ),
                              i
                        );
                    return Qnil;
                }
                break;
                }
            } /* End : for ( col = 0; col < 3; ++col ) */
        } /* End : for ( row = 0; row < 3; ++row ) */

        return self;
    } /* End : case 9 */
    break;

    default:
    {
        rb_raise( rb_eRuntimeError, "RMtx3_initialize : wrong # of arguments (%d)", argc );
        return Qnil;
    }
    break;
    } /* End : switch( argc ) */
}

/*
 * call-seq: to_s
 *
 * Returns human-readable string.
 */
static VALUE
RMtx3_to_s( VALUE self )
{
    RMtx3* v;
    char dest[3*128], work[3][32], *p;
    int row, col, n;
    rmReal val;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, v );

    p = dest;
    for ( row = 0; row < 3; ++row )
    {
        for ( col = 0; col < 3; ++col )
        {
            val = RMtx3GetElement( v, row, col );
            if ( isnan(val) )
                sprintf( work[col], "%s", val >= 0 ? "Inf" : "-Inf" );
            else if ( isinf(val) )
                sprintf( work[col], "%s", "NaN" );
            else
                sprintf( work[col], RMATH_FVAL_FMT, val );
        }
        n = sprintf( p, "(%s, %s, %s)\n", work[0], work[1], work[2] );
        p += n;
    }

    return rb_str_new2( dest );
}

/*
 * call-seq: inspect
 *
 * Returns human-readable string.
 */
static VALUE
RMtx3_inspect( VALUE self )
{
    return RMtx3_to_s(self);
}

/*
 * call-seq: to_a
 *
 * Returns its elements as a new Array.
 */
static VALUE
RMtx3_to_a( VALUE self )
{
    int row, col;
    RMtx3* v = NULL;
    VALUE dbl[9];
    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, v );

    /* column-major */
    for ( col = 0; col < 3; ++col )
    {
        for ( row = 0; row < 3; ++row )
        {
            int i = 3*col + row;
            dbl[i] = DBL2NUM(RMtx3GetElement(v,row,col));
        }
    }

    return rb_ary_new4( 9, dbl );
}

/*
 * call-seq: coerse(other)
 *
 * Resolves type mismatch.
 */
static VALUE
RMtx3_coerce( VALUE self, VALUE other )
{
    RMtx3* v = NULL;
    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, v );

    switch( TYPE(other) )
    {
    case T_FLOAT:
    case T_FIXNUM:
    case T_BIGNUM:
    {
        /* 'other (op) RMtx3'
         * -> try again as 'RMtx3 (op) other'
         */
        return rb_ary_new3( 2,  self, other );
    }
    break;

    default:
    {
        rb_raise( rb_eTypeError,
                  "%s can't be coerced into %s",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other ),
                  rb_obj_classname( self )
            );
        return Qnil;
    }
    break;
    }
}

/*
 * call-seq: setElements( e0, e1, ..., e8 )
 *
 * Stores given 9 new values.
 */
static VALUE
RMtx3_setElements( int argc, VALUE* argv, VALUE self )
{
    RMtx3* m = NULL;
    int row, col;

#ifdef RMATH_ENABLE_ARGUMENT_CHECK
    if ( argc != 9 )
    {
        rb_raise( rb_eRuntimeError, "RMtx3_setElements : wrong # of arguments (%d)", argc );
        return Qnil;
    }
#endif

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );
    for ( row = 0; row < 3; ++row )
    {
        for ( col = 0; col < 3; ++col )
        {
            int i = 3*row + col;
            RMtx3SetElement( m, row, col, NUM2DBL(argv[i]) );
        }
    }

    return self;
}

/*
 * call-seq: [row,col]= value
 *
 * Stores +value+ at (+row+,+col+).
 */
static VALUE
RMtx3_setElement( VALUE self, VALUE r, VALUE c, VALUE f )
{
    RMtx3* m = NULL;
    int row, col;
    rmReal flt;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );
    row = FIX2INT(r);
    col = FIX2INT(c);
    flt = NUM2DBL(f);

    RMtx3SetElement( m, row, col, flt );

    return self;
}

/*
 * call-seq: [row,col] -> value
 *
 * Returns the element at (+row+,+col+).
 */
static VALUE
RMtx3_getElement( VALUE self, VALUE r, VALUE c )
{
    RMtx3* m = NULL;
    int row, col;
    rmReal flt;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );
    row = FIX2INT(r);
    col = FIX2INT(c);
    flt = RMtx3GetElement( m, row, col );

    return DBL2NUM( flt );
}

/* Returns the element at row 0 and column 0.
 */
static VALUE
RMtx3_get_e00( VALUE self )
{
    RMtx3* m;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );

    return DBL2NUM( RMtx3GetElement( m, 0, 0 ) );
}

/* Returns the element at row 0 and column 1.
 */
static VALUE
RMtx3_get_e01( VALUE self )
{
    RMtx3* m;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );

    return DBL2NUM( RMtx3GetElement( m, 0, 1 ) );
}

/* Returns the element at row 0 and column 2.
 */
static VALUE
RMtx3_get_e02( VALUE self )
{
    RMtx3* m;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );

    return DBL2NUM( RMtx3GetElement( m, 0, 2 ) );
}

/* Returns the element at row 1 and column 0.
 */
static VALUE
RMtx3_get_e10( VALUE self )
{
    RMtx3* m;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );

    return DBL2NUM( RMtx3GetElement( m, 1, 0 ) );
}

/* Returns the element at row 1 and column 1.
 */
static VALUE
RMtx3_get_e11( VALUE self )
{
    RMtx3* m;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );

    return DBL2NUM( RMtx3GetElement( m, 1, 1 ) );
}

/* Returns the element at row 1 and column 2.
 */
static VALUE
RMtx3_get_e12( VALUE self )
{
    RMtx3* m;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );

    return DBL2NUM( RMtx3GetElement( m, 1, 2 ) );
}

/* Returns the element at row 2 and column 0.
 */
static VALUE
RMtx3_get_e20( VALUE self )
{
    RMtx3* m;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );

    return DBL2NUM( RMtx3GetElement( m, 2, 0 ) );
}

/* Returns the element at row 2 and column 1.
 */
static VALUE
RMtx3_get_e21( VALUE self )
{
    RMtx3* m;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );

    return DBL2NUM( RMtx3GetElement( m, 2, 1 ) );
}

/* Returns the element at row 2 and column 2.
 */
static VALUE
RMtx3_get_e22( VALUE self )
{
    RMtx3* m;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );

    return DBL2NUM( RMtx3GetElement( m, 2, 2 ) );
}


/* Replaces the element at row 0 and column 0 by +value+.
 */
static VALUE
RMtx3_set_e00( VALUE self, VALUE f )
{
    RMtx3* m;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );

    RMtx3SetElement( m, 0, 0, NUM2DBL(f) );

    return self;
}

/* Replaces the element at row 0 and column 1 by +value+.
 */
static VALUE
RMtx3_set_e01( VALUE self, VALUE f )
{
    RMtx3* m;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );

    RMtx3SetElement( m, 0, 1, NUM2DBL(f) );

    return self;
}

/* Replaces the element at row 0 and column 2 by +value+.
 */
static VALUE
RMtx3_set_e02( VALUE self, VALUE f )
{
    RMtx3* m;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );

    RMtx3SetElement( m, 0, 2, NUM2DBL(f) );

    return self;
}

/* Replaces the element at row 1 and column 0 by +value+.
 */
static VALUE
RMtx3_set_e10( VALUE self, VALUE f )
{
    RMtx3* m;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );

    RMtx3SetElement( m, 1, 0, NUM2DBL(f) );

    return self;
}

/* Replaces the element at row 1 and column 1 by +value+.
 */
static VALUE
RMtx3_set_e11( VALUE self, VALUE f )
{
    RMtx3* m;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );

    RMtx3SetElement( m, 1, 1, NUM2DBL(f) );

    return self;
}

/* Replaces the element at row 1 and column 2 by +value+.
 */
static VALUE
RMtx3_set_e12( VALUE self, VALUE f )
{
    RMtx3* m;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );

    RMtx3SetElement( m, 1, 2, NUM2DBL(f) );

    return self;
}

/* Replaces the element at row 2 and column 0 by +value+.
 */
static VALUE
RMtx3_set_e20( VALUE self, VALUE f )
{
    RMtx3* m;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );

    RMtx3SetElement( m, 2, 0, NUM2DBL(f) );

    return self;
}

/* Replaces the element at row 2 and column 1 by +value+.
 */
static VALUE
RMtx3_set_e21( VALUE self, VALUE f )
{
    RMtx3* m;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );

    RMtx3SetElement( m, 2, 1, NUM2DBL(f) );

    return self;
}

/* Replaces the element at row 2 and column 2 by +value+.
 */
static VALUE
RMtx3_set_e22( VALUE self, VALUE f )
{
    RMtx3* m;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );

    RMtx3SetElement( m, 2, 2, NUM2DBL(f) );

    return self;
}



/*
 * call-seq: mtx3.getRow(r) -> RVec3
 *
 * Returns +r+-th row vector.
 */
static VALUE
RMtx3_getRow( VALUE self, VALUE row )
{
    RMtx3* m = NULL;
    int at;
    RVec3 out;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );
    at = FIX2INT(row);
    RMtx3GetRow( &out, m, at );

    return RVec3_from_source( &out );
}

/*
 * call-seq: mtx3.getColumn(c) -> RVec3
 *
 * Returns +c+-th column vector.
 */
static VALUE
RMtx3_getColumn( VALUE self, VALUE column )
{
    RMtx3* m = NULL;
    int at;
    RVec3 out;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );
    at = FIX2INT(column);
    RMtx3GetColumn( &out, m, at );

    return RVec3_from_source( &out );
}


/*
 * call-seq: mtx3.setRow(v,r)
 *
 * Returns sets +r+-th row by vector +v+.
 */
static VALUE
RMtx3_setRow( VALUE self, VALUE v, VALUE row )
{
    RMtx3* m;
    RVec3* in;
    int at;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );
    TypedData_Get_Struct( v, RVec3, &RVec3_type, in );
    at = FIX2INT(row);
    RMtx3SetRow( m, in, at );

    return self;
}


/*
 * call-seq: mtx3.setColumn(v,c)
 *
 * Returns sets +c+-th column by vector +v+.
 */
static VALUE
RMtx3_setColumn( VALUE self, VALUE v, VALUE column )
{
    RMtx3* m;
    RVec3* in;
    int at;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );
    TypedData_Get_Struct( v, RVec3, &RVec3_type, in );
    at = FIX2INT(column);
    RMtx3SetColumn( m, in, at );

    return self;
}



/*
 * call-seq: setZero
 *
 * Clears all elements by 0.0
 */
static VALUE
RMtx3_setZero( VALUE self )
{
    RMtx3* m = NULL;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );
    RMtx3Zero( m );

    return self;
}

/*
 * call-seq: setIdentity
 *
 * Sets as identity matrix.
 */
static VALUE
RMtx3_setIdentity( VALUE self )
{
    RMtx3* m = NULL;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );
    RMtx3Identity( m );

    return self;
}

/*
 * call-seq: getDeterminant -> determinant
 *
 * Calculates determinant.
 */
static VALUE
RMtx3_getDeterminant( VALUE self )
{
    RMtx3* m = NULL;
    rmReal f;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );
    f = RMtx3Determinant( m );

    return DBL2NUM( f );
}

/*
 * call-seq: getTransposed
 *
 * Returns transposed matrix.
 */
static VALUE
RMtx3_transpose( VALUE self )
{
    RMtx3* m = NULL;
    RMtx3 out;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );
    RMtx3Transpose( &out, m );

    return RMtx3_from_source( &out );
}

/*
 * call-seq: transpose!
 *
 * Transposeas its elements.
 */
static VALUE
RMtx3_transpose_intrusive( VALUE self )
{
    RMtx3* m = NULL;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );
    RMtx3Transpose( m, m );

    return self;
}

/*
 * call-seq: getInverse -> inverse
 *
 * Returns the inverse.
 */
static VALUE
RMtx3_inverse( VALUE self )
{
    RMtx3* m = NULL;
    RMtx3 out;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );
    RMtx3Inverse( &out, m );

    return RMtx3_from_source( &out );
}

/*
 * call-seq: invert! -> self
 *
 * Makes itself as the inverse of the original matrix.
 */
static VALUE
RMtx3_invert( VALUE self )
{
    RMtx3* m = NULL;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );
    RMtx3Inverse( m, m );

    return self;
}

/*
 * call-seq: rotationX(radian) -> self
 *
 * Makes a matrix that rotates around the x-axis.
 */
static VALUE
RMtx3_rotationX( VALUE self, VALUE radian )
{
    RMtx3* m = NULL;
    rmReal angle_radian;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );
    angle_radian = NUM2DBL(radian);
    RMtx3RotationX( m, angle_radian );

    return self;
}

/*
 * call-seq: rotationY(radian) -> self
 *
 * Makes a matrix that rotates around the y-axis.
 */
static VALUE
RMtx3_rotationY( VALUE self, VALUE radian )
{
    RMtx3* m = NULL;
    rmReal angle_radian;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );
    angle_radian = NUM2DBL(radian);
    RMtx3RotationY( m, angle_radian );

    return self;
}

/*
 * call-seq: rotationZ(radian) -> self
 *
 * Makes a matrix that rotates around the z-axis.
 */
static VALUE
RMtx3_rotationZ( VALUE self, VALUE radian )
{
    RMtx3* m = NULL;
    rmReal angle_radian;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );
    angle_radian = NUM2DBL(radian);
    RMtx3RotationZ( m, angle_radian );

    return self;
}

/*
 * call-seq: rotationAxis(axis,radian) -> self
 *
 * Makes a matrix that rotates around the +axis+.
 */
static VALUE
RMtx3_rotationAxis( VALUE self, VALUE axis, VALUE radian )
{
    RMtx3* m = NULL;
    RVec3* vAxis = NULL;
    rmReal angle_radian;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );
    TypedData_Get_Struct( axis, RVec3, &RVec3_type, vAxis );
    angle_radian = NUM2DBL(radian);
    RMtx3RotationAxis( m, vAxis, angle_radian );

    return self;
}

/*
 * call-seq: rotationQuaternion(q) -> self
 *
 * Makes a rotation matrix from a normalized quaternion +q+.
 */
static VALUE
RMtx3_rotationQuaternion( VALUE self, VALUE quat )
{
    RMtx3* m = NULL;
    RQuat* q = NULL;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );
    TypedData_Get_Struct( quat, RQuat, &RQuat_type, q );
    RMtx3RotationQuaternion( m, q );

    return self;
}

/*
 * call-seq: scaling(sx,sy,sz) -> self
 *
 * Makes itself as a scaling matrix.
 */
static VALUE
RMtx3_scaling( VALUE self, VALUE x, VALUE y, VALUE z )
{
    RMtx3* m = NULL;
    rmReal sx, sy, sz;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );
    sx = NUM2DBL(x);
    sy = NUM2DBL(y);
    sz = NUM2DBL(z);
    RMtx3Scaling( m, sx, sy, sz );

    return self;
}

/*
 * call-seq: +
 *
 * +mtx : Unary plus operator.
 */
static VALUE
RMtx3_op_unary_plus( VALUE self )
{
    return self;
}

/*
 * call-seq: -
 *
 * -mtx : Unary minus operator.
 */
static VALUE
RMtx3_op_unary_minus( VALUE self )
{
    RMtx3* m = NULL;
    RMtx3 out;

    TypedData_Get_Struct( self, RMtx3, &RMtx3_type, m );
    RMtx3Scale( &out, m, (rmReal)(-1) );

    return RMtx3_from_source( &out );
}

/*
 * call-seq: +
 *
 * mtx1 + mtx2 : Binary plus operator.
 */
static VALUE
RMtx3_op_binary_plus( VALUE self, VALUE other )
{
    RMtx3* m1 = NULL;
    RMtx3* m2 = NULL;
    RMtx3 result;

#ifdef RMATH_ENABLE_ARGUMENT_CHECK
    if ( !IsRMtx3(other) )
    {
        rb_raise( rb_eTypeError,
                  "RMtx3#+ : Unknown type %s.",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other )
            );
        return Qnil;
    }
#endif

    TypedData_Get_Struct( self,  RMtx3, &RMtx3_type, m1 );
    TypedData_Get_Struct( other, RMtx3, &RMtx3_type, m2 );
    RMtx3Add( &result, m1, m2 );

    return RMtx3_from_source( &result );
}

/*
 * call-seq: -
 *
 * mtx1 - mtx2 : Binary minus operator.
 */
static VALUE
RMtx3_op_binary_minus( VALUE self, VALUE other )
{
    RMtx3* m1 = NULL;
    RMtx3* m2 = NULL;
    RMtx3 result;

#ifdef RMATH_ENABLE_ARGUMENT_CHECK
    if ( !IsRMtx3(other) )
    {
        rb_raise( rb_eTypeError,
                  "RMtx3#- : Unknown type %s.",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other )
            );
        return Qnil;
    }
#endif

    TypedData_Get_Struct( self,  RMtx3, &RMtx3_type, m1 );
    TypedData_Get_Struct( other, RMtx3, &RMtx3_type, m2 );
    RMtx3Sub( &result, m1, m2 );

    return RMtx3_from_source( &result );
}

/*
 * call-seq: **
 *
 * mtx1 * mtx2 : Binary multiply operator.
 */
static VALUE
RMtx3_op_binary_mult( VALUE self, VALUE other )
{
    RMtx3* m1 = NULL;
    RMtx3* m2 = NULL;
    rmReal f;
    RMtx3 result;

    TypedData_Get_Struct( self,  RMtx3, &RMtx3_type, m1 );

    if ( IsRMtx3(other) )
    {
        TypedData_Get_Struct( other, RMtx3, &RMtx3_type, m2 );
        RMtx3Mul( &result, m1, m2 );
    }
    else
    {
        f = NUM2DBL( other );
        RMtx3Scale( &result, m1, f );
    }

    return RMtx3_from_source( &result );
}

/*
 * call-seq: ==
 *
 * mtx1 == mtx2 : evaluates equality.
 */
static VALUE
RMtx3_op_binary_eq( VALUE self, VALUE other )
{
    if ( IsRMtx3(other) )
    {
        RMtx3* m1 = NULL;
        RMtx3* m2 = NULL;

        TypedData_Get_Struct( self,  RMtx3, &RMtx3_type, m1 );
        TypedData_Get_Struct( other, RMtx3, &RMtx3_type, m2 );

        if ( !RMtx3Equal(m1,m2) )
            return Qfalse;
        else
            return Qtrue;
    }
    else
    {
        return Qfalse;
    }
}

/*
 * call-seq: mtx1.add!( mtx2 )
 *
 * mtx1 += mtx2 : appends the elements of +mtx2+ into corresponding +mtx1+ elements.
 */
static VALUE
RMtx3_op_assign_plus( VALUE self, VALUE other )
{
    RMtx3* m1 = NULL;
    RMtx3* m2 = NULL;

#ifdef RMATH_ENABLE_ARGUMENT_CHECK
    if ( !IsRMtx3(other) )
    {
        rb_raise( rb_eTypeError,
                  "RMtx3#+= : Unknown type %s.",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other )
            );
        return Qnil;
    }
#endif

    TypedData_Get_Struct( self,  RMtx3, &RMtx3_type, m1 );
    TypedData_Get_Struct( other, RMtx3, &RMtx3_type, m2 );
    RMtx3Add( m1, m1, m2 );

    return self;
}

/*
 * call-seq: mtx1.sub!( mtx2 )
 *
 * mtx1 -= mtx2 : subtracts the elements of +mtx2+ from corresponding +mtx1+ elements.
 */
static VALUE
RMtx3_op_assign_minus( VALUE self, VALUE other )
{
    RMtx3* m1 = NULL;
    RMtx3* m2 = NULL;

#ifdef RMATH_ENABLE_ARGUMENT_CHECK
    if ( !IsRMtx3(other) )
    {
        rb_raise( rb_eTypeError,
                  "RMtx3#-= : Unknown type %s.",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other )
            );
        return Qnil;
    }
#endif

    TypedData_Get_Struct( self,  RMtx3, &RMtx3_type, m1 );
    TypedData_Get_Struct( other, RMtx3, &RMtx3_type, m2 );
    RMtx3Sub( m1, m1, m2 );

    return self;
}

/*
 * call-seq: mtx1.mul!( mtx2 )
 *
 * mtx1 *= mtx2
 */
static VALUE
RMtx3_op_assign_mult( VALUE self, VALUE other )
{
    RMtx3* m1 = NULL;
    RMtx3* m2 = NULL;
    rmReal f;

    TypedData_Get_Struct( self,  RMtx3, &RMtx3_type, m1 );

    if ( IsRMtx3(other) )
    {
        TypedData_Get_Struct( other, RMtx3, &RMtx3_type, m2 );
        RMtx3Mul( m1, m1, m2 );
    }
    else
    {
        f = NUM2DBL( other );
        RMtx3Scale( m1, m1, f );
    }

    return self;
}


/********************************************************************************
 *
 * RMtx4
 *
 ********************************************************************************/

/*
 * Document-class: RMath3D::RMtx4
 * provies 4x4 matrix arithmetic.
 *
 * <b>Notice</b>
 * * elements are stored in column-major order.
 */

static void
RMtx4_free( void* ptr )
{
	xfree( ptr );
}

static size_t
RMtx4_memsize( const void* ptr )
{
    const struct RMtx4* data = ptr;
    return data ? sizeof(*data) : 0;
}

static VALUE
RMtx4_from_source( RMtx4* src )
{
    RMtx4* v = ZALLOC( struct RMtx4 );
    RMtx4Copy( v, src );
    return TypedData_Wrap_Struct( rb_cRMtx4, &RMtx4_type, v );
}


static VALUE
RMtx4_allocate( VALUE klass )
{
    RMtx4* v = ZALLOC( RMtx4 );
    return TypedData_Wrap_Struct( klass, &RMtx4_type, v );
}


/*
 * call-seq:
 *   RMtx4.new -> ((1,0,0,0),(0,1,0,0),(0,0,1,0),(0,0,0,1))
 *   RMtx4.new(e) -> ((e,e,e,e),(e,e,e,e),(e,e,e,e),(e,e,e,e))
 *   RMtx4.new( other ) : Copy Constructor
 *   RMtx4.new( e0, e1, ..., e15 ) -> ((e0,e1,e2,e3),(e4,e5,e6,e7),(e8,e9,e10,e11),(e12,e13,e14,e15))
 *
 * Creates a new 4x4 matrix.
 */
static VALUE
RMtx4_initialize( int argc, VALUE* argv, VALUE self )
{
    RMtx4* v = NULL;
    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, v );

    switch( argc )
    {
    case 0:
    {
        /* Default Constructor */
        RMtx4Zero( v );
        return self;
    }
    break;

    case 1:
    {
        VALUE arg = argv[0];
        switch ( TYPE( arg ) )
        {
        case T_FIXNUM:
        case T_FLOAT:
        {
            /* convert to float */
            rmReal f = NUM2DBL( arg );
            RMtx4SetElements( v,
                              f,f,f,f,
                              f,f,f,f,
                              f,f,f,f,
                              f,f,f,f );
            return self;
        }
        break;

        case T_DATA:
        {
            if ( IsRMtx4(arg) )
            {
                /* Copy Constructor */
                RMtx4* other;
                TypedData_Get_Struct( arg , RMtx4, &RMtx4_type, other );
                RMtx4Copy( v, other );
                return self;
            }
            else
                return Qnil;
        }
        break;

        default:
        {
            rb_raise( rb_eTypeError,
                      "RMtx4_new : Unknown type %s.",
                      rb_special_const_p( self ) ? RSTRING_PTR( rb_inspect( self ) ) : rb_obj_classname( self )
                );
            return Qnil;
        }
        break;
        } /* End : switch ( TYPE( arg ) ) */
    } /* End : case 1 */
    break;

    case 16:
    {
        /* Element-wise setter */
        int row, col;
        for ( row = 0; row < 4; ++row )
        {
            for ( col = 0; col < 4; ++col )
            {
                int i = 4*row + col;
                switch ( TYPE( argv[i] ) )
                {
                case T_FIXNUM:
                case T_FLOAT:
                {
                    rmReal f = NUM2DBL( argv[i] );
                    RMtx4SetElement( v, row, col, f );
                }
                break;

                default:
                {
                    rb_raise( rb_eTypeError,
                              "RMtx4_new : Unknown type %s. at arg %d",
                              rb_special_const_p( self ) ? RSTRING_PTR( rb_inspect( self ) ) : rb_obj_classname( self ),
                              i
                        );
                    return Qnil;
                }
                break;
                }
            } /* End : for ( col = 0; col < 4; ++col ) */
        } /* End : for ( row = 0; row < 4; ++row ) */

        return self;
    } /* End : case 16 */
    break;

    default:
    {
        rb_raise( rb_eRuntimeError, "RMtx4_initialize : wrong # of arguments (%d)", argc );
        return Qnil;
    }
    break;
    } /* End : switch( argc ) */
}

/*
 * call-seq: to_s
 *
 * Returns human-readable string.
 */
static VALUE
RMtx4_to_s( VALUE self )
{
    RMtx4* v;
    char dest[4*128], work[4][32], *p;
    int row, col, n;
    rmReal val;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, v );

    p = dest;
    for ( row = 0; row < 4; ++row )
    {
        for ( col = 0; col < 4; ++col )
        {
            val = RMtx4GetElement( v, row, col );
            if ( isnan(val) )
                sprintf( work[col], "%s", val >= 0 ? "Inf" : "-Inf" );
            else if ( isinf(val) )
                sprintf( work[col], "%s", "NaN" );
            else
                sprintf( work[col], RMATH_FVAL_FMT, val );
        }
        n = sprintf( p, "(%s, %s, %s, %s)\n", work[0], work[1], work[2], work[3] );
        p += n;
    }

    return rb_str_new2( dest );
}


/*
 * call-seq: inspect
 *
 * Returns human-readable string.
 */
static VALUE
RMtx4_inspect( VALUE self )
{
    return RMtx4_to_s(self);
}


/*
 * call-seq: to_a
 *
 * Returns its elements as a new Array.
 */
static VALUE
RMtx4_to_a( VALUE self )
{
    int row, col;
    RMtx4* v = NULL;
    VALUE dbl[16];
    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, v );

    /* column-major */
    for ( col = 0; col < 4; ++col )
    {
        for ( row = 0; row < 4; ++row )
        {
            int i = 4*col + row;
            dbl[i] = DBL2NUM(RMtx4GetElement(v,row,col));
        }
    }

    return rb_ary_new4( 16, dbl );
}

/*
 * call-seq: coerse(other)
 *
 * Resolves type mismatch.
 */
static VALUE
RMtx4_coerce( VALUE self, VALUE other )
{
    RMtx4* v = NULL;
    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, v );

    switch( TYPE(other) )
    {
    case T_FLOAT:
    case T_FIXNUM:
    case T_BIGNUM:
    {
        /* 'other (op) RMtx4'
         * -> try again as 'RMtx4 (op) other'
         */
        return rb_ary_new3( 2,  self, other );
    }
    break;

    default:
    {
        rb_raise( rb_eTypeError,
                  "%s can't be coerced into %s",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other ),
                  rb_obj_classname( self )
            );
        return Qnil;
    }
    break;
    }
}

/*
 * call-seq: setElements( e0, e1, ..., e15 )
 *
 * Stores given 16 new values.
 */
static VALUE
RMtx4_setElements( int argc, VALUE* argv, VALUE self )
{
    RMtx4* m = NULL;
    int row, col;

#ifdef RMATH_ENABLE_ARGUMENT_CHECK
    if ( argc != 16 )
    {
        rb_raise( rb_eRuntimeError, "RMtx4_setElements : wrong # of arguments (%d)", argc );
        return Qnil;
    }
#endif

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );
    for ( row = 0; row < 4; ++row )
    {
        for ( col = 0; col < 4; ++col )
        {
            int i = 4*row + col;
            RMtx4SetElement( m, row, col, NUM2DBL(argv[i]) );
        }
    }

    return self;
}

/*
 * call-seq: [row,col]= value
 *
 * Stores +value+ at (+row+,+col+).
 */
static VALUE
RMtx4_setElement( VALUE self, VALUE r, VALUE c, VALUE f )
{
    RMtx4* m = NULL;
    int row, col;
    rmReal flt;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );
    row = FIX2INT(r);
    col = FIX2INT(c);
    flt = NUM2DBL(f);

    RMtx4SetElement( m, row, col, flt );

    return self;
}

/*
 * call-seq: [row,col] -> value
 *
 * Returns the element at (+row+,+col+).
 */
static VALUE
RMtx4_getElement( VALUE self, VALUE r, VALUE c )
{
    RMtx4* m = NULL;
    int row, col;
    rmReal flt;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );
    row = FIX2INT(r);
    col = FIX2INT(c);
    flt = RMtx4GetElement( m, row, col );

    return DBL2NUM( flt );
}

/* Returns the element at row 0 and column 0.
 */
static VALUE
RMtx4_get_e00( VALUE self )
{
    RMtx4* m;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );

    return DBL2NUM( RMtx4GetElement( m, 0, 0 ) );
}


/* Returns the element at row 0 and column 1.
 */
static VALUE
RMtx4_get_e01( VALUE self )
{
    RMtx4* m;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );

    return DBL2NUM( RMtx4GetElement( m, 0, 1 ) );
}


/* Returns the element at row 0 and column 2.
 */
static VALUE
RMtx4_get_e02( VALUE self )
{
    RMtx4* m;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );

    return DBL2NUM( RMtx4GetElement( m, 0, 2 ) );
}


/* Returns the element at row 0 and column 3.
 */
static VALUE
RMtx4_get_e03( VALUE self )
{
    RMtx4* m;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );

    return DBL2NUM( RMtx4GetElement( m, 0, 3 ) );
}


/* Returns the element at row 1 and column 0.
 */
static VALUE
RMtx4_get_e10( VALUE self )
{
    RMtx4* m;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );

    return DBL2NUM( RMtx4GetElement( m, 1, 0 ) );
}


/* Returns the element at row 1 and column 1.
 */
static VALUE
RMtx4_get_e11( VALUE self )
{
    RMtx4* m;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );

    return DBL2NUM( RMtx4GetElement( m, 1, 1 ) );
}


/* Returns the element at row 1 and column 2.
 */
static VALUE
RMtx4_get_e12( VALUE self )
{
    RMtx4* m;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );

    return DBL2NUM( RMtx4GetElement( m, 1, 2 ) );
}


/* Returns the element at row 1 and column 3.
 */
static VALUE
RMtx4_get_e13( VALUE self )
{
    RMtx4* m;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );

    return DBL2NUM( RMtx4GetElement( m, 1, 3 ) );
}


/* Returns the element at row 2 and column 0.
 */
static VALUE
RMtx4_get_e20( VALUE self )
{
    RMtx4* m;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );

    return DBL2NUM( RMtx4GetElement( m, 2, 0 ) );
}


/* Returns the element at row 2 and column 1.
 */
static VALUE
RMtx4_get_e21( VALUE self )
{
    RMtx4* m;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );

    return DBL2NUM( RMtx4GetElement( m, 2, 1 ) );
}


/* Returns the element at row 2 and column 2.
 */
static VALUE
RMtx4_get_e22( VALUE self )
{
    RMtx4* m;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );

    return DBL2NUM( RMtx4GetElement( m, 2, 2 ) );
}


/* Returns the element at row 2 and column 3.
 */
static VALUE
RMtx4_get_e23( VALUE self )
{
    RMtx4* m;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );

    return DBL2NUM( RMtx4GetElement( m, 2, 3 ) );
}


/* Returns the element at row 3 and column 0.
 */
static VALUE
RMtx4_get_e30( VALUE self )
{
    RMtx4* m;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );

    return DBL2NUM( RMtx4GetElement( m, 3, 0 ) );
}


/* Returns the element at row 3 and column 1.
 */
static VALUE
RMtx4_get_e31( VALUE self )
{
    RMtx4* m;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );

    return DBL2NUM( RMtx4GetElement( m, 3, 1 ) );
}


/* Returns the element at row 3 and column 2.
 */
static VALUE
RMtx4_get_e32( VALUE self )
{
    RMtx4* m;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );

    return DBL2NUM( RMtx4GetElement( m, 3, 2 ) );
}


/* Returns the element at row 3 and column 3.
 */
static VALUE
RMtx4_get_e33( VALUE self )
{
    RMtx4* m;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );

    return DBL2NUM( RMtx4GetElement( m, 3, 3 ) );
}

/* Replaces the element at row 0 and column 0 by +value+.
 */
static VALUE
RMtx4_set_e00( VALUE self, VALUE f )
{
    RMtx4* m;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );

    RMtx4SetElement( m, 0, 0, NUM2DBL(f) );

    return self;
}


/* Replaces the element at row 0 and column 1 by +value+.
 */
static VALUE
RMtx4_set_e01( VALUE self, VALUE f )
{
    RMtx4* m;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );

    RMtx4SetElement( m, 0, 1, NUM2DBL(f) );

    return self;
}


/* Replaces the element at row 0 and column 2 by +value+.
 */
static VALUE
RMtx4_set_e02( VALUE self, VALUE f )
{
    RMtx4* m;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );

    RMtx4SetElement( m, 0, 2, NUM2DBL(f) );

    return self;
}


/* Replaces the element at row 0 and column 3 by +value+.
 */
static VALUE
RMtx4_set_e03( VALUE self, VALUE f )
{
    RMtx4* m;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );

    RMtx4SetElement( m, 0, 3, NUM2DBL(f) );

    return self;
}


/* Replaces the element at row 1 and column 0 by +value+.
 */
static VALUE
RMtx4_set_e10( VALUE self, VALUE f )
{
    RMtx4* m;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );

    RMtx4SetElement( m, 1, 0, NUM2DBL(f) );

    return self;
}


/* Replaces the element at row 1 and column 1 by +value+.
 */
static VALUE
RMtx4_set_e11( VALUE self, VALUE f )
{
    RMtx4* m;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );

    RMtx4SetElement( m, 1, 1, NUM2DBL(f) );

    return self;
}


/* Replaces the element at row 1 and column 2 by +value+.
 */
static VALUE
RMtx4_set_e12( VALUE self, VALUE f )
{
    RMtx4* m;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );

    RMtx4SetElement( m, 1, 2, NUM2DBL(f) );

    return self;
}


/* Replaces the element at row 1 and column 3 by +value+.
 */
static VALUE
RMtx4_set_e13( VALUE self, VALUE f )
{
    RMtx4* m;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );

    RMtx4SetElement( m, 1, 3, NUM2DBL(f) );

    return self;
}


/* Replaces the element at row 2 and column 0 by +value+.
 */
static VALUE
RMtx4_set_e20( VALUE self, VALUE f )
{
    RMtx4* m;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );

    RMtx4SetElement( m, 2, 0, NUM2DBL(f) );

    return self;
}


/* Replaces the element at row 2 and column 1 by +value+.
 */
static VALUE
RMtx4_set_e21( VALUE self, VALUE f )
{
    RMtx4* m;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );

    RMtx4SetElement( m, 2, 1, NUM2DBL(f) );

    return self;
}


/* Replaces the element at row 2 and column 2 by +value+.
 */
static VALUE
RMtx4_set_e22( VALUE self, VALUE f )
{
    RMtx4* m;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );

    RMtx4SetElement( m, 2, 2, NUM2DBL(f) );

    return self;
}


/* Replaces the element at row 2 and column 3 by +value+.
 */
static VALUE
RMtx4_set_e23( VALUE self, VALUE f )
{
    RMtx4* m;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );

    RMtx4SetElement( m, 2, 3, NUM2DBL(f) );

    return self;
}


/* Replaces the element at row 3 and column 0 by +value+.
 */
static VALUE
RMtx4_set_e30( VALUE self, VALUE f )
{
    RMtx4* m;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );

    RMtx4SetElement( m, 3, 0, NUM2DBL(f) );

    return self;
}


/* Replaces the element at row 3 and column 1 by +value+.
 */
static VALUE
RMtx4_set_e31( VALUE self, VALUE f )
{
    RMtx4* m;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );

    RMtx4SetElement( m, 3, 1, NUM2DBL(f) );

    return self;
}


/* Replaces the element at row 3 and column 2 by +value+.
 */
static VALUE
RMtx4_set_e32( VALUE self, VALUE f )
{
    RMtx4* m;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );

    RMtx4SetElement( m, 3, 2, NUM2DBL(f) );

    return self;
}


/* Replaces the element at row 3 and column 3 by +value+.
 */
static VALUE
RMtx4_set_e33( VALUE self, VALUE f )
{
    RMtx4* m;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );

    RMtx4SetElement( m, 3, 3, NUM2DBL(f) );

    return self;
}


/*
 * call-seq: mtx4.getRow(r) -> RVec4
 *
 * Returns +r+-th row vector.
 */
static VALUE
RMtx4_getRow( VALUE self, VALUE row )
{
    RMtx4* m = NULL;
    int at;
    RVec4 out;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );
    at = FIX2INT(row);
    RMtx4GetRow( &out, m, at );

    return RVec4_from_source( &out );
}

/*
 * call-seq: mtx4.getColumn(c) -> RVec4
 *
 * Returns +c+-th column vector.
 */
static VALUE
RMtx4_getColumn( VALUE self, VALUE column )
{
    RMtx4* m = NULL;
    int at;
    RVec4 out;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );
    at = FIX2INT(column);
    RMtx4GetColumn( &out, m, at );

    return RVec4_from_source( &out );
}


/*
 * call-seq: mtx4.setRow(v,r)
 *
 * Returns sets +r+-th row by vector +v+.
 */
static VALUE
RMtx4_setRow( VALUE self, VALUE v, VALUE row )
{
    RMtx4* m;
    RVec4* in;
    int at;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );
    TypedData_Get_Struct( v, RVec4, &RVec4_type, in );
    at = FIX2INT(row);
    RMtx4SetRow( m, in, at );

    return self;
}


/*
 * call-seq: mtx4.setColumn(v,c)
 *
 * Returns sets +c+-th column by vector +v+.
 */
static VALUE
RMtx4_setColumn( VALUE self, VALUE v, VALUE column )
{
    RMtx4* m;
    RVec4* in;
    int at;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );
    TypedData_Get_Struct( v, RVec4, &RVec4_type, in );
    at = FIX2INT(column);
    RMtx4SetColumn( m, in, at );

    return self;
}


/*
 * call-seq: mtx4.getUpper3x3 -> RMtx3
 *
 * Returns RMtx3 containing upper 3 x 3 elements of mtx4.
 */
static VALUE
RMtx4_getUpper3x3( VALUE self )
{
    RMtx4* mtx4x4 = NULL;
    RMtx3 out;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, mtx4x4 );
    RMtx4GetUpper3x3( &out, mtx4x4 );

    return RMtx3_from_source( &out );
}


/*
 * call-seq: mtx4.setUpper3x3( mtx3 )
 *
 * Fills upper 3 x 3 elements of mtx4 with mtx3.
 */
static VALUE
RMtx4_setUpper3x3( VALUE self, VALUE in )
{
    RMtx3* mtx3x3 = NULL;
    RMtx4* mtx4x4 = NULL;

    TypedData_Get_Struct( in, RMtx3, &RMtx3_type, mtx3x3 );
    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, mtx4x4 );
    RMtx4SetUpper3x3( mtx4x4, mtx3x3 );

    return self;
}



/*
 * call-seq: setZero
 *
 * Clears all elements by 0.0
 */
static VALUE
RMtx4_setZero( VALUE self )
{
    RMtx4* m = NULL;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );
    RMtx4Zero( m );

    return self;
}

/*
 * call-seq: setIdentity
 *
 * Sets as identity matrix.
 */
static VALUE
RMtx4_setIdentity( VALUE self )
{
    RMtx4* m = NULL;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );
    RMtx4Identity( m );

    return self;
}

/*
 * call-seq: getDeterminant -> determinant
 *
 * Calculates determinant.
 */
static VALUE
RMtx4_getDeterminant( VALUE self )
{
    RMtx4* m = NULL;
    rmReal f;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );
    f = RMtx4Determinant( m );

    return DBL2NUM( f );
}

/*
 * call-seq: getTransposed
 *
 * Returns transposed matrix.
 */
static VALUE
RMtx4_transpose( VALUE self )
{
    RMtx4* m = NULL;
    RMtx4 out;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );
    RMtx4Transpose( &out, m );

    return RMtx4_from_source( &out );
}

/*
 * call-seq: transpose!
 *
 * Transposeas its elements.
 */
static VALUE
RMtx4_transpose_intrusive( VALUE self )
{
    RMtx4* m = NULL;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );
    RMtx4Transpose( m, m );

    return self;
}

/*
 * call-seq: getInverse -> inverse
 *
 * Returns the inverse.
 */
static VALUE
RMtx4_inverse( VALUE self )
{
    RMtx4* m = NULL;
    RMtx4 out;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );
    RMtx4Inverse( &out, m );

    return RMtx4_from_source( &out );
}

/*
 * call-seq: invert! -> self
 *
 * Makes itself as the inverse of the original matrix.
 */
static VALUE
RMtx4_inverse_intrusive( VALUE self )
{
    RMtx4* m = NULL;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );
    RMtx4Inverse( m, m );

    return self;
}

/*
 * call-seq: translation(tx,ty,tz) -> self
 *
 * Makes itself as a translation matrix.
 */
static VALUE
RMtx4_translation( VALUE self, VALUE x, VALUE y, VALUE z )
{
    RMtx4* m = NULL;
    rmReal tx, ty, tz;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );
    tx = NUM2DBL(x);
    ty = NUM2DBL(y);
    tz = NUM2DBL(z);
    RMtx4Translation( m, tx, ty, tz );

    return self;
}

/*
 * call-seq: rotationX(radian) -> self
 *
 * Makes a matrix that rotates around the x-axis.
 */
static VALUE
RMtx4_rotationX( VALUE self, VALUE radian )
{
    RMtx4* m = NULL;
    rmReal angle_radian;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );
    angle_radian = NUM2DBL(radian);
    RMtx4RotationX( m, angle_radian );

    return self;
}

/*
 * call-seq: rotationY(radian) -> self
 *
 * Makes a matrix that rotates around the y-axis.
 */
static VALUE
RMtx4_rotationY( VALUE self, VALUE radian )
{
    RMtx4* m = NULL;
    rmReal angle_radian;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );
    angle_radian = NUM2DBL(radian);
    RMtx4RotationY( m, angle_radian );

    return self;
}

/*
 * call-seq: rotationZ(radian) -> self
 *
 * Makes a matrix that rotates around the z-axis.
 */
static VALUE
RMtx4_rotationZ( VALUE self, VALUE radian )
{
    RMtx4* m = NULL;
    rmReal angle_radian;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );
    angle_radian = NUM2DBL(radian);
    RMtx4RotationZ( m, angle_radian );

    return self;
}

/*
 * call-seq: rotationAxis(axis,radian) -> self
 *
 * Makes a matrix that rotates around the +axis+.
 */
static VALUE
RMtx4_rotationAxis( VALUE self, VALUE axis, VALUE radian )
{
    RMtx4* m = NULL;
    RVec3* vAxis = NULL;
    rmReal angle_radian;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );
    TypedData_Get_Struct( axis, RVec3, &RVec3_type, vAxis );
    angle_radian = NUM2DBL(radian);
    RMtx4RotationAxis( m, vAxis, angle_radian );

    return self;
}

/*
 * call-seq: rotationQuaternion(q) -> self
 *
 * Makes a rotation matrix from a normalized quaternion +q+.
 */
static VALUE
RMtx4_rotationQuaternion( VALUE self, VALUE q )
{
    RMtx4*      m    = NULL;
    RQuat*      quat = NULL;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );
    TypedData_Get_Struct( q, RQuat, &RQuat_type, quat );
    RMtx4RotationQuaternion( m, quat );

    return self;
}

/*
 * call-seq: scaling(sx,sy,sz) -> self
 *
 * Makes itself as a scaling matrix.
 */
static VALUE
RMtx4_scaling( VALUE self, VALUE x, VALUE y, VALUE z )
{
    RMtx4* m = NULL;
    rmReal sx, sy, sz;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );
    sx = NUM2DBL(x);
    sy = NUM2DBL(y);
    sz = NUM2DBL(z);
    RMtx4Scaling( m, sx, sy, sz );

    return self;
}

/*
 * call-seq: lookAtRH(eye,at,up) -> self
 *
 * Builds a viewing matrix for a right-handed coordinate system from:
 * * eye position (+eye+: RVec3)
 * * a point looking at (+at+: RVec3)
 * * up vector (+up+: RVec3)
 */
static VALUE
RMtx4_lookAtRH( VALUE self, VALUE e, VALUE a, VALUE u )
{
    RMtx4*  m   = NULL;
    RVec3*  eye = NULL;
    RVec3*  at  = NULL;
    RVec3*  up  = NULL;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );
    TypedData_Get_Struct( e, RVec3, &RVec3_type, eye );
    TypedData_Get_Struct( a, RVec3, &RVec3_type, at );
    TypedData_Get_Struct( u, RVec3, &RVec3_type, up );
    RMtx4LookAtRH( m, eye, at, up );

    return self;
}

/*
 * call-seq: perspectiveRH(width,height,znear,zfar) -> self
 *
 * Builds a perspective projection matrix for a right-handed coordinate system from:
 * * View volume width (+width+)
 * * View volume height (+height+)
 * * Near clip plane distance (+znear+)
 * * Far clip plane distance (+zfar+)
 */
static VALUE
RMtx4_perspectiveRH( VALUE self, VALUE w, VALUE h, VALUE zn, VALUE zf )
{
    RMtx4* m = NULL;
    rmReal width, height, znear, zfar;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );
    width  = NUM2DBL(w);
    height = NUM2DBL(h);
    znear  = NUM2DBL(zn);
    zfar   = NUM2DBL(zf);

    RMtx4PerspectiveRH( m, width, height, znear, zfar );

    return self;
}

/*
 * call-seq: perspectiveFovRH(fovy,aspect,znear,zfar) -> self
 *
 * Builds a perspective projection matrix for a right-handed coordinate system from:
 * * Field of view in y direction (+fovy+ radian)
 * * Aspect ratio (+aspect+)
 * * Near clip plane distance (+znear+)
 * * Far clip plane distance (+zfar+)
 */
static VALUE
RMtx4_perspectiveFovRH( VALUE self, VALUE fovy, VALUE asp, VALUE zn, VALUE zf )
{
    RMtx4* m = NULL;
    rmReal fovy_radian, aspect, znear, zfar;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );
    fovy_radian = NUM2DBL(fovy);
    aspect      = NUM2DBL(asp);
    znear       = NUM2DBL(zn);
    zfar        = NUM2DBL(zf);

    RMtx4PerspectiveFovRH( m, fovy_radian, aspect, znear, zfar );

    return self;
}

/*
 * call-seq: perspectiveOffCenterRH(left,right,bottom,top,znear,zfar) -> self
 *
 * Builds a perspective projection matrix for a right-handed coordinate system from:
 * * Minimum value of the view volume width (+left+)
 * * Maximum value of the view volume width (+right+)
 * * Minimum value of the view volume height (+bottom+)
 * * Maximum value of the view volume height (+top+)
 * * Near clip plane distance (+znear+)
 * * Far clip plane distance (+zfar+)
 */
static VALUE
RMtx4_perspectiveOffCenterRH( VALUE self, VALUE l, VALUE r, VALUE b, VALUE t, VALUE zn, VALUE zf )
{
    RMtx4* m = NULL;
    rmReal left, right, bottom, top, znear, zfar;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );
    left   = NUM2DBL(l);
    right  = NUM2DBL(r);
    bottom = NUM2DBL(b);
    top    = NUM2DBL(t);
    znear  = NUM2DBL(zn);
    zfar   = NUM2DBL(zf);
    RMtx4PerspectiveOffCenterRH( m, left, right, bottom, top, znear, zfar );

    return self;
}

/*
 * call-seq: orthoRH(width,height,znear,zfar) -> self
 *
 * Builds a orthogonal projection matrix for a right-handed coordinate system from:
 * * View volume width (+width+)
 * * View volume height (+height+)
 * * Near clip plane distance (+znear+)
 * * Far clip plane distance (+zfar+)
 */
static VALUE
RMtx4_orthoRH( VALUE self, VALUE w, VALUE h, VALUE zn, VALUE zf )
{
    RMtx4* m = NULL;
    rmReal width, height, znear, zfar;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );
    width  = NUM2DBL(w);
    height = NUM2DBL(h);
    znear  = NUM2DBL(zn);
    zfar   = NUM2DBL(zf);

    RMtx4OrthoRH( m, width, height, znear, zfar );

    return self;
}

/*
 * call-seq: orthoOffCenterRH(left,right,bottom,top,znear,zfar) -> self
 *
 * Builds a orthogonal projection matrix for a right-handed coordinate system from:
 * * Minimum value of the view volume width (+left+)
 * * Maximum value of the view volume width (+right+)
 * * Minimum value of the view volume height (+bottom+)
 * * Maximum value of the view volume height (+top+)
 * * Near clip plane distance (+znear+)
 * * Far clip plane distance (+zfar+)
 */
static VALUE
RMtx4_orthoOffCenterRH( VALUE self, VALUE l, VALUE r, VALUE b, VALUE t, VALUE zn, VALUE zf )
{
    RMtx4* m = NULL;
    rmReal left, right, bottom, top, znear, zfar;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );
    left   = NUM2DBL(l);
    right  = NUM2DBL(r);
    bottom = NUM2DBL(b);
    top    = NUM2DBL(t);
    znear  = NUM2DBL(zn);
    zfar   = NUM2DBL(zf);
    RMtx4OrthoOffCenterRH( m, left, right, bottom, top, znear, zfar );

    return self;
}

/*
 * call-seq: +
 *
 * +mtx : Unary plus operator.
 */
static VALUE
RMtx4_op_unary_plus( VALUE self )
{
    return self;
}

/*
 * call-seq: -
 *
 * -mtx : Unary minus operator.
 */
static VALUE
RMtx4_op_unary_minus( VALUE self )
{
    RMtx4* m = NULL;
    RMtx4 out;

    TypedData_Get_Struct( self, RMtx4, &RMtx4_type, m );
    RMtx4Scale( &out, m, (rmReal)(-1) );

    return RMtx4_from_source( &out );
}

/*
 * call-seq: +
 *
 * mtx1 + mtx2 : Binary plus operator.
 */
static VALUE
RMtx4_op_binary_plus( VALUE self, VALUE other )
{
    RMtx4* m1 = NULL;
    RMtx4* m2 = NULL;
    RMtx4 result;

#ifdef RMATH_ENABLE_ARGUMENT_CHECK
    if ( !IsRMtx4(other) )
    {
        rb_raise( rb_eTypeError,
                  "RMtx4#+ : Unknown type %s.",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other )
            );
        return Qnil;
    }
#endif

    TypedData_Get_Struct( self,  RMtx4, &RMtx4_type, m1 );
    TypedData_Get_Struct( other, RMtx4, &RMtx4_type, m2 );
    RMtx4Add( &result, m1, m2 );

    return RMtx4_from_source( &result );
}

/*
 * call-seq: -
 *
 * mtx1 - mtx2 : Binary minus operator.
 */
static VALUE
RMtx4_op_binary_minus( VALUE self, VALUE other )
{
    RMtx4* m1 = NULL;
    RMtx4* m2 = NULL;
    RMtx4 result;

#ifdef RMATH_ENABLE_ARGUMENT_CHECK
    if ( !IsRMtx4(other) )
    {
        rb_raise( rb_eTypeError,
                  "RMtx4#- : Unknown type %s.",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other )
            );
        return Qnil;
    }
#endif

    TypedData_Get_Struct( self,  RMtx4, &RMtx4_type, m1 );
    TypedData_Get_Struct( other, RMtx4, &RMtx4_type, m2 );
    RMtx4Sub( &result, m1, m2 );

    return RMtx4_from_source( &result );
}

/*
 * call-seq: **
 *
 * mtx1 * mtx2 : Binary multiply operator.
 */
static VALUE
RMtx4_op_binary_mult( VALUE self, VALUE other )
{
    RMtx4* m1 = NULL;
    RMtx4* m2 = NULL;
    rmReal f;
    RMtx4 result;

    TypedData_Get_Struct( self,  RMtx4, &RMtx4_type, m1 );

    if ( IsRMtx4(other) )
    {
        TypedData_Get_Struct( other, RMtx4, &RMtx4_type, m2 );
        RMtx4Mul( &result, m1, m2 );
    }
    else
    {
        f = NUM2DBL( other );
        RMtx4Scale( &result, m1, f );
    }

    return RMtx4_from_source( &result );
}

/*
 * call-seq: ==
 *
 * mtx1 == mtx2 : evaluates equality.
 */
static VALUE
RMtx4_op_binary_eq( VALUE self, VALUE other )
{
    if ( IsRMtx4(other) )
    {
        RMtx4* m1 = NULL;
        RMtx4* m2 = NULL;

        TypedData_Get_Struct( self,  RMtx4, &RMtx4_type, m1 );
        TypedData_Get_Struct( other, RMtx4, &RMtx4_type, m2 );

        if ( !RMtx4Equal(m1,m2) )
            return Qfalse;
        else
            return Qtrue;
    }
    else
    {
        return Qfalse;
    }
}

/*
 * call-seq: mtx1.add!( mtx2 )
 *
 * mtx1 += mtx2 : appends the elements of +mtx2+ into corresponding +mtx1+ elements.
 */
static VALUE
RMtx4_op_assign_plus( VALUE self, VALUE other )
{
    RMtx4* m1 = NULL;
    RMtx4* m2 = NULL;

#ifdef RMATH_ENABLE_ARGUMENT_CHECK
    if ( !IsRMtx4(other) )
    {
        rb_raise( rb_eTypeError,
                  "RMtx4#+= : Unknown type %s.",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other )
            );
        return Qnil;
    }
#endif

    TypedData_Get_Struct( self,  RMtx4, &RMtx4_type, m1 );
    TypedData_Get_Struct( other, RMtx4, &RMtx4_type, m2 );
    RMtx4Add( m1, m1, m2 );

    return self;
}

/*
 * call-seq: mtx1.sub!( mtx2 )
 *
 * mtx1 -= mtx2 : subtracts the elements of +mtx2+ from corresponding +mtx1+ elements.
 */
static VALUE
RMtx4_op_assign_minus( VALUE self, VALUE other )
{
    RMtx4* m1 = NULL;
    RMtx4* m2 = NULL;

#ifdef RMATH_ENABLE_ARGUMENT_CHECK
    if ( !IsRMtx4(other) )
    {
        rb_raise( rb_eTypeError,
                  "RMtx4#-= : Unknown type %s.",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other )
            );
        return Qnil;
    }
#endif

    TypedData_Get_Struct( self,  RMtx4, &RMtx4_type, m1 );
    TypedData_Get_Struct( other, RMtx4, &RMtx4_type, m2 );
    RMtx4Sub( m1, m1, m2 );

    return self;
}

/*
 * call-seq: mtx1.mul!( mtx2 )
 *
 * mtx1 *= mtx2
 */
static VALUE
RMtx4_op_assign_mult( VALUE self, VALUE other )
{
    RMtx4* m1 = NULL;
    RMtx4* m2 = NULL;
    rmReal f;

    TypedData_Get_Struct( self,  RMtx4, &RMtx4_type, m1 );

    if ( IsRMtx4(other) )
    {
        TypedData_Get_Struct( other, RMtx4, &RMtx4_type, m2 );
        RMtx4Mul( m1, m1, m2 );
    }
    else
    {
        f = NUM2DBL( other );
        RMtx4Scale( m1, m1, f );
    }

    return self;
}


/********************************************************************************
 *
 * RQuat
 *
 ********************************************************************************/

/*
 * Document-class: RMath3D::RQuat
 * provies quaternion arithmetic.
 */

static void
RQuat_free( void* ptr )
{
	xfree( ptr );
}

static size_t
RQuat_memsize( const void* ptr )
{
    const struct RQuat* data = ptr;
    return data ? sizeof(*data) : 0;
}

static VALUE
RQuat_from_source( RQuat* src )
{
    RQuat* v = ZALLOC( struct RQuat );
    RQuatCopy( v, src );
    return TypedData_Wrap_Struct( rb_cRQuat, &RQuat_type, v );
}


static VALUE
RQuat_allocate( VALUE klass )
{
    RQuat* v = ZALLOC( RQuat );
    return TypedData_Wrap_Struct( klass, &RQuat_type, v );
}


/*
 * call-seq:
 *   RQuat.new -> (0,0,0,0)
 *   RQuat.new(e) -> (e,e,e,e)
 *   RQuat.new( other ) : Copy Constructor
 *   RQuat.new( e0, e1, e2, e3 ) -> (e0,e1,e2,e3)
 *
 * Creates a new quaternion.
 */
static VALUE
RQuat_initialize( int argc, VALUE* argv, VALUE self )
{
    RQuat* v = NULL;
    TypedData_Get_Struct( self, RQuat, &RQuat_type, v );

    switch( argc )
    {
    case 0:
    {
        /* Default Constructor */
        RQuatSetElements( v, 0.0f, 0.0f, 0.0f, 0.0f );
        return self;
    }
    break;

    case 1:
    {
        VALUE arg = argv[0];
        switch ( TYPE( arg ) )
        {
        case T_FIXNUM:
        case T_FLOAT:
        {
            /* convert to float */
            rmReal f = NUM2DBL( arg );
            RQuatSetElements( v, f,f,f,f );
            return self;
        }
        break;

        case T_DATA:
        {
            if ( IsRQuat(arg) )
            {
                /* Copy Constructor */
                RQuat* other;
                TypedData_Get_Struct( arg , RQuat, &RQuat_type, other );
                RQuatSetElements( v, other->x, other->y, other->z, other->w );
                return self;
            }
            else
                return Qnil;
        }
        break;

        default:
        {
            rb_raise( rb_eTypeError,
                      "RQuat_new : Unknown type %s.",
                      rb_special_const_p( self ) ? RSTRING_PTR( rb_inspect( self ) ) : rb_obj_classname( self )
                );
            return Qnil;
        }
        break;
        } /* End : switch ( TYPE( arg ) ) */
    } /* End : case 1 */
    break;

    case 4:
    {
        /* Element-wise setter */
        int i;
        for ( i = 0; i < argc; ++i )
        {
            switch ( TYPE( argv[i] ) )
            {
            case T_FIXNUM:
            case T_FLOAT:
            {
                rmReal f = NUM2DBL( argv[i] );
                v->e[i] = f;
            }
            break;

            default:
            {
                rb_raise( rb_eTypeError,
                          "RQuat_new : Unknown type %s. at arg %d",
                          rb_special_const_p( self ) ? RSTRING_PTR( rb_inspect( self ) ) : rb_obj_classname( self ),
                          i
                    );
                return Qnil;
            }
            break;
            }
        } /* End : for ( i = 0; i < argc; ++i ) */

        return self;
    } /* End : case 4 */
    break;

    default:
    {
        rb_raise( rb_eRuntimeError, "RQuat_initialize : wrong # of arguments (%d)", argc );
        return Qnil;
    }
    break;
    } /* End : switch( argc ) */
}

/*
 * call-seq: to_s
 *
 * Returns human-readable string.
 */
static VALUE
RQuat_to_s( VALUE self )
{
    RQuat* q;
    char dest[128], work[4][32];
    int i;

    TypedData_Get_Struct( self, RQuat, &RQuat_type, q );

    for ( i = 0; i < 4; ++i )
    {
        if ( isnan(q->e[i]) )
            sprintf( work[i], "%s", q->e[i] >= 0 ? "Inf" : "-Inf" );
        else if ( isinf(q->e[i]) )
            sprintf( work[i], "%s", "NaN" );
        else
            sprintf( work[i], RMATH_FVAL_FMT, q->e[i] );
    }
    sprintf( dest, "([%s, %s, %s], %s)", work[0], work[1], work[2], work[3] );

    return rb_str_new2( dest );
}

/*
 * call-seq: inspect
 *
 * Returns human-readable string.
 */
static VALUE
RQuat_inspect( VALUE self )
{
    return RQuat_to_s(self);
}

/*
 * call-seq: to_a
 *
 * Returns its elements as a new Array.
 */
static VALUE
RQuat_to_a( VALUE self )
{
    RQuat* v = NULL;
    VALUE dbl[4];
    TypedData_Get_Struct( self, RQuat, &RQuat_type, v );

    dbl[0] = DBL2NUM(RQuatGetElement(v,0));
    dbl[1] = DBL2NUM(RQuatGetElement(v,1));
    dbl[2] = DBL2NUM(RQuatGetElement(v,2));
    dbl[3] = DBL2NUM(RQuatGetElement(v,3));

    return rb_ary_new4( 4, dbl );
}

/*
 * call-seq: coerse(other)
 *
 * Resolves type mismatch.
 */
static VALUE
RQuat_coerce( VALUE self, VALUE other )
{
    RQuat* v = NULL;
    TypedData_Get_Struct( self, RQuat, &RQuat_type, v );

    switch( TYPE(other) )
    {
    case T_FLOAT:
    case T_FIXNUM:
    case T_BIGNUM:
    {
        /* 'other (op) RQuat'
         * -> try again as 'RQuat (op) other'
         */
        return rb_ary_new3( 2,  self, other );
    }
    break;

    default:
    {
        rb_raise( rb_eTypeError,
                  "%s can't be coerced into %s",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other ),
                  rb_obj_classname( self )
            );
        return Qnil;
    }
    break;
    }
}

/*
 * call-seq: setElements( e0, e1, e2, e3 )
 *
 * Stores given 4 new values.
 */
static VALUE
RQuat_setElements( VALUE self, VALUE x, VALUE y, VALUE z, VALUE w )
{
    RQuat* v = NULL;
    rmReal flt0, flt1, flt2, flt3;

    TypedData_Get_Struct( self, RQuat, &RQuat_type, v );
    flt0 = NUM2DBL(x);
    flt1 = NUM2DBL(y);
    flt2 = NUM2DBL(z);
    flt3 = NUM2DBL(w);

    RQuatSetElements( v, flt0, flt1, flt2, flt3 );

    return self;
}

/*
 * call-seq: quat[i]= value
 *
 * Stores +value+ at +i+.
 */
static VALUE
RQuat_setElement( VALUE self, VALUE i, VALUE f )
{
    RQuat* v = NULL;
    int at;
    rmReal flt;

    TypedData_Get_Struct( self, RQuat, &RQuat_type, v );
    at = NUM2INT(i);
    flt = NUM2DBL(f);

    RQuatSetElement( v, at, flt );

    return self;
}

/*
 * call-seq: x= value
 *
 * Stores +value+ as +x+.
 */
static VALUE
RQuat_setX( VALUE self, VALUE x )
{
    RQuat* v = NULL;
    rmReal flt0;

    TypedData_Get_Struct( self, RQuat, &RQuat_type, v );
    flt0 = NUM2DBL(x);

    RQuatSetX( v, flt0 );

    return self;
}

/*
 * call-seq: y= value
 *
 * Stores +value+ as +y+.
 */
static VALUE
RQuat_setY( VALUE self, VALUE y )
{
    RQuat* v = NULL;
    rmReal flt0;

    TypedData_Get_Struct( self, RQuat, &RQuat_type, v );
    flt0 = NUM2DBL(y);

    RQuatSetY( v, flt0 );

    return self;
}

/*
 * call-seq: z= value
 *
 * Stores +value+ as +z+.
 */
static VALUE
RQuat_setZ( VALUE self, VALUE z )
{
    RQuat* v = NULL;
    rmReal flt0;

    TypedData_Get_Struct( self, RQuat, &RQuat_type, v );
    flt0 = NUM2DBL(z);

    RQuatSetZ( v, flt0 );

    return self;
}

/*
 * call-seq: w= value
 *
 * Stores +value+ as +w+.
 */
static VALUE
RQuat_setW( VALUE self, VALUE w )
{
    RQuat* v = NULL;
    rmReal flt0;

    TypedData_Get_Struct( self, RQuat, &RQuat_type, v );
    flt0 = NUM2DBL(w);

    RQuatSetW( v, flt0 );

    return self;
}

/*
 * call-seq: xyz= vXYZ
 *
 * Copies the values of +vXYZ+(RVec3) into +x+, +y+ and +z+.
 */
static VALUE
RQuat_setXYZ( VALUE self, VALUE xyz )
{
    RQuat* v = NULL;
    RVec3* in = NULL;

    TypedData_Get_Struct( self, RQuat, &RQuat_type, v );
    TypedData_Get_Struct( xyz, RVec3, &RVec3_type, in );

    RQuatSetXYZ( v, in );

    return self;
}

/*
 * call-seq: quat[i] -> value
 *
 * Returns the element at +i+.
 */
static VALUE
RQuat_getElement( VALUE self, VALUE i )
{
    RQuat* v = NULL;
    int at;
    rmReal flt0;

    TypedData_Get_Struct( self, RQuat, &RQuat_type, v );
    at = FIX2INT(i);
    flt0 = RQuatGetElement( v, at );

    return DBL2NUM( flt0 );
}

/*
 * call-seq: x -> value
 *
 * Returns the value of +x+.
 */
static VALUE
RQuat_getX( VALUE self )
{
    RQuat* v = NULL;
    rmReal flt0;

    TypedData_Get_Struct( self, RQuat, &RQuat_type, v );
    flt0 = RQuatGetX( v );

    return DBL2NUM( flt0 );
}

/*
 * call-seq: y -> value
 *
 * Returns the value of +y+.
 */
static VALUE
RQuat_getY( VALUE self )
{
    RQuat* v = NULL;
    rmReal flt0;

    TypedData_Get_Struct( self, RQuat, &RQuat_type, v );
    flt0 = RQuatGetY( v );

    return DBL2NUM( flt0 );
}

/*
 * call-seq: z -> value
 *
 * Returns the value of +z+.
 */
static VALUE
RQuat_getZ( VALUE self )
{
    RQuat* v = NULL;
    rmReal flt0;

    TypedData_Get_Struct( self, RQuat, &RQuat_type, v );
    flt0 = RQuatGetZ( v );

    return DBL2NUM( flt0 );
}

/*
 * call-seq: w -> value
 *
 * Returns the value of +w+.
 */
static VALUE
RQuat_getW( VALUE self )
{
    RQuat* v = NULL;
    rmReal flt0;

    TypedData_Get_Struct( self, RQuat, &RQuat_type, v );
    flt0 = RQuatGetW( v );

    return DBL2NUM( flt0 );
}

/*
 * call-seq: xyz -> RVec3
 *
 * Returns the values of +x+, +y+ and +z+ with new RVec3(+x+,+y+,+z+).
 */
static VALUE
RQuat_getXYZ( VALUE self )
{
    RQuat* v = NULL;
    RVec3 out;

    TypedData_Get_Struct( self, RQuat, &RQuat_type, v );

    RQuatGetXYZ( &out, v );

    return RVec3_from_source( &out );
}

/*
 * call-seq: getLength
 *
 * Returns the Euclidean length.
 */
static VALUE
RQuat_getLength( VALUE self )
{
    RQuat* v = NULL;
    rmReal flt0;

    TypedData_Get_Struct( self, RQuat, &RQuat_type, v );
    flt0 = RQuatLength( v );

    return DBL2NUM( flt0 );
}

/*
 * call-seq: getLengthSq
 *
 * Returns the squared Euclidean length.
 */
static VALUE
RQuat_getLengthSq( VALUE self )
{
    RQuat* v = NULL;
    rmReal flt0;

    TypedData_Get_Struct( self, RQuat, &RQuat_type, v );
    flt0 = RQuatLengthSq( v );

    return DBL2NUM( flt0 );
}

/*
 * call-seq: RQuat.dot(q_a,q_b) -> value
 *
 * Calculates the dot product of +q_a+ and +q_b+.
 */
static VALUE
RQuat_dot( VALUE self, VALUE q1, VALUE q2 )
{
    RQuat* quat1 = NULL;
    RQuat* quat2 = NULL;
    rmReal result;

    TypedData_Get_Struct( q1, RQuat, &RQuat_type, quat1 );
    TypedData_Get_Struct( q2, RQuat, &RQuat_type, quat2 );
    result = RQuatDot( quat1, quat2 );

    return DBL2NUM( result );
}

/*
 * call-seq: RQuat.slerp( q_a, q_b, t ) -> interpolated quaternion
 *
 * Calculates the spherical linear interpolation between +q_a+ and
 * +q_b+ at time +t+ (0.0~1.0).
 */
static VALUE
RQuat_slerp( VALUE self, VALUE q1, VALUE q2, VALUE t )
{
    RQuat* quat1 = NULL;
    RQuat* quat2 = NULL;
    rmReal time;
    RQuat result;

    TypedData_Get_Struct( q1, RQuat, &RQuat_type, quat1 );
    TypedData_Get_Struct( q2, RQuat, &RQuat_type, quat2 );
    time = NUM2DBL( t );
    RQuatSlerp( &result, quat1, quat2, time );

    return RQuat_from_source( &result );
}

/*
 * call-seq: setIdentity
 *
 * Sets as identity quaternion.
 */
static VALUE
RQuat_setIdentity( VALUE self )
{
    RQuat* q = NULL;

    TypedData_Get_Struct( self, RQuat, &RQuat_type, q );
    RQuatIdentity( q );

    return self;
}

/*
 * call-seq: getConjugated
 *
 * Returns its conjugate quaternion.
 */
static VALUE
RQuat_conjugate( VALUE self )
{
    RQuat* q = NULL;
    RQuat out;

    TypedData_Get_Struct( self, RQuat, &RQuat_type, q );
    RQuatConjugate( &out, q );

    return RQuat_from_source( &out );
}

/*
 * call-seq: conjugate!
 *
 * Conjugates itself.
 */
static VALUE
RQuat_conjugate_intrusive( VALUE self )
{
    RQuat* q = NULL;

    TypedData_Get_Struct( self, RQuat, &RQuat_type, q );
    RQuatConjugate( q, q );

    return self;
}

/*
 * call-seq: getInverse -> inverse quaternion
 *
 * Returns the inverse.
 */
static VALUE
RQuat_inverse( VALUE self )
{
    RQuat* q = NULL;
    RQuat out;

    TypedData_Get_Struct( self, RQuat, &RQuat_type, q );
    RQuatInverse( &out, q );

    return RQuat_from_source( &out );
}

/*
 * call-seq: invert! -> self
 *
 * Inverts itself.
 */
static VALUE
RQuat_inverse_intrusive( VALUE self )
{
    RQuat* q = NULL;

    TypedData_Get_Struct( self, RQuat, &RQuat_type, q );
    RQuatInverse( q, q );

    return self;
}

/*
 * call-seq: getNormalized -> RQuat
 *
 * Returns normalized quaternion.
 */
static VALUE
RQuat_normalize( VALUE self )
{
    RQuat* v = NULL;
    RQuat out;

    TypedData_Get_Struct( self, RQuat, &RQuat_type, v );
    RQuatNormalize( &out, v );

    return RQuat_from_source( &out );
}

/*
 * call-seq: normalize! -> self
 *
 * Normalizes itself.
 */
static VALUE
RQuat_normalize_intrusive( VALUE self )
{
    RQuat* v = NULL;

    TypedData_Get_Struct( self, RQuat, &RQuat_type, v );
    RQuatNormalize( v, v );

    return self;
}

/*
 * call-seq: +
 *
 * +quat : Unary plus operator.
 */
static VALUE
RQuat_op_unary_plus( VALUE self )
{
    return self;
}

/*
 * call-seq: -
 *
 * -quat : Unary minus operator.
 */
static VALUE
RQuat_op_unary_minus( VALUE self )
{
    RQuat* v = NULL;
    RQuat out;

    TypedData_Get_Struct( self, RQuat, &RQuat_type, v );
    RQuatScale( &out, v, (rmReal)(-1) );

    return RQuat_from_source( &out );
}

/*
 * call-seq: +
 *
 * quat1 + quat2 : Binary plus operator.
 */
static VALUE
RQuat_op_binary_plus( VALUE self, VALUE other )
{
    RQuat* v1 = NULL;
    RQuat* v2 = NULL;
    RQuat result;

#ifdef RMATH_ENABLE_ARGUMENT_CHECK
    if ( !IsRQuat(other) )
    {
        rb_raise( rb_eTypeError,
                  "RQuat#+ : Unknown type %s.",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other )
            );
        return Qnil;
    }
#endif

    TypedData_Get_Struct( self, RQuat, &RQuat_type, v1 );
    TypedData_Get_Struct( other,  RQuat, &RQuat_type, v2 );
    RQuatAdd( &result, v1, v2 );

    return RQuat_from_source( &result );
}

/*
 * call-seq: -
 *
 * quat1 - quat2 : Binary minus operator.
 */
static VALUE
RQuat_op_binary_minus( VALUE self, VALUE other )
{
    RQuat* v1 = NULL;
    RQuat* v2 = NULL;
    RQuat result;

#ifdef RMATH_ENABLE_ARGUMENT_CHECK
    if ( !IsRQuat(other) )
    {
        rb_raise( rb_eTypeError,
                  "RQuat#- : Unknown type %s.",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other )
            );
        return Qnil;
    }
#endif

    TypedData_Get_Struct( self, RQuat, &RQuat_type, v1 );
    TypedData_Get_Struct( other,  RQuat, &RQuat_type, v2 );
    RQuatSub( &result, v1, v2 );

    return RQuat_from_source( &result );
}

/*
 * call-seq: **
 *
 * quat1 * quat2 : Binary multiply operator.
 */
static VALUE
RQuat_op_binary_mult( VALUE self, VALUE other )
{
    RQuat* v  = NULL;
    RQuat result;
    rmReal f;

    TypedData_Get_Struct( self, RQuat, &RQuat_type, v );
    if ( IsRQuat(other) )
    {
        RQuat* q = NULL;
        TypedData_Get_Struct( other, RQuat, &RQuat_type, q );
        RQuatMul( &result, v, q );

        return RQuat_from_source( &result );
    }
    else
    {
        switch( TYPE(other) )
        {
        case T_FIXNUM:
        case T_FLOAT:
        {
            f = NUM2DBL( other );
            RQuatScale( &result, v, f );

            return RQuat_from_source( &result );
        }
        break;

        default:
        {
            rb_raise( rb_eTypeError,
                      "RQuat#* : Unknown type %s.",
                      rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other )
                );
            return Qnil;
        }
        } /* End : switch( TYPE(other) ) */
    }
}

/*
 * call-seq: ==
 *
 * quat1 == quat2 : evaluates equality.
 */
static VALUE
RQuat_op_binary_eq( VALUE self, VALUE other )
{
    if ( IsRQuat(other) )
    {
        RQuat* v1 = NULL;
        RQuat* v2 = NULL;

        TypedData_Get_Struct( self,  RQuat, &RQuat_type, v1 );
        TypedData_Get_Struct( other, RQuat, &RQuat_type, v2 );

        if ( !RQuatEqual(v1,v2) )
            return Qfalse;
        else
            return Qtrue;
    }
    else
    {
        return Qfalse;
    }
}

/*
 * call-seq: quat1.add!( quat2 )
 *
 * quat1 += quat2 : appends the elements of +quat2+ into corresponding +quat1+ elements.
 */
static VALUE
RQuat_op_assign_plus( VALUE self, VALUE other )
{
    RQuat* v1 = NULL;
    RQuat* v2 = NULL;

#ifdef RMATH_ENABLE_ARGUMENT_CHECK
    if ( !IsRQuat(other) )
    {
        rb_raise( rb_eTypeError,
                  "RQuat#+= : Unknown type %s.",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other )
            );
        return Qnil;
    }
#endif

    TypedData_Get_Struct( self,  RQuat, &RQuat_type, v1 );
    TypedData_Get_Struct( other, RQuat, &RQuat_type, v2 );

    RQuatAdd( v1, v1, v2 );

    return self;
}

/*
 * call-seq: quat1.sub!( quat2 )
 *
 * quat1 -= quat2 : subtracts the elements of +quat2+ from corresponding +quat1+ elements.
 */
static VALUE
RQuat_op_assign_minus( VALUE self, VALUE other )
{
    RQuat* v1 = NULL;
    RQuat* v2 = NULL;

#ifdef RMATH_ENABLE_ARGUMENT_CHECK
    if ( !IsRQuat(other) )
    {
        rb_raise( rb_eTypeError,
                  "RQuat#-= : Unknown type %s.",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other )
            );
        return Qnil;
    }
#endif

    TypedData_Get_Struct( self,  RQuat, &RQuat_type, v1 );
    TypedData_Get_Struct( other, RQuat, &RQuat_type, v2 );

    RQuatSub( v1, v1, v2 );

    return self;
}

/*
 * call-seq: quat1.mul!( quat2 )
 *
 * quat1 *= quat2
 */
static VALUE
RQuat_op_assign_mult( VALUE self, VALUE other )
{
    RQuat* v = NULL;
    rmReal  f;

    TypedData_Get_Struct( self, RQuat, &RQuat_type, v );
    if ( IsRQuat(other) )
    {
        RQuat* q = NULL;
        TypedData_Get_Struct( other, RQuat, &RQuat_type, q );
        RQuatMul( v, v, q );

        return self;
    }
    else
    {
#ifdef RMATH_ENABLE_ARGUMENT_CHECK
        if ( TYPE(other) != T_FIXNUM && TYPE(other) != T_FLOAT )
        {
            rb_raise( rb_eTypeError,
                      "RQuat#mul! : Unknown type %s.",
                      rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other )
                );
            return Qnil;
        }
#endif
        f = NUM2DBL( other );
        RQuatScale( v, v, f );

        return self;
    }
}

/*
 * call-seq: rotationMarix(mtx4) -> self
 *
 * Makes a rotation quaternion from a rotation matrix +mtx4+ (RMtx4).
 */
static VALUE
RQuat_rotationMatrix( VALUE self, VALUE mtx )
{
    RQuat* q = NULL;
    RMtx4* m = NULL;

    TypedData_Get_Struct( self, RQuat, &RQuat_type, q );
    TypedData_Get_Struct( mtx,  RMtx4, &RMtx4_type, m );

    RQuatRotationMatrix( q, m );

    return self;
}

/*
 * call-seq: rotationAxis(axis,radian) -> self
 *
 * Makes a quaternion that rotates around the +axis+.
 */
static VALUE
RQuat_rotationAxis( VALUE self, VALUE axis, VALUE radian )
{
    RQuat* q = NULL;
    RVec3* vAxis = NULL;
    rmReal angle_radian;

    TypedData_Get_Struct( self, RQuat, &RQuat_type, q );
    TypedData_Get_Struct( axis, RVec3, &RVec3_type, vAxis );
    angle_radian = NUM2DBL( radian );

    RQuatRotationAxis( q, vAxis, angle_radian );

    return self;
}

/*
 * call-seq: toAxisAngle -> [axis,radian]
 *
 * Returns its rotation axis (RVec3) and rotation angle (in radian).
 */
static VALUE
RQuat_toAxisAngle( VALUE self )
{
    RQuat* q = NULL;
    RVec3 axis;
    rmReal radian;

    TypedData_Get_Struct( self, RQuat, &RQuat_type, q );

    RQuatToAxisAngle( q, &axis, &radian );

    return rb_ary_new3( 2, RVec3_from_source(&axis), DBL2NUM(radian) );
}


/********************************************************************************
 *
 * RVec2
 *
 ********************************************************************************/

/*
 * Document-class: RMath3D::RVec2
 * provies 3 element vector arithmetic.
 */

static void
RVec2_free( void* ptr )
{
	xfree( ptr );
}

static size_t
RVec2_memsize( const void* ptr )
{
    const struct RVec2* data = ptr;
    return data ? sizeof(*data) : 0;
}

static VALUE
RVec2_from_source( RVec2* src )
{
    RVec2* v = ZALLOC( struct RVec2 );
    RVec2Copy( v, src );
    return TypedData_Wrap_Struct( rb_cRVec2, &RVec2_type, v );
}


static VALUE
RVec2_allocate( VALUE klass )
{
    RVec2* v = ZALLOC( RVec2 );
    return TypedData_Wrap_Struct( klass, &RVec2_type, v );
}


/*
 * call-seq:
 *   RVec2.new -> (0,0,0)
 *   RVec2.new(e) -> (e,e,e)
 *   RVec2.new( other ) : Copy Constructor
 *   RVec2.new( e0, e1, e2 ) -> (e0,e1,e2)
 *
 * Creates a new 2 element vector.
 */
static VALUE
RVec2_initialize( int argc, VALUE* argv, VALUE self )
{
    RVec2* v = NULL;
    TypedData_Get_Struct( self, RVec2, &RVec2_type, v );

    switch( argc )
    {
    case 0:
    {
        /* Default Constructor */
        RVec2SetElements( v, 0.0f, 0.0f );
        return self;
    }
    break;

    case 1:
    {
        VALUE arg = argv[0];
        switch ( TYPE( arg ) )
        {
        case T_FIXNUM:
        case T_FLOAT:
        {
            /* convert to float */
            rmReal f = NUM2DBL( arg );
            RVec2SetElements( v, f,f );
            return self;
        }
        break;

        case T_DATA:
        {
            if ( IsRVec2(arg) )
            {
                /* Copy Constructor */
                RVec2* other;
                TypedData_Get_Struct( arg , RVec2, &RVec2_type, other );
                RVec2SetElements( v, other->x, other->y );
                return self;
            }
            else
                return Qnil;
        }
        break;

        default:
        {
            rb_raise( rb_eTypeError,
                      "RVec2_new : Unknown type %s.",
                      rb_special_const_p( self ) ? RSTRING_PTR( rb_inspect( self ) ) : rb_obj_classname( self )
                );
            return Qnil;
        }
        break;
        } /* End : switch ( TYPE( arg ) ) */
    } /* End : case 1 */
    break;

    case 2:
    {
        /* Element-wise setter */
        int i;
        for ( i = 0; i < argc; ++i )
        {
            switch ( TYPE( argv[i] ) )
            {
            case T_FIXNUM:
            case T_FLOAT:
            {
                rmReal f = NUM2DBL( argv[i] );
                v->e[i] = f;
            }
            break;

            default:
            {
                rb_raise( rb_eTypeError,
                          "RVec2_new : Unknown type %s. at arg %d",
                          rb_special_const_p( self ) ? RSTRING_PTR( rb_inspect( self ) ) : rb_obj_classname( self ),
                          i
                    );
                return Qnil;
            }
            break;
            }
        } /* End : for ( i = 0; i < argc; ++i ) */

        return self;
    } /* End : case 2 */
    break;

    default:
    {
        rb_raise( rb_eRuntimeError, "RVec2_initialize : wrong # of arguments (%d)", argc );
        return Qnil;
    }
    break;
    } /* End : switch( argc ) */
}

/*
 * call-seq: to_s
 *
 * Returns human-readable string.
 */
static VALUE
RVec2_to_s( VALUE self )
{
    RVec2* v;
    char dest[128], work[2][32];
    int i;

    TypedData_Get_Struct( self, RVec2, &RVec2_type, v );

    for ( i = 0; i < 2; ++i )
    {
        if ( isnan(v->e[i]) )
            sprintf( work[i], "%s", v->e[i] >= 0 ? "Inf" : "-Inf" );
        else if ( isinf(v->e[i]) )
            sprintf( work[i], "%s", "NaN" );
        else
            sprintf( work[i], RMATH_FVAL_FMT, v->e[i] );
    }
    sprintf( dest, "(%s, %s)", work[0], work[1] );

    return rb_str_new2( dest );
}

/*
 * call-seq: inspect
 *
 * Returns human-readable string.
 */
static VALUE
RVec2_inspect( VALUE self )
{
    return RVec2_to_s(self);
}

/*
 * call-seq: to_a
 *
 * Returns its elements as a new Array.
 */
static VALUE
RVec2_to_a( VALUE self )
{
    RVec2* v = NULL;
    VALUE dbl[2];
    TypedData_Get_Struct( self, RVec2, &RVec2_type, v );

    dbl[0] = DBL2NUM(RVec2GetElement(v,0));
    dbl[1] = DBL2NUM(RVec2GetElement(v,1));

    return rb_ary_new4( 2, dbl );
}

/*
 * call-seq: coerse(other)
 *
 * Resolves type mismatch.
 */
static VALUE
RVec2_coerce( VALUE self, VALUE other )
{
    RVec2* v = NULL;
    TypedData_Get_Struct( self, RVec2, &RVec2_type, v );

    switch( TYPE(other) )
    {
    case T_FLOAT:
    case T_FIXNUM:
    case T_BIGNUM:
    {
        /* 'other (op) RVec2'
         * -> try again as 'RVec2 (op) other'
         */
        return rb_ary_new3( 2,  self, other );
    }
    break;

    default:
    {
        rb_raise( rb_eTypeError,
                  "%s can't be coerced into %s",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other ),
                  rb_obj_classname( self )
            );
        return Qnil;
    }
    break;
    }
}

/*
 * call-seq: setElements( e0, e1 )
 *
 * Stores given 2 new values.
 */
static VALUE
RVec2_setElements( VALUE self, VALUE x, VALUE y )
{
    RVec2* v = NULL;
    rmReal flt0, flt1;

    TypedData_Get_Struct( self, RVec2, &RVec2_type, v );
    flt0 = NUM2DBL(x);
    flt1 = NUM2DBL(y);

    RVec2SetElements( v, flt0, flt1 );

    return Qnil;
}

/*
 * call-seq: vec2[i]= value
 *
 * Stores +value+ at +i+.
 */
static VALUE
RVec2_setElement( VALUE self, VALUE i, VALUE f )
{
    RVec2* v = NULL;
    int at;
    rmReal flt;

    TypedData_Get_Struct( self, RVec2, &RVec2_type, v );
    at = NUM2INT(i);
    flt = NUM2DBL(f);

    RVec2SetElement( v, at, flt );

    return self;
}

/*
 * call-seq: x= value
 *
 * Stores +value+ as +x+.
 */
static VALUE
RVec2_setX( VALUE self, VALUE x )
{
    RVec2* v = NULL;
    rmReal flt0;

    TypedData_Get_Struct( self, RVec2, &RVec2_type, v );
    flt0 = NUM2DBL(x);

    RVec2SetX( v, flt0 );

    return Qnil;
}

/*
 * call-seq: y= value
 *
 * Stores +value+ as +y+.
 */
static VALUE
RVec2_setY( VALUE self, VALUE y )
{
    RVec2* v = NULL;
    rmReal flt0;

    TypedData_Get_Struct( self, RVec2, &RVec2_type, v );
    flt0 = NUM2DBL(y);

    RVec2SetY( v, flt0 );

    return Qnil;
}

/*
 * call-seq: vec2[i] -> value
 *
 * Returns the element at +i+.
 */
static VALUE
RVec2_getElement( VALUE self, VALUE i )
{
    RVec2* v = NULL;
    int at;
    rmReal flt0;

    TypedData_Get_Struct( self, RVec2, &RVec2_type, v );
    at = FIX2INT(i);
    flt0 = RVec2GetElement( v, at );

    return DBL2NUM( flt0 );
}

/*
 * call-seq: x -> value
 *
 * Returns the value of +x+.
 */
static VALUE
RVec2_getX( VALUE self )
{
    RVec2* v = NULL;
    rmReal flt0;

    TypedData_Get_Struct( self, RVec2, &RVec2_type, v );
    flt0 = RVec2GetX( v );

    return DBL2NUM( flt0 );
}

/*
 * call-seq: y -> value
 *
 * Returns the value of +y+.
 */
static VALUE
RVec2_getY( VALUE self )
{
    RVec2* v = NULL;
    rmReal flt0;

    TypedData_Get_Struct( self, RVec2, &RVec2_type, v );
    flt0 = RVec2GetY( v );

    return DBL2NUM( flt0 );
}

/*
 * call-seq: getLength
 *
 * Returns the Euclidean length.
 */
static VALUE
RVec2_getLength( VALUE self )
{
    RVec2* v = NULL;
    rmReal flt0;

    TypedData_Get_Struct( self, RVec2, &RVec2_type, v );
    flt0 = RVec2Length( v );

    return DBL2NUM( flt0 );
}

/*
 * call-seq: getLengthSq
 *
 * Returns the squared Euclidean length.
 */
static VALUE
RVec2_getLengthSq( VALUE self )
{
    RVec2* v = NULL;
    rmReal flt0;

    TypedData_Get_Struct( self, RVec2, &RVec2_type, v );
    flt0 = RVec2LengthSq( v );

    return DBL2NUM( flt0 );
}

/*
 * call-seq: RVec2.dot(v_a,v_b) -> value
 *
 * Calculates the dot product of +v_a+ and +v_b+.
 */
static VALUE
RVec2_dot( VALUE self, VALUE v1, VALUE v2 )
{
    RVec2* vec1 = NULL;
    RVec2* vec2 = NULL;
    rmReal result;

    TypedData_Get_Struct( v1, RVec2, &RVec2_type, vec1 );
    TypedData_Get_Struct( v2, RVec2, &RVec2_type, vec2 );
    result = RVec2Dot( vec1, vec2 );

    return DBL2NUM( result );
}

/*
 * call-seq: RVec2.cross(v_a,v_b) -> value
 *
 * Calculates the cross product of +v_a+ and +v_b+.
 */
static VALUE
RVec2_cross( VALUE self, VALUE v1, VALUE v2 )
{
    RVec2* vec1 = NULL;
    RVec2* vec2 = NULL;
    rmReal result;

    TypedData_Get_Struct( v1, RVec2, &RVec2_type, vec1 );
    TypedData_Get_Struct( v2, RVec2, &RVec2_type, vec2 );
    result = RVec2Cross( vec1, vec2 );

    return DBL2NUM( result );
}

/*
 * call-seq: transform(mtx2) -> transformed RVec2
 *
 * Returns new RVec2 containing the result of the transformation of
 *  RVec2(self.x,self.y) by +mtx2+ (RMtx2).
 */
static VALUE
RVec2_transform( VALUE self, VALUE mtx )
{
    RVec2* v;
    RMtx2* m;
    RVec2 out;

    TypedData_Get_Struct( self, RVec2, &RVec2_type, v );
    TypedData_Get_Struct( mtx, RMtx2, &RMtx2_type, m );
    RVec2Transform( &out, m, v );

    return RVec2_from_source( &out );
}

/*
 * call-seq: getNormalized -> RVec2
 *
 * Returns normalized vector.
 */
static VALUE
RVec2_normalize( VALUE self )
{
    RVec2* v = NULL;
    RVec2 out;

    TypedData_Get_Struct( self, RVec2, &RVec2_type, v );
    RVec2Normalize( &out, v );

    return RVec2_from_source( &out );
}

/*
 * call-seq: normalize! -> self
 *
 * Normalizes itself.
 */
static VALUE
RVec2_normalize_intrusive( VALUE self )
{
    RVec2* v = NULL;

    TypedData_Get_Struct( self, RVec2, &RVec2_type, v );
    RVec2Normalize( v, v );

    return self;
}

/*
 * call-seq: +
 *
 * +vec : Unary plus operator.
 */
static VALUE
RVec2_op_unary_plus( VALUE self )
{
    return self;
}

/*
 * call-seq: -
 *
 * -vec : Unary minus operator.
 */
static VALUE
RVec2_op_unary_minus( VALUE self )
{
    RVec2* v   = NULL;
    RVec2 out;

    TypedData_Get_Struct( self, RVec2, &RVec2_type, v );
    RVec2Scale( &out, v, (rmReal)(-1) );

    return RVec2_from_source( &out );
}

/*
 * call-seq: +
 *
 * vec1 + vec2 : Binary plus operator.
 */
static VALUE
RVec2_op_binary_plus( VALUE self, VALUE other )
{
    RVec2* v1 = NULL;
    RVec2* v2 = NULL;
    RVec2 result;

#ifdef RMATH_ENABLE_ARGUMENT_CHECK
    if ( !IsRVec2(other) )
    {
        rb_raise( rb_eTypeError,
                  "RVec2#+ : Unknown type %s.",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other )
            );
        return Qnil;
    }
#endif

    TypedData_Get_Struct( self, RVec2, &RVec2_type, v1 );
    TypedData_Get_Struct( other,  RVec2, &RVec2_type, v2 );
    RVec2Add( &result, v1, v2 );

    return RVec2_from_source( &result );
}

/*
 * call-seq: -
 *
 * vec1 - vec2 : Binary minus operator.
 */
static VALUE
RVec2_op_binary_minus( VALUE self, VALUE other )
{
    RVec2* v1 = NULL;
    RVec2* v2 = NULL;
    RVec2 result;

#ifdef RMATH_ENABLE_ARGUMENT_CHECK
    if ( !IsRVec2(other) )
    {
        rb_raise( rb_eTypeError,
                  "RVec2#- : Unknown type %s.",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other )
            );
        return Qnil;
    }
#endif

    TypedData_Get_Struct( self, RVec2, &RVec2_type, v1 );
    TypedData_Get_Struct( other,  RVec2, &RVec2_type, v2 );
    RVec2Sub( &result, v1, v2 );

    return RVec2_from_source( &result );
}

/*
 * call-seq: **
 *
 * vec1 * vec2 : Binary multiply operator.
 */
static VALUE
RVec2_op_binary_mult( VALUE self, VALUE other )
{
    RVec2* v  = NULL;
    RVec2 result;
    rmReal f;

    switch( TYPE(other) )
    {
    case T_FIXNUM:
    case T_FLOAT:
    {
        TypedData_Get_Struct( self, RVec2, &RVec2_type, v );
        f = NUM2DBL( other );
        RVec2Scale( &result, v, f );

        return RVec2_from_source( &result );
    }
    break;

    default:
    {
        rb_raise( rb_eTypeError,
                  "RVec2#* : Unknown type %s.",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other )
            );
        return Qnil;
    }
    } /* End : switch( TYPE(other) ) */
}

/*
 * call-seq: ==
 *
 * vec1 == vec2 : evaluates equality.
 */
static VALUE
RVec2_op_binary_eq( VALUE self, VALUE other )
{
    if ( IsRVec2(other) )
    {
        RVec2* v1 = NULL;
        RVec2* v2 = NULL;

        TypedData_Get_Struct( self,  RVec2, &RVec2_type, v1 );
        TypedData_Get_Struct( other, RVec2, &RVec2_type, v2 );

        if ( !RVec2Equal(v1,v2) )
            return Qfalse;
        else
            return Qtrue;
    }
    else
    {
        return Qfalse;
    }
}

/*
 * call-seq: vec1.add!( vec2 )
 *
 * vec1 += vec2 : appends the elements of +vec2+ into corresponding +vec1+ elements.
 */
static VALUE
RVec2_op_assign_plus( VALUE self, VALUE other )
{
    RVec2* v1 = NULL;
    RVec2* v2 = NULL;

#ifdef RMATH_ENABLE_ARGUMENT_CHECK
    if ( !IsRVec2(other) )
    {
        rb_raise( rb_eTypeError,
                  "RVec2#+= : Unknown type %s.",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other )
            );
        return Qnil;
    }
#endif

    TypedData_Get_Struct( self,  RVec2, &RVec2_type, v1 );
    TypedData_Get_Struct( other, RVec2, &RVec2_type, v2 );

    RVec2Add( v1, v1, v2 );

    return self;
}

/*
 * call-seq: vec1.sub!( vec2 )
 *
 * vec1 -= vec2 : subtracts the elements of +vec2+ from corresponding +vec1+ elements.
 */
static VALUE
RVec2_op_assign_minus( VALUE self, VALUE other )
{
    RVec2* v1 = NULL;
    RVec2* v2 = NULL;

#ifdef RMATH_ENABLE_ARGUMENT_CHECK
    if ( !IsRVec2(other) )
    {
        rb_raise( rb_eTypeError,
                  "RVec2#-= : Unknown type %s.",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other )
            );
        return Qnil;
    }
#endif

    TypedData_Get_Struct( self,  RVec2, &RVec2_type, v1 );
    TypedData_Get_Struct( other, RVec2, &RVec2_type, v2 );

    RVec2Sub( v1, v1, v2 );

    return self;
}

/*
 * call-seq: vec1.mul!( vec2 )
 *
 * vec1 *= vec2
 */
static VALUE
RVec2_op_assign_mult( VALUE self, VALUE other )
{
    RVec2* v = NULL;
    rmReal  f;

#ifdef RMATH_ENABLE_ARGUMENT_CHECK
    if ( TYPE(other) != T_FIXNUM && TYPE(other) != T_FLOAT )
    {
        rb_raise( rb_eTypeError,
                  "RVec2#*= : Unknown type %s.",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other )
            );
        return Qnil;
    }
#endif

    TypedData_Get_Struct( self, RVec2, &RVec2_type, v );
    f = NUM2DBL( other );
    RVec2Scale( v, v, f );

    return self;
}


/********************************************************************************
 *
 * RVec3
 *
 ********************************************************************************/

/*
 * Document-class: RMath3D::RVec3
 * provies 3 element vector arithmetic.
 */

static void
RVec3_free( void* ptr )
{
	xfree( ptr );
}

static size_t
RVec3_memsize( const void* ptr )
{
    const struct RVec3* data = ptr;
    return data ? sizeof(*data) : 0;
}

static VALUE
RVec3_from_source( RVec3* src )
{
    RVec3* v = ZALLOC( struct RVec3 );
    RVec3Copy( v, src );
    return TypedData_Wrap_Struct( rb_cRVec3, &RVec3_type, v );
}


static VALUE
RVec3_allocate( VALUE klass )
{
    RVec3* v = ZALLOC( RVec3 );
    return TypedData_Wrap_Struct( klass, &RVec3_type, v );
}


/*
 * call-seq:
 *   RVec3.new -> (0,0,0)
 *   RVec3.new(e) -> (e,e,e)
 *   RVec3.new( other ) : Copy Constructor
 *   RVec3.new( e0, e1, e2 ) -> (e0,e1,e2)
 *
 * Creates a new 3 element vector.
 */
static VALUE
RVec3_initialize( int argc, VALUE* argv, VALUE self )
{
    RVec3* v = NULL;
    TypedData_Get_Struct( self, RVec3, &RVec3_type, v );

    switch( argc )
    {
    case 0:
    {
        /* Default Constructor */
        RVec3SetElements( v, 0.0f, 0.0f, 0.0f );
        return self;
    }
    break;

    case 1:
    {
        VALUE arg = argv[0];
        switch ( TYPE( arg ) )
        {
        case T_FIXNUM:
        case T_FLOAT:
        {
            /* convert to float */
            rmReal f = NUM2DBL( arg );
            RVec3SetElements( v, f,f,f );
            return self;
        }
        break;

        case T_DATA:
        {
            if ( IsRVec3(arg) )
            {
                /* Copy Constructor */
                RVec3* other;
                TypedData_Get_Struct( arg , RVec3, &RVec3_type, other );
                RVec3SetElements( v, other->x, other->y, other->z );
                return self;
            }
            else
                return Qnil;
        }
        break;

        default:
        {
            rb_raise( rb_eTypeError,
                      "RVec3_new : Unknown type %s.",
                      rb_special_const_p( self ) ? RSTRING_PTR( rb_inspect( self ) ) : rb_obj_classname( self )
                );
            return Qnil;
        }
        break;
        } /* End : switch ( TYPE( arg ) ) */
    } /* End : case 1 */
    break;

    case 3:
    {
        /* Element-wise setter */
        int i;
        for ( i = 0; i < argc; ++i )
        {
            switch ( TYPE( argv[i] ) )
            {
            case T_FIXNUM:
            case T_FLOAT:
            {
                rmReal f = NUM2DBL( argv[i] );
                v->e[i] = f;
            }
            break;

            default:
            {
                rb_raise( rb_eTypeError,
                          "RVec3_new : Unknown type %s. at arg %d",
                          rb_special_const_p( self ) ? RSTRING_PTR( rb_inspect( self ) ) : rb_obj_classname( self ),
                          i
                    );
                return Qnil;
            }
            break;
            }
        } /* End : for ( i = 0; i < argc; ++i ) */

        return self;
    } /* End : case 3 */
    break;

    default:
    {
        rb_raise( rb_eRuntimeError, "RVec3_initialize : wrong # of arguments (%d)", argc );
        return Qnil;
    }
    break;
    } /* End : switch( argc ) */
}

/*
 * call-seq: to_s
 *
 * Returns human-readable string.
 */
static VALUE
RVec3_to_s( VALUE self )
{
    RVec3* v;
    char dest[128], work[3][32];
    int i;

    TypedData_Get_Struct( self, RVec3, &RVec3_type, v );

    for ( i = 0; i < 3; ++i )
    {
        if ( isnan(v->e[i]) )
            sprintf( work[i], "%s", v->e[i] >= 0 ? "Inf" : "-Inf" );
        else if ( isinf(v->e[i]) )
            sprintf( work[i], "%s", "NaN" );
        else
            sprintf( work[i], RMATH_FVAL_FMT, v->e[i] );
    }
    sprintf( dest, "(%s, %s, %s)", work[0], work[1], work[2] );

    return rb_str_new2( dest );
}

/*
 * call-seq: inspect
 *
 * Returns human-readable string.
 */
static VALUE
RVec3_inspect( VALUE self )
{
    return RVec3_to_s(self);
}

/*
 * call-seq: to_a
 *
 * Returns its elements as a new Array.
 */
static VALUE
RVec3_to_a( VALUE self )
{
    RVec3* v = NULL;
    VALUE dbl[3];
    TypedData_Get_Struct( self, RVec3, &RVec3_type, v );

    dbl[0] = DBL2NUM(RVec3GetElement(v,0));
    dbl[1] = DBL2NUM(RVec3GetElement(v,1));
    dbl[2] = DBL2NUM(RVec3GetElement(v,2));

    return rb_ary_new4( 3, dbl );
}

/*
 * call-seq: coerse(other)
 *
 * Resolves type mismatch.
 */
static VALUE
RVec3_coerce( VALUE self, VALUE other )
{
    RVec3* v = NULL;
    TypedData_Get_Struct( self, RVec3, &RVec3_type, v );

    switch( TYPE(other) )
    {
    case T_FLOAT:
    case T_FIXNUM:
    case T_BIGNUM:
    {
        /* 'other (op) RVec3'
         * -> try again as 'RVec3 (op) other'
         */
        return rb_ary_new3( 2,  self, other );
    }
    break;

    default:
    {
        rb_raise( rb_eTypeError,
                  "%s can't be coerced into %s",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other ),
                  rb_obj_classname( self )
            );
        return Qnil;
    }
    break;
    }
}

/*
 * call-seq: setElements( e0, e1, e2 )
 *
 * Stores given 3 new values.
 */
static VALUE
RVec3_setElements( VALUE self, VALUE x, VALUE y, VALUE z )
{
    RVec3* v = NULL;
    rmReal flt0, flt1, flt2;

    TypedData_Get_Struct( self, RVec3, &RVec3_type, v );
    flt0 = NUM2DBL(x);
    flt1 = NUM2DBL(y);
    flt2 = NUM2DBL(z);

    RVec3SetElements( v, flt0, flt1, flt2 );

    return Qnil;
}

/*
 * call-seq: vec3[i]= value
 *
 * Stores +value+ at +i+.
 */
static VALUE
RVec3_setElement( VALUE self, VALUE i, VALUE f )
{
    RVec3* v = NULL;
    int at;
    rmReal flt;

    TypedData_Get_Struct( self, RVec3, &RVec3_type, v );
    at = NUM2INT(i);
    flt = NUM2DBL(f);

    RVec3SetElement( v, at, flt );

    return self;
}

/*
 * call-seq: x= value
 *
 * Stores +value+ as +x+.
 */
static VALUE
RVec3_setX( VALUE self, VALUE x )
{
    RVec3* v = NULL;
    rmReal flt0;

    TypedData_Get_Struct( self, RVec3, &RVec3_type, v );
    flt0 = NUM2DBL(x);

    RVec3SetX( v, flt0 );

    return Qnil;
}

/*
 * call-seq: y= value
 *
 * Stores +value+ as +y+.
 */
static VALUE
RVec3_setY( VALUE self, VALUE y )
{
    RVec3* v = NULL;
    rmReal flt0;

    TypedData_Get_Struct( self, RVec3, &RVec3_type, v );
    flt0 = NUM2DBL(y);

    RVec3SetY( v, flt0 );

    return Qnil;
}

/*
 * call-seq: z= value
 *
 * Stores +value+ as +z+.
 */
static VALUE
RVec3_setZ( VALUE self, VALUE z )
{
    RVec3* v = NULL;
    rmReal flt0;

    TypedData_Get_Struct( self, RVec3, &RVec3_type, v );
    flt0 = NUM2DBL(z);

    RVec3SetZ( v, flt0 );

    return Qnil;
}

/*
 * call-seq: vec3[i] -> value
 *
 * Returns the element at +i+.
 */
static VALUE
RVec3_getElement( VALUE self, VALUE i )
{
    RVec3* v = NULL;
    int at;
    rmReal flt0;

    TypedData_Get_Struct( self, RVec3, &RVec3_type, v );
    at = FIX2INT(i);
    flt0 = RVec3GetElement( v, at );

    return DBL2NUM( flt0 );
}

/*
 * call-seq: x -> value
 *
 * Returns the value of +x+.
 */
static VALUE
RVec3_getX( VALUE self )
{
    RVec3* v = NULL;
    rmReal flt0;

    TypedData_Get_Struct( self, RVec3, &RVec3_type, v );
    flt0 = RVec3GetX( v );

    return DBL2NUM( flt0 );
}

/*
 * call-seq: y -> value
 *
 * Returns the value of +y+.
 */
static VALUE
RVec3_getY( VALUE self )
{
    RVec3* v = NULL;
    rmReal flt0;

    TypedData_Get_Struct( self, RVec3, &RVec3_type, v );
    flt0 = RVec3GetY( v );

    return DBL2NUM( flt0 );
}

/*
 * call-seq: z -> value
 *
 * Returns the value of +z+.
 */
static VALUE
RVec3_getZ( VALUE self )
{
    RVec3* v = NULL;
    rmReal flt0;

    TypedData_Get_Struct( self, RVec3, &RVec3_type, v );
    flt0 = RVec3GetZ( v );

    return DBL2NUM( flt0 );
}

/*
 * call-seq: getLength
 *
 * Returns the Euclidean length.
 */
static VALUE
RVec3_getLength( VALUE self )
{
    RVec3* v = NULL;
    rmReal flt0;

    TypedData_Get_Struct( self, RVec3, &RVec3_type, v );
    flt0 = RVec3Length( v );

    return DBL2NUM( flt0 );
}

/*
 * call-seq: getLengthSq
 *
 * Returns the squared Euclidean length.
 */
static VALUE
RVec3_getLengthSq( VALUE self )
{
    RVec3* v = NULL;
    rmReal flt0;

    TypedData_Get_Struct( self, RVec3, &RVec3_type, v );
    flt0 = RVec3LengthSq( v );

    return DBL2NUM( flt0 );
}

/*
 * call-seq: RVec3.dot(v_a,v_b) -> value
 *
 * Calculates the dot product of +v_a+ and +v_b+.
 */
static VALUE
RVec3_dot( VALUE self, VALUE v1, VALUE v2 )
{
    RVec3* vec1 = NULL;
    RVec3* vec2 = NULL;
    rmReal result;

    TypedData_Get_Struct( v1, RVec3, &RVec3_type, vec1 );
    TypedData_Get_Struct( v2, RVec3, &RVec3_type, vec2 );
    result = RVec3Dot( vec1, vec2 );

    return DBL2NUM( result );
}

/*
 * call-seq: RVec3.cross(v_a,v_b) -> RVec3(v_a x v_b)
 *
 * Calculates the cross product of +v_a+ and +v_b+.
 */
static VALUE
RVec3_cross( VALUE self, VALUE v1, VALUE v2 )
{
    RVec3* vec1 = NULL;
    RVec3* vec2 = NULL;
    RVec3 out;

    TypedData_Get_Struct( v1, RVec3, &RVec3_type, vec1 );
    TypedData_Get_Struct( v2, RVec3, &RVec3_type, vec2 );
    RVec3Cross( &out, vec1, vec2 );

    return RVec3_from_source( &out );
}

/*
 * call-seq: transform(mtx4) -> transformed RVec4
 *
 * Returns new RVec4 containing the result of the transformation of
 *  RVec4(self.x,self.y,self.z,1.0) by +mtx4+ (RMtx4).
 */
static VALUE
RVec3_transform( VALUE self, VALUE mtx )
{
    RVec3* v;
    RMtx4* m;
    RVec4 out;

    TypedData_Get_Struct( self, RVec3, &RVec3_type, v );
    TypedData_Get_Struct( mtx, RMtx4, &RMtx4_type, m );
    RVec3Transform( &out, m, v );

    return RVec4_from_source( &out );
}

/*
 * call-seq: transformCoord(mtx) -> transformed RVec3
 *
 * Returns RVec3(x/w, y/w, z/w), where x,y,z and w are the elements of
 * the transformation result:
 *  RVec4(self.x,self.y,self.z,1.0).transform(+mtx+) -> RVec4(x,y,z,w). (mtx : RMtx4)
 */
static VALUE
RVec3_transformCoord( VALUE self, VALUE mtx )
{
    RVec3* v;
    RMtx4* m;
    RVec3 out;

    TypedData_Get_Struct( self, RVec3, &RVec3_type, v );
    TypedData_Get_Struct( mtx, RMtx4, &RMtx4_type, m );
    RVec3TransformCoord( &out, m, v );

    return RVec3_from_source( &out );
}

/*
 * call-seq: transformCoord!(mtx) -> self
 *
 * Make itself as RVec3(x/w, y/w, z/w), where x,y,z and w are the elements of
 * the transformation result:
 *  RVec4(self.x,self.y,self.z,1.0).transform(+mtx+) -> RVec4(x,y,z,w). (mtx : RMtx4)
 */
static VALUE
RVec3_transformCoord_intrusive( VALUE self, VALUE mtx )
{
    RVec3* v;
    RMtx4* m;

    TypedData_Get_Struct( self, RVec3, &RVec3_type, v );
    TypedData_Get_Struct( mtx, RMtx4, &RMtx4_type, m );
    RVec3TransformCoord( v, m, v );

    return self;
}

/*
 * call-seq: transformNormal(mtx) -> transformed RVec3
 *
 * Returns the transformation result of
 *  RVec4(self.x,self.y,self.z,0.0).transform(mtx).xyz
 *
 * === Notice
 * * mtx : RMtx4
 */
static VALUE
RVec3_transformNormal( VALUE self, VALUE mtx )
{
    RVec3* v;
    RMtx4* m;
    RVec3 out;

    TypedData_Get_Struct( self, RVec3, &RVec3_type, v );
    TypedData_Get_Struct( mtx, RMtx4, &RMtx4_type, m );
    RVec3TransformNormal( &out, m, v );

    return RVec3_from_source( &out );
}

/*
 * call-seq: transformNormal!(mtx) -> self
 *
 * Make itself as the transformation result of
 *  RVec4(self.x,self.y,self.z,0.0).transform(mtx).xyz
 *
 * === Notice
 * * mtx : RMtx4
 */
static VALUE
RVec3_transformNormal_intrusive( VALUE self, VALUE mtx )
{
    RVec3* v;
    RMtx4* m;

    TypedData_Get_Struct( self, RVec3, &RVec3_type, v );
    TypedData_Get_Struct( mtx, RMtx4, &RMtx4_type, m );
    RVec3TransformNormal( v, m, v );

    return self;
}

/*
 * call-seq: transformRS(mtx) -> transformed RVec3
 *
 * Returns the transformation result of
 *  RVec3(self.x,self.y,self.z).transform(mtx)
 *
 * === Notice
 * * mtx : RMtx3
 * * the suffix "RS" means "matrix representing Rotational and Scaling
 *   transformation".
 */
static VALUE
RVec3_transformRS( VALUE self, VALUE mtx )
{
    RVec3* v;
    RMtx3* m;
    RVec3 out;

    TypedData_Get_Struct( self, RVec3, &RVec3_type, v );
    TypedData_Get_Struct( mtx, RMtx3, &RMtx3_type, m );
    RVec3TransformRS( &out, m, v );

    return RVec3_from_source( &out );
}

/*
 * call-seq: transformRS!(mtx) -> self
 *
 * Makes itself as the transformation result of
 *  RVec3(self.x,self.y,self.z).transform(mtx)
 *
 * === Notice
 * * mtx : RMtx3
 * * the suffix "RS" means "matrix representing Rotational and Scaling
 *   transformation".
 */
static VALUE
RVec3_transformRS_intrusive( VALUE self, VALUE mtx )
{
    RVec3* v;
    RMtx3* m;

    TypedData_Get_Struct( self, RVec3, &RVec3_type, v );
    TypedData_Get_Struct( mtx, RMtx3, &RMtx3_type, m );
    RVec3TransformRS( v, m, v );

    return self;
}

/*
 * call-seq: transformRSTransposed(mtx) -> RVec3 transformed by mtx^T
 *
 * Returns the transformation result of
 *  RVec3(self.x,self.y,self.z).transform(mtx^T)
 *
 * === Notice
 * * mtx : RMtx3
 * * the suffix "RS" means "matrix representing Rotational and Scaling
 *   transformation".
 */
static VALUE
RVec3_transformRSTransposed( VALUE self, VALUE mtx )
{
    RVec3* v;
    RMtx3* m;
    RVec3 out;

    TypedData_Get_Struct( self, RVec3, &RVec3_type, v );
    TypedData_Get_Struct( mtx, RMtx3, &RMtx3_type, m );
    RVec3TransformRSTransposed( &out, m, v );

    return RVec3_from_source( &out );
}

/*
 * call-seq: transformRSTransposed!(mtx) -> self
 *
 * Makes itself as the transformation result of
 *  RVec3(self.x,self.y,self.z).transform(mtx^T)
 *
 * === Notice
 * * mtx : RMtx3
 * * the suffix "RS" means "matrix representing Rotational and Scaling
 *   transformation".
 */
static VALUE
RVec3_transformRSTransposed_intrusive( VALUE self, VALUE mtx )
{
    RVec3* v;
    RMtx3* m;

    TypedData_Get_Struct( self, RVec3, &RVec3_type, v );
    TypedData_Get_Struct( mtx, RMtx3, &RMtx3_type, m );
    RVec3TransformRSTransposed( v, m, v );

    return self;
}

/*
 * call-seq: transformByQuaternion(q) -> transformed RVec3
 */
static VALUE
RVec3_transformByQuaternion( VALUE self, VALUE quat )
{
    RVec3* v;
    RQuat* q;
    RVec3 out;

    TypedData_Get_Struct( self, RVec3, &RVec3_type, v );
    TypedData_Get_Struct( quat, RQuat, &RQuat_type, q );
    RVec3TransformByQuaternion( &out, q, v );

    return RVec3_from_source( &out );
}

/*
 * call-seq: transformByQuaternion!(q) -> self
 */
static VALUE
RVec3_transformByQuaternion_intrusive( VALUE self, VALUE quat )
{
    RVec3* v;
    RQuat* q;

    TypedData_Get_Struct( self, RVec3, &RVec3_type, v );
    TypedData_Get_Struct( quat, RQuat, &RQuat_type, q );
    RVec3TransformByQuaternion( v, q, v );

    return self;
}

/*
 * call-seq: getNormalized -> RVec3
 *
 * Returns normalized vector.
 */
static VALUE
RVec3_normalize( VALUE self )
{
    RVec3* v = NULL;
    RVec3 out;

    TypedData_Get_Struct( self, RVec3, &RVec3_type, v );
    RVec3Normalize( &out, v );

    return RVec3_from_source( &out );
}

/*
 * call-seq: normalize! -> self
 *
 * Normalizes itself.
 */
static VALUE
RVec3_normalize_intrusive( VALUE self )
{
    RVec3* v = NULL;

    TypedData_Get_Struct( self, RVec3, &RVec3_type, v );
    RVec3Normalize( v, v );

    return self;
}

/*
 * call-seq: +
 *
 * +vec : Unary plus operator.
 */
static VALUE
RVec3_op_unary_plus( VALUE self )
{
    return self;
}

/*
 * call-seq: -
 *
 * -vec : Unary minus operator.
 */
static VALUE
RVec3_op_unary_minus( VALUE self )
{
    RVec3* v   = NULL;
    RVec3 out;

    TypedData_Get_Struct( self, RVec3, &RVec3_type, v );
    RVec3Scale( &out, v, (rmReal)(-1) );

    return RVec3_from_source( &out );
}

/*
 * call-seq: +
 *
 * vec1 + vec2 : Binary plus operator.
 */
static VALUE
RVec3_op_binary_plus( VALUE self, VALUE other )
{
    RVec3* v1 = NULL;
    RVec3* v2 = NULL;
    RVec3 result;

#ifdef RMATH_ENABLE_ARGUMENT_CHECK
    if ( !IsRVec3(other) )
    {
        rb_raise( rb_eTypeError,
                  "RVec3#+ : Unknown type %s.",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other )
            );
        return Qnil;
    }
#endif

    TypedData_Get_Struct( self, RVec3, &RVec3_type, v1 );
    TypedData_Get_Struct( other,  RVec3, &RVec3_type, v2 );
    RVec3Add( &result, v1, v2 );

    return RVec3_from_source( &result );
}

/*
 * call-seq: -
 *
 * vec1 - vec2 : Binary minus operator.
 */
static VALUE
RVec3_op_binary_minus( VALUE self, VALUE other )
{
    RVec3* v1 = NULL;
    RVec3* v2 = NULL;
    RVec3 result;

#ifdef RMATH_ENABLE_ARGUMENT_CHECK
    if ( !IsRVec3(other) )
    {
        rb_raise( rb_eTypeError,
                  "RVec3#- : Unknown type %s.",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other )
            );
        return Qnil;
    }
#endif

    TypedData_Get_Struct( self, RVec3, &RVec3_type, v1 );
    TypedData_Get_Struct( other,  RVec3, &RVec3_type, v2 );
    RVec3Sub( &result, v1, v2 );

    return RVec3_from_source( &result );
}

/*
 * call-seq: **
 *
 * vec1 * vec2 : Binary multiply operator.
 */
static VALUE
RVec3_op_binary_mult( VALUE self, VALUE other )
{
    RVec3* v  = NULL;
    RVec3 result;
    rmReal f;

    switch( TYPE(other) )
    {
    case T_FIXNUM:
    case T_FLOAT:
    {
        TypedData_Get_Struct( self, RVec3, &RVec3_type, v );
        f = NUM2DBL( other );
        RVec3Scale( &result, v, f );

        return RVec3_from_source( &result );
    }
    break;

    default:
    {
        rb_raise( rb_eTypeError,
                  "RVec3#* : Unknown type %s.",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other )
            );
        return Qnil;
    }
    } /* End : switch( TYPE(other) ) */
}

/*
 * call-seq: ==
 *
 * vec1 == vec2 : evaluates equality.
 */
static VALUE
RVec3_op_binary_eq( VALUE self, VALUE other )
{
    if ( IsRVec3(other) )
    {
        RVec3* v1 = NULL;
        RVec3* v2 = NULL;

        TypedData_Get_Struct( self,  RVec3, &RVec3_type, v1 );
        TypedData_Get_Struct( other, RVec3, &RVec3_type, v2 );

        if ( !RVec3Equal(v1,v2) )
            return Qfalse;
        else
            return Qtrue;
    }
    else
    {
        return Qfalse;
    }
}

/*
 * call-seq: vec1.add!( vec2 )
 *
 * vec1 += vec2 : appends the elements of +vec2+ into corresponding +vec1+ elements.
 */
static VALUE
RVec3_op_assign_plus( VALUE self, VALUE other )
{
    RVec3* v1 = NULL;
    RVec3* v2 = NULL;

#ifdef RMATH_ENABLE_ARGUMENT_CHECK
    if ( !IsRVec3(other) )
    {
        rb_raise( rb_eTypeError,
                  "RVec3#+= : Unknown type %s.",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other )
            );
        return Qnil;
    }
#endif

    TypedData_Get_Struct( self,  RVec3, &RVec3_type, v1 );
    TypedData_Get_Struct( other, RVec3, &RVec3_type, v2 );

    RVec3Add( v1, v1, v2 );

    return self;
}

/*
 * call-seq: vec1.sub!( vec2 )
 *
 * vec1 -= vec2 : subtracts the elements of +vec2+ from corresponding +vec1+ elements.
 */
static VALUE
RVec3_op_assign_minus( VALUE self, VALUE other )
{
    RVec3* v1 = NULL;
    RVec3* v2 = NULL;

#ifdef RMATH_ENABLE_ARGUMENT_CHECK
    if ( !IsRVec3(other) )
    {
        rb_raise( rb_eTypeError,
                  "RVec3#-= : Unknown type %s.",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other )
            );
        return Qnil;
    }
#endif

    TypedData_Get_Struct( self,  RVec3, &RVec3_type, v1 );
    TypedData_Get_Struct( other, RVec3, &RVec3_type, v2 );

    RVec3Sub( v1, v1, v2 );

    return self;
}

/*
 * call-seq: vec1.mul!( vec2 )
 *
 * vec1 *= vec2
 */
static VALUE
RVec3_op_assign_mult( VALUE self, VALUE other )
{
    RVec3* v = NULL;
    rmReal  f;

#ifdef RMATH_ENABLE_ARGUMENT_CHECK
    if ( TYPE(other) != T_FIXNUM && TYPE(other) != T_FLOAT )
    {
        rb_raise( rb_eTypeError,
                  "RVec3#*= : Unknown type %s.",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other )
            );
        return Qnil;
    }
#endif

    TypedData_Get_Struct( self, RVec3, &RVec3_type, v );
    f = NUM2DBL( other );
    RVec3Scale( v, v, f );

    return self;
}


/********************************************************************************
 *
 * RVec4
 *
 ********************************************************************************/

/*
 * Document-class: RMath3D::RVec4
 * provies 4 element vector arithmetic.
 */

static void
RVec4_free( void* ptr )
{
	xfree( ptr );
}

static size_t
RVec4_memsize( const void* ptr )
{
    const struct RVec4* data = ptr;
    return data ? sizeof(*data) : 0;
}

static VALUE
RVec4_from_source( RVec4* src )
{
    RVec4* v = ZALLOC( struct RVec4 );
    RVec4Copy( v, src );
    return TypedData_Wrap_Struct( rb_cRVec4, &RVec4_type, v );
}


static VALUE
RVec4_allocate( VALUE klass )
{
    RVec4* v = ZALLOC( RVec4 );
    return TypedData_Wrap_Struct( klass, &RVec4_type, v );
}


/*
 * call-seq:
 *   RVec4.new -> (0,0,0,0)
 *   RVec4.new(e) -> (e,e,e,e)
 *   RVec4.new( other ) : Copy Constructor
 *   RVec4.new( e0, e1, e2, e3 ) -> (e0,e1,e2,e3)
 *
 * Creates a new 4 element vector.
 */
static VALUE
RVec4_initialize( int argc, VALUE* argv, VALUE self )
{
    RVec4* v = NULL;
    TypedData_Get_Struct( self, RVec4, &RVec4_type, v );

    switch( argc )
    {
    case 0:
    {
        /* Default Constructor */
        RVec4SetElements( v, 0.0f, 0.0f, 0.0f, 0.0f );
        return self;
    }
    break;

    case 1:
    {
        VALUE arg = argv[0];
        switch ( TYPE( arg ) )
        {
        case T_FIXNUM:
        case T_FLOAT:
        {
            /* convert to float */
            rmReal f = NUM2DBL( arg );
            RVec4SetElements( v, f,f,f,f );
            return self;
        }
        break;

        case T_DATA:
        {
            if ( IsRVec3(arg) )
            {
                /* Create from RVec3 */
                RVec3* other;
                TypedData_Get_Struct( arg , RVec3, &RVec3_type, other );
                RVec4SetElements( v, other->x, other->y, other->z, 0.0f );
                return self;
            }
            else if ( IsRVec4(arg) )
            {
                /* Copy Constructor */
                RVec4* other;
                TypedData_Get_Struct( arg , RVec4, &RVec4_type, other );
                RVec4SetElements( v, other->x, other->y, other->z, other->w );
                return self;
            }
            else
                return Qnil;
        }
        break;

        default:
        {
            rb_raise( rb_eTypeError,
                      "RVec4_new : Unknown type %s.",
                      rb_special_const_p( self ) ? RSTRING_PTR( rb_inspect( self ) ) : rb_obj_classname( self )
                );
            return Qnil;
        }
        break;
        } /* End : switch ( TYPE( arg ) ) */
    } /* End : case 1 */
    break;

    case 4:
    {
        /* Element-wise setter */
        int i;
        for ( i = 0; i < argc; ++i )
        {
            switch ( TYPE( argv[i] ) )
            {
            case T_FIXNUM:
            case T_FLOAT:
            {
                rmReal f = NUM2DBL( argv[i] );
                v->e[i] = f;
            }
            break;

            default:
            {
                rb_raise( rb_eTypeError,
                          "RVec4_new : Unknown type %s. at arg %d",
                          rb_special_const_p( self ) ? RSTRING_PTR( rb_inspect( self ) ) : rb_obj_classname( self ),
                          i
                    );
                return Qnil;
            }
            break;
            }
        } /* End : for ( i = 0; i < argc; ++i ) */

        return self;
    } /* End : case 4 */
    break;

    default:
    {
        rb_raise( rb_eRuntimeError, "RVec4_initialize : wrong # of arguments (%d)", argc );
        return Qnil;
    }
    break;
    } /* End : switch( argc ) */
}

/*
 * call-seq: to_s
 *
 * Returns human-readable string.
 */
static VALUE
RVec4_to_s( VALUE self )
{
    RVec4* v;
    char dest[128], work[4][32];
    int i;

    TypedData_Get_Struct( self, RVec4, &RVec4_type, v );

    for ( i = 0; i < 4; ++i )
    {
        if ( isnan(v->e[i]) )
            sprintf( work[i], "%s", v->e[i] >= 0 ? "Inf" : "-Inf" );
        else if ( isinf(v->e[i]) )
            sprintf( work[i], "%s", "NaN" );
        else
            sprintf( work[i], RMATH_FVAL_FMT, v->e[i] );
    }
    sprintf( dest, "(%s, %s, %s, %s)", work[0], work[1], work[2], work[3] );

    return rb_str_new2( dest );
}

/*
 * call-seq: inspect
 *
 * Returns human-readable string.
 */
static VALUE
RVec4_inspect( VALUE self )
{
    return RVec4_to_s(self);
}

/*
 * call-seq: to_a
 *
 * Returns its elements as a new Array.
 */
static VALUE
RVec4_to_a( VALUE self )
{
    RVec4* v = NULL;
    VALUE dbl[4];
    TypedData_Get_Struct( self, RVec4, &RVec4_type, v );

    dbl[0] = DBL2NUM(RVec4GetElement(v,0));
    dbl[1] = DBL2NUM(RVec4GetElement(v,1));
    dbl[2] = DBL2NUM(RVec4GetElement(v,2));
    dbl[3] = DBL2NUM(RVec4GetElement(v,3));

    return rb_ary_new4( 4, dbl );
}

/*
 * call-seq: coerse(other)
 *
 * Resolves type mismatch.
 */
static VALUE
RVec4_coerce( VALUE self, VALUE other )
{
    RVec4* v = NULL;
    TypedData_Get_Struct( self, RVec4, &RVec4_type, v );

    switch( TYPE(other) )
    {
    case T_FLOAT:
    case T_FIXNUM:
    case T_BIGNUM:
    {
        /* 'other (op) RVec4'
         * -> try again as 'RVec4 (op) other'
         */
        return rb_ary_new3( 2,  self, other );
    }
    break;

    default:
    {
        rb_raise( rb_eTypeError,
                  "%s can't be coerced into %s",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other ),
                  rb_obj_classname( self )
            );
        return Qnil;
    }
    break;
    }
}

/*
 * call-seq: setElements( e0, e1, e2, e3 )
 *
 * Stores given 4 new values.
 */
static VALUE
RVec4_setElements( VALUE self, VALUE x, VALUE y, VALUE z, VALUE w )
{
    RVec4* v = NULL;
    rmReal flt0, flt1, flt2, flt3;

    TypedData_Get_Struct( self, RVec4, &RVec4_type, v );
    flt0 = NUM2DBL(x);
    flt1 = NUM2DBL(y);
    flt2 = NUM2DBL(z);
    flt3 = NUM2DBL(w);

    RVec4SetElements( v, flt0, flt1, flt2, flt3 );

    return self;
}

/*
 * call-seq: [i]= value
 *
 * Stores +value+ at +i+.
 */
static VALUE
RVec4_setElement( VALUE self, VALUE i, VALUE f )
{
    RVec4* v = NULL;
    int at;
    rmReal flt;

    TypedData_Get_Struct( self, RVec4, &RVec4_type, v );
    at = NUM2INT(i);
    flt = NUM2DBL(f);

    RVec4SetElement( v, at, flt );

    return self;
}

/*
 * call-seq: x= value
 *
 * Stores +value+ as +x+.
 */
static VALUE
RVec4_setX( VALUE self, VALUE x )
{
    RVec4* v = NULL;
    rmReal flt0;

    TypedData_Get_Struct( self, RVec4, &RVec4_type, v );
    flt0 = NUM2DBL(x);

    RVec4SetX( v, flt0 );

    return self;
}

/*
 * call-seq: y= value
 *
 * Stores +value+ as +y+.
 */
static VALUE
RVec4_setY( VALUE self, VALUE y )
{
    RVec4* v = NULL;
    rmReal flt0;

    TypedData_Get_Struct( self, RVec4, &RVec4_type, v );
    flt0 = NUM2DBL(y);

    RVec4SetY( v, flt0 );

    return self;
}

/*
 * call-seq: z= value
 *
 * Stores +value+ as +z+.
 */
static VALUE
RVec4_setZ( VALUE self, VALUE z )
{
    RVec4* v = NULL;
    rmReal flt0;

    TypedData_Get_Struct( self, RVec4, &RVec4_type, v );
    flt0 = NUM2DBL(z);

    RVec4SetZ( v, flt0 );

    return self;
}

/*
 * call-seq: w= value
 *
 * Stores +value+ as +w+.
 */
static VALUE
RVec4_setW( VALUE self, VALUE w )
{
    RVec4* v = NULL;
    rmReal flt0;

    TypedData_Get_Struct( self, RVec4, &RVec4_type, v );
    flt0 = NUM2DBL(w);

    RVec4SetW( v, flt0 );

    return self;
}

/*
 * call-seq: xyz= vXYZ
 *
 * Copies the values of +vXYZ+(RVec3) into +x+, +y+ and +z+.
 */
static VALUE
RVec4_setXYZ( VALUE self, VALUE xyz )
{
    RVec4* v = NULL;
    RVec3* in = NULL;

    TypedData_Get_Struct( self, RVec4, &RVec4_type, v );
    TypedData_Get_Struct( xyz, RVec3, &RVec3_type, in );

    RVec4SetXYZ( v, in );

    return self;
}

/*
 * call-seq: [i] -> value
 *
 * Returns the element at +i+.
 */
static VALUE
RVec4_getElement( VALUE self, VALUE i )
{
    RVec4* v = NULL;
    int at;
    rmReal flt0;

    TypedData_Get_Struct( self, RVec4, &RVec4_type, v );
    at = FIX2INT(i);
    flt0 = RVec4GetElement( v, at );

    return DBL2NUM( flt0 );
}

/*
 * call-seq: x -> value
 *
 * Returns the value of +x+.
 */
static VALUE
RVec4_getX( VALUE self )
{
    RVec4* v = NULL;
    rmReal flt0;

    TypedData_Get_Struct( self, RVec4, &RVec4_type, v );
    flt0 = RVec4GetX( v );

    return DBL2NUM( flt0 );
}

/*
 * call-seq: y -> value
 *
 * Returns the value of +y+.
 */
static VALUE
RVec4_getY( VALUE self )
{
    RVec4* v = NULL;
    rmReal flt0;

    TypedData_Get_Struct( self, RVec4, &RVec4_type, v );
    flt0 = RVec4GetY( v );

    return DBL2NUM( flt0 );
}

/*
 * call-seq: z -> value
 *
 * Returns the value of +z+.
 */
static VALUE
RVec4_getZ( VALUE self )
{
    RVec4* v = NULL;
    rmReal flt0;

    TypedData_Get_Struct( self, RVec4, &RVec4_type, v );
    flt0 = RVec4GetZ( v );

    return DBL2NUM( flt0 );
}

/*
 * call-seq: w -> value
 *
 * Returns the value of +w+.
 */
static VALUE
RVec4_getW( VALUE self )
{
    RVec4* v = NULL;
    rmReal flt0;

    TypedData_Get_Struct( self, RVec4, &RVec4_type, v );
    flt0 = RVec4GetW( v );

    return DBL2NUM( flt0 );
}

/*
 * call-seq: xyz -> RVec3
 *
 * Returns the values of +x+, +y+ and +z+ with new RVec3(+x+,+y+,+z+).
 */
static VALUE
RVec4_getXYZ( VALUE self )
{
    RVec4* v = NULL;
    RVec3 out;

    TypedData_Get_Struct( self, RVec4, &RVec4_type, v );

    RVec4GetXYZ( &out, v );

    return RVec3_from_source( &out );
}

/*
 * call-seq: getLength
 *
 * Returns the Euclidean length.
 */
static VALUE
RVec4_getLength( VALUE self )
{
    RVec4* v = NULL;
    rmReal flt0;

    TypedData_Get_Struct( self, RVec4, &RVec4_type, v );
    flt0 = RVec4Length( v );

    return DBL2NUM( flt0 );
}

/*
 * call-seq: getLengthSq
 *
 * Returns the squared Euclidean length.
 */
static VALUE
RVec4_getLengthSq( VALUE self )
{
    RVec4* v = NULL;
    rmReal flt0;

    TypedData_Get_Struct( self, RVec4, &RVec4_type, v );
    flt0 = RVec4LengthSq( v );

    return DBL2NUM( flt0 );
}

/*
 * call-seq: RVec4.dot(v1,v2) -> value
 *
 * Calculates the dot product of +v1+ and +v2+.
 */
static VALUE
RVec4_dot( VALUE self, VALUE v1, VALUE v2 )
{
    RVec4* vec1 = NULL;
    RVec4* vec2 = NULL;
    rmReal result;

    TypedData_Get_Struct( v1, RVec4, &RVec4_type, vec1 );
    TypedData_Get_Struct( v2, RVec4, &RVec4_type, vec2 );
    result = RVec4Dot( vec1, vec2 );

    return DBL2NUM( result );
}

/*
 * call-seq: transform(mtx4) -> transformed RVec4
 *
 * Returns new RVec4 containing the result of the transformation by +mtx4+ (RMtx4).
 */
static VALUE
RVec4_transform( VALUE self, VALUE mtx )
{
    RVec4* v;
    RMtx4* m;
    RVec4 out;

    TypedData_Get_Struct( self, RVec4, &RVec4_type, v );
    TypedData_Get_Struct( mtx, RMtx4, &RMtx4_type, m );
    RVec4Transform( &out, m, v );

    return RVec4_from_source( &out );
}

/*
 * call-seq: transform(mtx4) -> self
 *
 * Applies the transform matrix +mtx4+ (RMtx4).
 */
static VALUE
RVec4_transform_intrusive( VALUE self, VALUE mtx )
{
    RVec4* v;
    RMtx4* m;

    TypedData_Get_Struct( self, RVec4, &RVec4_type, v );
    TypedData_Get_Struct( mtx, RMtx4, &RMtx4_type, m );
    RVec4Transform( v, m, v );

    return self;
}

/*
 * call-seq: transformTransposed(mtx4) -> RVec4 transformed by mtx4^T
 *
 * Returns new RVec4 containing the result of the transformation by +mtx4^T+ (RMtx4).
 */
static VALUE
RVec4_transformTransposed( VALUE self, VALUE mtx )
{
    RVec4* v;
    RMtx4* m;
    RVec4 out;

    TypedData_Get_Struct( self, RVec4, &RVec4_type, v );
    TypedData_Get_Struct( mtx, RMtx4, &RMtx4_type, m );
    RVec4TransformTransposed( &out, m, v );

    return RVec4_from_source( &out );
}

/*
 * call-seq: transformTransposed(mtx4) -> self
 *
 * Applies the transform matrix +mtx4^T+ (RMtx4).
 */
static VALUE
RVec4_transformTransposed_intrusive( VALUE self, VALUE mtx )
{
    RVec4* v;
    RMtx4* m;

    TypedData_Get_Struct( self, RVec4, &RVec4_type, v );
    TypedData_Get_Struct( mtx, RMtx4, &RMtx4_type, m );
    RVec4TransformTransposed( v, m, v );

    return self;
}

/*
 * call-seq: getNormalized -> RVec4
 *
 * Returns normalized vector.
 */
static VALUE
RVec4_normalize( VALUE self )
{
    RVec4* v = NULL;
    RVec4 out;

    TypedData_Get_Struct( self, RVec4, &RVec4_type, v );
    RVec4Normalize( &out, v );

    return RVec4_from_source( &out );
}

/*
 * call-seq: normalize! -> self
 *
 * Normalizes itself.
 */
static VALUE
RVec4_normalize_intrusive( VALUE self )
{
    RVec4* v = NULL;

    TypedData_Get_Struct( self, RVec4, &RVec4_type, v );
    RVec4Normalize( v, v );

    return self;
}

/*
 * call-seq: +
 *
 * +vec : Unary plus operator.
 */
static VALUE
RVec4_op_unary_plus( VALUE self )
{
    return self;
}

/*
 * call-seq: -
 *
 * -vec : Unary minus operator.
 */
static VALUE
RVec4_op_unary_minus( VALUE self )
{
    RVec4* v = NULL;
    RVec4 out;

    TypedData_Get_Struct( self, RVec4, &RVec4_type, v );
    RVec4Scale( &out, v, (rmReal)(-1) );

    return RVec4_from_source( &out );
}

/*
 * call-seq: +
 *
 * vec1 + vec2 : Binary plus operator.
 */
static VALUE
RVec4_op_binary_plus( VALUE self, VALUE other )
{
    RVec4* v1 = NULL;
    RVec4* v2 = NULL;
    RVec4 result;

#ifdef RMATH_ENABLE_ARGUMENT_CHECK
    if ( !IsRVec4(other) )
    {
        rb_raise( rb_eTypeError,
                  "RVec4#+ : Unknown type %s.",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other )
            );
        return Qnil;
    }
#endif

    TypedData_Get_Struct( self, RVec4, &RVec4_type, v1 );
    TypedData_Get_Struct( other,  RVec4, &RVec4_type, v2 );
    RVec4Add( &result, v1, v2 );

    return RVec4_from_source( &result );
}

/*
 * call-seq: -
 *
 * vec1 - vec2 : Binary minus operator.
 */
static VALUE
RVec4_op_binary_minus( VALUE self, VALUE other )
{
    RVec4* v1 = NULL;
    RVec4* v2 = NULL;
    RVec4 result;

#ifdef RMATH_ENABLE_ARGUMENT_CHECK
    if ( !IsRVec4(other) )
    {
        rb_raise( rb_eTypeError,
                  "RVec4#- : Unknown type %s.",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other )
            );
        return Qnil;
    }
#endif

    TypedData_Get_Struct( self, RVec4, &RVec4_type, v1 );
    TypedData_Get_Struct( other,  RVec4, &RVec4_type, v2 );
    RVec4Sub( &result, v1, v2 );

    return RVec4_from_source( &result );
}

/*
 * call-seq: **
 *
 * vec1 * vec2 : Binary multiply operator.
 */
static VALUE
RVec4_op_binary_mult( VALUE self, VALUE other )
{
    RVec4* v  = NULL;
    RVec4 result;
    rmReal f;

    switch( TYPE(other) )
    {
    case T_FIXNUM:
    case T_FLOAT:
    {
        TypedData_Get_Struct( self, RVec4, &RVec4_type, v );
        f = NUM2DBL( other );
        RVec4Scale( &result, v, f );

        return RVec4_from_source( &result );
    }
    break;

    default:
    {
        rb_raise( rb_eTypeError,
                  "RVec4#* : Unknown type %s.",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other )
            );
        return Qnil;
    }
    } /* End : switch( TYPE(other) ) */
}

/*
 * call-seq: ==
 *
 * vec1 == vec2 : evaluates equality.
 */
static VALUE
RVec4_op_binary_eq( VALUE self, VALUE other )
{
    if ( IsRVec4(other) )
    {
        RVec4* v1 = NULL;
        RVec4* v2 = NULL;

        TypedData_Get_Struct( self,  RVec4, &RVec4_type, v1 );
        TypedData_Get_Struct( other, RVec4, &RVec4_type, v2 );

        if ( !RVec4Equal(v1,v2) )
            return Qfalse;
        else
            return Qtrue;
    }
    else
    {
        return Qfalse;
    }
}

/*
 * call-seq: vec1.add!( vec2 )
 *
 * vec1 += vec2 : appends the elements of +vec2+ into corresponding +vec1+ elements.
 */
static VALUE
RVec4_op_assign_plus( VALUE self, VALUE other )
{
    RVec4* v1 = NULL;
    RVec4* v2 = NULL;

#ifdef RMATH_ENABLE_ARGUMENT_CHECK
    if ( !IsRVec4(other) )
    {
        rb_raise( rb_eTypeError,
                  "RVec4#+= : Unknown type %s.",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other )
            );
        return Qnil;
    }
#endif

    TypedData_Get_Struct( self,  RVec4, &RVec4_type, v1 );
    TypedData_Get_Struct( other, RVec4, &RVec4_type, v2 );

    RVec4Add( v1, v1, v2 );

    return self;
}

/*
 * call-seq: vec1.sub!( vec2 )
 *
 * vec1 -= vec2 : subtracts the elements of +vec2+ from corresponding +vec1+ elements.
 */
static VALUE
RVec4_op_assign_minus( VALUE self, VALUE other )
{
    RVec4* v1 = NULL;
    RVec4* v2 = NULL;

#ifdef RMATH_ENABLE_ARGUMENT_CHECK
    if ( !IsRVec4(other) )
    {
        rb_raise( rb_eTypeError,
                  "RVec4#-= : Unknown type %s.",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other )
            );
        return Qnil;
    }
#endif

    TypedData_Get_Struct( self,  RVec4, &RVec4_type, v1 );
    TypedData_Get_Struct( other, RVec4, &RVec4_type, v2 );

    RVec4Sub( v1, v1, v2 );

    return self;
}

/*
 * call-seq: vec1.mul!( vec2 )
 *
 * vec1 *= vec2
 */
static VALUE
RVec4_op_assign_mult( VALUE self, VALUE other )
{
    RVec4* v = NULL;
    rmReal  f;

#ifdef RMATH_ENABLE_ARGUMENT_CHECK
    if ( TYPE(other) != T_FIXNUM && TYPE(other) != T_FLOAT )
    {
        rb_raise( rb_eTypeError,
                  "RVec4#*= : Unknown type %s.",
                  rb_special_const_p( other ) ? RSTRING_PTR( rb_inspect( other ) ) : rb_obj_classname( other )
            );
        return Qnil;
    }
#endif

    TypedData_Get_Struct( self, RVec4, &RVec4_type, v );
    f = NUM2DBL( other );
    RVec4Scale( v, v, f );

    return self;
}


/********************************************************************************
 *
 * Init_RMath
 *
 ********************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*
= RMath module

provides fundamental vector and matrix arithmeic for 3D applications.

== Feature

=== Supports frequently-used vector and matrix classes:

RMtx3 (3x3 matrix), RMtx4 (4x4 matrix), RQuat (Quaternion), RVec3 (3 element vector), RVec4 (4 element vector)

=== Two implementations that are interchangeable with each other:

1. RMath.so : C implemantation (a.k.a Ruby extension library) for faster execution.
2. RMath.rb : Ruby implemantation for debugging use.

 */
RMATH_EXPORT void
Init_rmath3d()
{
    rb_mRMath = rb_define_module( "RMath3D" );

    rb_cRVec2 = rb_define_class_under( rb_mRMath, "RVec2", rb_cObject );
    rb_cRVec3 = rb_define_class_under( rb_mRMath, "RVec3", rb_cObject );
    rb_cRVec4 = rb_define_class_under( rb_mRMath, "RVec4", rb_cObject );
    rb_cRQuat = rb_define_class_under( rb_mRMath, "RQuat", rb_cObject );
    rb_cRMtx2 = rb_define_class_under( rb_mRMath, "RMtx2", rb_cObject );
    rb_cRMtx3 = rb_define_class_under( rb_mRMath, "RMtx3", rb_cObject );
    rb_cRMtx4 = rb_define_class_under( rb_mRMath, "RMtx4", rb_cObject );

    rb_define_const( rb_mRMath, "TOLERANCE", DBL2NUM(RMATH_TOLERANCE) );

    /********************************************************************************
     * RMtx2
     ********************************************************************************/

    rb_define_alloc_func( rb_cRMtx2, RMtx2_allocate );

    rb_define_private_method( rb_cRMtx2, "initialize", RMtx2_initialize, -1 );
    rb_define_method( rb_cRMtx2, "to_s", RMtx2_to_s, 0 );
    rb_define_method( rb_cRMtx2, "inspect", RMtx2_inspect, 0 );
    rb_define_method( rb_cRMtx2, "to_a", RMtx2_to_a, 0 );
    rb_define_method( rb_cRMtx2, "coerce", RMtx2_coerce, 1 );

    rb_define_method( rb_cRMtx2, "setElements", RMtx2_setElements, -1 );
    rb_define_method( rb_cRMtx2, "[]=", RMtx2_setElement, 3 );
    rb_define_method( rb_cRMtx2, "[]", RMtx2_getElement, 2 );

    rb_define_method( rb_cRMtx2, "e00", RMtx2_get_e00, 0 );
    rb_define_method( rb_cRMtx2, "e01", RMtx2_get_e01, 0 );
    rb_define_method( rb_cRMtx2, "e10", RMtx2_get_e10, 0 );
    rb_define_method( rb_cRMtx2, "e11", RMtx2_get_e11, 0 );

    rb_define_method( rb_cRMtx2, "e00=", RMtx2_set_e00, 1 );
    rb_define_method( rb_cRMtx2, "e01=", RMtx2_set_e01, 1 );
    rb_define_method( rb_cRMtx2, "e10=", RMtx2_set_e10, 1 );
    rb_define_method( rb_cRMtx2, "e11=", RMtx2_set_e11, 1 );

    rb_define_alias( rb_cRMtx2, "setElement", "[]=" );
    rb_define_alias( rb_cRMtx2, "getElement", "[]"  );

    rb_define_method( rb_cRMtx2, "getRow", RMtx2_getRow, 1 );
    rb_define_method( rb_cRMtx2, "getColumn", RMtx2_getColumn, 1 );
    rb_define_method( rb_cRMtx2, "setRow", RMtx2_setRow, 2 );
    rb_define_method( rb_cRMtx2, "setColumn", RMtx2_setColumn, 2 );

    rb_define_method( rb_cRMtx2, "setZero", RMtx2_setZero, 0 );
    rb_define_method( rb_cRMtx2, "setIdentity", RMtx2_setIdentity, 0 );
    rb_define_method( rb_cRMtx2, "getDeterminant", RMtx2_getDeterminant, 0 );
    rb_define_method( rb_cRMtx2, "getTransposed", RMtx2_transpose, 0 );
    rb_define_method( rb_cRMtx2, "transpose!", RMtx2_transpose_intrusive, 0 );
    rb_define_method( rb_cRMtx2, "getInverse", RMtx2_inverse, 0 );
    rb_define_method( rb_cRMtx2, "invert!", RMtx2_invert, 0 );

    rb_define_method( rb_cRMtx2, "rotation", RMtx2_rotation, 1 );
    rb_define_method( rb_cRMtx2, "scaling", RMtx2_scaling, 2 );

    rb_define_method( rb_cRMtx2, "+@", RMtx2_op_unary_plus, 0 );
    rb_define_method( rb_cRMtx2, "-@", RMtx2_op_unary_minus, 0 );
    rb_define_method( rb_cRMtx2, "+",  RMtx2_op_binary_plus, 1 );
    rb_define_method( rb_cRMtx2, "-",  RMtx2_op_binary_minus, 1 );
    rb_define_method( rb_cRMtx2, "*",  RMtx2_op_binary_mult, 1 );
    rb_define_method( rb_cRMtx2, "==", RMtx2_op_binary_eq, 1 );
    rb_define_method( rb_cRMtx2, "add!", RMtx2_op_assign_plus, 1 );
    rb_define_method( rb_cRMtx2, "sub!", RMtx2_op_assign_minus, 1 );
    rb_define_method( rb_cRMtx2, "mul!", RMtx2_op_assign_mult, 1 );

    /********************************************************************************
     * RMtx3
     ********************************************************************************/

    rb_define_alloc_func( rb_cRMtx3, RMtx3_allocate );

    rb_define_private_method( rb_cRMtx3, "initialize", RMtx3_initialize, -1 );
    rb_define_method( rb_cRMtx3, "to_s", RMtx3_to_s, 0 );
    rb_define_method( rb_cRMtx3, "inspect", RMtx3_inspect, 0 );
    rb_define_method( rb_cRMtx3, "to_a", RMtx3_to_a, 0 );
    rb_define_method( rb_cRMtx3, "coerce", RMtx3_coerce, 1 );

    rb_define_method( rb_cRMtx3, "setElements", RMtx3_setElements, -1 );
    rb_define_method( rb_cRMtx3, "[]=", RMtx3_setElement, 3 );
    rb_define_method( rb_cRMtx3, "[]", RMtx3_getElement, 2 );

    rb_define_method( rb_cRMtx3, "e00", RMtx3_get_e00, 0 );
    rb_define_method( rb_cRMtx3, "e01", RMtx3_get_e01, 0 );
    rb_define_method( rb_cRMtx3, "e02", RMtx3_get_e02, 0 );
    rb_define_method( rb_cRMtx3, "e10", RMtx3_get_e10, 0 );
    rb_define_method( rb_cRMtx3, "e11", RMtx3_get_e11, 0 );
    rb_define_method( rb_cRMtx3, "e12", RMtx3_get_e12, 0 );
    rb_define_method( rb_cRMtx3, "e20", RMtx3_get_e20, 0 );
    rb_define_method( rb_cRMtx3, "e21", RMtx3_get_e21, 0 );
    rb_define_method( rb_cRMtx3, "e22", RMtx3_get_e22, 0 );

    rb_define_method( rb_cRMtx3, "e00=", RMtx3_set_e00, 1 );
    rb_define_method( rb_cRMtx3, "e01=", RMtx3_set_e01, 1 );
    rb_define_method( rb_cRMtx3, "e02=", RMtx3_set_e02, 1 );
    rb_define_method( rb_cRMtx3, "e10=", RMtx3_set_e10, 1 );
    rb_define_method( rb_cRMtx3, "e11=", RMtx3_set_e11, 1 );
    rb_define_method( rb_cRMtx3, "e12=", RMtx3_set_e12, 1 );
    rb_define_method( rb_cRMtx3, "e20=", RMtx3_set_e20, 1 );
    rb_define_method( rb_cRMtx3, "e21=", RMtx3_set_e21, 1 );
    rb_define_method( rb_cRMtx3, "e22=", RMtx3_set_e22, 1 );

    rb_define_alias( rb_cRMtx3, "setElement", "[]=" );
    rb_define_alias( rb_cRMtx3, "getElement", "[]"  );

    rb_define_method( rb_cRMtx3, "getRow", RMtx3_getRow, 1 );
    rb_define_method( rb_cRMtx3, "getColumn", RMtx3_getColumn, 1 );
    rb_define_method( rb_cRMtx3, "setRow", RMtx3_setRow, 2 );
    rb_define_method( rb_cRMtx3, "setColumn", RMtx3_setColumn, 2 );

    rb_define_method( rb_cRMtx3, "setZero", RMtx3_setZero, 0 );
    rb_define_method( rb_cRMtx3, "setIdentity", RMtx3_setIdentity, 0 );
    rb_define_method( rb_cRMtx3, "getDeterminant", RMtx3_getDeterminant, 0 );
    rb_define_method( rb_cRMtx3, "getTransposed", RMtx3_transpose, 0 );
    rb_define_method( rb_cRMtx3, "transpose!", RMtx3_transpose_intrusive, 0 );
    rb_define_method( rb_cRMtx3, "getInverse", RMtx3_inverse, 0 );
    rb_define_method( rb_cRMtx3, "invert!", RMtx3_invert, 0 );

    rb_define_method( rb_cRMtx3, "rotationX", RMtx3_rotationX, 1 );
    rb_define_method( rb_cRMtx3, "rotationY", RMtx3_rotationY, 1 );
    rb_define_method( rb_cRMtx3, "rotationZ", RMtx3_rotationZ, 1 );
    rb_define_method( rb_cRMtx3, "rotationAxis", RMtx3_rotationAxis, 2 );
    rb_define_method( rb_cRMtx3, "rotationQuaternion", RMtx3_rotationQuaternion, 1 );
    rb_define_method( rb_cRMtx3, "scaling", RMtx3_scaling, 3 );

    rb_define_method( rb_cRMtx3, "+@", RMtx3_op_unary_plus, 0 );
    rb_define_method( rb_cRMtx3, "-@", RMtx3_op_unary_minus, 0 );
    rb_define_method( rb_cRMtx3, "+",  RMtx3_op_binary_plus, 1 );
    rb_define_method( rb_cRMtx3, "-",  RMtx3_op_binary_minus, 1 );
    rb_define_method( rb_cRMtx3, "*",  RMtx3_op_binary_mult, 1 );
    rb_define_method( rb_cRMtx3, "==", RMtx3_op_binary_eq, 1 );
    rb_define_method( rb_cRMtx3, "add!", RMtx3_op_assign_plus, 1 );
    rb_define_method( rb_cRMtx3, "sub!", RMtx3_op_assign_minus, 1 );
    rb_define_method( rb_cRMtx3, "mul!", RMtx3_op_assign_mult, 1 );

    /********************************************************************************
     * RMtx4
     ********************************************************************************/

    rb_define_alloc_func( rb_cRMtx4, RMtx4_allocate );

    rb_define_private_method( rb_cRMtx4, "initialize", RMtx4_initialize, -1 );
    rb_define_method( rb_cRMtx4, "to_s", RMtx4_to_s, 0 );
    rb_define_method( rb_cRMtx4, "inspect", RMtx4_inspect, 0 );
    rb_define_method( rb_cRMtx4, "to_a", RMtx4_to_a, 0 );
    rb_define_method( rb_cRMtx4, "coerce", RMtx4_coerce, 1 );

    rb_define_method( rb_cRMtx4, "setElements", RMtx4_setElements, -1 );
    rb_define_method( rb_cRMtx4, "[]=", RMtx4_setElement, 3 );
    rb_define_method( rb_cRMtx4, "[]", RMtx4_getElement, 2 );

    rb_define_method( rb_cRMtx4, "e00", RMtx4_get_e00, 0 );
    rb_define_method( rb_cRMtx4, "e01", RMtx4_get_e01, 0 );
    rb_define_method( rb_cRMtx4, "e02", RMtx4_get_e02, 0 );
    rb_define_method( rb_cRMtx4, "e03", RMtx4_get_e03, 0 );
    rb_define_method( rb_cRMtx4, "e10", RMtx4_get_e10, 0 );
    rb_define_method( rb_cRMtx4, "e11", RMtx4_get_e11, 0 );
    rb_define_method( rb_cRMtx4, "e12", RMtx4_get_e12, 0 );
    rb_define_method( rb_cRMtx4, "e13", RMtx4_get_e13, 0 );
    rb_define_method( rb_cRMtx4, "e20", RMtx4_get_e20, 0 );
    rb_define_method( rb_cRMtx4, "e21", RMtx4_get_e21, 0 );
    rb_define_method( rb_cRMtx4, "e22", RMtx4_get_e22, 0 );
    rb_define_method( rb_cRMtx4, "e23", RMtx4_get_e23, 0 );
    rb_define_method( rb_cRMtx4, "e30", RMtx4_get_e30, 0 );
    rb_define_method( rb_cRMtx4, "e31", RMtx4_get_e31, 0 );
    rb_define_method( rb_cRMtx4, "e32", RMtx4_get_e32, 0 );
    rb_define_method( rb_cRMtx4, "e33", RMtx4_get_e33, 0 );

    rb_define_method( rb_cRMtx4, "e00=", RMtx4_set_e00, 1 );
    rb_define_method( rb_cRMtx4, "e01=", RMtx4_set_e01, 1 );
    rb_define_method( rb_cRMtx4, "e02=", RMtx4_set_e02, 1 );
    rb_define_method( rb_cRMtx4, "e03=", RMtx4_set_e03, 1 );
    rb_define_method( rb_cRMtx4, "e10=", RMtx4_set_e10, 1 );
    rb_define_method( rb_cRMtx4, "e11=", RMtx4_set_e11, 1 );
    rb_define_method( rb_cRMtx4, "e12=", RMtx4_set_e12, 1 );
    rb_define_method( rb_cRMtx4, "e13=", RMtx4_set_e13, 1 );
    rb_define_method( rb_cRMtx4, "e20=", RMtx4_set_e20, 1 );
    rb_define_method( rb_cRMtx4, "e21=", RMtx4_set_e21, 1 );
    rb_define_method( rb_cRMtx4, "e22=", RMtx4_set_e22, 1 );
    rb_define_method( rb_cRMtx4, "e23=", RMtx4_set_e23, 1 );
    rb_define_method( rb_cRMtx4, "e30=", RMtx4_set_e30, 1 );
    rb_define_method( rb_cRMtx4, "e31=", RMtx4_set_e31, 1 );
    rb_define_method( rb_cRMtx4, "e32=", RMtx4_set_e32, 1 );
    rb_define_method( rb_cRMtx4, "e33=", RMtx4_set_e33, 1 );

    rb_define_alias( rb_cRMtx4, "setElement", "[]=" );
    rb_define_alias( rb_cRMtx4, "getElement", "[]" );

    rb_define_method( rb_cRMtx4, "getRow", RMtx4_getRow, 1 );
    rb_define_method( rb_cRMtx4, "getColumn", RMtx4_getColumn, 1 );
    rb_define_method( rb_cRMtx4, "setRow", RMtx4_setRow, 2 );
    rb_define_method( rb_cRMtx4, "setColumn", RMtx4_setColumn, 2 );

    rb_define_method( rb_cRMtx4, "getUpper3x3", RMtx4_getUpper3x3, 0 );
    rb_define_method( rb_cRMtx4, "setUpper3x3", RMtx4_setUpper3x3, 1 );

    rb_define_method( rb_cRMtx4, "setZero", RMtx4_setZero, 0 );
    rb_define_method( rb_cRMtx4, "setIdentity", RMtx4_setIdentity, 0 );
    rb_define_method( rb_cRMtx4, "getDeterminant", RMtx4_getDeterminant, 0 );
    rb_define_method( rb_cRMtx4, "getTransposed", RMtx4_transpose, 0 );
    rb_define_method( rb_cRMtx4, "transpose!", RMtx4_transpose_intrusive, 0 );
    rb_define_method( rb_cRMtx4, "getInverse", RMtx4_inverse, 0 );
    rb_define_method( rb_cRMtx4, "invert!", RMtx4_inverse_intrusive, 0 );

    rb_define_method( rb_cRMtx4, "translation", RMtx4_translation, 3 );
    rb_define_method( rb_cRMtx4, "rotationX", RMtx4_rotationX, 1 );
    rb_define_method( rb_cRMtx4, "rotationY", RMtx4_rotationY, 1 );
    rb_define_method( rb_cRMtx4, "rotationZ", RMtx4_rotationZ, 1 );
    rb_define_method( rb_cRMtx4, "rotationAxis", RMtx4_rotationAxis, 2 );
    rb_define_method( rb_cRMtx4, "rotationQuaternion", RMtx4_rotationQuaternion, 1 );
    rb_define_method( rb_cRMtx4, "scaling", RMtx4_scaling, 3 );

    rb_define_method( rb_cRMtx4, "lookAtRH", RMtx4_lookAtRH, 3 );
    rb_define_method( rb_cRMtx4, "perspectiveRH", RMtx4_perspectiveRH, 4 );
    rb_define_method( rb_cRMtx4, "perspectiveFovRH", RMtx4_perspectiveFovRH, 4 );
    rb_define_method( rb_cRMtx4, "perspectiveOffCenterRH", RMtx4_perspectiveOffCenterRH, 6 );
    rb_define_method( rb_cRMtx4, "orthoRH", RMtx4_orthoRH, 4 );
    rb_define_method( rb_cRMtx4, "orthoOffCenterRH", RMtx4_orthoOffCenterRH, 6 );

    rb_define_method( rb_cRMtx4, "+@", RMtx4_op_unary_plus, 0 );
    rb_define_method( rb_cRMtx4, "-@", RMtx4_op_unary_minus, 0 );
    rb_define_method( rb_cRMtx4, "+",  RMtx4_op_binary_plus, 1 );
    rb_define_method( rb_cRMtx4, "-",  RMtx4_op_binary_minus, 1 );
    rb_define_method( rb_cRMtx4, "*",  RMtx4_op_binary_mult, 1 );
    rb_define_method( rb_cRMtx4, "==", RMtx4_op_binary_eq, 1 );
    rb_define_method( rb_cRMtx4, "add!", RMtx4_op_assign_plus, 1 );
    rb_define_method( rb_cRMtx4, "sub!", RMtx4_op_assign_minus, 1 );
    rb_define_method( rb_cRMtx4, "mul!", RMtx4_op_assign_mult, 1 );

    /********************************************************************************
     * RQuat
     ********************************************************************************/

    rb_define_alloc_func( rb_cRQuat, RQuat_allocate );

    rb_define_private_method( rb_cRQuat, "initialize", RQuat_initialize, -1 );
    rb_define_method( rb_cRQuat, "to_s", RQuat_to_s, 0 );
    rb_define_method( rb_cRQuat, "inspect", RQuat_inspect, 0 );
    rb_define_method( rb_cRQuat, "to_a", RQuat_to_a, 0 );
    rb_define_method( rb_cRQuat, "coerce", RQuat_coerce, 1 );

    rb_define_method( rb_cRQuat, "setElements", RQuat_setElements, 4 );
    rb_define_method( rb_cRQuat, "[]=", RQuat_setElement, 2 );
    rb_define_method( rb_cRQuat, "x=", RQuat_setX, 1 );
    rb_define_method( rb_cRQuat, "y=", RQuat_setY, 1 );
    rb_define_method( rb_cRQuat, "z=", RQuat_setZ, 1 );
    rb_define_method( rb_cRQuat, "w=", RQuat_setW, 1 );
    rb_define_method( rb_cRQuat, "xyz=", RQuat_setXYZ, 1 );

    rb_define_method( rb_cRQuat, "[]", RQuat_getElement, 1 );
    rb_define_method( rb_cRQuat, "x", RQuat_getX, 0 );
    rb_define_method( rb_cRQuat, "y", RQuat_getY, 0 );
    rb_define_method( rb_cRQuat, "z", RQuat_getZ, 0 );
    rb_define_method( rb_cRQuat, "w", RQuat_getW, 0 );
    rb_define_method( rb_cRQuat, "xyz", RQuat_getXYZ, 0 );

    rb_define_method( rb_cRQuat, "getLength", RQuat_getLength, 0 );
    rb_define_method( rb_cRQuat, "getLengthSq", RQuat_getLengthSq, 0 );
    rb_define_method( rb_cRQuat, "setIdentity", RQuat_setIdentity, 0 );
    rb_define_method( rb_cRQuat, "getConjugated", RQuat_conjugate, 0 );
    rb_define_method( rb_cRQuat, "conjugate!", RQuat_conjugate_intrusive, 0 );
    rb_define_method( rb_cRQuat, "getInverse", RQuat_inverse, 0 );
    rb_define_method( rb_cRQuat, "invert!", RQuat_inverse_intrusive, 0 );
    rb_define_method( rb_cRQuat, "getNormalized", RQuat_normalize, 0 );
    rb_define_method( rb_cRQuat, "normalize!", RQuat_normalize_intrusive, 0 );

    rb_define_method( rb_cRQuat, "+@", RQuat_op_unary_plus, 0 );
    rb_define_method( rb_cRQuat, "-@", RQuat_op_unary_minus, 0 );
    rb_define_method( rb_cRQuat, "+",  RQuat_op_binary_plus, 1 );
    rb_define_method( rb_cRQuat, "-",  RQuat_op_binary_minus, 1 );
    rb_define_method( rb_cRQuat, "*",  RQuat_op_binary_mult, 1 );
    rb_define_method( rb_cRQuat, "==", RQuat_op_binary_eq, 1 );
    rb_define_method( rb_cRQuat, "add!", RQuat_op_assign_plus, 1 );
    rb_define_method( rb_cRQuat, "sub!", RQuat_op_assign_minus, 1 );
    rb_define_method( rb_cRQuat, "mul!", RQuat_op_assign_mult, 1 );

    rb_define_method( rb_cRQuat, "rotationMatrix", RQuat_rotationMatrix, 1 );
    rb_define_method( rb_cRQuat, "rotationAxis", RQuat_rotationAxis, 2 );
    rb_define_method( rb_cRQuat, "toAxisAngle", RQuat_toAxisAngle, 0 );

    rb_define_singleton_method( rb_cRQuat, "dot", RQuat_dot, 2 );
    rb_define_singleton_method( rb_cRQuat, "slerp", RQuat_slerp, 3 );

    /********************************************************************************
     * RVec2
     ********************************************************************************/

    rb_define_alloc_func( rb_cRVec2, RVec2_allocate );

    rb_define_private_method( rb_cRVec2, "initialize", RVec2_initialize, -1 );
    rb_define_method( rb_cRVec2, "to_s", RVec2_to_s, 0 );
    rb_define_method( rb_cRVec2, "inspect", RVec2_inspect, 0 );
    rb_define_method( rb_cRVec2, "to_a", RVec2_to_a, 0 );
    rb_define_method( rb_cRVec2, "coerce", RVec2_coerce, 1 );

    rb_define_method( rb_cRVec2, "setElements", RVec2_setElements, 2 );
    rb_define_method( rb_cRVec2, "[]=", RVec2_setElement, 2 );
    rb_define_method( rb_cRVec2, "x=", RVec2_setX, 1 );
    rb_define_method( rb_cRVec2, "y=", RVec2_setY, 1 );

    rb_define_method( rb_cRVec2, "[]", RVec2_getElement, 1 );
    rb_define_method( rb_cRVec2, "x", RVec2_getX, 0 );
    rb_define_method( rb_cRVec2, "y", RVec2_getY, 0 );

    rb_define_method( rb_cRVec2, "getLength", RVec2_getLength, 0 );
    rb_define_method( rb_cRVec2, "getLengthSq", RVec2_getLengthSq, 0 );
    rb_define_method( rb_cRVec2, "getNormalized", RVec2_normalize, 0 );
    rb_define_method( rb_cRVec2, "normalize!", RVec2_normalize_intrusive, 0 );

    rb_define_method( rb_cRVec2, "+@", RVec2_op_unary_plus, 0 );
    rb_define_method( rb_cRVec2, "-@", RVec2_op_unary_minus, 0 );
    rb_define_method( rb_cRVec2, "+",  RVec2_op_binary_plus, 1 );
    rb_define_method( rb_cRVec2, "-",  RVec2_op_binary_minus, 1 );
    rb_define_method( rb_cRVec2, "*",  RVec2_op_binary_mult, 1 );
    rb_define_method( rb_cRVec2, "==", RVec2_op_binary_eq, 1 );
    rb_define_method( rb_cRVec2, "add!", RVec2_op_assign_plus, 1 );
    rb_define_method( rb_cRVec2, "sub!", RVec2_op_assign_minus, 1 );
    rb_define_method( rb_cRVec2, "mul!", RVec2_op_assign_mult, 1 );

    rb_define_singleton_method( rb_cRVec2, "dot", RVec2_dot, 2 );
    rb_define_singleton_method( rb_cRVec2, "cross", RVec2_cross, 2 );

    rb_define_method( rb_cRVec2, "transform", RVec2_transform, 1 );

    /********************************************************************************
     * RVec3
     ********************************************************************************/

    rb_define_alloc_func( rb_cRVec3, RVec3_allocate );

    rb_define_private_method( rb_cRVec3, "initialize", RVec3_initialize, -1 );
    rb_define_method( rb_cRVec3, "to_s", RVec3_to_s, 0 );
    rb_define_method( rb_cRVec3, "inspect", RVec3_inspect, 0 );
    rb_define_method( rb_cRVec3, "to_a", RVec3_to_a, 0 );
    rb_define_method( rb_cRVec3, "coerce", RVec3_coerce, 1 );

    rb_define_method( rb_cRVec3, "setElements", RVec3_setElements, 3 );
    rb_define_method( rb_cRVec3, "[]=", RVec3_setElement, 2 );
    rb_define_method( rb_cRVec3, "x=", RVec3_setX, 1 );
    rb_define_method( rb_cRVec3, "y=", RVec3_setY, 1 );
    rb_define_method( rb_cRVec3, "z=", RVec3_setZ, 1 );

    rb_define_method( rb_cRVec3, "[]", RVec3_getElement, 1 );
    rb_define_method( rb_cRVec3, "x", RVec3_getX, 0 );
    rb_define_method( rb_cRVec3, "y", RVec3_getY, 0 );
    rb_define_method( rb_cRVec3, "z", RVec3_getZ, 0 );

    rb_define_method( rb_cRVec3, "getLength", RVec3_getLength, 0 );
    rb_define_method( rb_cRVec3, "getLengthSq", RVec3_getLengthSq, 0 );
    rb_define_method( rb_cRVec3, "getNormalized", RVec3_normalize, 0 );
    rb_define_method( rb_cRVec3, "normalize!", RVec3_normalize_intrusive, 0 );

    rb_define_method( rb_cRVec3, "+@", RVec3_op_unary_plus, 0 );
    rb_define_method( rb_cRVec3, "-@", RVec3_op_unary_minus, 0 );
    rb_define_method( rb_cRVec3, "+",  RVec3_op_binary_plus, 1 );
    rb_define_method( rb_cRVec3, "-",  RVec3_op_binary_minus, 1 );
    rb_define_method( rb_cRVec3, "*",  RVec3_op_binary_mult, 1 );
    rb_define_method( rb_cRVec3, "==", RVec3_op_binary_eq, 1 );
    rb_define_method( rb_cRVec3, "add!", RVec3_op_assign_plus, 1 );
    rb_define_method( rb_cRVec3, "sub!", RVec3_op_assign_minus, 1 );
    rb_define_method( rb_cRVec3, "mul!", RVec3_op_assign_mult, 1 );

    rb_define_singleton_method( rb_cRVec3, "dot", RVec3_dot, 2 );
    rb_define_singleton_method( rb_cRVec3, "cross", RVec3_cross, 2 );

    rb_define_method( rb_cRVec3, "transform", RVec3_transform, 1 );
    rb_define_method( rb_cRVec3, "transformCoord", RVec3_transformCoord, 1 );
    rb_define_method( rb_cRVec3, "transformCoord!", RVec3_transformCoord_intrusive, 1 );
    rb_define_method( rb_cRVec3, "transformNormal", RVec3_transformNormal, 1 );
    rb_define_method( rb_cRVec3, "transformNormal!", RVec3_transformNormal_intrusive, 1 );
    rb_define_method( rb_cRVec3, "transformRS", RVec3_transformRS, 1 );
    rb_define_method( rb_cRVec3, "transformRS!", RVec3_transformRS_intrusive, 1 );
    rb_define_method( rb_cRVec3, "transformRSTransposed", RVec3_transformRSTransposed, 1 );
    rb_define_method( rb_cRVec3, "transformRSTransposed!", RVec3_transformRSTransposed_intrusive, 1 );
    rb_define_method( rb_cRVec3, "transformByQuaternion", RVec3_transformByQuaternion, 1 );
    rb_define_method( rb_cRVec3, "transformByQuaternion!", RVec3_transformByQuaternion_intrusive, 1 );

    /********************************************************************************
     * RVec4
     ********************************************************************************/

    rb_define_alloc_func( rb_cRVec4, RVec4_allocate );

    rb_define_private_method( rb_cRVec4, "initialize", RVec4_initialize, -1 );
    rb_define_method( rb_cRVec4, "to_s", RVec4_to_s, 0 );
    rb_define_method( rb_cRVec4, "inspect", RVec4_inspect, 0 );
    rb_define_method( rb_cRVec4, "to_a", RVec4_to_a, 0 );
    rb_define_method( rb_cRVec4, "coerce", RVec4_coerce, 1 );

    rb_define_method( rb_cRVec4, "setElements", RVec4_setElements, 4 );
    rb_define_method( rb_cRVec4, "[]=", RVec4_setElement, 2 );
    rb_define_method( rb_cRVec4, "x=", RVec4_setX, 1 );
    rb_define_method( rb_cRVec4, "y=", RVec4_setY, 1 );
    rb_define_method( rb_cRVec4, "z=", RVec4_setZ, 1 );
    rb_define_method( rb_cRVec4, "w=", RVec4_setW, 1 );
    rb_define_method( rb_cRVec4, "xyz=", RVec4_setXYZ, 1 );

    rb_define_method( rb_cRVec4, "[]", RVec4_getElement, 1 );
    rb_define_method( rb_cRVec4, "x", RVec4_getX, 0 );
    rb_define_method( rb_cRVec4, "y", RVec4_getY, 0 );
    rb_define_method( rb_cRVec4, "z", RVec4_getZ, 0 );
    rb_define_method( rb_cRVec4, "w", RVec4_getW, 0 );
    rb_define_method( rb_cRVec4, "xyz", RVec4_getXYZ, 0 );

    rb_define_method( rb_cRVec4, "getLength", RVec4_getLength, 0 );
    rb_define_method( rb_cRVec4, "getLengthSq", RVec4_getLengthSq, 0 );
    rb_define_method( rb_cRVec4, "getNormalized", RVec4_normalize, 0 );
    rb_define_method( rb_cRVec4, "normalize!", RVec4_normalize_intrusive, 0 );

    rb_define_method( rb_cRVec4, "+@", RVec4_op_unary_plus, 0 );
    rb_define_method( rb_cRVec4, "-@", RVec4_op_unary_minus, 0 );
    rb_define_method( rb_cRVec4, "+",  RVec4_op_binary_plus, 1 );
    rb_define_method( rb_cRVec4, "-",  RVec4_op_binary_minus, 1 );
    rb_define_method( rb_cRVec4, "*",  RVec4_op_binary_mult, 1 );
    rb_define_method( rb_cRVec4, "==", RVec4_op_binary_eq, 1 );
    rb_define_method( rb_cRVec4, "add!", RVec4_op_assign_plus, 1 );
    rb_define_method( rb_cRVec4, "sub!", RVec4_op_assign_minus, 1 );
    rb_define_method( rb_cRVec4, "mul!", RVec4_op_assign_mult, 1 );

    rb_define_singleton_method( rb_cRVec4, "dot", RVec4_dot, 2 );

    rb_define_method( rb_cRVec4, "transform", RVec4_transform, 1 );
    rb_define_method( rb_cRVec4, "transform!", RVec4_transform_intrusive, 1 );
    rb_define_method( rb_cRVec4, "transformTransposed", RVec4_transformTransposed, 1 );
    rb_define_method( rb_cRVec4, "transformTransposed!", RVec4_transformTransposed_intrusive, 1 );
}

#ifdef __cplusplus
}
#endif

/*
RMath : Ruby math module for 3D Applications
Copyright (c) 2008- vaiorabbit  <http://twitter.com/vaiorabbit>

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
