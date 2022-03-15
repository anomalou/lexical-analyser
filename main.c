#include <stdio.h>
#include <string.h>
#include <malloc.h>
#define BUFFER_SIZE 256
#define NUMBER_OF_STATES 22
#define NUMBER_OF_SIGNAL 18
#define LEX_ERROR "lex error"
#define END_OF_FILE "end of file"

char buffer[BUFFER_SIZE];
int buffer_offset = 0;

char lex_val[BUFFER_SIZE];
int lex_length;

typedef enum {
    S0 = 0,
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
    S_ERROR
} state;

typedef enum {
    IDENTIFICATOR = 23,
    NUMBER_LEXEM,
    MATH_SIGN,
    LOGIC_SIGN,
    IF,
    SWITCH
} lexem;

typedef enum {
    DOT = 0,
    COMMA,
    SPACE,
    OTHER,
    ENDF,
    LETTER,
    NUMBER_SIGNAL,
    PLUS,
    MINUS,
    SCREAMER, //!
    EQUAL,
    S_LETTER,
    W_LETTER,
    I_LETTER,
    T_LETTER,
    C_LETTER,
    H_LETTER,
    F_LETTER
} input_signal;

int transitions[NUMBER_OF_SIGNAL][NUMBER_OF_STATES] = {
    //      0|       1|       2|       3|       4|       5|       6|       7|       8|       9|      10|      11|      12|      13|      14|      15|      16|      17|      18|      19|      20|      21|      
    { S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR,     S14, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR},//DOT
    { S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR,     S14, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR},//COMMA
    { S_ERROR, S_ERROR,      IF, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR,  SWITCH, S_ERROR, IDENTIFICATOR, S_ERROR, NUMBER_LEXEM, S_ERROR, NUMBER_LEXEM, S_ERROR, MATH_SIGN, S_ERROR, S_ERROR, LOGIC_SIGN, S_ERROR},//SPACE
    { S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR},//OTHER
    { S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR},//ENDF
    {     S11,     S11,     S11, S_ERROR,     S11,     S11,     S11,     S11,     S11,     S11, S_ERROR,     S11, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR},//LETTER
    {     S13, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR,     S11, S_ERROR,     S13,     S15,     S15, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR},//NUMBER_SIGNAL
    {     S17, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR},//PLUS
    {     S17, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR},//MINUS
    {     S19, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR},//SCREAMER
    {     S19, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR,     S20, S_ERROR, S_ERROR},//EQUAL
    {      S4,     S11,     S11, S_ERROR,     S11,     S11,     S11,     S11,     S11,     S11, S_ERROR,     S11, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR},//S_LETTER
    {     S11,     S11,     S11, S_ERROR,      S5,     S11,     S11,     S11,     S11,     S11, S_ERROR,     S11, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR},//W_LETTER
    {      S1,     S11,     S11, S_ERROR,     S11,      S6,     S11,     S11,     S11,     S11, S_ERROR,     S11, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR},//I_LETTER
    {     S11,     S11,     S11, S_ERROR,     S11,     S11,      S7,     S11,     S11,     S11, S_ERROR,     S11, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR},//T_LETTER
    {     S11,     S11,     S11, S_ERROR,     S11,     S11,     S11,      S8,     S11,     S11, S_ERROR,     S11, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR},//C_LETTER
    {     S11,     S11,     S11, S_ERROR,     S11,     S11,     S11,     S11,      S9,     S11, S_ERROR,     S11, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR},//H_LETTER
    {     S11,      S2,     S11, S_ERROR,     S11,     S11,     S11,     S11,     S11,     S11, S_ERROR,     S11, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR, S_ERROR},//F_LETTER
};

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
        case '+': return PLUS;
        case '-': return MINUS;
        case '!': return SCREAMER;
        case '=': return EQUAL;
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
        
        if (cur_state == S_ERROR)
            return LEX_ERROR;

        cur_input = recognize();
    }

    return END_OF_FILE;
}

int main() {
    printf("Hello world\n");

    gets(buffer);
    sprintf(buffer, "%s ", buffer);
    
    char* status = malloc(sizeof(char) * BUFFER_SIZE);
    
    while (strcmp(status, LEX_ERROR) != 0 && strcmp(status, END_OF_FILE) != 0)
    {
        strcpy(status, state_machine());
        printf("%s: %s\n", lex_val, status);
    }
    
    return 0;
}