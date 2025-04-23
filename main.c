#include <stdio.h>
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
    Vec2 dir;                  // Camera direction
    Vec2 plane;                // Camera projection plane
} Cam;

typedef struct {
    Vec2 hit_pos;              // In world coordinates
    float ray_dist;
    int cell_x, cell_y, wall;
    int side;
} CastResult;

// Convert world coordinates to map coordinates
Vec2 world_to_map(Map map, Vec2 v)
{
    return vec2_add(vec2_mul(v, map.ratio), map.pos);
}

CastResult cast_ray(Vec2 ray_dir, Vec2 player)
{
    CastResult res = {{0, 0}, 0, 0, 0, 0, 0};

    res.cell_x = (int)player.x;
    res.cell_y = (int)player.y;

    float delta_x = (ray_dir.x == 0) ? 1e30 : fabs(1 / ray_dir.x);
    float delta_y = (ray_dir.y == 0) ? 1e30 : fabs(1 / ray_dir.y);

    int step_x = 1;
    int step_y = 1;
    float dist_x;
    float dist_y;

    if (ray_dir.x < 0) {
        step_x = -1;
        dist_x = (player.x - (float)res.cell_x) * delta_x;
    }
    else dist_x = ((float)res.cell_x + 1.0 - player.x) * delta_x;

    if (ray_dir.y < 0) {
        step_y = -1;
        dist_y = (player.y - (float)res.cell_y) * delta_y;
    }
    else dist_y = ((float)res.cell_y + 1.0 - player.y) * delta_y;

    int max = 0;
    while (max < 15 && res.wall == 0) {
        if (dist_x < dist_y) {
            dist_x += delta_x;
            res.cell_x += step_x;
            res.side = 0;
        }
        else {
            dist_y += delta_y;
            res.cell_y += step_y;
            res.side = 1;
        }

        if (res.cell_x >= 0 && res.cell_x < WORLD_WIDTH &&
            res.cell_y >= 0 && res.cell_y < WORLD_HEIGHT) {
            res.wall = world[res.cell_y][res.cell_x];
        }

        max++;
    }

    if (res.wall != 0) {
        if (res.side == 0) {
            res.hit_pos = vec2_mul(ray_dir, (Vec2){dist_x - delta_x, dist_x - delta_x});
            res.hit_pos = vec2_add(res.hit_pos, player);
            res.ray_dist = dist_x - delta_x;
        }
        else {
            res.hit_pos = vec2_mul(ray_dir, (Vec2){dist_y - delta_y, dist_y - delta_y});
            res.hit_pos = vec2_add(res.hit_pos, player);
            res.ray_dist = dist_y - delta_y;
        }
    }

    return res;
}

void render_world(Cam camera, Vec2 player)
{
    CastResult res;

    for (int x = 0; x < WINDOW_WIDTH; x++) {
        float depth = 2 * x / (float)WINDOW_WIDTH - 1;
        float ray_x = camera.dir.x + camera.plane.x * depth;
        float ray_y = camera.dir.y + camera.plane.y * depth;

        res = cast_ray((Vec2){ray_x, ray_y}, player);
        if (res.wall > 0) {
            int h = (int)(WINDOW_HEIGHT / res.ray_dist);

            int y1 = -h / 2 + WINDOW_HEIGHT / 2;
            if (y1 < 0) y1 = 0;

            int y2 = h / 2 + WINDOW_HEIGHT / 2;
            if (y2 >= WINDOW_HEIGHT) y2 = WINDOW_HEIGHT - 1;

            Color color = wall_colors[res.wall];
            if (res.side == 1) {
                color = ColorBrightness(color, 0.2f);
            }

            DrawLineEx((Vector2){x, y1}, (Vector2){x, y2}, 1.1f, color);
        }
    }
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

void render_rays_map(Map map, Cam camera, Vec2 player)
{
    Vec2 pp = world_to_map(map, player);

    for (int x = 0; x < WINDOW_WIDTH; x += 10) {
        float depth = 2 * x / (float)WINDOW_WIDTH - 1;
        float ray_x = camera.dir.x + camera.plane.x * depth;
        float ray_y = camera.dir.y + camera.plane.y * depth;
        Vec2 ray_dir = {ray_x, ray_y};

        CastResult res = cast_ray(ray_dir, player);
        if (res.wall != 0) {
            Vec2 mh = world_to_map(map, res.hit_pos);
            DrawLine((int)pp.x, (int)pp.y, (int)mh.x, (int)mh.y, GREEN);
            DrawCircle((int)mh.x, (int)mh.y, 2.5f, YELLOW);
        }
        else {
            Vec2 wr = vec2_add(player, ray_dir);
            Vec2 mr = world_to_map(map, wr);
            DrawLine((int)pp.x, (int)pp.y, (int)mr.x, (int)mr.y, ORANGE);
        }
    }
}

int main(void)
{
    bool show_map = true;
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
        else if (IsKeyPressed(KEY_M)) {
            show_map = !show_map;
        }

        BeginDrawing();
        ClearBackground(BACKGROUND);
        render_world(camera, player);
        if (show_map) {
            render_map(map);
            render_map_player(map, camera, player);
            render_rays_map(map, camera, player);
        }
        EndDrawing();
    }

  CloseWindow();
  return 0;
}
