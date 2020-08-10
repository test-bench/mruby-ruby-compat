MRuby::Gem::Specification.new('mruby-ruby-compat') do |spec|
  spec.licenses = ["MIT"]
  spec.authors = ["Nathan Ladd"]

  spec.summary = "Backfills support for some ruby affordances used by typical TestBench projects"
  spec.homepage = "https://github.com/test-bench/mruby-ruby-compat"

  spec.bins = ['mruby-require']

  spec.add_dependency 'mruby-metaprog'
  spec.add_dependency 'mruby-env'
end
