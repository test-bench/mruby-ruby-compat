MRuby::Gem::Specification.new('mruby-local') do |spec|
  spec.licenses = ["MIT"]
  spec.authors = ["Nathan Ladd"]

  spec.summary = "Allows mruby to load ruby projects"
  spec.homepage = "https://github.com/ntl/mruby-local"

  spec.bins = %w(mruby-local)

  spec.add_dependency('mruby-kernel-ext')
end
