#include <stdio.h>
#include <string.h>
#include <malloc.h>
#define BUFFER_SIZE 256
#define NUMBER_OF_STATES 23
#define NUMBER_OF_SIGNAL 14
#define LEX_ERROR "lex error"

char buffer[BUFFER_SIZE];
int buffer_offset = 0;

char lex_val[BUFFER_SIZE];
int lex_length;

typedef enum {
    S0,
    S1,
    S2,
    S3,
    S4,
    S5,
    S6,
    S7,
    S8,
    S9,
    S10,
    S11,
    S12,
    S13,
    S14,
    S15,
    S16,
    S17,
    S18,
    S19,
    S20,
    S21,
    S_ERROR = -100
} state;

int transitions[NUMBER_OF_STATES][NUMBER_OF_SIGNAL] = {
    { S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR },
    { S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR },
    { S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR },
    { S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR },
    { S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR },
    { S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR },
    { S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR },
    { S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR },
    { S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR },
    { S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR },
    { S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR },
    { S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR },
    { S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR },
    { S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR },
};

typedef enum {
    DOT,
    COMMA,
    SPACE,
    OTHER,
    ENDF,
    LETTER,
    NUMBER_SIGNAL,
    S_LETTER,
    W_LETTER,
    I_LETTER,
    T_LETTER,
    C_LETTER,
    H_LETTER,
    F_LETTER
} input_signal;

typedef enum {
    IDENTIFICATOR,
    NUMBER_LEXEM,
    MATH_SIGN,
    LOGIC_SIGN,
    IF,
    SWITCH
} lexem;

void appendToLex(char symbol) {
    lex_val[lex_length] = symbol;
    lex_length++;
}

short isLetter(char symbol) {
    int integer_value = (int) symbol;
    return integer_value <= 90 && integer_value >= 65 || integer_value <= 122 && integer_value >= 97; 
}

short isNumber(char symbol) {
    int integer_value = (int) symbol;
    return integer_value <= 57 && integer_value >= 48;
}

void reset_lex(){
    memset(lex_val, 0, lex_length);
    lex_length = 0;
}

short isStateFinal(int state) {
    return state == S3 || state == S10 || state == S12 || state == S16 || state == S18 || state == S21;
}

char* state_machine() {
    reset_lex();
    
    state cur_state = S0;
    input_signal cur_input = recognize();

    while (cur_input != ENDF) {
        cur_state = transitions[cur_input][cur_state];
        
        if (cur_state == IDENTIFICATOR) 
            return "IDENTIFICATOR";
        if (cur_state == NUMBER_LEXEM) 
            return "NUMBER_LEXEM";
        if (cur_state == MATH_SIGN) 
            return "MATH_SIGN";
        if (cur_state == LOGIC_SIGN) 
            return "LOGIC_SIGN";
        if (cur_state == IF) 
            return "IF";
        if (cur_state == SWITCH) 
            return "SWITCH";
        
        if (cur_state == S_ERROR || cur_input == ENDF)
            return LEX_ERROR;

    }
}

int recognize(){

    if (buffer_offset > BUFFER_SIZE - 1)
        return ENDF;

    char letter = buffer[buffer_offset];
    buffer_offset++;
    
    input_signal result;

    appendToLex(letter);

    switch (letter) {
        case 's': return S_LETTER;
        case 'w': return W_LETTER;
        case 'i': return I_LETTER;
        case 't': return T_LETTER;
        case 'c': return C_LETTER;
        case 'h': return H_LETTER;
        case 'f': return F_LETTER;
        case '.': return DOT;
        case ',': return COMMA;
        case ' ': return SPACE;
        case '\0': return ENDF;
        
        default:
            if (isLetter(letter))
                return LETTER;
            else if (isNumber(letter))
                return NUMBER_SIGNAL;
            else
                return OTHER;
    }
}



int main() {
    printf("Hello world");

    scanf("%s", buffer);
    
    char* status = malloc(sizeof(char) * BUFFER_SIZE);
    
    while (strcmp(status, LEX_ERROR) != 0)
    {
        strcpy(status, state_machine());
        printf("%s: %s", lex_val, status);
    }
    
    return 0;
}