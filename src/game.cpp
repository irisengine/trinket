////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "game.h"

#include <algorithm>
#include <any>
#include <chrono>
#include <cmath>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include "iris/core/camera.h"
#include "iris/core/colour.h"
#include "iris/core/error_handling.h"
#include "iris/core/looper.h"
#include "iris/core/random.h"
#include "iris/core/root.h"
#include "iris/core/transform.h"
#include "iris/events/event.h"
#include "iris/graphics/mesh_manager.h"
#include "iris/graphics/render_graph/component_node.h"
#include "iris/graphics/render_graph/conditional_node.h"
#include "iris/graphics/render_graph/render_graph.h"
#include "iris/graphics/render_graph/texture_node.h"
#include "iris/graphics/render_graph/value_node.h"
#include "iris/graphics/render_pipeline.h"
#include "iris/graphics/render_target_manager.h"
#include "iris/graphics/scene.h"
#include "iris/graphics/single_entity.h"
#include "iris/graphics/text_factory.h"
#include "iris/graphics/texture_manager.h"
#include "iris/graphics/window.h"
#include "iris/graphics/window_manager.h"
#include "iris/physics/physics_manager.h"
#include "iris/physics/physics_system.h"
#include "iris/physics/rigid_body.h"
#include "iris/physics/rigid_body_type.h"

#include "config.h"
#include "enemy.h"
#include "game_object.h"
#include "hud.h"
#include "input_handler.h"
#include "kill_enemy_quest.h"
#include "maths.h"
#include "message_type.h"
#include "player.h"
#include "publisher.h"
#include "quest_manager.h"
#include "third_person_camera.h"
#include "zone_loader.h"

using namespace std::literals::chrono_literals;

namespace trinket
{

Game::Game(std::unique_ptr<Config> config, std::vector<std::unique_ptr<ZoneLoader>> &&zone_loaders)
    : running_(true)
    , config_(std::move(config))
    , zone_loaders_(std::move(zone_loaders))
    , current_zone_(nullptr)
    , next_zone_(nullptr)
    , window_(nullptr)
    , portal_(nullptr)
    , portal_destination_()
    , state_(GameState::PLAYING)
{
    const auto starting_zone_name = config_->string_option(ConfigOption::STARTING_ZONE);

    auto starting_zone =
        std::find_if(std::begin(zone_loaders_), std::end(zone_loaders_), [&starting_zone_name](auto &element) {
            return element->name() == starting_zone_name;
        });
    iris::ensure(starting_zone != std::end(zone_loaders_), "missing starting zone");

    next_zone_ = starting_zone->get();

    subscribe(MessageType::QUIT);
    subscribe(MessageType::KEY_PRESS);
    subscribe(MessageType::PLAYER_DIED);
}

void Game::run()
{
    window_ = iris::Root::window_manager().create_window(
        config_->uint32_option(ConfigOption::SCREEN_WIDTH), config_->uint32_option(ConfigOption::SCREEN_HEIGHT));

    // the main game loops keeps going until signalled to stop
    do
    {
        // update current zone and reset next zone
        current_zone_ = next_zone_;
        next_zone_ = nullptr;

        // actually run the game
        run_zone();
    } while (running_);
}

void Game::run_zone()
{
    // we run the game in this function, that way we can keep all variables local and can load a new zone by simply
    // setting next_zone_ and returning (run() will call us again with the next zone)

    auto *ps = iris::Root::physics_manager().create_physics_system();
    auto &mesh_manager = iris::Root::mesh_manager();
    auto &rtm = iris::Root::texture_manager();

    auto render_pipeline = std::make_unique<iris::RenderPipeline>(window_->width(), window_->height());

    // basic scene setup
    auto *final_scene = render_pipeline->create_scene();
    auto *game_scene = render_pipeline->create_scene();
    auto *rt = iris::Root::render_target_manager().create();

    // setup game objects
    std::vector<std::unique_ptr<GameObject>> objects{};
    objects.emplace_back(std::make_unique<InputHandler>(window_));
    objects.emplace_back(
        std::make_unique<Player>(game_scene, ps, current_zone_->player_start_position(), *render_pipeline));
    auto *player = static_cast<Player *>(objects.back().get());

    objects.emplace_back(std::make_unique<ThirdPersonCamera>(player, window_->width(), window_->height(), ps));
    auto *camera = static_cast<ThirdPersonCamera *>(objects.back().get());

    objects.emplace_back(std::make_unique<HUD>(100.0f, rt->width(), rt->height(), final_scene));
    auto *hud = static_cast<HUD *>(objects.back().get());

    objects.emplace_back(std::make_unique<QuestManager>());
    auto *qm = static_cast<QuestManager *>(objects.back().get());
    qm->create<KillEnemyQuest>(2u);

    auto *rg = render_pipeline->create_render_graph();
    rg->render_node()->set_colour_input(rg->create<iris::TextureNode>(rt->colour_texture()));
    final_scene->create_entity<iris::SingleEntity>(
        rg,
        mesh_manager.sprite({}),
        iris::Transform{{}, {}, {static_cast<float>(window_->width()), static_cast<float>(window_->height()), 0.0f}});
    iris::Camera final_camera{iris::CameraType::ORTHOGRAPHIC, window_->width(), window_->height()};

    // load data from zone
    current_zone_->load_static_geometry(ps, game_scene, *render_pipeline);
    current_zone_->load_enemies(ps, game_scene, *render_pipeline, objects, player, camera);

    // lighting setup
    game_scene->set_ambient_light({0.5f, 0.5f, 0.5f, 1.0f});
    auto *light = game_scene->create_light<iris::PointLight>(iris::Vector3{10.0f}, iris::Colour{10.0f, 10.0f, 10.0f});
    game_scene->create_light<iris::DirectionalLight>(iris::Vector3{0.0, -1.0f, 0.0f}, true);

    // setup portal
    const auto [portal_transform, destination] = current_zone_->portal();
    game_scene->create_entity<iris::SingleEntity>(nullptr, mesh_manager.cube({}), portal_transform);
    portal_ = ps->create_rigid_body(
        portal_transform.translation(),
        ps->create_box_collision_shape(portal_transform.scale()),
        iris::RigidBodyType::GHOST);
    portal_destination_ = destination;

    // optional debug draw
    if (config_->bool_option(ConfigOption::PHYSICS_DEBUG_DRAW))
    {
        ps->enable_debug_draw(game_scene);
    }

    const auto *game_sky_box = iris::Root::texture_manager().create(
        iris::Colour{0.275f, 0.51f, 0.796f}, iris::Colour{0.5f, 0.5f, 0.5f}, 2048u, 2048u);

    // setup the game pass
    auto *game_pass = render_pipeline->create_render_pass(game_scene);
    game_pass->camera = camera->camera();
    game_pass->colour_target = rt;
    game_pass->sky_box = game_sky_box;
    game_pass->post_processing_description = {
        .ambient_occlusion = iris::AmbientOcclusionDescription{},
    };

    // setup the final pass (so we can apply anti aliasing)
    auto *final_pass = render_pipeline->create_render_pass(final_scene);
    final_pass->camera = &final_camera;
    final_pass->post_processing_description = {
        .colour_adjust = iris::ColourAdjustDescription{}, .anti_aliasing = iris::AntiAliasingDescription{}};

    window_->set_render_pipeline(std::move(render_pipeline));

    // setup game loop
    iris::Looper looper{
        0ms,
        16ms,
        [ps, player, this](auto, std::chrono::microseconds delta) {
            if (state_ == GameState::PLAYING)
            {
                // update physics
                ps->step(std::chrono::duration_cast<std::chrono::milliseconds>(delta));

                // check if we are intersecting portal
                for (const auto &contact : ps->contacts(portal_))
                {
                    if (contact.contact == player->rigid_body())
                    {
                        auto destination =
                            std::find_if(std::begin(zone_loaders_), std::end(zone_loaders_), [this](auto &element) {
                                return element->name() == portal_destination_;
                            });

                        iris::ensure(destination != std::cend(zone_loaders_), "missing zone");

                        // set next zone which will get loaded when we return
                        next_zone_ = destination->get();
                    }
                }
            }

            return true;
        },
        [&](std::chrono::microseconds elapsed, auto) {
            // move light with player
            light->set_position(player->position() + iris::Vector3{0.0f, 10.0f, 0.0f});

            // update game objects
            for (auto &object : objects)
            {
                object->update(elapsed);
            }

            window_->render();

            return running_ && (next_zone_ == nullptr);
        }};

    state_ = GameState::PLAYING;
    looper.run();
}

void Game::handle_message(MessageType message_type, const std::any &data)
{
    switch (message_type)
    {
        case MessageType::QUIT: running_ = false; break;
        case MessageType::KEY_PRESS:
        {
            const auto key = std::any_cast<iris::KeyboardEvent>(data);

            // pressing R has two outcomes
            //   player alive - reload current scene (useful for testing)
            //   player dead  - reload starting scene
            if ((key.key == iris::Key::R) && (key.state == iris::KeyState::DOWN))
            {
                if (state_ == GameState::PLAYING)
                {
                    next_zone_ = current_zone_;
                }
                else
                {
                    const auto starting_zone_name = config_->string_option(ConfigOption::STARTING_ZONE);

                    auto starting_zone = std::find_if(
                        std::begin(zone_loaders_), std::end(zone_loaders_), [&starting_zone_name](auto &element) {
                            return element->name() == starting_zone_name;
                        });

                    next_zone_ = starting_zone->get();
                }
            }
            break;
        }
        case MessageType::PLAYER_DIED:
        {
            state_ = GameState::DEAD;

            // create death screen

            auto pipeline = std::make_unique<iris::RenderPipeline>(window_->width(), window_->height());
            auto *scene = pipeline->create_scene();

            scene->create_entity<iris::SingleEntity>(
                nullptr,
                iris::Root::mesh_manager().sprite({}),
                iris::Transform{
                    {}, {}, {static_cast<float>(window_->width()), static_cast<float>(window_->height()), 1.0f}});

            auto *text = iris::text_factory::create("Arial", 72, "you died 😢", {1.0f, 1.0f, 1.0f, 1.0f});
            auto *rg = pipeline->create_render_graph();
            rg->render_node()->set_colour_input(rg->create<iris::TextureNode>(text));
            scene->create_entity<iris::SingleEntity>(
                rg,
                iris::Root::mesh_manager().sprite({1.0f, 1.0f, 1.0f}),
                iris::Transform{
                    {0.0f, 0.0f, 1.0f},
                    {},
                    {static_cast<float>(text->width()), static_cast<float>(text->height()), 1.0f}},
                true);

            static const iris::Camera camera{iris::CameraType::ORTHOGRAPHIC, window_->width(), window_->height()};

            auto *pass = pipeline->create_render_pass(scene);
            pass->camera = &camera;

            window_->set_render_pipeline(std::move(pipeline));
            break;
        }
        default: break;
    }
}
}