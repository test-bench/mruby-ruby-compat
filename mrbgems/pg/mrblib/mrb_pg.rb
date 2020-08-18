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

      convert(typname, value)
    end

    def name(value)
      value
    end
  end

  class BasicTypeMapForResults < TypeMap
    DATE_PATTERN = %r{(?<year>[[:digit:]]{4})-(?<month>[[:digit:]]{1,2})-(?<day>[[:digit:]]{1,2})}

    TIMESTAMP_HMS_PATTERN = %r{(?<hour>[[:digit:]]{1,2}):(?<minute>[[:digit:]]{1,2}):(?<second>[[:digit:]]{1,2})}
    TIMESTAMP_USEC_PATTERN = %r{(?:\.(?<usec>[[:digit:]]+))?}
    TIMESTAMP_PATTERN = %r{^#{DATE_PATTERN} #{TIMESTAMP_HMS_PATTERN}#{TIMESTAMP_USEC_PATTERN}}

    TIMESTAMP_TZ_PATTERN = %r{#{TIMESTAMP_PATTERN}(?<offset>-?[[:digit:]]{1,2})}

    def convert(typename, str)
      case typename
      when :int2, :int4, :int8
        str.to_i
      when :numeric, :float4, :float8
        str.to_f
      when :date
        match_data = DATE_PATTERN.match(str)

        if match_data.nil?
          raise TypeError, "Malformed PG date (Value: #{str.inspect})"
        end

        year = match_data[:year].to_i
        month = match_data[:month].to_i
        day = match_data[:day].to_i

        Time.local(year, month, day)

      when :timestamp
        match_data = TIMESTAMP_PATTERN.match(str)

        if match_data.nil?
          raise TypeError, "Malformed PG timestamp (Value: #{str.inspect})"
        end

        year = match_data[:year].to_i
        month = match_data[:month].to_i
        day = match_data[:day].to_i

        hour = match_data[:hour].to_i
        minute = match_data[:minute].to_i
        second = match_data[:second].to_i

        usec = match_data[:usec].to_i

        Time.local(year, month, day, hour, minute, second, usec)

      when :timestamptz
        match_data = TIMESTAMP_TZ_PATTERN.match(str)

        if match_data.nil?
          raise TypeError, "Malformed PG timestamptz (Value: #{str.inspect})"
        end

        year = match_data[:year].to_i
        month = match_data[:month].to_i
        day = match_data[:day].to_i

        hour = match_data[:hour].to_i
        minute = match_data[:minute].to_i
        second = match_data[:second].to_i

        usec = match_data[:usec].to_i

        offset_hours = match_data[:offset].to_i
        offset_seconds = offset_hours * (60*60)

        time = Time.local(year, month, day, hour, minute, second, usec)

        unless time.utc_offset == offset_seconds
          raise ArgumentError, "Only local and UTC timezones are supported (Timestamp: #{str.inspect})"
        end

        time

      when :bool
        if str == 'f'
          false
        elsif str == 't'
          true
        else
          raise TypeError, "Malformed PG bool (Value: #{str.inspect})"
        end

      when :json, :jsonb
        JSON.parse(str)

      else
        str
      end
    end
  end

  NumericValueOutOfRange = Error
  StringDataRightTruncation = Error
  InvalidTextRepresentation = Error
end
