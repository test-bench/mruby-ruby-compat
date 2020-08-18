assert('MDB expected version') do
  @conn = PG::Connection.new('dbname=message_store user=message_store')

  assert_equal(PG::CONNECTION_OK, @conn.status)

  sql = "SELECT write_message($1::varchar, $2::varchar, $3::varchar, $4::jsonb, $5::jsonb, $6::bigint);"

  params = [
    SecureRandom.uuid,
    "test#{SecureRandom.hex(10)}-#{SecureRandom.uuid}",
    "SomeType",
    "{\"attribute\":\"9b1985ada9d5b99e6f09322e84a42625\"}",
    "{\"metaAttribute\":\"a562eb29193ecc29d00cd913dc2bd678\"}",
    111
  ]

  begin
    @conn.transaction do
      @conn.exec(sql, params)
    end
  rescue PG::RaiseException => e
  end

  assert_not_nil(e)
  assert_true(e.to_s.include?("Wrong expected version: 111"))
end
