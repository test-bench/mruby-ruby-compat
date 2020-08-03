MRuby::Gem::Specification.new('mruby-local') do |spec|
  spec.licenses = ["MIT"]
  spec.authors = ["Nathan Ladd"]

  spec.summary = "Backfills mruby with support needed to load ruby projects"
  spec.homepage = "https://github.com/ntl/mruby-local"

  spec.bins = %w(mruby-local)

  # TODO: List the specific dependencies
  #spec.add_dependency('mruby-kernel-ext')

  spec.add_dependency('mruby-dir-glob')
  spec.add_dependency('mruby-env')
  spec.add_dependency('mruby-onig-regexp', :github => 'ntl/mruby-onig-regexp')
end
