module PG
  class Connection
    CONNECT_ARGUMENT_ORDER = %w[host port options tty dbname user password]

    attr_accessor :type_map_for_results
    attr_accessor :type_map_for_queries

    def self.open(*args)
      new(*args)
    end

    def self.quote_connstr(value)
      return "'" + value.to_s.gsub("'") {|m| '\\' + m } + "'"
    end

    def self.parse_connect_args(*args)
      return '' if args.empty?

      connopts = []

      # Handle an options hash first
      if args.last.is_a?(Hash)
        opthash = args.pop
        opthash.each do |key, val|
          connopts.push("#{key}=#{PG::Connection.quote_connstr(val)}")
        end
      end

      # Option string style
      if args.length == 1 && args.first.to_s.index('=')
        connopts.unshift(args.first)
      else
        args.each_with_index do |val, i|
          next unless val # Skip nil placeholders

          key = CONNECT_ARGUMENT_ORDER[i] or
            raise ArgumentError, "Extra positional parameter %d: %p" % [ i+1, val ]
          connopts.push("#{key}=#{PG::Connection.quote_connstr(val.to_s)}")
        end
      end

      return connopts.join(' ')
    end

    # call-seq:
    #    conn.transaction { |conn| ... } -> result of the block
    # 
    # Executes a +BEGIN+ at the start of the block,
    # and a +COMMIT+ at the end of the block, or
    # +ROLLBACK+ if any exception occurs.
    def transaction(&block)
      res = exec("BEGIN")
      res.check
      block_result = nil
      begin
        block_result = block.call
      rescue Exception
        res = exec("ROLLBACK")
        res.check
        return
      end
      res = exec("COMMIT")
      res.check
      return block_result
    end
  end

  class Result
    def each(&block)
      return to_enum :each unless block_given?

      begin
        idx, length = -1, self.length-1
        while idx < length and length <= self.length and length = self.length-1
          elm = self[idx += 1]
          unless elm
            if elm == nil and length >= self.length
              break
            end
          end
          block.call(elm)
        end
        self
      ensure
        clear
      end
    end
  end

  class TypeMap
    def initialize(connection)
      @connection = connection
    end

    def call(value, oid)
      @typname_cache ||= { 19 => :name }

      if @typname_cache.key?(oid)
        typname = @typname_cache[oid]
      else
        result = @connection.exec_params("SELECT typname FROM pg_type WHERE oid = $1", [oid])
        typname = result[0]['typname'].to_sym

        @typname_cache[oid] = typname
      end

      if respond_to?(typname)
        send(typname, value)
      else
        raise "Could not convert #{typname} (Value: #{value})"
        value
      end
    end

    def name(value)
      value
    end
  end

  class BasicTypeMapForResults < TypeMap
    def int2(str)
      str.to_i
    end

    def int4(str)
      str.to_i
    end

    def int8(str)
      str.to_i
    end

    def uuid(str)
      str
    end
  end

  NumericValueOutOfRange = Error
end
