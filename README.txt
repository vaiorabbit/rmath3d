=            rmath3d : Ruby Math Module for 3D Applications

rmath3d is a math module for 3D game programming and computer graphics.

                                             Last Update: Jul 15, 2013
                                                   Since: Jul 20, 2008
                                                   by vaiorabbit
== Features

=== Supports frequently-used vector and matrix classes

* RMtx3 (3x3 matrix)
* RMtx4 (4x4 matrix)
* RQuat (Quaternion)
* RVec3 (3 element vector)
* RVec4 (4 element vector)

=== Two implementations that are interchangeable with each other

1. rmath3d.{so|bundle} : Ruby extension library for faster execution.
2. rmath3d_plain.rb    : Ruby implemantation for debugging use.


== Building rmath3d.{so|bundle}

=== From source package

  $ rake install

=== For mkmf users

  $ ruby extconf.rb
  $ make

For Windows users, type commands below via the "Visual Studio 200{5|8}
Command Prompt". This process requires "cl.exe", "link.exe and
"mt.exe" to be exist in the program PATH:

  X:\> ruby extconf.rb
  X:\> nmake

You might encounter the "MSC version unmatch: _MSC_VER: XXXX is
expected." error at the time compiling "rmath3d.c". See the
instruction in the extconf.rb to avoid this error.


=== Embedding manifest file (For Windows users)

Make sure that your rmath3d.so has "rmath3d.so.manifest" embedded into itself.
Otherwise, while using rmath3d.so in your application, you might come
across an error dialog saying:

  ---------------------------
  Microsoft Visual C++ Runtime Library
  ---------------------------
  Runtime Error!
  
  Program: d:\ruby\bin\ruby.exe
  
  R6034
  An application has made an attempt to load the C runtime library incorrectly.
  Please contact the application's support team for more information.
  
  ---------------------------
  OK   
  ---------------------------

To ensure the file status, try checking if the string "manifest" can
be found in the file:

  X:\> strings rmath3d.so | grep manifest

If you don't see any outputs, it's time to embed "rmath3d.so.manifest". 

  X:\> mt -manifest rmath3d.so.manifest -outputresource:rmath3d.so;2

or run the "embed_manifest.bat" instead. Then check again the status.

  X:\> strings rmath3d.so | grep manifest
  <assembly xmlns="urn:schemas-microsoft-com:asm.v1" manifestVersion="1.0">

  X:\> 

If you see some similar output like that, the manifest file
"rmath3d.so.manifest" is successfully embedded.


== About rmath3d_plain.rb

rmath3d_plain.rb is a pure Ruby version of rmath3d.so: all the functionality of
rmath3d.so is implemented with plain Ruby code.
It is possible to trace the bugs caused by rmath3d.so by replacing the code

  gem 'rmath3d'
  require 'rmath3d/rmath3d'

with

  gem 'rmath3d'
  require 'rmath3d/rmath3d_plain'

rmath3d_plain.rb performs more strict type checking, that might be some burden
for its execution speed. So don't expect the performance efficiency
when you use rmath3d_plain.rb instead of rmath3d.so.


== Notes

=== Collaboration with ruby-opengl

The instances of RMtx4 can be used directly as the arguments of the
ruby-opengl functions that accept matrices.

For example, this code snippet using RMath:

  eye = RVec3.new(0.0, 15.0, 15.0)
  at  = RVec3.new(0.0,  0.0,  0.0)
  up  = RVec3.new(0.0,  1.0,  0.0)
  mtxLookAt = RMtx4.new.lookAtRH( eye, at, up )
  glMatrixMode( GL_PROJECTION )
  glLoadMatrix( @mtxProj )
  
  mtxProj = RMtx4.new.perspectiveFovRH( 30.0*Math::PI/180.0, WINDOW_WIDTH/WINDOW_HEIGHT, 0.1, 1000.0 )
  glMatrixMode( GL_MODELVIEW )
  glLoadMatrix( @mtxLookAt )

has the same transformation effect with the OpenGL fixed pipeline:

  glMatrixMode( GL_PROJECTION )
  gluPerspective( 30.0*Math::PI/180.0, WINDOW_WIDTH/WINDOW_HEIGHT, 0.1, 1000.0 )

  glMatrixMode( GL_MODELVIEW )
  gluLookAt( 0.0,15.0,15.0, 0.0,0.0,0.0, 0.0,1.0,0.0 )


=== Compatibility with matrix.rb

Currently, there is no combenient way to convert between matrix.rb's
+Matrix+ and +RMtx{3|4}".

(The matrix.rb is a standard Ruby library that provides N-dimensional
+Vector+ class and "N x M" +Matrix+ class.)



== Credits

* vaiorabbit  <http://twitter.com/vaiorabbit>

 
== License

All source codes are available under the terms of the zlib/libpng license
(see LICENSE.txt).
