#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <emscripten/emscripten.h>

#define MAX_EXPR_LEN 1024  // tamanho máximo da expressão infixa
#define STACK_SIZE   1024  // tamanho máximo da pilha de operadores

// Estrutura para pilha de operadores (tipo char)
typedef struct {
    char items[STACK_SIZE];
    int top;
} OpStack;

// Inicializa pilha de operadores (topo = -1)
void init_op_stack(OpStack *s) {
    s->top = -1;
}

// Verifica se a pilha de operadores está vazia
int is_op_empty(const OpStack *s) {
    return s->top == -1;
}

// Empilha um operador na pilha
void push_op(OpStack *s, char op) {
    if (s->top < STACK_SIZE - 1) {
        s->items[++s->top] = op;  // incrementa topo e coloca operador
    } else {
        fprintf(stderr, "Erro: pilha de operadores cheia\n");
        exit(EXIT_FAILURE);
    }
}

// Desempilha e retorna o operador do topo
enum { FAIL_CHAR = '\0' };
char pop_op(OpStack *s) {
    if (!is_op_empty(s)) {
        return s->items[s->top--];  // retorna e decrementa topo
    }
    return FAIL_CHAR;  // pilha vazia
}

// Espia o topo sem desempilhar
char peek_op(const OpStack *s) {
    if (!is_op_empty(s)) {
        return s->items[s->top];
    }
    return FAIL_CHAR;
}

// Precedência dos operadores (+- =1, */ =2)
int precedence(char op) {
    switch (op) {
        case '+': case '-': return 1;
        case '*': case '/': return 2;
    }
    return 0;
}

// Verifica se operador é associativo à esquerda
int is_left_assoc(char op) {
    switch (op) {
        case '+': case '-': case '*': case '/':
            return 1;  // todos suportados são left-assoc
    }
    return 0;
}

// Converte expressão infixa para postfix (Shunting Yard)
// Retorna uma string dinâmica com a expressão em RPN
EMSCRIPTEN_KEEPALIVE
char* infix_to_postfix(const char *infix) {
    OpStack ops;
    init_op_stack(&ops);

    // saída: espaço para MAX_EXPR_LEN chars
    char *output = malloc(MAX_EXPR_LEN);
    if (!output) exit(EXIT_FAILURE);
    output[0] = '\0';  // string vazia

    char numbuf[64];  // buffer temporário para capturar números
    int numpos = 0;

    for (int i = 0; infix[i] != '\0'; ++i) {
        char c = infix[i];
        if (isspace((unsigned char)c)) {
            continue;  // ignora espaços
        }
        
        // se for dígito ou ponto, faz parte de um número
        if (isdigit((unsigned char)c) || c == '.') {
            numbuf[numpos++] = c;
        } else {
            // houve troca de token, flush do buffer de número
            if (numpos > 0) {
                numbuf[numpos] = '\0';
                strcat(output, numbuf);
                strcat(output, " ");
                numpos = 0;
            }
            
            if (c == '(') {
                // abre parêntese: empilha
                push_op(&ops, c);
            } else if (c == ')') {
                // fecha parêntese: desempilha até encontrar '('
                while (!is_op_empty(&ops) && peek_op(&ops) != '(') {
                    char op = pop_op(&ops);
                    strncat(output, &op, 1);
                    strcat(output, " ");
                }
                pop_op(&ops);  // remove o '('
            } else if (strchr("+-*/", c)) {
                // operador: desempilha operadores de maior ou igual precedência
                while (!is_op_empty(&ops) && strchr("+-*/", peek_op(&ops)) &&
                       (precedence(peek_op(&ops)) > precedence(c) ||
                        (precedence(peek_op(&ops)) == precedence(c) && is_left_assoc(c)))) {
                    char op = pop_op(&ops);
                    strncat(output, &op, 1);
                    strcat(output, " ");
                }
                // empilha o operador atual
                push_op(&ops, c);
            }
        }
    }
    // flush de qualquer número restante
    if (numpos > 0) {
        numbuf[numpos] = '\0';
        strcat(output, numbuf);
        strcat(output, " ");
    }
    // desempilha todos os operadores restantes
    while (!is_op_empty(&ops)) {
        char op = pop_op(&ops);
        strncat(output, &op, 1);
        strcat(output, " ");
    }
    
    return output;  // string em RPN pronta para uso
}