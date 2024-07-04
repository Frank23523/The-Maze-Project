#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define WINDOW_TITLE "Ray Casted"
#define GREY_COLOR 128
#define PLAYER_SIZE 10
#define YELLOW_COLOR 255, 255, 0
#define PLAYER_SPEED 5
#define MAP_WIDTH 8
#define MAP_HEIGHT 8
#define MAP_WALL 1
#define MAP_FLOOR 0
#define TILE_SIZE 128
#define SCREEN_WIDTH (MAP_WIDTH * TILE_SIZE)
#define SCREEN_HEIGHT (MAP_HEIGHT * TILE_SIZE)
#define PROJECTION_PLANE_DISTANCE ((WINDOW_WIDTH / 2) / tan(DEGREE_TO_RADIAN(FOV_DEGREES / 2)))
#define PI 3.14159265
#define ROTATION_SPEED 0.1
#define FOV_DEGREES 60
#define NUM_RAYS WINDOW_WIDTH
#define DEGREE_TO_RADIAN(deg) ((deg) * PI / 180.0)
#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 500
#define MAX_LINE_HEIGHT 500
#define MAP_WALL_CHAR '1'
#define MAP_FLOOR_CHAR '0'

struct Player {
    float x;
    float y;
    float dirX;
    float dirY;
    float angle;
};

struct Game {
    SDL_Window *window;
    SDL_Renderer *renderer;
    struct Player player;
    int map[MAP_WIDTH * MAP_HEIGHT];
    float rotation_speed;
    bool show_map;
};

void game_cleanup(struct Game *game, int exit_status);
bool sdl_initialize(struct Game *game, const char *map_file_path);
void draw_player(SDL_Renderer *renderer, struct Player *player);
void draw_map(SDL_Renderer *renderer, int *map);
void cast_single_ray(struct Game *game, float ray_angle, int ray_index, float *ray_end_x, float *ray_end_y);
void cast_rays(struct Game *game, float ray_ends_x[NUM_RAYS], float ray_ends_y[NUM_RAYS]);
void draw_2d_map(struct Game *game);
void draw_3d_view(struct Game *game, float ray_angle, float distance, int ray_index, int is_vertical);
void draw_rays_on_map(SDL_Renderer *renderer, struct Player *player, float ray_ends_x[NUM_RAYS], float ray_ends_y[NUM_RAYS]);
bool parse_map_file(struct Game *game, const char *file_path);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <map_file_path>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *map_file_path = argv[1];

    struct Game game = {
        .window = NULL,
        .renderer = NULL
    };

    if (sdl_initialize(&game, map_file_path))
    {
        game_cleanup(&game, EXIT_FAILURE);
    }

    float ray_ends_x[NUM_RAYS];
    float ray_ends_y[NUM_RAYS];

    while (true)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    game_cleanup(&game, EXIT_SUCCESS);
                    break;
                case SDL_KEYDOWN:
                    {
                        float newX = game.player.x;
                        float newY = game.player.y;
                        switch (event.key.keysym.scancode)
                        {
                            case SDL_SCANCODE_ESCAPE:
                                game_cleanup(&game, EXIT_SUCCESS);
                                break;
                            case SDL_SCANCODE_UP:
                                newX += game.player.dirX * PLAYER_SPEED;
                                newY += game.player.dirY * PLAYER_SPEED;
                                break;
                            case SDL_SCANCODE_DOWN:
                                newX -= game.player.dirX * PLAYER_SPEED;
                                newY -= game.player.dirY * PLAYER_SPEED;
                                break;
                            case SDL_SCANCODE_W:
                                game.player.angle -= game.rotation_speed;
                                break;
                            case SDL_SCANCODE_S:
                                game.player.angle += game.rotation_speed;
                                break;
                            case SDL_SCANCODE_A:
                                game.player.angle -= game.rotation_speed;
                                break;
                            case SDL_SCANCODE_D:
                                game.player.angle += game.rotation_speed;
                                break;
                            case SDL_SCANCODE_M:
                                game.show_map = !game.show_map;
                                break;
                            default:
                                break;
                        }
                        // Normalize the angle
                        game.player.angle = fmodf(game.player.angle, 2*PI);
                        if (game.player.angle < 0) game.player.angle += 2*PI;

                        // Update direction vectors
                        game.player.dirX = cosf(game.player.angle);
                        game.player.dirY = sinf(game.player.angle);

                        // Check for collision (only for movement, not rotation)
                        if (event.key.keysym.scancode == SDL_SCANCODE_UP || event.key.keysym.scancode == SDL_SCANCODE_DOWN)
                        {
                            float buffer = PLAYER_SIZE / 2.0f;
                            int map_x_forward = (int)((newX + buffer * game.player.dirX) / TILE_SIZE);
                            int map_y_forward = (int)((newY + buffer * game.player.dirY) / TILE_SIZE);
                            int map_x_backward = (int)((newX - buffer * game.player.dirX) / TILE_SIZE);
                            int map_y_backward = (int)((newY - buffer * game.player.dirY) / TILE_SIZE);

                            if (game.map[map_y_forward * MAP_WIDTH + map_x_forward] == MAP_FLOOR &&
                                game.map[map_y_backward * MAP_WIDTH + map_x_backward] == MAP_FLOOR)
                            {
                                game.player.x = newX;
                                game.player.y = newY;
                            }
                        }
                    }
                    break;
                default:
                    break;
            }
        }
        // Clear the renderer
        SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 255);
        SDL_RenderClear(game.renderer);

        // Draw the 3D view (this should not draw any rays)
        cast_rays(&game, ray_ends_x, ray_ends_y);

        // Reset viewport to full window
        SDL_RenderSetViewport(game.renderer, NULL);

        if (game.show_map) {
            draw_2d_map(&game);
        }

        // Present the renderer
        SDL_RenderPresent(game.renderer);

        SDL_Delay(16);
    }

    game_cleanup(&game, EXIT_SUCCESS);

    return 0;
}


bool parse_map_file(struct Game *game, const char *file_path)
{
    FILE *file = fopen(file_path, "r");
    if (!file)
    {
        fprintf(stderr, "Error opening map file: %s\n", file_path);
        return false;
    }

    char line[MAP_WIDTH + 2]; // +2 to account for the newline and null terminator
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        if (fgets(line, sizeof(line), file) == NULL)
        {
            fprintf(stderr, "Error reading map file or incorrect format.\n");
            fclose(file);
            return false;
        }

        for (int x = 0; x < MAP_WIDTH; x++)
        {
            if (line[x] == '1')
            {
                game->map[y * MAP_WIDTH + x] = MAP_WALL;
            }
            else if (line[x] == '0')
            {
                game->map[y * MAP_WIDTH + x] = MAP_FLOOR;
            }
            else
            {
                fprintf(stderr, "Invalid character in map file at line %d, column %d.\n", y + 1, x + 1);
                fclose(file);
                return false;
            }
        }
    }

    fclose(file);
    return true;
}


// Other functions remain unchanged...
void cast_rays(struct Game *game, float ray_ends_x[NUM_RAYS], float ray_ends_y[NUM_RAYS])
{
    float ray_angle = game->player.angle - DEGREE_TO_RADIAN(FOV_DEGREES / 2);
    float angle_step = DEGREE_TO_RADIAN(FOV_DEGREES) / (NUM_RAYS - 1);

    for (int i = 0; i < NUM_RAYS; i++)
    {
        cast_single_ray(game, ray_angle, i, &ray_ends_x[i], &ray_ends_y[i]);
        ray_angle += angle_step;
    }
}


void cast_single_ray(struct Game *game, float ray_angle, int ray_index, float *ray_end_x, float *ray_end_y)
{
    float ray_x, ray_y;
    float horiz_x, horiz_y, vert_x, vert_y;
    float horiz_distance = INFINITY, vert_distance = INFINITY;

    // Normalize the angle
    ray_angle = fmodf(ray_angle, 2*PI);
    if (ray_angle < 0) ray_angle += 2*PI;

    // Determine direction
    int facing_down = (ray_angle > 0 && ray_angle < PI);
    int facing_right = (ray_angle < 0.5*PI || ray_angle > 1.5*PI);

    // Horizontal ray-grid intersection
    {
        float y_intercept = floor(game->player.y / TILE_SIZE) * TILE_SIZE;
        y_intercept += facing_down ? TILE_SIZE : 0;

        float x_intercept = game->player.x + (y_intercept - game->player.y) / tan(ray_angle);

        float y_step = TILE_SIZE;
        y_step *= facing_down ? 1 : -1;

        float x_step = TILE_SIZE / tan(ray_angle);
        x_step *= (facing_right && x_step < 0) ? -1 : 1;
        x_step *= (!facing_right && x_step > 0) ? -1 : 1;

        float next_horiz_x = x_intercept;
        float next_horiz_y = y_intercept;

        while (next_horiz_x >= 0 && next_horiz_x < SCREEN_WIDTH && next_horiz_y >= 0 && next_horiz_y < SCREEN_HEIGHT)
        {
            float x_to_check = next_horiz_x;
            float y_to_check = next_horiz_y + (facing_down ? 0 : -1);

            if (x_to_check < 0 || x_to_check >= MAP_WIDTH * TILE_SIZE || y_to_check < 0 || y_to_check >= MAP_HEIGHT * TILE_SIZE)
                break;

            int map_x = (int)(x_to_check / TILE_SIZE);
            int map_y = (int)(y_to_check / TILE_SIZE);

            if (game->map[map_y * MAP_WIDTH + map_x] == MAP_WALL)
            {
                horiz_x = next_horiz_x;
                horiz_y = next_horiz_y;
                horiz_distance = sqrt(pow(horiz_x - game->player.x, 2) + pow(horiz_y - game->player.y, 2));
                break;
            }

            next_horiz_x += x_step;
            next_horiz_y += y_step;
        }
    }

    // Vertical ray-grid intersection
    {
        float x_intercept = floor(game->player.x / TILE_SIZE) * TILE_SIZE;
        x_intercept += facing_right ? TILE_SIZE : 0;

        float y_intercept = game->player.y + (x_intercept - game->player.x) * tan(ray_angle);

        float x_step = TILE_SIZE;
        x_step *= facing_right ? 1 : -1;

        float y_step = TILE_SIZE * tan(ray_angle);
        y_step *= (facing_down && y_step < 0) ? -1 : 1;
        y_step *= (!facing_down && y_step > 0) ? -1 : 1;

        float next_vert_x = x_intercept;
        float next_vert_y = y_intercept;

        while (next_vert_x >= 0 && next_vert_x < SCREEN_WIDTH && next_vert_y >= 0 && next_vert_y < SCREEN_HEIGHT)
        {
            float x_to_check = next_vert_x + (facing_right ? 0 : -1);
            float y_to_check = next_vert_y;

            if (x_to_check < 0 || x_to_check >= MAP_WIDTH * TILE_SIZE || y_to_check < 0 || y_to_check >= MAP_HEIGHT * TILE_SIZE)
                break;

            int map_x = (int)(x_to_check / TILE_SIZE);
            int map_y = (int)(y_to_check / TILE_SIZE);

            if (game->map[map_y * MAP_WIDTH + map_x] == MAP_WALL)
            {
                vert_x = next_vert_x;
                vert_y = next_vert_y;
                vert_distance = sqrt(pow(vert_x - game->player.x, 2) + pow(vert_y - game->player.y, 2));
                break;
            }

            next_vert_x += x_step;
            next_vert_y += y_step;
        }
    }

    // Choose the closer intersection
    float distance;
    int is_vertical;
    if (horiz_distance < vert_distance) {
        ray_x = horiz_x;
        ray_y = horiz_y;
        distance = horiz_distance;
        is_vertical = 0;
    } else {
        ray_x = vert_x;
        ray_y = vert_y;
        distance = vert_distance;
        is_vertical = 1;
    }

    // Store the ray endpoint
   *ray_end_x = ray_x;
    *ray_end_y = ray_y;

    // Draw the 3D view (without drawing the ray)
    draw_3d_view(game, ray_angle, distance, ray_index, is_vertical);
}


void draw_3d_view(struct Game *game, float ray_angle, float distance, int ray_index, int is_vertical)
{
    float fish_eye_correction = cos(ray_angle - game->player.angle);
    float corrected_distance = distance * fish_eye_correction;

    float line_height = (TILE_SIZE / corrected_distance) * PROJECTION_PLANE_DISTANCE;
    
    if (line_height > MAX_LINE_HEIGHT) {
        line_height = MAX_LINE_HEIGHT;
    }

    float line_offset = (WINDOW_HEIGHT - line_height) / 2;

    int strip_width = WINDOW_WIDTH / NUM_RAYS;
    SDL_Rect wall_strip = {
        .x = ray_index * strip_width,
        .y = line_offset,
        .w = strip_width,
        .h = line_height
    };

    // Calculate shade based on distance
    float shade_factor = 1.0 - (corrected_distance / (MAP_WIDTH * TILE_SIZE));
    shade_factor = fmaxf(0.3f, shade_factor); // Ensure minimum brightness
    if (shade_factor > 1) shade_factor = 1;

    // Choose base color based on wall orientation
    int r, g, b;
    if (is_vertical) {
        // EAST and WEST walls (vertical)
        r = 200;
        g = 100;
        b = 100;
    } else {
        // NORTH and SOUTH walls (horizontal)
        r = 100;
        g = 200;
        b = 100;
    }

    // Apply shading
    r = (int)(r * shade_factor);
    g = (int)(g * shade_factor);
    b = (int)(b * shade_factor);

    SDL_SetRenderDrawColor(game->renderer, r, g, b, 255);
    SDL_RenderFillRect(game->renderer, &wall_strip);
}


void draw_2d_map(struct Game *game)
{
    int map_size = WINDOW_HEIGHT / 4;
    int tile_size = map_size / MAP_HEIGHT;
    int map_x = WINDOW_WIDTH - map_size;
    int map_y = WINDOW_HEIGHT - map_size;

    // Draw map background
    SDL_Rect map_rect = {map_x, map_y, map_size, map_size};
    SDL_SetRenderDrawColor(game->renderer, 100, 100, 100, 128);
    SDL_RenderFillRect(game->renderer, &map_rect);

    // Draw map tiles
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (game->map[y * MAP_WIDTH + x] == MAP_WALL) {
                SDL_Rect wall_rect = {
                    map_x + x * tile_size,
                    map_y + y * tile_size,
                    tile_size,
                    tile_size
                };
                SDL_SetRenderDrawColor(game->renderer, 200, 200, 200, 255);
                SDL_RenderFillRect(game->renderer, &wall_rect);
            }
        }
    }

    // Draw player (smaller size)
    int player_size = tile_size / 4;  // Reduced from tile_size / 2
    int player_x = map_x + (game->player.x * tile_size / TILE_SIZE) - player_size / 2;
    int player_y = map_y + (game->player.y * tile_size / TILE_SIZE) - player_size / 2;
    SDL_Rect player_rect = {player_x, player_y, player_size, player_size};
    SDL_SetRenderDrawColor(game->renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(game->renderer, &player_rect);

    // Draw player's field of view (60 degrees)
    float ray_length = tile_size * 3;  // Adjust this value to change ray length
    float angle_step = DEGREE_TO_RADIAN(FOV_DEGREES) / 19;  // 20 lines in total

    SDL_SetRenderDrawColor(game->renderer, 0, 255, 0, 255);

    // Draw rays to the left of the player's view angle
    for (int i = 1; i <= 6; i++) {
        float ray_angle = game->player.angle - i * angle_step;
        float end_x = game->player.x + cosf(ray_angle) * ray_length;
        float end_y = game->player.y + sinf(ray_angle) * ray_length;

        // Cast ray and check for collisions with walls
        float ray_dx = end_x - game->player.x;
        float ray_dy = end_y - game->player.y;
        float ray_distance = sqrt(ray_dx * ray_dx + ray_dy * ray_dy);

        // Step through the ray to check for collisions
        float step_x = ray_dx / ray_distance;
        float step_y = ray_dy / ray_distance;

        float ray_x = game->player.x;
        float ray_y = game->player.y;

        while (ray_x >= 0 && ray_x < MAP_WIDTH * TILE_SIZE &&
               ray_y >= 0 && ray_y < MAP_HEIGHT * TILE_SIZE) {
            int map_x = (int)(ray_x / TILE_SIZE);
            int map_y = (int)(ray_y / TILE_SIZE);

            if (game->map[map_y * MAP_WIDTH + map_x] == MAP_WALL) {
                break; // Hit a wall, stop the ray
            }

            ray_x += step_x;
            ray_y += step_y;
        }

        // Draw the ray
        int ray_end_x = map_x + (ray_x * tile_size / TILE_SIZE);
        int ray_end_y = map_y + (ray_y * tile_size / TILE_SIZE);
        SDL_RenderDrawLine(game->renderer,
                           player_x + player_size / 2,
                           player_y + player_size / 2,
                           ray_end_x,
                           ray_end_y);
    }

    // Draw rays in the middle (directly ahead of the player)
    for (int i = -5; i <= 5; i++) {
        if (i == 0) continue; // Skip the center ray (already drawn)
        
        float ray_angle = game->player.angle + i * angle_step;
        float end_x = game->player.x + cosf(ray_angle) * ray_length;
        float end_y = game->player.y + sinf(ray_angle) * ray_length;

        // Cast ray and check for collisions with walls
        float ray_dx = end_x - game->player.x;
        float ray_dy = end_y - game->player.y;
        float ray_distance = sqrt(ray_dx * ray_dx + ray_dy * ray_dy);

        // Step through the ray to check for collisions
        float step_x = ray_dx / ray_distance;
        float step_y = ray_dy / ray_distance;

        float ray_x = game->player.x;
        float ray_y = game->player.y;

        while (ray_x >= 0 && ray_x < MAP_WIDTH * TILE_SIZE &&
               ray_y >= 0 && ray_y < MAP_HEIGHT * TILE_SIZE) {
            int map_x = (int)(ray_x / TILE_SIZE);
            int map_y = (int)(ray_y / TILE_SIZE);

            if (game->map[map_y * MAP_WIDTH + map_x] == MAP_WALL) {
                break; // Hit a wall, stop the ray
            }

            ray_x += step_x;
            ray_y += step_y;
        }

        // Draw the ray
        int ray_end_x = map_x + (ray_x * tile_size / TILE_SIZE);
        int ray_end_y = map_y + (ray_y * tile_size / TILE_SIZE);
        SDL_RenderDrawLine(game->renderer,
                           player_x + player_size / 2,
                           player_y + player_size / 2,
                           ray_end_x,
                           ray_end_y);
    }

    // Draw rays to the right of the player's view angle
    for (int i = 6; i <= 11; i++) {
        float ray_angle = game->player.angle + i * angle_step;
        float end_x = game->player.x + cosf(ray_angle) * ray_length;
        float end_y = game->player.y + sinf(ray_angle) * ray_length;

        // Cast ray and check for collisions with walls
        float ray_dx = end_x - game->player.x;
        float ray_dy = end_y - game->player.y;
        float ray_distance = sqrt(ray_dx * ray_dx + ray_dy * ray_dy);

        // Step through the ray to check for collisions
        float step_x = ray_dx / ray_distance;
        float step_y = ray_dy / ray_distance;

        float ray_x = game->player.x;
        float ray_y = game->player.y;

        while (ray_x >= 0 && ray_x < MAP_WIDTH * TILE_SIZE &&
               ray_y >= 0 && ray_y < MAP_HEIGHT * TILE_SIZE) {
            int map_x = (int)(ray_x / TILE_SIZE);
            int map_y = (int)(ray_y / TILE_SIZE);

            if (game->map[map_y * MAP_WIDTH + map_x] == MAP_WALL) {
                break; // Hit a wall, stop the ray
            }

            ray_x += step_x;
            ray_y += step_y;
        }

        // Draw the ray
        int ray_end_x = map_x + (ray_x * tile_size / TILE_SIZE);
        int ray_end_y = map_y + (ray_y * tile_size / TILE_SIZE);
        SDL_RenderDrawLine(game->renderer,
                           player_x + player_size / 2,
                           player_y + player_size / 2,
                           ray_end_x,
                           ray_end_y);
    }
}



void game_cleanup(struct Game *game, int exit_status)
{
    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);
    SDL_Quit();
    exit(exit_status);
}

bool sdl_initialize(struct Game *game, const char *map_file_path)
{
    if (SDL_Init(SDL_INIT_EVERYTHING))
    {
        fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
        return true;
    }

    game->window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED,
                        SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!game->window)
    {
        fprintf(stderr, "Error creating window: %s\n", SDL_GetError());
        return true;
    }

    game->renderer = SDL_CreateRenderer(game->window, -1, 0);
    if (!game->renderer)
    {
        fprintf(stderr, "Error creating renderer: %s\n", SDL_GetError());
        return true;
    }

    // Set initial player position
    game->player.x = TILE_SIZE + TILE_SIZE / 2;
    game->player.y = TILE_SIZE + TILE_SIZE / 2;
    game->player.angle = 0;
    game->player.dirX = cos(game->player.angle);
    game->player.dirY = sin(game->player.angle);

    if (!parse_map_file(game, map_file_path))
    {
        return true;
    }

    game->rotation_speed = 0.05f;
    game->show_map = false;

    return false;
}
