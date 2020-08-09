#!/usr/bin/env ruby

require 'stringio'

path = ARGV.fetch(0) do
  fail "Must specifiy a file"
end

puts <<~TEXT

Replace Heredocs (Path: #{path})
= = =

TEXT

contents = File.read(path)

output = StringIO.new

label_pattern = %r{<<~(?<label>[A-Z_]+)}

indent_pattern = %r{^(?<indent>[[:blank:]]+)}

current_heredoc_label = nil

define_method(:heredoc?) do
  current_heredoc_label.nil? ? false : true
end

heredoc_contents = []
min_indent = Float::INFINITY

contents.each_line do |line|
  if not heredoc?
    match_data = label_pattern.match(line)

    line.gsub!(label_pattern) do |str|
      current_heredoc_label = label_pattern.match(str)[:label]

      "<<#{current_heredoc_label}"
    end

    output.puts line
  else
    heredoc_contents << line

    if %r{^[[:blank:]]*#{Regexp.escape(current_heredoc_label)}$}.match?(line)
      heredoc_contents.each do |line|
        line.gsub!(%r{^[[:blank:]]{0,#{min_indent}}}, '')

        output.puts line
      end

      current_heredoc_label = nil
      heredoc_contents.clear
      min_indent = Float::INFINITY
    elsif line.chomp.empty?
      next
    else
      indent = indent_pattern.match(line)[:indent].length

      min_indent = [indent, min_indent].min
    end
  end
end

File.write(path, output.string)
