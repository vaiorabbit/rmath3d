# -*- encoding: utf-8 -*-
lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)

Gem::Specification.new do |gem|
  gem.name          = "rmath3d_plain"
  gem.version       = "1.0.1"
  gem.authors       = ["vaiorabbit"]
  gem.email         = ["vaiorabbit@gmail.com"]
  gem.summary       = %q{Ruby Math Module for 3D Applications}
  gem.homepage      = "https://github.com/vaiorabbit/rmath3d"
  gem.require_paths = ["lib"]
  gem.license       = "zlib/libpng"

  gem.test_files    = gem.files.grep(%r{^(test|spec|features)/})

  gem.description   = <<-DESC
Provides vector3/4, matrix3x3/4x4 and quaternion in plain Ruby form.
  DESC

  gem.files = 
    Dir.glob("lib/rmath3d/*.rb") +
    Dir.glob("sample/**/*.rb") +
    Dir.glob("test/*.rb") +
    ["ChangeLog", "LICENSE.txt", "README.md"]
end
