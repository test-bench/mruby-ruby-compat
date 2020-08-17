MRuby::Gem::Specification.new('mruby-json-with-casing') do |spec|
  spec.licenses = ["MIT"]
  spec.authors = ["mattn", "Nathan Ladd"]

  spec.cc.defines << 'JSON_FIXED_NUMBER'

  spec.summary = "JSON parsing library, which supports converting to/from camel cased keys"
  spec.homepage = "https://github.com/test-bench/mruby-ruby-compat"

  spec.add_dependency 'mruby-metaprog'
  spec.add_dependency 'mruby-print'
end
