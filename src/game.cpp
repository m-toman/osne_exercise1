#include <iostream>
#include <random>

#include "gfx.h"

namespace game {

struct Level {
   float road_len = 300.0f;
   float road_width = 10.0f;
   float tree_dist = 10.0f;
};

class Ryder {
   Vector3 velocity = {0.0f, 0.0f, 0.0f};
   // C. Jarmack: this could be a C++ weak pointer, what happens
   // when the scene graph deletes it and we dont know?
   // we'll access garbage and in the best case crash
   // at least the ownership model is clearly documented in "Scene"
   gfx::Object* model; 
  public:
   Ryder(const Vector3& pos, float scale, const Color& color) {
      model = new gfx::Car(pos, scale, color);
   }

   void accelerate(float x, float y, float z) {
      velocity.x += x;
      velocity.y += y;
      velocity.z += z;
   }

   void update(float time_passed) {
      model->translate(
         velocity.x * time_passed,
         velocity.y * time_passed,
         velocity.z * time_passed);
   }

   const Vector3& get_position() {
      return model->get_position();
   }

   gfx::Object* get_model() {
      return model;
   }
};

void init() {
   int screenWidth = 1024;
   int screenHeight = 768;

   InitWindow(screenWidth, screenHeight, "Ryskim");
   SetTargetFPS(60);
}

void shutdown() {
   CloseWindow();
}

Camera get_default_camera() {
   // Define the camera to look into our 3d world
   Camera camera = {0};
   camera.position = (Vector3){0.0f, 15.0f, 10.0f};
   camera.target = (Vector3){0.0f, 0.0f, 0.0f};
   camera.up = (Vector3){0.0f, 1.0f, 0.0f};
   camera.fovy = 45.0f;
   camera.type = CAMERA_PERSPECTIVE;

   return camera;
}

void generate_trees(gfx::Scene& scene, const Level& level) {
   std::default_random_engine generator;
   std::normal_distribution<float> height_distribution(5.0, 0.01);
   std::normal_distribution<float> width_distribution(3.0, 0.2);
   auto tree_dist = level.road_width / 2.0f + 1.0f;
   int tree_num = 0;

   for (float posz = 0.0f; posz >= -level.road_len; posz -= level.tree_dist) {
      Vector3 treepos_left = {-tree_dist, 0.0f, posz};
      auto tree_left = new gfx::Tree(treepos_left,
                              height_distribution(generator),
                              width_distribution(generator));
      scene.add_object(tree_left);
      Vector3 treepos_right = {tree_dist, 0.0f, posz};
      auto tree_right = new gfx::Tree(treepos_right,
                              height_distribution(generator),
                              width_distribution(generator));
      scene.add_object(tree_right);
      tree_num += 2;
   }
   std::cout << "Planting " << tree_num << " trees." << std::endl;
}

void run() {
   init();

   // generate level and renderer scene
   Level level;
   gfx::Scene scene(get_default_camera());
   float cam_distance = 15.0f;

   // build the road
   auto road = new gfx::Road(10.0f, -level.road_len, level.road_width);
   scene.add_object(road);

   // plant a couple trees for visual pleasure
   generate_trees(scene, level);

   Vector3 playerPosition = {0.0f, 0.5f, 2.0f};
   Ryder ryder(playerPosition, 1.0f, RED);
   scene.add_object(ryder.get_model());

   bool collision = false;

   while (!WindowShouldClose()) {
      // Update
      //----------------------------------------------------------------------------------

      // Move player
      if (IsKeyDown(KEY_RIGHT))
         //ryder.accelerate()
         ryder.accelerate(1.0f, 0.0f, 0.0f);
      if (IsKeyDown(KEY_LEFT))
         ryder.accelerate(-1.0f, 0.0f, 0.0f);
      if (IsKeyDown(KEY_DOWN))
         ryder.accelerate(0.0f, 0.0f, 1.0f);
      if (IsKeyDown(KEY_UP))
         ryder.accelerate(0.0f, 0.0f, -1.0f);
      ryder.update(GetFrameTime());

      // follow cam
      scene.get_camera().position.z = ryder.get_position().z + cam_distance;
      scene.get_camera().target.z = ryder.get_position().z;

      // collisions
      collision = false;

      /*
      // Check collisions player vs enemy-box
      if (CheckCollisionBoxes(
              (BoundingBox){(Vector3){playerPosition.x - playerSize.x / 2,
                                      playerPosition.y - playerSize.y / 2,
                                      playerPosition.z - playerSize.z / 2},
                            (Vector3){playerPosition.x + playerSize.x / 2,
                                      playerPosition.y + playerSize.y / 2,
                                      playerPosition.z + playerSize.z / 2}},
              (BoundingBox){(Vector3){enemyBoxPos.x - enemyBoxSize.x / 2,
                                      enemyBoxPos.y - enemyBoxSize.y / 2,
                                      enemyBoxPos.z - enemyBoxSize.z / 2},
                            (Vector3){enemyBoxPos.x + enemyBoxSize.x / 2,
                                      enemyBoxPos.y + enemyBoxSize.y / 2,
                                      enemyBoxPos.z + enemyBoxSize.z / 2}}))
         collision = true;

      // Check collisions player vs enemy-sphere
      if (CheckCollisionBoxSphere(
              (BoundingBox){(Vector3){playerPosition.x - playerSize.x / 2,
                                      playerPosition.y - playerSize.y / 2,
                                      playerPosition.z - playerSize.z / 2},
                            (Vector3){playerPosition.x + playerSize.x / 2,
                                      playerPosition.y + playerSize.y / 2,
                                      playerPosition.z + playerSize.z / 2}},
              enemySpherePos, enemySphereSize))
         collision = true;

      if (collision)
         playerColor = RED;
      else
         playerColor = GREEN;
      */
      //----------------------------------------------------------------------------------

      // Draw
      //----------------------------------------------------------------------------------
      BeginDrawing();

      ClearBackground(RAYWHITE);

      BeginMode3D(scene.get_camera());

      scene.draw();

      //DrawGrid(100, 1.0f); // Draw a grid

      EndMode3D();

      //DrawText("Move player with cursors to collide", 220, 40, 20, GRAY);

      DrawFPS(10, 10);

      EndDrawing();
   }

   shutdown();
}

} // namespace game