# Eventide MRuby Progress

## Libraries (Current Progress)

Legend:
- :heavy_check_mark: Master branch of library already passes under MRuby
- :white_check_mark: Changes had to be made to the test suite in order to pass, but library is usable under MRuby without symlinking
- :heavy_plus_sign: The project's principal interface works under MRuby, but some non-conventional use cases aren't supported
- :link: The project required changes in order to work under MRuby, the `mruby-compatibility` branch must be used via symlinking under MRuby
- :x: MRuby cannot be made compatible with the library at all
- :grey_question: Library has not yet been assessed for MRuby compatibility

All libraries with a :white_check_mark: or a :link: have a `mruby-compatibility` branch push to github.com/eventide-project.

### Core / Utility

- :heavy_check_mark: async-invocation
- :heavy_check_mark: attribute
- :link: casing ()
  - Symbol is converted to string before calling String#[], which MRuby does not have
- :heavy_check_mark: clock (tests require changes for MRuby support)
- :heavy_check_mark: collection
- :heavy_plus_sign: configure
  - The configure macro cannot be called a second time (which likely has never occurred in the wild)
- :heavy_check_mark: dependency
- :x: diagnostics-sample
  - Relies on Process::CLOCK_MONOTIC to sample time values
- :heavy_check_mark: identifier-uuid
- :heavy_plus_sign: :link: initializer
  - Use of class_eval is replaced with MRuby-compatible eval (does not work with anonymous classes)
- :x: invocation
  - Relies on `binding` to trap the arguments passed to the surrounding method, and MRuby cannot support `binding`
- :link: log
  - Symbol is converted to string before calling String#[], which MRuby does not have
  - Library mistakenly uses $INPUT_RECORD_SEPARATOR, which happens to be from English (and is not available under MRuby)
- :heavy_plus_sign: :link: mimic (uses eval which does not work for anonymous classes)
  - Mimic.preserved_methods is not mutated by a test, which causes a false negative in MRuby
  - Method missing presence is queried differently for MRuby
  - Invocation library is not actuated under MRuby, since it cannot be supported
  - Heredoc usage is consistent with MRuby's more limited heredoc capabilities
- :heavy_check_mark: poll
- :heavy_check_mark: reflect
- :heavy_check_mark: set_attributes
- :heavy_check_mark: validate
- :heavy_check_mark: retry
- :heavy_check_mark: schema
- :heavy_check_mark: set-attributes
- :heavy_check_mark: subst-attr
- :heavy_check_mark: telemetry
- :heavy_check_mark: terminal-colors
- :heavy_check_mark: transform
- :heavy_check_mark: try
- :heavy_check_mark: virtual
- :heavy_plus_sign: settings
  - Use of `Pathname` from Ruby standard library was replaced with equivalent code based on File
  - Minor adjustment to JSON parsing

### Messaging

- :heavy_check_mark: message-store
- :grey_question: messaging
- :grey_question: entity-cache
- :grey_question: entity-projection
- :grey_question: entity-store
- :grey_question: consumer
- :grey_question: message-store-postgres
- :grey_question: messaging-postgres
- :grey_question: consumer-postgres
- :grey_question: entity-snapshot-postgres
- :grey_question: entity-store-postgres-test

### Component Host

- :grey_question: actor
- :grey_question: component-host

### View Data

- :grey_question: view-data-commands
- :grey_question: view-data-pg

### Fixtures

- :grey_question: entity-projection-fixtures
- :grey_question: messaging-fixtures
- :grey_question: schema-fixtures

## A Note About Standard Libraries

The following standard libraries are included in one or more projects, and have to be compiled in to MRuby in order to run:

- English
- OpenStruct
- Pretty print
- SecureRandom
- StringIO
- Time

To ensure the require statement (i.e. `require "pp"`) functions, the libraries must be added to `spec.cc.flags` in `build_config.rb`:

```ruby
  conf.gem './mrbgems/require' do |spec|
    spec.cc.flags << %(-DMRUBY_REQUIRE_COMPILED_FEATURES='"English,ostruct,pp,securerandom,set,stringio,time"')
  end
```
