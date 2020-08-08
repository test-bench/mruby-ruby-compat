MRuby::Gem::Specification.new('test-bench-mruby') do |spec|
  spec.licenses = ["MIT"]
  spec.authors = ["Nathan Ladd"]

  spec.summary = "Allows mruby to load ruby projects that use TestBench"
  spec.homepage = "https://github.com/test-bench/test-bench-mruby"

  spec.bins = %w(mruby-local)

  spec.add_dependency('mruby-kernel-ext')
  spec.add_dependency('mruby-metaprog')
  spec.add_dependency('mruby-time')

  spec.add_dependency('mruby-at_exit')
  spec.add_dependency('mruby-dir-glob')
  spec.add_dependency('mruby-env')
  spec.add_dependency('mruby-logger')
  spec.add_dependency('mruby-onig-regexp')
  spec.add_dependency('mruby-optparse')
  spec.add_dependency('mruby-ostruct')
  spec.add_dependency('mruby-secure-random')
  spec.add_dependency('mruby-stringio')
  spec.add_dependency('mruby-tempfile')
end
