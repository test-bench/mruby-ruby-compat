MRuby::Gem::Specification.new('mruby-ruby-compat') do |spec|
  spec.licenses = ["MIT"]
  spec.authors = ["Nathan Ladd"]

  spec.summary = "Backfills support for some ruby affordances used by typical TestBench projects"
  spec.homepage = "https://github.com/test-bench/mruby-ruby-compat"

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
