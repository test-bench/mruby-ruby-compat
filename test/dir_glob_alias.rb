assert('Dir glob alias (Dir["some/path"])' do
  control_files = Dir.glob('test/**/*.rb')
  assert(control_files.count > 0)

  compare_files = Dir['test/**/*.rb']

  assert(compare_files == control_files)
end
