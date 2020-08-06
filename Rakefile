require 'pathname'

def project_name
  File.read('mrbgem.rake').each_line do |line|
    match_data = line.match(/MRuby::Gem::Specification.new\(['\"](?<project_name>.*?)['\"]\) do \|spec\|/)

    return match_data[:project_name] unless match_data.nil?
  end

  fail "Could not determine project name"
end

def mruby_root
  Pathname(ENV.fetch('MRUBY_ROOT'))
end

def mruby_config_file
  mruby_root
    .join("#{project_name.gsub('-', '_')}_config.rb")
    .to_s
end

ENV['MRUBY_ROOT'] ||= mruby_root
ENV['MRUBY_CONFIG'] ||= mruby_config_file

build_config_rb = <<~BUILD_CONFIG_RB
MRuby::Build.new do |conf|
  toolchain :gcc
  enable_debug
  conf.gembox 'full-core'
  conf.gem(#{__dir__.inspect})
end
BUILD_CONFIG_RB

unless File.size?(mruby_config_file) && File.read(mruby_config_file) == build_config_rb
  File.write(mruby_config_file, build_config_rb)
end

import mruby_root.join('Rakefile').to_s

task :copy_executables do
  mkdir_p 'bin'

  mruby_root.glob('bin/*').each do |bin|
    cp bin, 'bin'
  end
end

task :default => [:all, :copy_executables]
