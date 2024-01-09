#include <SDL.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#define ECRA_ALTURA 400   
#define ECRA_COMPRIMENTO 600

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

void colocaFruta(struct Jogador *jogador)
{
	jogador->frutaX = floor(rand() % (560 / 20)) * 20;
	jogador->frutaY = floor(rand() % (360 / 20)) * 20;
}

void adicionarParte(struct Jogador *jogador)
{
	struct Parte parte = {0};
	parte.dirHorizontal = jogador->snake[jogador->partePosi - 1].dirHorizontal;
	parte.dirVertical = jogador->snake[jogador->partePosi - 1].dirVertical;
	parte.x = jogador->snake[jogador->partePosi - 1].x - (20 * jogador->snake[jogador->partePosi - 1].dirHorizontal);
	parte.y = jogador->snake[jogador->partePosi - 1].y - (20 * jogador->snake[jogador->partePosi - 1].dirVertical);

	jogador->partePosi += 1;
	jogador->snake[jogador->partePosi] = parte;
}

int detetaColisao(struct Jogador *jogador)
{
	// Verifica se entrou em colisão com as partes - Algoritmo AABB
	for (int i = 1; i < jogador->partePosi; i++)
	{
		if (jogador->snake[0].x < jogador->snake[i].x + 20 &&
			jogador->snake[0].x + 20 > jogador->snake[i].x &&
			jogador->snake[0].y < jogador->snake[i].y + 20 &&
			jogador->snake[0].y + 20 > jogador->snake[i].y)
			return 1;
	}

	// Verifica se saiu fora do ecrã
	if (!(jogador->snake[0].x < ECRA_COMPRIMENTO &&
		  jogador->snake[0].x + 20 > 0 &&
		  jogador->snake[0].y < ECRA_ALTURA &&
		  jogador->snake[0].y + 20 > 0))
		return 1;

	// Verifica se colide com uma fruta
	if (jogador->snake[0].x < jogador->frutaX + 20 &&
		jogador->snake[0].x + 20 > jogador->frutaX &&
		jogador->snake[0].y < jogador->frutaY + 20 &&
		jogador->snake[0].y + 20 > jogador->frutaY)
	{
		colocaFruta(jogador);
		adicionarParte(jogador);
	}
	return 0;
}

int main(int argc, char *args[])
{
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window *window = SDL_CreateWindow("SnakeSimplesSDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ECRA_COMPRIMENTO, ECRA_ALTURA, SDL_WINDOW_SHOWN);
	SDL_Renderer *render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_RaiseWindow(window);
	srand(time(NULL));

	float ultimoFrame = 0;
	float esteFrame = 0;

	struct Jogador jogador;
	struct Parte parte;
	jogador.partePosi = 1;
	jogador.dirVertical = 1;
	jogador.dirHorizontal = 0;
	parte.dirHorizontal = 1;
	parte.dirVertical = 0;
	parte.x = 300;
	parte.y = 100;
	jogador.snake[0] = parte;

	adicionarParte(&jogador);
	colocaFruta(&jogador);

	float tempoPassado = 0;
	int emJogo = 1;

	while (emJogo)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				return 0;
			}
		}

		// Obter o frametime para controlar velocidades do jogo em diferentes frame rates
		esteFrame = SDL_GetTicks() / 1000.0f;
		float delta = esteFrame - ultimoFrame;
		ultimoFrame = esteFrame;

		// Movimento Input
		const Uint8 *currentKeyStates = SDL_GetKeyboardState(NULL);
		if (currentKeyStates[SDL_SCANCODE_UP] && (jogador.dirVertical != 1))
		{
			jogador.dirVertical = -1;
			jogador.dirHorizontal = 0;
		}
		else if (currentKeyStates[SDL_SCANCODE_DOWN] && (jogador.dirVertical != -1))
		{
			jogador.dirVertical = 1;
			jogador.dirHorizontal = 0;
		}
		else if (currentKeyStates[SDL_SCANCODE_LEFT] && (jogador.dirHorizontal != 1))
		{
			jogador.dirHorizontal = -1;
			jogador.dirVertical = 0;
		}
		else if (currentKeyStates[SDL_SCANCODE_RIGHT] && (jogador.dirHorizontal != -1))
		{
			jogador.dirHorizontal = 1;
			jogador.dirVertical = 0;
		}

		tempoPassado += delta;
		if (tempoPassado > 0.1f)
		{ // a cada 0.1ms a cobra atualiza o seu movimento
			struct Parte parte;
			parte.x = jogador.snake[0].x + (20 * jogador.dirHorizontal);
			parte.y = jogador.snake[0].y + (20 * jogador.dirVertical);

			// Avança a array para direita colocar a nova posição da cabeça da cobra no inicio da array
			for (int i = jogador.partePosi - 1; i >= 0; i--)
				jogador.snake[i + 1] = jogador.snake[i];
			jogador.snake[0] = parte;

			tempoPassado = 0;

			if (detetaColisao(&jogador))
			{
				emJogo = 0;
			}
		}

		// Limpa o Ecrã
		SDL_SetRenderDrawColor(render, 0, 60, 0, 255);
		SDL_RenderClear(render);

		// Desenha Gráficos
		for (int i = 1; i < jogador.partePosi; i++) // Desenha Cobra
		{
			SDL_Rect fillRect = {jogador.snake[i].x, jogador.snake[i].y, 20, 20}; // Posição, Tamanho
			SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
			SDL_RenderFillRect(render, &fillRect);
		}
		SDL_Rect topo = {jogador.snake[0].x, jogador.snake[0].y, 20, 20}; // Mantem sempre a cabeça como desenho frontal sobre as outras partes, em caso de colisão fica por cima
		SDL_SetRenderDrawColor(render, 170, 140, 180, 255);
		SDL_RenderFillRect(render, &topo);

		// Desenha o Fruto
		SDL_Rect fruto = {jogador.frutaX, jogador.frutaY, 20, 20}; // Posição, Tamanho
		SDL_SetRenderDrawColor(render, 100, 100, 100, 255);
		SDL_RenderFillRect(render, &fruto);

		// Atualiza o render da janela
		SDL_RenderPresent(render);
	}

	SDL_Delay(2000);

	return 0;
}
