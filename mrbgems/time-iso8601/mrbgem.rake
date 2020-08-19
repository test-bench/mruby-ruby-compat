MRuby::Gem::Specification.new('mruby-time-iso8601') do |spec|
  spec.licenses = ["MIT"]
  spec.authors = ["Nathan Ladd"]

  spec.summary = "Time#iso8601 backfill"
  spec.homepage = "https://github.com/test-bench/time-iso8601"

  spec.add_dependency 'mruby-time-strftime', :mgem => 'mruby-time-strftime'

  spec.add_dependency 'mruby-onig-regexp', :mgem => 'mruby-onig-regexp'
end
