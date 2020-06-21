# begin 
#   require 'rmath3d/rmath3d'
# rescue LoadError
#   require 'rmath3d/rmath3d_plain'
# end
# require_relative '../ext/rmath3d/rmath3d'
 require_relative '../lib/rmath3d/rmath3d_plain'
include RMath3D

# Test::Unit
require 'minitest/autorun'

# Test cases
require_relative 'test_RVec2.rb'
require_relative 'test_RVec3.rb'
require_relative 'test_RVec4.rb'
require_relative 'test_RQuat.rb'
require_relative 'test_RMtx2.rb'
require_relative 'test_RMtx3.rb'
require_relative 'test_RMtx4.rb'
