toplevel_binding_receiver = TOPLEVEL_BINDING.receiver

puts <<TEXT
                     self: #{self.object_id.to_s(16)} (#{self.class})
TOPLEVEL_BINDING.receiver: #{toplevel_binding_receiver.object_id.to_s(16)} (#{toplevel_binding_receiver.class})

TEXT
