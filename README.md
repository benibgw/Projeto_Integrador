# Robô Desenhista

## Ideia Geral
O robô desenhista recebe uma sequência de comandos do usuário, que determinam sua movimentação. Ele executa esses comandos na ordem definida, deixando um rastro sobre a superfície, formando assim um desenho. Durante a execução, o robô é capaz de detectar obstáculos, pausar, e retomar ou cancelar o percurso conforme a interação do usuário.

## Funcionamento Detalhado

### 1. Inicialização do robô
- Ao ser ligado, o robô inicia todos os sistemas internos e chama a função responsável por iniciar sua rede.  
- Durante a tentativa de criação:
  - O `LED_BUILTIN` pisca alternadamente para indicar que o robô está tentando iniciar sua rede.  
- Quando a conexão é estabelecida:
  - O `LED_BUILTIN` permanece aceso, indicando que o robô está online.  
  - O dns do robô é exibido no Serial Monitor para que o usuário saiba como acessar a interface web.  

### 2. Hospedagem da interface web
- O robô hospeda uma pequena página web na rede, que permite ao usuário interagir com ele.  
- A interface contém:
  - Três botões direcionais para registrar movimentos (ex.: frente, trás, esquerda, direita).  
  - Um botão para apagar o último comando inserido.  
  - Um botão para limpar completamente a lista de comandos.  
  - Um botão **Start** para iniciar a execução do percurso.  
  - Um botão **Cancel** para interromper o percurso durante a execução.

### 3. Registro de comandos do usuário
- O robô entra em um loop contínuo para ler os comandos da interface web:  
  - Sempre que um botão direcional é pressionado, o comando correspondente é adicionado a um vetor dinâmico, mantendo a ordem dos movimentos.  
  - Se o usuário pressionar o botão de apagar último comando, o último elemento do vetor é removido.  
  - Se o usuário pressionar o botão de limpar lista, todos os comandos são apagados.  

### 4. Início do percurso
- Quando o usuário pressiona o botão **Start**:
  - O robô chama a função responsável por percorrer o vetor de comandos.  
  - Para cada comando no vetor:
    1. O robô verifica o sensor frontal para detectar obstáculos.  
    2. Se nenhum obstáculo for detectado, o robô executa o comando (movimenta-se conforme a direção especificada).  
    3. Se um obstáculo for detectado:
       - O robô para imediatamente.  
       - Entra em um estado de espera até que alguma das condições a seguir seja atendida:  
         - O robô é movido manualmente;  
         - O obstáculo é removido;  
         - O usuário pressiona o botão **Cancel** na interface.  
       - Se o botão **Cancel** for pressionado:
         - O robô interrompe o percurso e limpa o vetor de comandos, voltando ao estado inicial.

### 5. Continuação e conclusão do percurso
- Após a resolução do obstáculo (ou se não houver nenhum):
  - O robô retoma a execução do vetor de comandos do ponto em que parou.  
  - Cada comando é executado até que todos os movimentos da lista tenham sido realizados.  
- Ao final do percurso:
  - O vetor de comandos é limpo automaticamente.  
  - O robô volta ao estado inicial, pronto para receber uma nova sequência de comandos do usuário.

### 6. Verificação contínua de conexão
- Durante toda a operação, o robô monitora sua conexão com a rede:  
  - Se a conexão cair, o `LED_BUILTIN` é desligado e o robô tenta conectar-se à rede automaticamente.  
  - Quando a conexão é restabelecida, o LED volta a acender e a interface web continua disponível para o usuário.
