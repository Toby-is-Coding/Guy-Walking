#pragma once
#include "ForAll.h"
class LTexture
{
public:
    //Initializes variables
    LTexture();

    //Deallocates memory
    ~LTexture();

    //Loads image at specified path, colorKey if you so wish
    bool loadFromFile(std::string path, bool colorKey = false, SDL_Color* colour = NULL, int nColours = 1);

    //Creates image from font string
    bool loadFromRenderedText(std::string text, TTF_Font* font, SDL_Color textColour);

    //Deallocates texture
    void free();

    //Set color modulation
    void modColor(Uint8 r, Uint8 g, Uint8 b);

    //Set blending
    void setBlendMode(SDL_BlendMode blending);

    //Set alpha modulation
    void setAlpha(Uint8 alpha);

    //Renders texture at given point
    void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE, double scaleFac = 1);

    //Gets image dimensions
    int getWidth();
    int getHeight();

private:
    //The actual hardware texture
    SDL_Texture* mTexture;

    //Image dimensions
    int mWidth;
    int mHeight;
};