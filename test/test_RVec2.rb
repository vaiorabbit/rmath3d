require 'minitest/autorun'
class TC_RVec2 < Minitest::Test
  def setup
    @tolerance = RMath3D::TOLERANCE
    @zero = RVec2.new( 0, 0 )
    @ax   = RVec2.new( 1, 0 )
    @ay   = RVec2.new( 0, 1 )
  end

  def teardown
  end

  def test_initialize
    v0 = RVec2.new
    assert_in_delta( 0, v0.x, @tolerance )
    assert_in_delta( 0, v0.y, @tolerance )

    v1 = RVec2.new( 1, 2 )
    assert_in_delta( 1, v1.x, @tolerance )
    assert_in_delta( 2, v1.y, @tolerance )

    v2 = RVec2.new( v1 )
    assert_in_delta( 1, v2.x, @tolerance )
    assert_in_delta( 2, v2.y, @tolerance )
  end

  def test_to_s
    assert_respond_to( @zero, :to_s )
  end

  def test_coerce
    assert_respond_to( @zero, :coerce )
  end

  def test_setElements
    v = RVec2.new
    v.setElements( 1, 2 )
    assert_in_delta( 1, v.x, @tolerance )
    assert_in_delta( 2, v.y, @tolerance )
  end

  def test_setElement
    v = RVec2.new

    # x=
    v.x = 1
    assert_in_delta( 1, v.x, @tolerance )
    v[0] = 2
    assert_in_delta( 2, v.x, @tolerance )

    # y=
    v.y = 1
    assert_in_delta( 1, v.y, @tolerance )
    v[1] = 2
    assert_in_delta( 2, v.y, @tolerance )
  end

  def test_getElement
    assert_in_delta( 1, @ax[0], @tolerance )
    assert_in_delta( 1, @ay[1], @tolerance )

    assert_in_delta( 1, @ax.x, @tolerance )
    assert_in_delta( 1, @ay.y, @tolerance )
  end

  def test_getLength
    len_sq = 1.0*1.0 + 2.0*2.0 # == 5.0
    len = Math.sqrt( len_sq ) # == 2.23606797749979

    v = RVec2.new( 1, 2 )
    assert_in_delta( len_sq, v.getLengthSq, @tolerance )
    assert_in_delta( len   , v.getLength  , @tolerance )
  end

  def test_normalize
    len_sq = 1.0*1.0 + 2.0*2.0 # == 5.0
    len = Math.sqrt( len_sq ) # == 2.23606797749979
    x = 1.0 / len
    y = 2.0 / len
    v = RVec2.new( 1, 2 )
    # getNormalized
    v0 = v.getNormalized
    assert_in_delta( x, v0.x, @tolerance )
    assert_in_delta( y, v0.y, @tolerance )

    # normalize!
    v.normalize!
    assert_in_delta( x, v.x, @tolerance )
    assert_in_delta( y, v.y, @tolerance )
  end

  def test_unary_operators
    v = RVec2.new( 1, 2 )

    # RVec2#+@
    vp = +v
    assert_in_delta( 1, vp.x, @tolerance )
    assert_in_delta( 2, vp.y, @tolerance )

    # RVec2#-@
    vm = -v
    assert_in_delta( -1, vm.x, @tolerance )
    assert_in_delta( -2, vm.y, @tolerance )
  end

  def test_plus_operations
    v0 = RVec2.new( 1, 1 )
    v1 = RVec2.new( 2, 2 )

    # RVec2#+
    vr = v0 + v1
    assert_in_delta( 3, vr.x, @tolerance )
    assert_in_delta( 3, vr.y, @tolerance )

    # RVec2#add!
    v0.add!( v1 )
    assert_in_delta( 3, v0.x, @tolerance )
    assert_in_delta( 3, v0.y, @tolerance )

    assert_raises( TypeError ) { v0 + 1.0 }
    assert_raises( TypeError ) { 1.0 + v0 }
  end

  def test_minus_operations
    v0 = RVec2.new( 1, 1 )
    v1 = RVec2.new( 2, 2 )

    # RVec2#-
    vr = v0 - v1
    assert_in_delta( -1, vr.x, @tolerance )
    assert_in_delta( -1, vr.y, @tolerance )

    # RVec2#sub!
    v0.sub!( v1 )
    assert_in_delta( -1, v0.x, @tolerance )
    assert_in_delta( -1, v0.y, @tolerance )

    assert_raises( TypeError ) { v0 - 1.0 }
    assert_raises( TypeError ) { 1.0 - v0 }
  end

  def test_mult_operations
    v0 = RVec2.new( 1, 1 )

    vr = v0 * 2.0
    assert_in_delta( 2.0, vr.x, @tolerance )
    assert_in_delta( 2.0, vr.y, @tolerance )

    vr = 2.0 * v0
    assert_in_delta( 2.0, vr.x, @tolerance )
    assert_in_delta( 2.0, vr.y, @tolerance )

    v0.mul!( 2.0 )
    assert_in_delta( 2.0, v0.x, @tolerance )
    assert_in_delta( 2.0, v0.y, @tolerance )

    assert_raises( TypeError ) { v0 * @zero }
  end

  def test_equality_operations
    v = RVec2.new
    assert( v == @zero )
    assert( v != @ax )
  end

  def test_dot
    v0 = RVec2.new( 1, 1 )
    v1 = RVec2.new( 2, 2 )
    assert_in_delta( 4.0, RVec2.dot(v0, v1), @tolerance )
    assert_in_delta( 0.0, RVec2.dot(@ax, @ay), @tolerance )
  end

  def test_cross
    c = RVec2.cross( @ax, @ay )
    assert_in_delta( c, 1.0, @tolerance )
  end

  def test_transform
    m = RMtx2.new.rotation( Math::PI/4.0 )
    va = RVec2.new( Math.sqrt(2)/2, Math.sqrt(2)/2 )

    vr = @ax.transform( m )
    assert_kind_of( RVec2, vr )
    assert_in_delta( va.x, vr.x, @tolerance )
    assert_in_delta( va.y, vr.y, @tolerance )
  end
end
