#include "LTexture.h"

LTexture::LTexture()
	: mTexture(NULL), mWidth(0), mHeight(0)
{
}

LTexture::~LTexture()
{
	free();
}

bool LTexture::loadFromFile(std::string path, bool colorKey, SDL_Color* colour, int nColours)
{
	free(); //get rid of preexisting texture
	SDL_Texture* newTexture = NULL;

	SDL_Surface* desiredImage = IMG_Load(path.c_str());
	if (desiredImage == NULL) printf(IMG_GetError());
	else {
		if (colorKey) {
			//Colour key image
			if (nColours <= 1) {
				SDL_SetColorKey(desiredImage, SDL_TRUE, SDL_MapRGB(desiredImage->format, colour->r, colour->g, colour->b));
			}
			else { //Colour key multiple colours
				for (int i = 0; i < nColours; ++i) {
					SDL_SetColorKey(desiredImage, SDL_TRUE, SDL_MapRGB(desiredImage->format, colour[i].r, colour[i].g, colour[i].b));
				}
			}
		}
		
		newTexture = SDL_CreateTextureFromSurface(renderer, desiredImage); //create texture
		if (newTexture == NULL) printf(SDL_GetError());
		else {
			//Get image dimensions
			mWidth = desiredImage->w;
			mHeight = desiredImage->h;
		}
		SDL_FreeSurface(desiredImage); //no need no more, so free
	}
	mTexture = newTexture;
	return mTexture != NULL;
}

bool LTexture::loadFromRenderedText(std::string text, TTF_Font* font, SDL_Color textColour)
{
	free(); //get rid of preexisting texture

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), textColour);
	if (textSurface != NULL) {
		mTexture = SDL_CreateTextureFromSurface(renderer, textSurface); //create texture
		mWidth = textSurface->w;
		mHeight = textSurface->h;

		SDL_FreeSurface(textSurface); //free surface
	}
	return mTexture != NULL;
}

void LTexture::free()
{
	//Free texture if it exists
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::modColor(Uint8 r, Uint8 g, Uint8 b)
{
	SDL_SetTextureColorMod(mTexture, r, g, b);
}

void LTexture::setBlendMode(SDL_BlendMode blending)
{
	SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha)
{
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip, double scaleFac)
{
	//render at pos (x,y) whole texture
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	if (clip != NULL) { //but maybe we want to render only part of the texture
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	renderQuad.w *= scaleFac;
	renderQuad.h *= scaleFac;

	SDL_RenderCopyEx(renderer, mTexture, clip, &renderQuad, angle, center, flip); //render
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}
