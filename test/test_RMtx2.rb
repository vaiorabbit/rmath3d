class TC_RMtx2 < Minitest::Test

  def setup
    @tolerance = RMath3D::TOLERANCE
    @mZero = RMtx2.new.setZero
    @mIdentity = RMtx2.new.setIdentity
  end

  def teardown
  end

  def test_RMtx_initialize
    m0 = RMtx2.new
    for r in 0...2 do
      for c in 0...2 do
        assert_equal( 0.0, m0.getElement(r,c) )
      end
    end

    m1 = RMtx2.new( 0, 1,
                    2, 3 )
    assert_equal( 0, m1.getElement(0,0) )
    assert_equal( 1, m1.getElement(0,1) )
    assert_equal( 2, m1.getElement(1,0) )
    assert_equal( 3, m1.getElement(1,1) )

    m2 = RMtx2.new( m1 )
    for r in 0...2 do
      for c in 0...2 do
        assert_equal( 2*r+c, m2.getElement(r,c) )
      end
    end
  end

  def test_to_s
    assert_respond_to( @mZero, :to_s )
  end

  def test_coerce
    assert_respond_to( @mZero, :coerce )
  end

  def test_setElements
    @mZero.setElements( 0, 1, 2, 3 )
    for r in 0...2 do
      for c in 0...2 do
        assert_equal( 2*r+c, @mZero.getElement(r,c) )
      end
    end
  end

  def test_setElement
    for r in 0...2 do
      for c in 0...2 do
        @mZero.setElement( r, c, 2*r+c )
      end
    end
    for r in 0...2 do
      for c in 0...2 do
        assert_equal( 2*r+c, @mZero.getElement(r,c) )
      end
    end

    for r in 0...2 do
      for c in 0...2 do
        @mZero[r, c] = 2*c+r
      end
    end
    for r in 0...2 do
      for c in 0...2 do
        assert_equal( 2*c+r, @mZero[r,c] )
      end
    end
  end

  def test_getElement
    assert_respond_to( @mIdentity, :getElement )
    for r in 0...2 do
      for c in 0...2 do
        e = @mIdentity.getElement( r, c )
        if ( r == c )
          assert_equal( 1.0, e )
        else
          assert_equal( 0.0, e )
        end
      end
    end

    for r in 0...2 do
      for c in 0...2 do
        e = @mIdentity[ r, c ]
        if ( r == c )
          assert_equal( 1.0, e )
        else
          assert_equal( 0.0, e )
        end
      end
    end

    mtx = RMtx2.new(1,2,
                    3,4)
    assert_equal( mtx.e00, 1 )
    assert_equal( mtx.e01, 2 )
    assert_equal( mtx.e10, 3 )
    assert_equal( mtx.e11, 4 )
  end

  def test_getRowColumn
    mtx = RMtx2.new(1,2,
                    3,4)

    v = mtx.getRow(0)
    assert_equal( v.x, 1 )
    assert_equal( v.y, 2 )

    v = mtx.getRow(1)
    assert_equal( v.x, 3 )
    assert_equal( v.y, 4 )

    v = mtx.getColumn(0)
    assert_equal( v.x, 1 )
    assert_equal( v.y, 3 )

    v = mtx.getColumn(1)
    assert_equal( v.x, 2 )
    assert_equal( v.y, 4 )
  end

  def test_setRowColumn
    mtx = RMtx2.new

    vr = [RVec2.new(1,2),RVec2.new(3,4)]
    mtx.setRow(vr[0],0)
    mtx.setRow(vr[1],1)
    assert_equal( mtx.e00, 1 )
    assert_equal( mtx.e01, 2 )
    assert_equal( mtx.e10, 3 )
    assert_equal( mtx.e11, 4 )

    vc = [RVec2.new(1,2),RVec2.new(3,4)]
    mtx.setColumn(vc[0],0)
    mtx.setColumn(vc[1],1)
    assert_equal( mtx.e00, 1 )
    assert_equal( mtx.e01, 3 )
    assert_equal( mtx.e10, 2 )
    assert_equal( mtx.e11, 4 )
  end

  def test_setZero
    m = RMtx2.new( 1, 2, 3, 4 )
    m.setZero
    for r in 0...2 do
      for c in 0...2 do
        assert_equal( 0.0, m.getElement( r, c ) )
      end
    end
  end

  def test_setIdentity
    m = RMtx2.new( 1, 2, 3, 4 )
    m.setIdentity
    for r in 0...2 do
      for c in 0...2 do
        e = @mIdentity.getElement( r, c )
        if ( r == c )
          assert_equal( 1.0, e )
        else
          assert_equal( 0.0, e )
        end
      end
    end
  end

  def test_getDeterminant
    m1 = RMtx2.new( 2, -3,
                    1,  3 )
    assert_equal( 9.0, m1.getDeterminant )
  end

  def test_transpose
    m0 = RMtx2.new( -2, -3,
                    -1,  3 )
    # RMtx2#getTransposed
    m1 = m0.getTransposed
    for r in 0...2 do
      for c in 0...2 do
        assert_equal( m0.getElement(c,r), m1.getElement(r,c) )
      end
    end

    # RMtx2#transpose!
    m0.transpose!
    for r in 0...2 do
      for c in 0...2 do
        assert_equal( m0.getElement(r,c), m1.getElement(r,c) )
      end
    end
  end

  def test_inverse
    # RMtx2#getInverse
    m0 = RMtx2.new( 1, 2,
                    3, 4 )

    m0inv = RMtx2.new(-2.0,  1.0,
                       1.5, -0.5 )
    m1 =  m0.getInverse
    for r in 0...2 do
      for c in 0...2 do
        assert_in_delta( m0inv.getElement(r,c), m1.getElement(r,c), @tolerance )
      end
    end

    # RMtx2#invert!
    m0.invert!

    for r in 0...2 do
      for c in 0...2 do
        assert_in_delta( m0inv.getElement(r,c), m0.getElement(r,c), @tolerance )
      end
    end
  end

  def test_rotation
    m0 = RMtx2.new(  Math::sqrt(2)/2, -Math::sqrt(2)/2,
                     Math::sqrt(2)/2,  Math::sqrt(2)/2 )
    m1 = RMtx2.new.rotation( Math::PI/4.0 )

    for r in 0...2 do
      for c in 0...2 do
        assert_in_delta( m0.getElement(r,c), m1.getElement(r,c), @tolerance )
      end
    end
  end

  def test_scaling
    m0 = RMtx2.new( 10.0,  0.0,
                     0.0, 20.0 )
    m1 = RMtx2.new.scaling( 10.0, 20.0 )
    for r in 0...2 do
      for c in 0...2 do
        assert_in_delta( m0.getElement(r,c), m1.getElement(r,c), @tolerance )
      end
    end
  end

  def test_unary_operators
    # RMtx2#+@
    m0 = RMtx2.new( 0, 1, 2, 3 )
    m1 = RMtx2.new( 0, 1, 2, 3 )
    m2 = +m0

    assert_same( m0, m2 )
    assert( m1 == m2 )

    for r in 0...2 do
      for c in 0...2 do
        assert_in_delta( 2*r+c, m2.getElement(r,c), @tolerance )
      end
    end

    # RMtx2#-@
    m2 = -m0
    for r in 0...2 do
      for c in 0...2 do
        assert_in_delta( m0.getElement(r,c), -m2.getElement(r,c), @tolerance )
      end
    end
  end

  def test_binary_plus
    m0 = RMtx2.new( 0, 1, 2, 3 )
    m1 = RMtx2.new( 9,10,11,12 )
    m2 = RMtx2.new( 9,11,13,15 )

    # RMtx2#+
    m3 = m0 + m1
    for r in 0...2 do
      for c in 0...2 do
        assert_in_delta( m2.getElement(r,c), m3.getElement(r,c), @tolerance )
      end
    end

    # RMtx2#add!
    m0.add!( m1 )
    for r in 0...2 do
      for c in 0...2 do
        assert_in_delta( m2.getElement(r,c), m0.getElement(r,c), @tolerance )
      end
    end
  end

  def test_binary_minus
    m0 = RMtx2.new( 0, 1, 2, 3 )
    m1 = RMtx2.new( 9,10,11,12 )
    m2 = RMtx2.new(-9,-9,-9,-9 )

    # RMtx2#-
    m3 = m0 - m1
    for r in 0...2 do
      for c in 0...2 do
        assert_in_delta( m2.getElement(r,c), m3.getElement(r,c), @tolerance )
      end
    end

    # RMtx2#sub!
    m0.sub!( m1 )
    for r in 0...2 do
      for c in 0...2 do
        assert_in_delta( m2.getElement(r,c), m0.getElement(r,c), @tolerance )
      end
    end
  end

  def test_binary_mult
    m0 = RMtx2.new( 0, 1, 2, 3 )
    m1 = RMtx2.new( 9,10,11,12 )
    m0x1 = RMtx2.new( 11, 12, 51, 56)
    m1x0 = RMtx2.new( 20, 39, 24, 47)

    # RMtx2#*
    m2 = m0 * m1
    for r in 0...2 do
      for c in 0...2 do
        assert_in_delta( m0x1.getElement(r,c), m2.getElement(r,c), @tolerance )
      end
    end

    m2 = m1 * m0
    for r in 0...2 do
      for c in 0...2 do
        assert_in_delta( m1x0.getElement(r,c), m2.getElement(r,c), @tolerance )
      end
    end

    # RMtx2#mul!
    m2 = RMtx2.new( m0 )
    m2.mul!( m1 )
    for r in 0...2 do
      for c in 0...2 do
        assert_in_delta( m0x1.getElement(r,c), m2.getElement(r,c), @tolerance )
      end
    end

    m2 = RMtx2.new( m1 )
    m2.mul!( m0 )
    for r in 0...2 do
      for c in 0...2 do
        assert_in_delta( m1x0.getElement(r,c), m2.getElement(r,c), @tolerance )
      end
    end
  end

  def test_equality_operators
    m0 = RMtx2.new( 0, 1, 2, 3 )
    m1 = RMtx2.new( 0, 1, 2, 3 )
    m2 = RMtx2.new( 9,10,11,12 )

    assert( m0 == m1 )
    assert( m0 != m2 )
  end

end
