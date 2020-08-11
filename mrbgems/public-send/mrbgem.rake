MRuby::Gem::Specification.new('mruby-public-send') do |spec|
  spec.licenses = ["MIT"]
  spec.authors = ["Nathan Ladd"]

  spec.summary = "public_send backfill (calls send)"
  spec.homepage = "https://github.com/test-bench/mruby-ruby-compat"

  spec.add_dependency 'mruby-metaprog'
end
