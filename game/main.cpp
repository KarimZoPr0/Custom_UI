#include <SDL2/SDL.h>

// If you're going to render widgets to the same
// UI from different source files, you can avoid
// ID collisions by defining IMGUI_SRC_ID before
// this define block:
#ifdef IMGUI_SRC_ID
#define GEN_ID ((IMGUI_SRC_ID) + (__LINE__))
#else
#define GEN_ID (__LINE__)
#endif

typedef struct UIState
{
    int mouseX;
    int mouseY;
    int mouseDown;

    int hotItem;
    int activeItem;
} UIState;

typedef struct App
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event event;
    bool running;

    UIState uiState = { 0 };

} App;

static App app;

void close(  );
void render(  );
void processInput(  );
int regionHit( int x, int y, int w, int h );
int button( int id, int x, int y );
void drawRect(int x, int y, int w, int h, int color);
void imgui_prepare();
void imgui_finish();
int slider(int id, int x, int y, int max, int &value);
int checkbox(int id, int x, int y);

int init( )
{
    // Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        SDL_Log( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError( ) );
        return 0;
    }

    // Create window
    app.window = SDL_CreateWindow( "Custom_UI", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480,
                                   SDL_WINDOW_SHOWN );
    if( app.window == NULL )
    {
        SDL_Log( "Window could not be created! SDL_Error: %s\n", SDL_GetError( ) );
        SDL_Quit( );
        return 0;
    }

    // Create renderer
    app.renderer = SDL_CreateRenderer( app.window, -1, SDL_RENDERER_ACCELERATED );
    if( app.renderer == NULL )
    {
        SDL_Log( "Renderer could not be created! SDL_Error: %s\n", SDL_GetError( ) );
        SDL_DestroyWindow( app.window );
        SDL_Quit( );
        return 0;
    }

    app.running = true;

    return 1;
}


int main( int argc, char *argv[] )
{
    if( !init( ) )
    {
        return -1;
    }

    while( app.running )
    {
        // Handle events
        processInput(  );
        render(  );

    }
    close( );

    return 0;
}

void processInput( )
{
    while( SDL_PollEvent( &app.event ) != 0 )
    {
        switch( app.event.type )
        {
            case SDL_MOUSEMOTION:
                // update mouse position
                app.uiState.mouseX = app.event.motion.x;
                app.uiState.mouseY = app.event.motion.y;
                break;
            case SDL_MOUSEBUTTONDOWN:
                // update button down state if left-clicking
                if( app.event.button.button == 1 )
                    app.uiState.mouseDown = 1;
                break;
            case SDL_MOUSEBUTTONUP:
                // update button down state if left-clicking
                if( app.event.button.button == 1 )
                    app.uiState.mouseDown = 0;
                break;
            case SDL_KEYUP:
                switch( app.event.key.keysym.sym )
                {
                    case SDLK_ESCAPE:
                        // If escape is pressed, return (and thus, quit)
                        app.running = false;
                }
                break;
            case SDL_QUIT:
                app.running = false;
                break;
        }
    }
}

void render( )
{
    static Uint32 bgColor = 0xFF161616;

    int sliderValue = bgColor & 0xFF;

    SDL_SetRenderDrawColor( app.renderer, (bgColor >> 16) & 0xFF, (bgColor >> 8) & 0xFF, (bgColor) & 0xFF, (bgColor >> 24) & 0xFF ); 
    SDL_RenderClear( app.renderer );

    imgui_prepare();

    button(GEN_ID,50,50);
    button(GEN_ID, 150, 50);

    if( button(GEN_ID,50,150)) bgColor = (SDL_GetTicks() * 0xc0cac01a) | 0xFF161616;

    if( button(GEN_ID, 150,150)) exit(0);

    if (checkbox(GEN_ID, 300, 50))
    {
        if( slider( GEN_ID, 450, 40, 255, sliderValue ) ) bgColor = ( bgColor & 0xFFFF00 ) | sliderValue;
        sliderValue = ( ( bgColor >> 10 ) & 0x3F );

        if( slider( GEN_ID, 500, 40, 63, sliderValue ) ) bgColor = ( bgColor & 0xff00ff ) | ( sliderValue << 10 );
        sliderValue = ( ( bgColor >> 20 ) & 0xf );

        if( slider( GEN_ID, 550, 40, 15, sliderValue ) ) bgColor = ( bgColor & 0x00ffff ) | ( sliderValue << 20 );
    }

    imgui_finish( );

    // Update screen
    SDL_RenderPresent( app.renderer );
}

void close( )
{
    SDL_DestroyRenderer( app.renderer );
    SDL_DestroyWindow( app.window );
    SDL_Quit( );
}

void drawRect(int x, int y, int w, int h, int color)
{
    // 0xFFFF0000
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;

    Uint8 a = (color >> 24) & 0xFF;
    Uint8 r = (color >> 16) & 0xFF;
    Uint8 g = (color >> 8) & 0xFF;
    Uint8 b = (color) & 0xFF;

    SDL_SetRenderDrawColor( app.renderer, r, g, b, a );
    SDL_RenderFillRect(app.renderer, &rect);
}

// Check whether current mouse position is within a rectangle
int regionHit(  int x, int y, int w, int h )
{
    if( app.uiState.mouseX < x || app.uiState.mouseY < y || app.uiState.mouseX >= x + w || app.uiState.mouseY >= y + h )
    {
        return 0;
    }
    return 1;
}

// Simple button IMGUI widget
int button( int id, int x, int y )
{
    // Check whether the button should be hot
    if( regionHit(x, y, 64, 48 ) )
    {
        app.uiState.hotItem = id;
        if( app.uiState.activeItem == 0 && app.uiState.mouseDown )
        {
            app.uiState.activeItem = id;
        }
    }


    // Render button
    drawRect(x+8, y+8, 64, 48, 0);
    if(app.uiState.hotItem == id)
    {
        if(app.uiState.activeItem == id)
        {
            // Button is both 'hot' and 'active'
            drawRect(x+2, y+2, 64, 48, 0xFFFFFFFF);
        }
        else
        {
            // Button is merely 'hot'
            drawRect(x, y, 64, 48, 0xFFFFFFFF);
        }
    }
    else
    {
        // button is not hot, but it may be active
        drawRect(x, y, 64, 48, 0xAAAAAAAA);
    }

    // If button is hot and active, but mouse button is not
    // down, the user must have clicked the button.
    if(app.uiState.mouseDown == 0 && app.uiState.hotItem == id && app.uiState.activeItem == id)
    {
        return 1;
    }

    // Otherwise, no clicky.
    return 0;
}

void imgui_prepare()
{
    app.uiState.hotItem = 0;
}

void imgui_finish()
{
    if(app.uiState.mouseDown == 0)
    {
        app.uiState.activeItem = 0;
    }
    else
    {
        if(app.uiState.activeItem == 0)
        {
            app.uiState.activeItem = -1;
        }
    }
}

// Simple scroll bar IMGUI widget
int slider(int id, int x, int y, int max, int &value)
{
    // Calculate the mouse cursor´s relative y offset
    int yPos = ((256-16) * value) / max;

    // Check for hotness
    if( regionHit(x+8,y+8, 16, 255))
    {
        app.uiState.hotItem = id;
        if(app.uiState.activeItem == 0 && app.uiState.mouseDown)
        {
            app.uiState.activeItem = id;
        }
    }

    drawRect(x,y, 32, 256+16, 0x777777);

    if(app.uiState.activeItem == id || app.uiState.hotItem == id)
    {
        drawRect(x+8, y+8 + yPos, 16, 16, 0xFFFFFF);
    }
    else
    {
        drawRect(x+8, y+8 + yPos, 16,16, 0xAAAAAA);
    }

    // Update widget value
    if (app.uiState.activeItem == id)
    {
        int mousePos = app.uiState.mouseY - (y + 8);
        if (mousePos < 0) mousePos = 0;
        if (mousePos > 255) mousePos = 255;
        int v = (mousePos * max) / 255;
        if (v != value)
        {
            value = v;
            return 1;
        }
    }

    return 0;
}

// Simple checkbox IMGUI widget
int checkbox(int id, int x, int y)
{
    static int isChecked = 0;
    const int boxSize = 20; // Size of the checkbox

    // Check if the checkbox area is hit
    if (regionHit(x, y, boxSize, boxSize)) {
        app.uiState.hotItem = id;
        if (app.uiState.activeItem == 0 && app.uiState.mouseDown) {
            app.uiState.activeItem = id;
        }
    }

    // Render checkbox background
    drawRect(x, y, boxSize, boxSize, 0x77777777); // Light grey background

    // If checkbox is hot and active, but mouse button is not down, toggle the state
    if (!app.uiState.mouseDown && app.uiState.hotItem == id && app.uiState.activeItem == id) {
        isChecked = !(isChecked); // Toggle
        app.uiState.activeItem = 0; // Reset
    }

    // If the checkbox is checked, draw an inner box to indicate it's checked
    if (isChecked) {
        drawRect(x + 4, y + 4, boxSize - 8, boxSize - 8, 0xFFFFFFFF); // White inner box for checked state
    }

    // Reset hot and active items if mouse is not down
    if (!app.uiState.mouseDown) {
        app.uiState.activeItem = 0;
    }

    return isChecked;
}
