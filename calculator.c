#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <emscripten/emscripten.h>

#define MAX_EXPR_LEN 1024  // tamanho máximo da expressão
#define STACK_SIZE   1024  // tamanho da pilha

// Estrutura para pilha de valores (tipo double)
typedef struct {
    double items[STACK_SIZE];
    int top;
} ValStack;

// Inicializa a pilha de valores (define topo = -1)
void init_val_stack(ValStack *s) {
    s->top = -1;
}

// Verifica se a pilha está vazia (topo == -1)
int is_val_empty(const ValStack *s) {
    return s->top == -1;
}

// Empilha um valor na pilha
void push_val(ValStack *s, double v) {
    if (s->top < STACK_SIZE - 1) {
        s->items[++s->top] = v;  // incrementa topo e coloca valor
    } else {
        fprintf(stderr, "Erro: pilha cheia (overflow)\n");
        exit(EXIT_FAILURE);
    }
}

// Desempilha um valor (remove e retorna o topo)
double pop_val(ValStack *s) {
    if (!is_val_empty(s)) {
        return s->items[s->top--];  // retorna valor e decrementa topo
    }
    fprintf(stderr, "Erro: pilha vazia (underflow)\n");
    exit(EXIT_FAILURE);
}

// Avalia expressão em Notação Polonesa Reversa (postfix)
// Cada token deve estar separado por espaço
EMSCRIPTEN_KEEPALIVE
double evaluate_rpn(const char *postfix) {
    ValStack vals;
    init_val_stack(&vals);  // inicializa pilha

    char *expr = strdup(postfix);  // copia string para poder usar strtok
    if (!expr) exit(EXIT_FAILURE);

    char *token = strtok(expr, " ");
    while (token) {
        // se for operador: + - * /
        if (strlen(token) == 1 && strchr("+-*/", token[0])) {
            double b = pop_val(&vals);  // segundo operando
            double a = pop_val(&vals);  // primeiro operando
            switch (token[0]) {
                case '+': push_val(&vals, a + b); break;  // soma
                case '-': push_val(&vals, a - b); break;  // subtração
                case '*': push_val(&vals, a * b); break;  // multiplicação
                case '/':                                   
                    if (b == 0) {                          
                        fprintf(stderr, "Erro: divisão por zero\n");
                        exit(EXIT_FAILURE);
                    }
                    push_val(&vals, a / b);  // divisão
                    break;
            }
        } else {
            // se não for operador, deve ser número
            double v = strtod(token, NULL);  // converte string para double
            push_val(&vals, v);               // empilha número
        }
        token = strtok(NULL, " ");  // próximo token
    }

    // resultado final é o único valor na pilha
    double result = pop_val(&vals);
    free(expr);  // libera memória
    return result;
}
