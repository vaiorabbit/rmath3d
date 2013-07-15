gem 'rmath3d'
# require 'rmath3d/rmath3d_plain'
require 'rmath3d/rmath3d'
include RMath3D

99999.times do
  v = RVec3.new( rand(), rand(), rand() )
  m = RMtx3.new.rotationX( Math::PI/4.0 )
  p v
  v2 = v.transformRS( m )
  p v2
end
