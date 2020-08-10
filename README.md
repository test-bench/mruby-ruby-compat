# Test Bench MRuby support

## Installation

Download the MRuby project's source code:

    ./download-mruby.sh

Compile MRuby with `mruby-local`:

    ./compile-mruby.sh

Install:

    ./install.sh

Uninstall:

    ./uninstall.sh

## Incompatibilities / Notes

- **Time.new** - The final two arguments to `Time.new` differ between MRI and mruby:
  - In MRI, the penultimate argument is a decimal value for seconds (e.g. `1.11` means one second and 11 milliseconds), and the final argument is the time zone offset.
  - In mruby, the penultimate argument is an integer value for seconds, and the final argument is an integer value for the number of microseconds. In the above example of `1.11`, you'd pass `1` and `110000` as the final two arguments to mruby.

- **Standard libraries** - The mruby equivalent of Ruby's standard libraries cannot be required; they must be compiled in to mruby itself. This means e.g. `require 'json'` must be "guarded" by a check for mruby, e.g. `require 'json' unless RUBY_ENGINE == 'mruby'`. This also applies to `require 'test_bench'` and `require 'test_bench/fixture'`, as they are compiled in to MRuby, too.

- **defined?** - `defined?` does not work on mruby, which means e.g. `defined?(Foo::Bar)` must be written as `Object.const_defined?(:Foo) && Foo.const_defined?(:Bar)`

- **Backtrace locations** - In MRI, both call stacks and backtraces (which are just call stacks stored in an exception) come in two variants, `caller`/`backtrace` (which return an array of strings) and `caller_locations`/`backtrace_locations` (which return an array of `Thread::Backtrace::Location` objects). MRuby only has `caller` and `backtrace`, not `caller_locations` and `backtrace_locations`

- **Struct instance variables** - Struct classes cannot have attributes like `attr_accessor`, since mruby does not allow instance variables on core classes.

- **Heredoc** - Heredoc support in mruby is limited to `<<TEXT`; `<<~TEXT` is not supported

- **Exit** Kernel's `#exit` in MRuby does not raise a SystemExit exception, `raise SystemExit.new(1)` must be used

- **Warn** Kernel's `#warn` is not available in MRuby

- **StringIO puts** StringIO's `#puts` does not print a newline character when no arguments are given (it does in MRI)

- **Tempfile directory** Tempfiles in MRuby must have a directory specified when a filename is given. Supplying a directory in this case doesn't cause a problem for MRI

- **Array * method** In MRI, `Array#*` is a multi purpose method: `[0] * 3` is `[0, 0, 0]`, and `['a', 'b', 'c'] * '+'` is `"a+b+c"`. The method isn't in MRuby, and backfilling support would be more trouble than it's worth, as all the different purposes of the method would have to be ported.
