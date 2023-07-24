#include <iostream>
#include <SDL.h>

// Estructura para representar un color RGB
struct Color {
    Uint8 r, g, b;

    Color(Uint8 red, Uint8 green, Uint8 blue)
            : r(red), g(green), b(blue) {}

    // Constructor por defecto
    Color() : r(0), g(0), b(0) {}

    // Algunos colores predefinidos
    static Color Black() { return Color(0, 0, 0); }
    static Color White() { return Color(255, 255, 255); }
    static Color Red() { return Color(255, 0, 0); }
    static Color Green() { return Color(0, 255, 0); }
    static Color Blue() { return Color(0, 0, 255); }
    // Puedes agregar más colores según tus necesidades
};

// Dimensiones de la ventana (ajustado para mostrar más células)
const int SCREEN_WIDTH = 400;
const int SCREEN_HEIGHT = 400;

// Dimensiones de la matriz del juego (ajustado para más células)
const int BOARD_WIDTH = 400;
const int BOARD_HEIGHT = 400;

// Resto del código...


// Método para limpiar el framebuffer con un color de fondo
void clear(Color* framebuffer, const Color& backgroundColor) {
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; ++i) {
        framebuffer[i] = backgroundColor;
    }
}

// Método para cambiar el color de un píxel específico en el framebuffer
void point(Color* framebuffer, int x, int y, const Color& color) {
    if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
        framebuffer[y * SCREEN_WIDTH + x] = color;
    }
}

// Método para renderizar el framebuffer en la ventana
void renderBuffer(SDL_Renderer* renderer, Color* framebuffer) {
    // Copiar el contenido del framebuffer al renderizador
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STATIC,
                                             SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!texture) {
        printf("La textura no pudo ser creada. Error: %s\n", SDL_GetError());
        return;
    }

    SDL_UpdateTexture(texture, nullptr, framebuffer, SCREEN_WIDTH * sizeof(Color));
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);

    // Liberar la textura después de usarla
    SDL_DestroyTexture(texture);
}

// Método para contar los vecinos vivos de una célula en la posición (x, y)
int countNeighbors(bool gameBoard[BOARD_WIDTH][BOARD_HEIGHT], int x, int y) {
    int count = 0;
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) continue; // No contar la célula actual

            // Obtener las coordenadas de los vecinos, teniendo en cuenta las orillas
            int nx = (x + dx + BOARD_WIDTH) % BOARD_WIDTH;
            int ny = (y + dy + BOARD_HEIGHT) % BOARD_HEIGHT;

            if (gameBoard[nx][ny]) {
                count++;
            }
        }
    }
    return count;
}


// Método para aplicar las reglas de Conway y actualizar el estado del juego
void updateGameBoard(bool gameBoard[BOARD_WIDTH][BOARD_HEIGHT]) {
    bool newGameBoard[BOARD_WIDTH][BOARD_HEIGHT] = {false}; // Nueva matriz del juego

    for (int x = 0; x < BOARD_WIDTH; ++x) {
        for (int y = 0; y < BOARD_HEIGHT; ++y) {
            int neighbors = countNeighbors(gameBoard, x, y);
            if (gameBoard[x][y]) {
                // Reglas para células vivas
                newGameBoard[x][y] = (neighbors == 2 || neighbors == 3);
            } else {
                // Reglas para células muertas
                newGameBoard[x][y] = (neighbors == 3);
            }
        }
    }

    // Copiar la nueva matriz del juego al estado actual
    for (int x = 0; x < BOARD_WIDTH; ++x) {
        for (int y = 0; y < BOARD_HEIGHT; ++y) {
            gameBoard[x][y] = newGameBoard[x][y];
        }
    }
}

// Método para realizar el dibujo y renderizado
void render(SDL_Renderer* renderer, Color* framebuffer, bool gameBoard[BOARD_WIDTH][BOARD_HEIGHT]) {
    // Lógica de dibujo en el framebuffer
    // Por ejemplo, cambiar el color de algunos píxeles
    Color backgroundColor = Color::Black();
    clear(framebuffer, backgroundColor);

    Color pixelColor = Color::White();

    // Dibujar el estado del juego en el framebuffer
    for (int x = 0; x < BOARD_WIDTH; ++x) {
        for (int y = 0; y < BOARD_HEIGHT; ++y) {
            if (gameBoard[x][y]) {
                point(framebuffer, x, y, pixelColor);
            }
        }
    }

    // Renderizar el framebuffer en la ventana
    renderBuffer(renderer, framebuffer);
}

// Método para inicializar un patrón en el tablero de juego
void initializePattern(bool gameBoard[BOARD_WIDTH][BOARD_HEIGHT], int x, int y, const bool* pattern, int patternWidth, int patternHeight) {
    for (int dx = 0; dx < patternWidth; ++dx) {
        for (int dy = 0; dy < patternHeight; ++dy) {
            int nx = x + dx;
            int ny = y + dy;
            if (nx >= 0 && nx < BOARD_WIDTH && ny >= 0 && ny < BOARD_HEIGHT) {
                gameBoard[nx][ny] = pattern[dy * patternWidth + dx];
            }
        }
    }
}

// Método para inicializar un patrón en el tablero de juego en una posición aleatoria
void initializeRandomPattern(bool gameBoard[BOARD_WIDTH][BOARD_HEIGHT], const bool* pattern, int patternWidth, int patternHeight) {
    int x = rand() % (BOARD_WIDTH - patternWidth);
    int y = rand() % (BOARD_HEIGHT - patternHeight);
    initializePattern(gameBoard, x, y, pattern, patternWidth, patternHeight);
}

int main(int argc, char* args[]) {
    // Inicializar SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL no pudo inicializarse. Error: %s\n", SDL_GetError());
        return 1;
    }

    // Crear la ventana
    SDL_Window* window = SDL_CreateWindow(
            "Conway's Game of Life",   // Título de la ventana
            SDL_WINDOWPOS_UNDEFINED,   // Posición x de la ventana
            SDL_WINDOWPOS_UNDEFINED,   // Posición y de la ventana
            SCREEN_WIDTH,              // Ancho de la ventana
            SCREEN_HEIGHT,             // Alto de la ventana
            SDL_WINDOW_SHOWN           // Bandera de la ventana
    );

    if (!window) {
        printf("La ventana no pudo ser creada. Error: %s\n", SDL_GetError());
        return 1;
    }

    // Obtener el renderizador
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("El renderizador no pudo ser creado. Error: %s\n", SDL_GetError());
        return 1;
    }

    // Crear el framebuffer como un arreglo de píxeles
    Color framebuffer[SCREEN_WIDTH * SCREEN_HEIGHT];

    // Matriz para el estado del juego (células vivas y muertas)
    bool gameBoard[BOARD_WIDTH][BOARD_HEIGHT] = {false};

    // Definir el patrón inicial de un Glider
    const bool gliderPattern[] = {
            0, 1, 0,
            0, 0, 1,
            1, 1, 1
    };
    for (int i = 0; i < 20; ++i) {
        initializeRandomPattern(gameBoard, gliderPattern, 3, 3);
    }

    // Definir el patrón inicial de un Block
    const bool blockPattern[] = {
            1, 1,
            1, 1
    };
    for (int i = 0; i < 25; ++i) {
        initializeRandomPattern(gameBoard, blockPattern, 2, 2);
    }

    // Definir el patrón inicial de un Loaf
    const bool loafPattern[] = {
            0, 1, 1, 0,
            1, 0, 0, 1,
            0, 1, 0, 1,
            0, 0, 1, 0
    };
    for (int i = 0; i < 20; ++i) {
        initializeRandomPattern(gameBoard, loafPattern, 4, 4);
    }

    // Definir el patrón inicial de un Tub
    const bool tubPattern[] = {
            0, 1, 0,
            1, 0, 1,
            0, 1, 0
    };
    for (int i = 0; i < 30; ++i) {
        initializeRandomPattern(gameBoard, tubPattern, 3, 3);
    }

    // Definir el patrón inicial de un Blinker
    const bool blinkerPattern[] = {
            1, 1, 1
    };
    for (int i = 0; i < 25; ++i) {
        initializeRandomPattern(gameBoard, blinkerPattern, 3, 1);
    }

    // Definir el patrón inicial de un Pulsar
    const bool pulsarPattern[] = {
            0, 0, 1, 1, 1, 0, 0,
            0, 0, 0, 0, 0, 0, 0,
            1, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 1,
            0, 0, 0, 0, 0, 0, 0,
            0, 0, 1, 1, 1, 0, 0
    };
    for (int i = 0; i < 10; ++i) {
        initializeRandomPattern(gameBoard, pulsarPattern, 7, 7);
    }

    // Definir el patrón inicial de un Spaceship (Glider)
    for (int i = 0; i < 30; ++i) {
        initializeRandomPattern(gameBoard, gliderPattern, 3, 3);
    }

    // Ciclo principal del juego
    bool quit = false;
    while (!quit) {
        // Manejo de eventos
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Actualizar el estado del juego
        updateGameBoard(gameBoard);

        // Realizar el dibujo y renderizado
        render(renderer, framebuffer, gameBoard);
    }

    // Liberar memoria y cerrar SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
