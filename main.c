#include <stdio.h>
#include <math.h>

#include <stdlib.h>

#include <SDL2/SDL.h>

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

double camX0 = -2.0;
double camX1 = 2.0;
double camY0 = -2.0;
double camY1 = 2.0;

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 1000;
const int bytesPerPixel = 4;

int MAX_ITERATIONS = 50;

SDL_Window *window = NULL;

SDL_Renderer *renderer = NULL;
SDL_Texture *texture = NULL;

void *pixels = NULL;

int timerFps;
const int FRAMERATE = 30;

const double MOVE_DISTANCE = 0.1;


unsigned char init() {
    SDL_Init(SDL_INIT_VIDEO);
    
    window = SDL_CreateWindow("Window",
			      SDL_WINDOWPOS_CENTERED,
			      SDL_WINDOWPOS_CENTERED,
			      SCREEN_WIDTH,
			      SCREEN_HEIGHT,
			      SDL_WINDOW_SHOWN);
    if(window)
    {
	renderer = SDL_CreateRenderer(window, -1, 0);
	if(renderer)
	{
	    texture = SDL_CreateTexture(renderer,
					SDL_PIXELFORMAT_ARGB8888,
					SDL_TEXTUREACCESS_STREAMING,
					SCREEN_WIDTH,
					SCREEN_HEIGHT);
	    pixels = malloc(SCREEN_WIDTH * SCREEN_HEIGHT * 4); //Number of bytes.

	    if (SDL_UpdateTexture(texture, 0, pixels, SCREEN_WIDTH*4))
	    {
		//TODO: Handle error
	    }

       
	}
	else
	{
	    //TODO: handle no renderer error
	}
    }
    else
    {
	printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
    }
    return 0;
}

int inMandelBrot(double a, double b) {
    double an;
    double bn;

    double ac = a;
    double bc = b;
    

    
    for (int i = 0; i < MAX_ITERATIONS; i++) {
	an = (a*a) - (b*b);
	bn = (2*a*b);

	a = an + ac;
	b = bn + bc;

	if ((an*an) + (bn*bn) > 4 ) {
	    return i;
	}
    }

    return MAX_ITERATIONS;

}

double map(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void renderMandelbrot()
{    
    int width = SCREEN_WIDTH;
    int height = SCREEN_HEIGHT;

    int pitch = width * bytesPerPixel;
    uint8 *row = (uint8 *)pixels;    

    for(int y = 0; y < SCREEN_HEIGHT; y++)
    {
        uint32 *pixel = (uint32 *)row;
        for(int x = 0;x < SCREEN_WIDTH; x++)
        {
	    //Translate x and y of screen into -2.0 to 2.0

	    double newX = map(x, 0, SCREEN_WIDTH, camX0, camX1);
	    double newY = map(y, 0, SCREEN_HEIGHT, camY0, camY1);
	    
	    int iterations = inMandelBrot(newX, newY);

	    //These vars represent the channels.
	    //We are using a 32 bit write to *pixel, just have to shift everything along, and then or them together.
	    uint8 brightness = map(iterations, 0, MAX_ITERATIONS, 0, 255);

	    if (iterations == 100) {
		brightness = 0;
	    }
            uint8 blue = brightness;
            uint8 green = brightness;
	    uint8 red = brightness;

	    
	    
	    *pixel = ((red << 16 )|(green << 8) | blue);

	    pixel++;
        }

        row += pitch;
    }
}

void updateWindow()
{
    SDL_RenderClear(renderer);
    SDL_UpdateTexture(texture, 0, pixels, SCREEN_WIDTH*4);
    SDL_RenderCopy(renderer, texture, 0, 0);
    SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[])
{
    init();
    
    SDL_Event e;
    unsigned char quit = 0;

    while (!quit){
	while (SDL_PollEvent(&e)){
	    if (e.type == SDL_QUIT){
		quit = 1;
	    }
	    if (e.type == SDL_KEYDOWN){

		switch( e.key.keysym.sym )
		{
		case SDLK_w:
		    camY0 -= MOVE_DISTANCE;
		    camY1 -= MOVE_DISTANCE;
		    break;
		    
		case SDLK_s:
		    camY0 += MOVE_DISTANCE;
		    camY1 += MOVE_DISTANCE;
		    break;
		    
		case SDLK_a:
		    camX0 -= MOVE_DISTANCE;
		    camX1 -= MOVE_DISTANCE;
		    break;
		    
		case SDLK_d:
		    camX0 += MOVE_DISTANCE;
		    camX1 += MOVE_DISTANCE;
		    break;

		case SDLK_UP:
		    camX0 += 0.1;
		    camX1 -= 0.1;
		    camY0 += 0.1;
		    camY1 -= 0.1;
		    break;
		case SDLK_DOWN:
		    camX0 -= 0.1;
		    camX1 += 0.1;
		    camY0 -= 0.1;
		    camY1 += 0.1;
		    break;	    

		case SDLK_LEFT:
		    MAX_ITERATIONS--;
		    break;

		case SDLK_RIGHT:
		    MAX_ITERATIONS++;
		    break;
		    
		case SDLK_ESCAPE:
		    quit = 1;
		default:
		    break;
		}
		
	    }
	    if (e.type == SDL_MOUSEBUTTONDOWN){
		//quit = 1;
		
	    }
	    
	}
	
    renderMandelbrot();
    updateWindow();
    }

    //Destroy stuff
    SDL_DestroyWindow( window );
    SDL_DestroyRenderer(renderer);

    SDL_Quit();

    return 0;
}
