
void aes_add_round_key ( char * state, char * exp_key);

void aes_byte_sub ( char * state );
void aes_byte_sub_r (char * state );

void aes_shift_row ( char * state );
void aes_shift_row_r ( char * state );
  
void aes_mix_column ( char * state );
void aes_mix_column_r ( char* state );