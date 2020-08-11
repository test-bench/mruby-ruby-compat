MRuby::Gem::Specification.new('mruby-bundle-standalone') do |spec|
  spec.licenses = ["MIT"]
  spec.authors = ["Nathan Ladd"]

  spec.summary = "Supports loading local ruby gems installed with bundle --standalone"
  spec.homepage = "https://github.com/test-bench/mruby-ruby-compat"

  spec.add_dependency 'mruby-onig-regexp', :mgem => 'mruby-onig-regexp'
  spec.add_dependency 'mruby-dir-glob', :mgem => 'mruby-dir-glob'

  spec.add_dependency 'mruby-require', :github => 'test-bench/mruby-ruby-compat', :path => 'mrbgems/require'
end
