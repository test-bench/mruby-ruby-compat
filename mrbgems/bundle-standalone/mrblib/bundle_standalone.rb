module MRuby
  module RubyCompat
    module Bundler
      def self.setup_standalone_bundle(bundler_setup_file)
        Object.const_set(:RbConfig, RbConfig)

        root_dir = File.expand_path('../..', bundler_setup_file)

        versions = []

        Dir.glob(File.join(root_dir, 'ruby', '*')).each do |version_dir|
          version = File.basename(version_dir)

          versions << version
        end

        if versions.length > 1
          raise "Standalone bundle contains gems from multiple Ruby versions (Versions: #{versions.join(', ')})"
        elsif versions.length == 0
          raise "Standalone bundle does not contain any gems (Versions: #{versions.join(', ')})"
        end

        version = versions[0]

        mruby_dir = File.join(root_dir, RUBY_ENGINE)

        unless File.directory?(mruby_dir)
          Dir.mkdir(mruby_dir)
        end

        symlink_source = File.join(root_dir, 'ruby', version)
        symlink_target = File.join(root_dir, RUBY_ENGINE, MRUBY_VERSION)

        unless File.symlink?(symlink_target)
          File.symlink(symlink_source, symlink_target)
        end

        load bundler_setup_file
      end

      module RbConfig
        CONFIG = {
          'ruby_version' => MRUBY_VERSION
        }
      end
    end
  end
end

['bundle/bundler/setup.rb', 'gems/bundler/setup.rb'].each do |bundler_setup_file|
  if File.exist?(bundler_setup_file)
    MRuby::RubyCompat::Bundler.setup_standalone_bundle(bundler_setup_file)
  end
end
