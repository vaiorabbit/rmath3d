# require 'rmath3d/rmath3d_plain'
require 'rmath3d/rmath3d'
include RMath3D

require 'glfw'
require 'opengl'

include GLFW
include OpenGL

case OpenGL.get_platform
when :OPENGL_PLATFORM_WINDOWS
  OpenGL.load_dll('opengl32.dll', 'C:/Windows/System32')
  GLFW.load_dll('glfw3.dll', '.')
when :OPENGL_PLATFORM_MACOSX
  OpenGL.load_dll('libGL.dylib', '/System/Library/Frameworks/OpenGL.framework/Libraries')
  GLFW.load_dll('libglfw.dylib', '.')
else
  raise RuntimeError, "Unsupported platform."
end


def drawCube
  size = 1.0
  scale = 0.2
  delta = 0.1

  v = [
       [ size,  size,  size * scale + delta ], 
       [ size,  size, -size * scale + delta ], 
       [ size, -size, -size * scale ], 
       [ size, -size,  size * scale ],
       [-size,  size,  size * scale + delta ],
       [-size,  size, -size * scale + delta ],
       [-size, -size, -size * scale ],
       [-size, -size,  size * scale ]
      ]

  cube = [
          [ [1,0,0], v[3],v[2],v[1],v[0] ], # normal, vertices
          [ [-1,0,0], v[6],v[7],v[4],v[5] ],
          [ [0,0,-1], v[2],v[6],v[5],v[1] ],
          [ [0,0,1], v[7],v[3],v[0],v[4] ],
          [ [0,1,0], v[4],v[0],v[1],v[5] ],
          [ [0,-1,0], v[6],v[2],v[3],v[7] ]
         ]

  glBegin(GL_QUADS)
  cube.each do |side|
    glNormal3fv(side[0].pack('F*'))

    glTexCoord2f(1,1)
    glVertex3fv(side[1].pack('F*'))
    glTexCoord2f(0,1)
    glVertex3fv(side[2].pack('F*'))
    glTexCoord2f(0,0)
    glVertex3fv(side[3].pack('F*'))
    glTexCoord2f(1,0)
    glVertex3fv(side[4].pack('F*'))
  end
  glEnd()
end


class App

  def draw
    glPushAttrib( GL_ALL_ATTRIB_BITS )

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT )

    glMatrixMode( GL_MODELVIEW )
    glLoadMatrixf( @mtxLookAt.to_a.pack('F16') )

    glLightfv( GL_LIGHT0, GL_POSITION, @light_pos.pack('F*') )
    glLightfv( GL_LIGHT0, GL_DIFFUSE,  @light_diffuse.pack('F*') )
    glLightfv( GL_LIGHT0, GL_SPECULAR, @light_specular.pack('F*') )
    glLightfv( GL_LIGHT0, GL_AMBIENT,  @light_ambient.pack('F*') )

    glPushMatrix()

    @mtxTeapotRotY.rotationY( @teapot_rad_y )
    glMultMatrixf( @mtxTeapotRotY.to_a.pack('F16') )
    @teapot_rad_y += 2.0*Math::PI/60.0

    glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE,   @teapot_diffuse.pack('F*') )
    glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR,  @teapot_specular.pack('F*') )
    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT,   @teapot_ambient.pack('F*') )
    glMaterialf(  GL_FRONT_AND_BACK, GL_SHININESS, @teapot_shininess )
    drawCube() # glutSolidTeapot( 2.0 )

    glPopMatrix()

    glPopAttrib()
  end

  def key_callback( window_handle, key, scancode, action, mods )
    case key
    when GLFW_KEY_ESCAPE, GLFW_KEY_Q
      glfwSetWindowShouldClose(window_handle, 1)
    end
  end

  def size_callback( window_handle, w, h )
    glViewport( 0, 0, w, h )
    glMatrixMode( GL_PROJECTION )
    @mtxProj.perspectiveFovRH( 30.0*Math::PI/180.0, w.to_f/h.to_f, 0.1, 1000.0, true )
    glLoadMatrixf( @mtxProj.to_a.pack('F16') )

    @window_width  = w
    @window_height = h
  end

  def initialize
    @window_width  = 320
    @window_height = 240

    @eye = RVec3.new(0.0, 15.0, 15.0)
    @at  = RVec3.new(0.0,  0.0,  0.0)
    @up  = RVec3.new(0.0,  1.0,  0.0)
    @mtxLookAt = RMtx4.new.lookAtRH( @eye, @at, @up )
    @mtxProj = RMtx4.new.perspectiveFovRH( 30.0*Math::PI/180.0, @window_width.to_f/@window_height.to_f, 0.1, 1000.0, true )

    @light_pos = [2.5,0,5,1]
    @light_diffuse  = [1,1,1,1]
    @light_specular = [1,1,1,1]
    @light_ambient  = [1,1,1,1]

    @teapot_diffuse = [0.8,1,0,1]
    @teapot_specular = [1,1,1,1]
    @teapot_ambient = [0.2,0.2,0,1]
    @teapot_shininess = 32.0

    @teapot_rad_y = 0.0
    @mtxTeapotRotY = RMtx4.new.rotationY( @teapot_rad_y )

    glfwInit()

    @window_handle = glfwCreateWindow( @window_width, @window_height, self.class.to_s, nil, nil )
    glfwMakeContextCurrent( @window_handle )
    # glfwSetKeyCallback( $window_handle, $key_callback )
    glfwSetKeyCallback( @window_handle, GLFW::create_callback(:GLFWkeyfun, method(:key_callback).to_proc) )
    # glfwSetWindowSizeCallback( $window_handle, $size_callback )
    glfwSetWindowSizeCallback( @window_handle, GLFW::create_callback(:GLFWwindowsizefun, method(:size_callback).to_proc ) )

    width_ptr = '    '
    height_ptr = '    '
    glfwGetFramebufferSize(@window_handle, width_ptr, height_ptr)
    width = width_ptr.unpack('L')[0]
    height = height_ptr.unpack('L')[0]
    size_callback( @window_handle, width, height )

    # Common rendering state : reused by PushAttrib/PopAttrib
    glEnable( GL_DEPTH_TEST )
    glEnable( GL_LIGHTING )
    glEnable( GL_LIGHT0 )
    glEnable( GL_NORMALIZE )
    glClearColor( 0.0, 0.0, 0.0, 1 )
  end

  def start
    while glfwWindowShouldClose( @window_handle ) == 0
      draw()
      glfwSwapBuffers( @window_handle )
      glfwPollEvents()
    end

    glfwDestroyWindow( @window_handle )
    glfwTerminate()
  end

end

App.new.start
