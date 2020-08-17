#include <mruby.h>
#include <mruby/string.h>

#include <string.h>

#include "symbol_key_conversion.h"

mrb_value
json_key_to_ruby_symbol(mrb_state* mrb, const char* const key) {
  mrb_value rb_symbol;
  const char* key_ptr = key;
  const int len = strlen(key);

  rb_symbol = mrb_str_new_capa(mrb, len);

  for(int index = 0; index < len; index++) {
    char chr = key[index];

    if(chr >= 'A' && chr <= 'Z') {
      mrb_str_cat(mrb, rb_symbol, key_ptr, index - (key_ptr - key));

      /* Converts A-Z to a-z */
      chr ^= 0x20;

      if(index > 0 && index < (len - 1)) {
        char prev_chr = key[index - 1];
        char next_chr = key[index + 1];

        if((prev_chr >= 'a' && prev_chr <= 'z') || (next_chr >= 'a' && next_chr <= 'z')) {
          mrb_str_cat(mrb, rb_symbol, "_", 1);
        }
      }
      mrb_str_cat(mrb, rb_symbol, &chr, 1);

      key_ptr = key + index + 1;
    }
  }

  mrb_str_cat(mrb, rb_symbol, key_ptr, len - (key_ptr - key));

  return mrb_str_intern(mrb, rb_symbol);
}

mrb_value
ruby_symbol_to_json_key(mrb_state* mrb, mrb_sym rb_symbol) {
  const char* symbol;
  const char* symbol_ptr;
  mrb_int len;
  mrb_value json_key;

  symbol = mrb_sym_name_len(mrb, rb_symbol, &len);

  symbol_ptr = symbol;

  json_key = mrb_str_new_capa(mrb, len + 2);

  mrb_str_cat(mrb, json_key, "\"", 1);

  for(mrb_int index = 1; index < (len - 1); index++) {
    const char prev_chr = symbol[index - 1];
    char next_chr = symbol[index + 1];

    if(symbol[index] == '_' && (prev_chr >= 'a' && prev_chr <= 'z') && (next_chr >= 'a' && next_chr <= 'z')) {
      mrb_str_cat(mrb, json_key, symbol_ptr, index - (symbol_ptr - symbol));

      /* Converts a-z to A-Z */
      next_chr ^= 0x20;

      mrb_str_cat(mrb, json_key, &next_chr, 1);

      symbol_ptr = symbol + index + 2;
    }
  }

  if((symbol_ptr - symbol) < len) {
    mrb_str_cat(mrb, json_key, symbol_ptr, len - (symbol_ptr - symbol));
  }

  mrb_str_cat(mrb, json_key, "\"", 1);

  return json_key;
}
