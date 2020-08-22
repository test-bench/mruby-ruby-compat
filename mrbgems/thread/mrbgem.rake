MRuby::Gem::Specification.new('mruby-thread') do |spec|
  spec.license = 'MIT'
  spec.authors = ['mattn', 'Nathan Ladd']

  if build.toolchains.include?('androideabi')
    spec.cc.defines << 'HAVE_PTHREADS'
  else
    spec.linker.flags << '-pthread'
  end

  spec.add_dependency 'mruby-require', :github => 'test-bench/mruby-ruby-compat', :path => 'mrbgems/require'
end
