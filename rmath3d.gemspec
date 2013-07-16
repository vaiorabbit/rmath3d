# -*- encoding: utf-8 -*-
lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)

Gem::Specification.new do |gem|
  gem.name          = "rmath3d"
  gem.version       = "20130715"
  gem.authors       = ["vaiorabbit"]
  gem.email         = ["vaiorabbit@gmail.com"]
  gem.description   = %q{Ruby Math Module for 3D Applications}
  gem.summary       = %q{Ruby Math Module for 3D Applications}
  gem.homepage      = "https://github.com/vaiorabbit/rmath3d"

  gem.files         = `git ls-files`.split($/)
  gem.extensions    = ['ext/rmath3d/extconf.rb']
  gem.executables   = gem.files.grep(%r{^bin/}).map{ |f| File.basename(f) }
  gem.test_files    = gem.files.grep(%r{^(test|spec|features)/})
  gem.require_paths = ["lib"]
end
