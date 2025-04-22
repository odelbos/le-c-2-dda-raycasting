#include <math.h>
#include <raylib.h>

#define BACKGROUND CLITERAL(Color){20, 20, 20, 255}

#define WINDOW_WIDTH   960
#define WINDOW_HEIGHT  540

#define WORLD_WIDTH    8
#define WORLD_HEIGHT   8

int world[WORLD_HEIGHT][WORLD_WIDTH] = {
    {1, 0, 0, 0, 0, 0, 2, 3},
    {0, 0, 1, 2, 1, 0, 0, 1},
    {0, 0, 0, 0, 1, 0, 0, 1},
    {0, 0, 0, 0, 3, 0, 0, 3},
    {0, 0, 0, 2, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}
};

Color wall_colors[] = {
    RED,
    BLUE,
    GREEN,
    MAGENTA
};

typedef struct {
    float x, y;
} Vec2;

Vec2 vec2_add(Vec2 a, Vec2 b)
{
    return (Vec2){a.x + b.x, a.y + b.y};
}

Vec2 vec2_sub(Vec2 a, Vec2 b)
{
    return (Vec2){a.x - b.x, a.y - b.y};
}

Vec2 vec2_mul(Vec2 a, Vec2 b)
{
    return (Vec2){a.x * b.x, a.y * b.y};
}

Vec2 vec2_rotate(Vec2 v, float angle)
{
    float ca = cosf(angle);
    float sa = sinf(angle);
    return (Vec2){v.x*ca - v.y*sa, v.x*sa + v.y*ca};
}

typedef struct {
    Vec2 pos;
    int w, h;
    Vec2 ratio;
} Map;

typedef struct {
    Vec2 dir;            // Camera direction
    Vec2 plane;          // Camera projection plane
} Cam;

// Convert world coordinates to map coordinates
Vec2 world_to_map(Map map, Vec2 v)
{
    return vec2_add(vec2_mul(v, map.ratio), map.pos);
}

void render_map(Map map)
{
    DrawRectangle(map.pos.x, map.pos.y, map.w, map.h, BLACK);

    float cell_width = (float)map.w / (float)WORLD_WIDTH;
    float cell_height = (float)map.h / (float)WORLD_HEIGHT;

    // Draw walls
    for (int y = 0; y < WORLD_HEIGHT; y++) {
        for (int x = 0; x < WORLD_WIDTH; x++) {
            int wall = world[y][x];
            if (wall != 0) {
                DrawRectangle(
                    (int)(x * cell_width) + map.pos.x, (int)(y * cell_height) + map.pos.y,
                    (int)(cell_width), (int)(cell_height),
                    wall_colors[wall]);
            }
        }
    }

    int at;
    // Vertical grid lines
    for (int x = 0; x <= WORLD_WIDTH; x++) {
        at = (int)(x * cell_width);
        DrawLine(at + map.pos.x, map.pos.y, at + map.pos.x, map.h + map.pos.y, GRAY);
    }
    // Horizontal grid lines
    for (int y = 0; y <= WORLD_HEIGHT; y++) {
        at = (int)(y * cell_height);
        DrawLine(map.pos.x, at + map.pos.y, map.w + map.pos.x, at + map.pos.y, GRAY);
    }
}

void render_map_player(Map map, Cam camera, Vec2 player)
{
    Vec2 pp = world_to_map(map, player);
    DrawCircle((int)pp.x, (int)pp.y, 5.0f, RED);

    Vec2 dp = vec2_add(camera.dir, player);
    Vec2 mp = world_to_map(map, dp);
    DrawLine((int)pp.x, (int)pp.y, (int)mp.x, (int)mp.y, BLUE);
}

int main(void)
{
    Vec2 move_speed = {0.025f, 0.025f};
    float rot_speed = 0.02f;

    // Define mini map position and size
    Map map = {{20, 20}, 300, 300, {0, 0}};
    map.ratio.x = (float)map.w / (float)WORLD_WIDTH;
    map.ratio.y = (float)map.h / (float)WORLD_HEIGHT;

    Vec2 player = {5.5f, 6.5f};

    Cam camera = {
        {-1, 0},
        {0, -0.66},
    };

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Raycasting in C");
    SetTargetFPS(60);

    while ( ! WindowShouldClose()) {
        if (IsKeyDown(KEY_UP)) {
            player = vec2_add(player, vec2_mul(camera.dir, move_speed));
        }
        else if (IsKeyDown(KEY_DOWN)) {
            player = vec2_sub(player, vec2_mul(camera.dir, move_speed));
        }
        else if (IsKeyDown(KEY_LEFT)) {
            camera.dir = vec2_rotate(camera.dir, -rot_speed);
            camera.plane = vec2_rotate(camera.plane, -rot_speed);
        }
        else if (IsKeyDown(KEY_RIGHT)) {
            camera.dir = vec2_rotate(camera.dir, rot_speed);
            camera.plane = vec2_rotate(camera.plane, rot_speed);
        }

        BeginDrawing();
        ClearBackground(BACKGROUND);
        render_map(map);
        render_map_player(map, camera, player);
        EndDrawing();
    }

  CloseWindow();
  return 0;
}
