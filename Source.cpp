#include <iostream>
#include "LTexture.h"
#include "ForAll.h"
#include <thread>
#include <chrono>

using namespace std;

enum GUY_FRAMES { //FRAMES OF GUY WALKING
	GUY_WALKING = 0
};

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* Pixelmania = NULL;
TTF_Font* Pixelated = NULL;
LTexture title;
LTexture text;
LTexture background;
LTexture guy;
#define ANIMATION_FRAMES 4
SDL_Rect guyAnims[ANIMATION_FRAMES];

void mainMenu(bool& osc, Uint8 titleAlpha); //manage main menu loading

bool init() {
	//Initialize
	if (SDL_Init(SDL_INIT_VIDEO) == -1 || !(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) || TTF_Init() == -1) return false;
	window = SDL_CreateWindow("Guy Walking", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == NULL) return false;

	//Renderer
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL) return false;
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	return true;
}

bool loadMedia() {
	//background
	if (!background.loadFromFile("Assets/background.png")) return false;

	//guy
	SDL_Colour ignoreColours[2] = { {1, 0, 0}, {255, 0, 255} }; //ignore pixel guy's background and frame seperators
	if (!guy.loadFromFile("Assets/pixel guy.png", true, ignoreColours, 2)) return false;
	else {
		//Initialize animation frames
		guyAnims[0] = { 0, 0, 20, 29 };
		guyAnims[1] = { 20, 0, 20, 29 };
		guyAnims[2] = { 40, 0, 20, 29 };
		guyAnims[3] = { 60, 0, 20, 29 };
	}

	//font & text
	Pixelmania = TTF_OpenFont("Assets/Pixelmania.ttf", 36);
	if (Pixelmania == NULL) return false;
	else if (!title.loadFromRenderedText("GUY WALKING", Pixelmania, { 255, 127, 127 })) return false; //Render title
	else {
		title.setBlendMode(SDL_BLENDMODE_BLEND); //for cool fading effect
	}

	Pixelated = TTF_OpenFont("Assets/Pixelated.ttf", 24);
	if (Pixelated == NULL) return false;
	else if (!text.loadFromRenderedText("Press Enter", Pixelated, { 127, 0, 0 })) return false; //Render text

	return true;
}

void close() {
	//Free loaded images
	title.free();
	guy.free();
	background.free();

	//Free font
	TTF_CloseFont(Pixelmania);
	TTF_CloseFont(Pixelated);
	Pixelmania = NULL;
	Pixelated = NULL;

	//Destroy window
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	renderer = NULL;
	window = NULL;

	//Quit SDL subsystems
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

int main(int argc, char* args[]) {
	if (!init() || !loadMedia()) {
		cout << SDL_GetError() << endl;
		close();
		return 0;
	}

	bool osc = false, oscRunning = false, terminateOscillator = false;
	thread oscillator([&osc, &oscRunning, &terminateOscillator](unsigned frequency) { //oscillator
		while (!terminateOscillator) { //to keep thread running even when oscillator isn't
			while (oscRunning) {
				this_thread::sleep_for(chrono::milliseconds(frequency));
				osc = !osc;
			}
		}
		}, 100);

	SDL_Event e;
	bool quit = false;

	const int VEL = 10; //velocity of movement

	int guy_walk_frame = 0; //current walking frame
	SDL_Point GuyPos = { 100, 200 }; //position of guy
	SDL_Point GuyDirec = { 0, 0 }; //guy's direction of movement
	bool moveGuy = false; //move guy or not?
	double angle = 0;
	SDL_RendererFlip flipType = SDL_FLIP_NONE;

	bool loadMainMenu = true;
	Uint8 mainMenuAlpha = 255; //title visibility, for fading out effect

	oscRunning = true;
	//MAIN LOOP
	while (!quit) {
		//Event Handler
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = true;
				oscRunning = false; //stop oscillator
				terminateOscillator = true; //terminate thread
			}
			else if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
				case SDLK_RETURN:
					loadMainMenu = false;
					break;
				case SDLK_LEFT:
					moveGuy = true;
					GuyDirec.x = -VEL;
					flipType = SDL_FLIP_HORIZONTAL;
					angle = 0;
					break;
				case SDLK_RIGHT:
					moveGuy = true;
					GuyDirec.x = VEL;
					flipType = SDL_FLIP_NONE;
					angle = 0;
					break;
				case SDLK_UP:
					moveGuy = true;
					GuyDirec.y = -VEL;
					break;
				case SDLK_DOWN:
					moveGuy = true;
					GuyDirec.y = VEL;
					break;
				}
			}
			else if (e.type == SDL_KEYUP) {
				moveGuy = false;
				GuyDirec = { 0, 0 };
				guy_walk_frame = 0;
			}
		}

		//Clear screen
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);

		//render background texture
		background.render(0, 0);

		if (loadMainMenu) {
			mainMenu(osc, mainMenuAlpha); //load main menu
		}
		else {
			//fade out main menu once forgone
			if (mainMenuAlpha > 0) {
				static bool do_once = [&mainMenuAlpha] {mainMenuAlpha -= 17; return true; }(); //this code runs once throughout the entire program the first time it's reached
				if (osc) {
					osc = false;
					mainMenuAlpha -= 17;
				}
				mainMenu(osc, mainMenuAlpha);
			}

			//render guy
			guy.render(GuyPos.x, GuyPos.y, &guyAnims[GUY_WALKING + guy_walk_frame], angle, NULL, flipType, 5);
			if (osc && moveGuy) {
				osc = false;
				GuyPos.x += GuyDirec.x;
				GuyPos.y += GuyDirec.y;
				++guy_walk_frame %= ANIMATION_FRAMES; //iterate to next walk frame
			}
		}

		//Update screen
		SDL_RenderPresent(renderer);
	}

	close();
	oscillator.join(); //wait for thread to finish
	return 0;
}

void mainMenu(bool& osc, Uint8 titleAlpha) {
	static int guy_walk_frame = 0; //guy's walk animation frames

#define SPLASH_MAX_SIZE 2
#define SPLASH_MIN_SIZE 1
	static double splashScale = 1.5; //scale factor for splash text
	static double splashScaleBS = 0.25; //adding factor for splashScale (BS - Bigger/Smaller)

	static int titleRotateAngle = 0; //title start rotate when begin fading

	if (titleAlpha == 0xFF) { //only render title once it starts fading
		//render mysterious guy form
		guy.modColor(20, 0, 0);
		if (osc) {
			osc = false; //do once
			++guy_walk_frame %= ANIMATION_FRAMES; //iterate to next walk frame

			//get bigger-smaller effect for splash text
			if (splashScale == SPLASH_MAX_SIZE || splashScale == SPLASH_MIN_SIZE) {
				splashScaleBS *= -1;
			}
			splashScale += splashScaleBS; //make bigger/smaller
		}
		guy.render(SCREEN_WIDTH / 2 - guyAnims[GUY_WALKING].w * 9 - 70, SCREEN_HEIGHT / 3, &guyAnims[GUY_WALKING + guy_walk_frame], -15, NULL, SDL_FLIP_NONE, 9);

		//render splash text such that when scaled up or down, it stays around its relative center
		text.render(SCREEN_WIDTH / 2 - text.getWidth() * (splashScale / 2) + 120, SCREEN_HEIGHT / 2 - text.getHeight() * (splashScale / 2) + text.getHeight(), NULL, 15, NULL, SDL_FLIP_NONE, splashScale);
	}
	else {
		titleRotateAngle += 1; //when fading begins also begin rotating
	}

	//render title
	title.setAlpha(titleAlpha);
	title.render((SCREEN_WIDTH - title.getWidth()) / 2, (SCREEN_HEIGHT - title.getHeight()) / 3.5, NULL, -titleRotateAngle);

	//reset guy colours
	guy.modColor(255, 255, 255);
}