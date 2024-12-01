#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define BUFFER_SIZE 256

int test = 0;
typedef enum tokens {
 OUTPUT = '.',
 INPUT = ',',
 INCREMENT ='+',
 DECREMENT = '-',
 INCREMENT_POINTER = '>',
 DECREMENT_POINTER ='<',
 OPEN_BRACKET = '[',
 CLOSE_BRACKET = ']',
} tokens;

typedef struct main_buffer_t{
    char container[BUFFER_SIZE];
    int currentPointer;
}main_buffer_t;

typedef struct stack_t{
    size_t top; 
    size_t capacity;
    size_t* container;
}stack_t;

typedef struct statement_buffer_t{
    bool isInterpretingStatement;
    size_t loopCount;
    size_t size;
    size_t capacity;
    size_t currentPointer;
    char* container;
} statement_buffer_t;


FILE* validate_file_or_error(char* sourceFile);

void interpret_source_file(FILE* file);

void validate_token_or_error(char inputToken, size_t *lineCount, size_t *columnCount);

bool is_valid_token(char token);

void modify_buffer(FILE *file, char inputToken, main_buffer_t* buffer, statement_buffer_t *stmtBuffer, stack_t *stack, size_t *lineCount, size_t *columnCount);


statement_buffer_t* create_statement_buffer(size_t capacity);

void increase_buffer_capacity(statement_buffer_t *buffer);

void append_char(statement_buffer_t* buffer, char val);

void clear_statement_buffer(statement_buffer_t *buffer);


stack_t* create_stack(size_t capacity);

void increase_stack_capacity(stack_t *stack);

void stack_push(stack_t* stack, size_t val);

size_t stack_pop(stack_t* stack);

size_t stack_top(stack_t* stack);

bool is_empty(stack_t* stack);


int main(int argc, char **argv)
{
    FILE *sourceFile = validate_file_or_error(argv[1]);
    interpret_source_file(sourceFile);
    return 0;
}

FILE* validate_file_or_error(char* sourceFile){
    if(sourceFile == NULL){
        fprintf(stderr,"\nInvalid Command, No source file!\nExample: './bfk.exe example.bf\n");
        exit(0);
    }

    FILE* file = fopen(sourceFile, "r");
    if(!file){
        fprintf(stderr,"\nNo such file found!\nPlease make sure there's a file called '%s'\n", sourceFile);
        exit(0);
    }
    return file;
}

bool is_valid_token(char token){
    switch (token) {
        case INCREMENT: return true;
        case DECREMENT: return true;
        case INPUT: return true;
        case OUTPUT: return true;
        case INCREMENT_POINTER: return true;
        case DECREMENT_POINTER: return true;
        case OPEN_BRACKET: return true;
        case CLOSE_BRACKET: return true;
        default: return false;
    }
}

void increase_stack_capacity(stack_t *stack){
    stack->container = (size_t*)realloc(stack->container, (stack->capacity = (stack->capacity * 2)));
    if(!stack->container){
        fprintf(stderr, "\nUnable to increase Stack memory\nDamn what are you running this on?\n");
        exit(-1);
    }
}

bool is_empty(stack_t* stack){
    if(stack->top == -1){
        return true;
    }
    return false;
}

void stack_push(stack_t* stack, size_t index){
    if((stack->top + 1) == stack->capacity){
        increase_stack_capacity(stack);
    }
    stack->container[++stack->top] = index;
}

size_t stack_pop(stack_t* stack){
    if(is_empty(stack)){
        fprintf(stderr, "\nEmpty stack pop operation something went wrong!\n");
        exit(0);
    }
    return stack->container[stack->top--];
}

size_t stack_top(stack_t* stack){
    if(is_empty(stack)){
        fprintf(stderr, "\nEmpty stack access something went wrong!\n");
        exit(0);
    }
    return stack->container[stack->top];
}

void append_char(statement_buffer_t* buffer, char val){
    if((buffer->size + 1) >= buffer->capacity){
        increase_buffer_capacity(buffer);
    }
    buffer->container[buffer->size++] = val;
}

stack_t* create_stack(size_t capacity){
    stack_t *stack = (stack_t *)malloc(sizeof(stack_t));
    stack->top = -1;
    stack->capacity = capacity;
    stack->container = (size_t*)malloc(sizeof(size_t) + capacity);
    if(!stack->container){
        fprintf(stderr, "\nUnable to allocate memory for stack\nDamn what are you running this on?\n");
        exit(-1);
    }
    return stack;
}

statement_buffer_t* create_statement_buffer(size_t capacity){
    statement_buffer_t *buffer = (statement_buffer_t *)malloc(sizeof(statement_buffer_t));
    buffer->size = 0;
    buffer->loopCount = 0;
    buffer->currentPointer = 0;
    buffer->capacity = capacity;
    buffer->isInterpretingStatement = false;
    buffer->container = (char*)malloc(sizeof(char) + capacity);
    if(!buffer->container){
        fprintf(stderr, "\nUnable to allocate memory for stack\nDamn what are you running this on?\n");
        exit(-1);
    }
    return buffer;
}

void clear_statement_buffer(statement_buffer_t *buffer){
    buffer->size = 0;
    buffer->loopCount = 0;
    buffer->currentPointer = 0;
    buffer->isInterpretingStatement = false;
}

void increase_buffer_capacity(statement_buffer_t *buffer){
    buffer->container = (char*)realloc(buffer->container, (buffer->capacity = (buffer->capacity * 2)));
    if(!buffer->container){
        fprintf(stderr, "\nUnable to allocate Statement-buffer memory\nDamn what are you running this on?\n");
        exit(-1);
    }
}
void validate_token_or_error(char inputToken, size_t *lineCount, size_t *columnCount){
    if(!is_valid_token(inputToken) && inputToken != ' ' && inputToken != '\n' && inputToken != '\t'){
            fprintf(stderr, "\nError: Invalid Token!\n[line %llu: column %llu] > Symbol '%c' is not a valid token\n", *lineCount, *columnCount, inputToken);
            exit(-1);
    }
    (*columnCount)++;
    if(inputToken == '\n'){
        (*lineCount)++;
        (*columnCount) = 0;
    }
}

bool is_statement_complete(statement_buffer_t *buffer, char token){
    if(token == '['){
        buffer->loopCount++;
    }
    if(token == ']'){
        buffer->loopCount--;
    }
    if(buffer->loopCount == 0){
        return true;
    }
    return false;
}

void modify_buffer(FILE *file,char inputToken, main_buffer_t* buffer, statement_buffer_t *stmtBuffer, stack_t *stack, size_t *lineCount, size_t *columnCount){
    char consoleInput;  

    switch (inputToken) {
        case INCREMENT: 
            buffer->container[buffer->currentPointer]++;
            return;
        case DECREMENT:
            buffer->container[buffer->currentPointer]--;
            return;
        case INPUT: 
            consoleInput = getchar();
            while(consoleInput == ' ' || consoleInput == '\n' || consoleInput == '\t'){
                consoleInput = getchar();
            }
            buffer->container[buffer->currentPointer] = consoleInput;
            return;
        case OUTPUT: 
            printf("%c", buffer->container[buffer->currentPointer]);
            return;
        case INCREMENT_POINTER: 
            buffer->currentPointer++;
            buffer->currentPointer = buffer->currentPointer % BUFFER_SIZE;
            return;
        case DECREMENT_POINTER:
            buffer->currentPointer--;
            buffer->currentPointer = buffer->currentPointer % BUFFER_SIZE;
            return;
        case OPEN_BRACKET:
            if(!stmtBuffer->isInterpretingStatement){
                stmtBuffer->isInterpretingStatement = true;

                while(!is_statement_complete(stmtBuffer, inputToken)){
                    validate_token_or_error(inputToken, lineCount, columnCount);
                    append_char(stmtBuffer, inputToken);
                
                    while((inputToken = fgetc(file)) != EOF && (inputToken == ' ' || inputToken == '\n' || inputToken == '\t'));
                } 
                append_char(stmtBuffer, inputToken);

                while(stmtBuffer->currentPointer < stmtBuffer->size){
                    if(stmtBuffer->container[stmtBuffer->currentPointer] == '['){
                        stack_push(stack, stmtBuffer->currentPointer);
                
                        if(buffer->container[buffer->currentPointer] == 0){
                            while(stmtBuffer->currentPointer <= stmtBuffer->size && is_statement_complete(stmtBuffer, stmtBuffer->container[stmtBuffer->currentPointer++]));
                           
                            stack_pop(stack);
                            if(is_empty(stack)){
                                clear_statement_buffer(stmtBuffer);
                                return;
                            }
                        }
                    }else{
                        modify_buffer(file, stmtBuffer->container[stmtBuffer->currentPointer], buffer, stmtBuffer, stack, lineCount, columnCount);
                    }
                    stmtBuffer->currentPointer++;
                }
                stmtBuffer->currentPointer = 0;
            }

            return;
        case CLOSE_BRACKET:
            if(buffer->container[buffer->currentPointer] == 0){
                stack_pop(stack);
                if(is_empty(stack)){
                    clear_statement_buffer(stmtBuffer);
                    return;
                }
            }else{
                stmtBuffer->currentPointer = stack_top(stack);
            }
            return;
        
    }   
}

void interpret_source_file(FILE* file){
    char inputToken;
    main_buffer_t buffer;
    memset(&buffer, 0, sizeof(buffer));
   
    statement_buffer_t *stmtBuffer = create_statement_buffer(BUFFER_SIZE);
    stack_t *stack = create_stack(BUFFER_SIZE);

    size_t lineCount = 1, columnCount = 1;

    while ((inputToken = fgetc(file)) != EOF) {
        validate_token_or_error(inputToken, &lineCount, &columnCount);
        modify_buffer(file, inputToken , &buffer, stmtBuffer, stack, &lineCount, &columnCount);
    }
}