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
- :heavy_plus_sign: :link: clock
  - `Controls::Time::Offset` may not have the same precision as MRI, since MRI can add Rational values to Time objects, and MRuby must convert them to Float first
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
- :heavy_check_mark: message-store-postgres
- :heavy_plus_sign: :link: messaging
  - Handler session configuration requires a workaround for a MRuby 2.x bug; MRuby 2.x cannot detect keyword argument parameters
  - The random number control (`MessageStore::Postgres::Controls::Random::Number`) requires a less terse approach under MRuby
  - `Messaging::Message.copy` exception code sets the backtrace of an exception; this needs to be done a special way under MRuby
- :heavy_plus_sign: :link: entity-cache
  - MRuby does not have a threading library that works like MRI's `Thread` class. Therefore, the thread-scoped internal store is disabled under MRuby, and the default scope is `exclusive`
  - Native threads / actors are still possible with the [ZeroMQ MRuby gem](https://github.com/zeromq/mruby-zmq)
- :white_check_mark: entity-projection
  - Some of the tests make use of control projection classes that are anonymous. EntityProjection has to define its initializer directly, without the `initializer` macro, since `initializer` under MRuby does not work with anonymous classes
- :heavy_check_mark: entity-store
- :heavy_check_mark: messaging-postgres
- :heavy_check_mark: entity-snapshot-postgres
- :heavy_check_mark: entity-store-postgres-test

### Component Host, Consumer

- :grey_question: actor
- :grey_question: component-host
- :grey_question: consumer
- :grey_question: consumer-postgres

### View Data

- :heavy_check_mark: view-data-commands
- :grey_question: view-data-pg

### Fixtures

- :heavy_check_mark: schema-fixtures
- :heavy_check_mark: entity-projection-fixtures
- :heavy_check_mark: messaging-fixtures

## A Note About Standard Libraries

The following standard libraries are included in one or more projects, and have to be compiled in to MRuby in order to run:

- English
- JSON
- OpenStruct
- Pretty print
- SecureRandom
- StringIO
- Tempfile / Tmpdir
- Time
- YAML

To ensure the require statement (i.e. `require "pp"`) functions, the libraries must be added to `spec.cc.flags` in `build_config.rb`:

```ruby
  conf.gem './mrbgems/require' do |spec|
    spec.cc.flags << %(-DMRUBY_REQUIRE_COMPILED_FEATURES='"English,ostruct,pp,securerandom,set,stringio,time"')
  end
```

## Threads, Fibers, RbActor

Threads and Ruby share a rather sordid relationship, and Matz has indicated that he regrets adding them to MRI. Under MRuby, Threads are not guaranteed to be available, and even when they are, they come with their own limitations (for instance, they crash if there are any non-primitive objects on the stack at the time a thread is spawned). It's probably best to avoid threads for MRuby support, and given the direction of Ruby 3 and RbActor, this is likely a good decision for Ruby implementations as well.

This has ramifications for two libraries: `entity-cache` and `actor`

### EntityCache

- EntityCache's internal (in-memory) store has three scopes: `exclusive`, `thread`, and `global`
  - `exclusive` - Effectively disables in-memory caching, since the consumer constructs a new handler for each new message it consumes
  - `thread` - Cache scope is per-consumer, since each consumer runs in its own thread
  - `global` - Cache is shared across all threads. Requires the entity to implement serialization, so that immutable copies can be stored in the cache

- Proposal #1: a `fiber` scope is added to `entity-cache`, and becomes the new default scope (at least under MRuby). This brings about greater portability, at the expense of not allowing multiple fibers to share a cache. Thus, if a handler implementation made use of multiple fibers, their entity stores would not share an in-memory cache.

- Proposal #2: a `global-unguarded` scope is added to `entity-cache` that does not serialize entities (and is thus not threadsafe). It would be safe to use if `fork` were used for process isolation instead of threads. A stable release of `RbActor` for both Ruby and MRuby would make `fork` less desirable, but the future of `RbActor` is not guaranteed until it is released with Ruby.

### Actor

The `actor` library, in its current form, cannot be made compatible with MRuby. Classes that include the `Actor` mixin spawn threads that then hold references to the outer class instance. This cannot be done with MRuby threads; because MRuby does not have a GVL/GIL, threads cannot share object references to Ruby objects that aren't Ruby core classes (like String, Symbol, etc.). Similarly, the `RbActor` library coming in Ruby 3 will allow `actor` to be implemented without threads altogether.

Actor should stop using threads to both isolate processes _and_ provide parallelism. Instead, it should use Fibers for process isolation, and either `fork` or `RbActor` for parallelism.

It's also worth calling into question whether the Actor computation model is overkill for its purpose; concurrent sequential processes (abbreviated "CSP") are similar to Actors, but do not put queues in between processes, which can consume arbitrary resources and can be more difficult to debug.
