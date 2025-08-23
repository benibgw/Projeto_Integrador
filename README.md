Algoritmo de Funcionamento – Robô Desenhista

Ideia Geral:
O robô desenhista será capaz de seguir uma sequência de direções pré-definidas pelo usuário. Ao executar esses movimentos na ordem programada, ele deixará um rastro em seu percurso, formando assim um desenho no chão ou na superfície sobre a qual se move.

Funcionamento:
Inicialmente, o usuário irá programar os movimentos do robô por meio de quatro botões localizados em sua carcaça:

    Três botões direcionais: frente, esquerda e direita.

    Um botão de início (Start): para iniciar a execução do trajeto.

O usuário pressiona os botões direcionais na ordem desejada, e em seguida aciona o botão Start. O robô então executará os comandos na sequência registrada.
Durante o percurso, o robô utilizará um sensor frontal para detectar obstáculos. Se não houver nada em seu caminho, continuará seguindo normalmente. Caso detecte algum objeto, o robô irá parar e aguardar até que o obstáculo seja removido. Ele permanecerá parado até que:

    O objeto seja retirado;

    O próprio robô seja movido manualmente;

    Ou o percurso seja cancelado.

Melhorias Futuras:
Com o avanço do projeto e o aperfeiçoamento do algoritmo, está prevista a implementação de um site que permitirá a entrada dos comandos diretamente pela interface digital, substituindo o uso dos botões físicos.
