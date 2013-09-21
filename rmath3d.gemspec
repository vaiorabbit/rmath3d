# -*- encoding: utf-8 -*-
lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)

Gem::Specification.new do |gem|
  gem.name          = "rmath3d"
  gem.version       = "1.0.1"
  gem.authors       = ["vaiorabbit"]
  gem.email         = ["vaiorabbit@gmail.com"]
  gem.summary       = %q{Ruby Math Module for 3D Applications}
  gem.homepage      = "https://github.com/vaiorabbit/rmath3d"
  gem.require_paths = ["lib"]
  gem.license       = "zlib/libpng"

  gem.extensions    = ['ext/rmath3d/extconf.rb']
  gem.executables   = gem.files.grep(%r{^bin/}).map{ |f| File.basename(f) }
  gem.test_files    = gem.files.grep(%r{^(test|spec|features)/})

  gem.description   = <<-DESC
Provides vector3/4, matrix3x3/4x4 and quaternion in C extension library form (and plain Ruby form with the same interface for debugging use).
Notice: This library provides native extension. You must setup development environment (or DevKit) before installation.
  DESC

  gem.files = 
    Dir.glob("ext/rmath3d/*.{c,h,rb}") +
    Dir.glob("lib/rmath3d/*.rb") +
    Dir.glob("sample/**/*.rb") +
    Dir.glob("test/*.rb") +
    ["ChangeLog", "LICENSE.txt", "README.md", "Rakefile", "Gemfile"]
end
