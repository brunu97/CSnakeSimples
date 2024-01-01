#include <arch/zx.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <input.h>

struct Parte
{
	int dirVertical;
	int dirHorizontal;
	float x;
	float y;
};

struct Jogador
{
	int partePosi;
	struct Parte snake[999];
	int dirVertical;
	int dirHorizontal;
	int frutaX;
	int frutaY;
};

// https://www.z88dk.org/forum/viewtopic.php?t=9969
void printc(unsigned char x, unsigned char y, unsigned char c)
{
   unsigned char *p, *dat;
   unsigned char i;

   dat = (unsigned char *)15360 + c*8; 
   p = zx_cxy2saddr(x, y);

   for (i = 0; i < 8; ++i)
   {
      *p = *dat++;
      p += 256;
   }
}


void colocaFruta(struct Jogador *jogador)
{
	(*jogador).frutaX = rand() % 30 + 2;
	(*jogador).frutaY = rand() % 22 + 2;
}

void adicionarParte(struct Jogador *jogador)
{
	struct Parte parte = {0};
	parte.dirHorizontal = jogador->snake[jogador->partePosi - 1].dirHorizontal;
	parte.dirVertical = jogador->snake[jogador->partePosi - 1].dirVertical;
	parte.x = jogador->snake[jogador->partePosi - 1].x - ( jogador->snake[jogador->partePosi - 1].dirHorizontal);
	parte.y = jogador->snake[jogador->partePosi - 1].y - (jogador->snake[jogador->partePosi - 1].dirVertical);

	jogador->partePosi += 1;
	jogador->snake[jogador->partePosi] = parte;
}

int detetaColisao(struct Jogador *jogador)
{
	// Verifica se entrou em colisão com as partes - Algoritmo AABB
	for (int i = 1; i < jogador->partePosi; i++)
	{
		if (jogador->snake[0].x == jogador->snake[i].x && jogador->snake[0].y == jogador->snake[i].y)
			return 1;
	}

	// Verifica se saiu fora do ecrã
	if (!(jogador->snake[0].x < 31 && jogador->snake[0].x > 0 && jogador->snake[0].y < 23 && jogador->snake[0].y > 0))
		return 1;

	// Verifica se colide com uma fruta
	if (jogador->snake[0].x == jogador->frutaX && jogador->snake[0].y == jogador->frutaY)
	{
		printc(jogador->frutaX, jogador->frutaY, ' ');
		colocaFruta(jogador);
		adicionarParte(jogador);
	}

	return 0;
}

void desenharRetangulo()
{
	// Desenha muro Topo
	for (int i = 0; i < 32; i++) {
		printc(i, 0, '#');
	}

	// Desenha muro Baixo
	for (int i = 0; i < 32; i++) {
		printc(i, 23, '#');
	}

	// Desenha muros Esquerda e Direita
	for (int y = 1; y < 23; y++)
	{
		printc(0, y, '#');
		printc(31, y, '#');
	}
}


int main()
{
	zx_cls(PAPER_WHITE);
	zx_border(INK_BLUE);

	float ultimoFrame = 0;
	float esteFrame = 0;

	struct Jogador jogador;
	struct Parte parte;
	jogador.partePosi = 1;
	jogador.dirVertical = 1;
	jogador.dirHorizontal = 0;
	parte.dirHorizontal = 1;
	parte.dirVertical = 0;
	parte.x = 10;
	parte.y = 10;
	jogador.snake[0] = parte;

	adicionarParte(&jogador);
	colocaFruta(&jogador);
	desenharRetangulo();

	int ticksPassados = 0;
	int emJogo = 1;
	unsigned char tecla_input;

	while (emJogo)
	{
		// Movimento Input
    	tecla_input = in_inkey();
		if ((tecla_input == 'w') && (jogador.dirVertical != 1))
		{
			jogador.dirVertical = -1;
			jogador.dirHorizontal = 0;
		}
		else if ((tecla_input == 's') && (jogador.dirVertical != -1))
		{
			jogador.dirVertical = 1;
			jogador.dirHorizontal = 0;
		}
		else if ((tecla_input == 'a') && (jogador.dirHorizontal != 1))
		{
			jogador.dirHorizontal = -1;
			jogador.dirVertical = 0;
		}
		else if ((tecla_input == 'd') && (jogador.dirHorizontal != -1))
		{
			jogador.dirHorizontal = 1;
			jogador.dirVertical = 0;
		}

		ticksPassados += 1;
		 if (ticksPassados > 350) { // a cada 350 ticks a cobra atualiza o seu movimento
			struct Parte parte;
			parte.x = jogador.snake[0].x + (1 * jogador.dirHorizontal);
			parte.y = jogador.snake[0].y + (1 * jogador.dirVertical);

			printc(jogador.snake[jogador.partePosi - 1].x, jogador.snake[jogador.partePosi - 1].y, ' '); // para maxima performance e reduzir piscas do ecrã, apenas apaga a ultima parte da cobra

			// Avança a array para direita colocar a nova posição da cabeça da cobra no inicio da array
			for (int i = jogador.partePosi - 1; i >= 0; i--)
				jogador.snake[i + 1] = jogador.snake[i];
			
			jogador.snake[0] = parte;

			ticksPassados = 0;

			if (detetaColisao(&jogador)) { emJogo = 0; }

			// Desenha Cobra
			for (int i = 1; i < jogador.partePosi; i++) 
			{
				printc(jogador.snake[i].x, jogador.snake[i].y, '*');
			}
			printc(jogador.snake[0].x, jogador.snake[0].y, 'O');

			// Desenha o Fruto
			printc(jogador.frutaX, jogador.frutaY, '+');
		}
	}

	printf("Perdestes! - Fim do jogo!");
	return 0;
}
