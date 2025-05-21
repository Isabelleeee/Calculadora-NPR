Calculadora de Notação Polonesa Reversa (NPR)

Este projeto implementa uma calculadora que avalia expressões em Notação Polonesa Reversa (postfix) e um módulo de conversão que transforma expressões infixas convencionais para NPR. O código-fonte principal está em C e é compilado para WebAssembly, permitindo sua execução em navegadores modernos.

---

Estrutura do Código

rpn-project/
├── calculator.c      # Avaliação de expressões em RPN
├── converter.c       # Conversão de infixo para postfix (Shunting Yard)
├── index.html        # Interface web e integração com WebAssembly
├── rpn.js            # Glue-code gerado pelo Emscripten
├── rpn.wasm          # WebAssembly module
└── README.txt         # Documentação 

---

Lógica de Implementação

1. Avaliação de RPN (evaluate_rpn)
- Utiliza uma pilha de valores (ValStack) para armazenar operandos.
- Percorre os tokens de entrada (separados por espaço). Para cada token:
  1. Se for número, converte para double e empilha.
  2. Se for operador (+ - * /), desempilha dois valores, aplica a operação e empilha o resultado.
- No fim, sobra na pilha o resultado final.

2. Conversão Infix → Postfix (infix_to_postfix)
Implementa o algoritmo de Shunting Yard:
1. Pilha de operadores (OpStack) para armazenar + - * / e parênteses.
2. Buffer temporário para ler números (fluxo contínuo de dígitos e .).
3. Ao encontrar:
   - Dígito/ponto: acumula no buffer.
   - Operador: descarrega buffer de número e:
     - Enquanto topo da pilha tiver operador de maior ou igual precedência, remove e adiciona ao output.
     - Empilha o operador atual.
   - '(': empilha.
   - ')': desempilha até encontrar '('.
4. Ao final, desempilha todos operadores restantes.
5. Gera string com tokens separados por espaço.

---

Exemplos de Uso

Conversão (Infix → NPR):
char *rpn = infix_to_postfix("3+4*2/(1-5)");
rpn: "3 4 2 * 1 5 - / + "

Avaliação (RPN → Resultado):
double res = evaluate_rpn("3 4 2 * 1 5 - / +");
res: 1

Na interface Web, ao digitar 3+4*2/(1-5) e clicar =, aparece:
3 4 2 * 1 5 - / + 
1

---

Estruturas de Dados

Optou-se por **pilhas (LIFO)** para valores e operadores, pois:
- A lógica de parentesização e ordem de avaliação segue LIFO.
- Pilhas permitem empilhar operandos até aplicar o operador apropriado.
- Shunting Yard exige remoção do último operador inserido.

---

Análise de Complexidade

- Tempo:
  - infix_to_postfix: O(n), n = tamanho da expressão infixa.
  - evaluate_rpn: O(n), n = número de tokens na expressão postfix.
- Espaço:
  - Duas pilhas com O(n) no pior caso.

Desempenho linear adequado para uso interativo em navegadores.

---

Como Testar/Usar: 

- Copie e Cole essa URL: "https://isabelleeee.github.io/Calculadora-NPR/" em seu navegador.
- Clique nos botões para forma uma equação e aperte o "=", assim aparecerá a forma polonesa e resultado da sua equação.
