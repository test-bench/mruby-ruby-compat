MRuby::Gem::Specification.new('mruby-pg') do |spec|
  spec.license = 'MIT'
  spec.authors = ['Narihiro Nakamura', 'Nathan Ladd']
  spec.summary = 'postgresql mruby binding'

  # for debug
  # spec.cc.flags = ["-g3", "-std=gnu99", "-O0", "-Wall", "-Werror-implicit-function-declaration", "-Wdeclaration-after-statement"]

  spec.cc.include_paths << `pg_config --includedir`.chomp
  spec.linker.libraries << 'pq'

  spec.add_dependency 'mruby-io'
  spec.add_dependency 'mruby-secure-random'
  spec.add_dependency 'mruby-metaprog'
  spec.add_dependency 'mruby-numeric-ext'
  spec.add_dependency 'mruby-object-ext'

  spec.add_dependency 'mruby-onig-regexp'
  spec.add_dependency 'mruby-time-iso8601', :github => 'test-bench/mruby-ruby-compat', :path => 'mrbgems/time-iso8601'
  spec.add_dependency 'mruby-json-with-casing', :github => 'test-bench/mruby-ruby-compat', :path => 'mrbgems/json-with-casing'
end
