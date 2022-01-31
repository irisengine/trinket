////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "third_person_camera.h"

#include <any>
#include <cstdint>

#include "iris/core/camera_type.h"
#include "iris/core/error_handling.h"
#include "iris/core/vector3.h"
#include "iris/events/event.h"
#include "iris/events/keyboard_event.h"
#include "iris/events/scroll_wheel_event.h"
#include "iris/log/log.h"
#include "iris/physics/physics_system.h"
#include "iris/physics/ray_cast_result.h"

#include "maths.h"
#include "player.h"

namespace trinket
{

ThirdPersonCamera::ThirdPersonCamera(Player *player, std::uint32_t width, std::uint32_t height, iris::PhysicsSystem *ps)
    : player_(player)
    , camera_(iris::CameraType::PERSPECTIVE, width, height, 10000u)
    , key_map_()
    , azimuth_(pi_2)
    , altitude_(pi_4 / 2.0f)
    , camera_distance_(20.0f)
    , ps_(ps)
{
    key_map_ = {
        {iris::Key::W, iris::KeyState::UP},
        {iris::Key::A, iris::KeyState::UP},
        {iris::Key::S, iris::KeyState::UP},
        {iris::Key::D, iris::KeyState::UP},
    };

    camera_.set_position({0.0f, 0.0f, 800.0f});
    camera_.set_pitch(-altitude_);

    subscribe(MessageType::MOUSE_MOVE);
    subscribe(MessageType::KEY_PRESS);
    subscribe(MessageType::SCROLL_WHEEL);
}

void ThirdPersonCamera::update(std::chrono::microseconds)
{
    iris::Vector3 walk_direction{};

    if (key_map_.at(iris::Key::W) == iris::KeyState::DOWN)
    {
        walk_direction += camera_.direction();
    }

    if (key_map_.at(iris::Key::S) == iris::KeyState::DOWN)
    {
        walk_direction -= camera_.direction();
    }

    if (key_map_.at(iris::Key::A) == iris::KeyState::DOWN)
    {
        walk_direction -= camera_.right();
    }

    if (key_map_.at(iris::Key::D) == iris::KeyState::DOWN)
    {
        walk_direction += camera_.right();
    }

    walk_direction.y = 0.0f;
    walk_direction.normalise();

    player_->set_walk_direction(walk_direction);

    auto distance = camera_distance_;

    // cast a ray from player to camera to see if anything is in the way
    auto hits = ps_->ray_cast(
        player_->position(),
        iris::Vector3::normalise(camera_.position() - player_->position()),
        {player_->rigid_body()});

    // only care about occlusion from static bodies
    hits.erase(
        std::remove_if(
            std::begin(hits),
            std::end(hits),
            [](const iris::RayCastResult &result) { return result.body->type() != iris::RigidBodyType::STATIC; }),
        std::end(hits));

    if (!hits.empty())
    {
        // we only want to move the camera if it is "in" an object
        // to do this we take the first object the ray hits and then find the last hit for that object
        // if the camera is between the player and the last ht then we consider ourselves "in" the object and make
        // adjustments
        const auto *first_body = hits.front().body;
        const auto last_hit = std::find_if(
            std::crbegin(hits),
            std::crend(hits),
            [first_body](const iris::RayCastResult &result) { return result.body == first_body; });

        const auto player_camera_distance = iris::Vector3::distance(camera_.position(), player_->position());
        const auto player_first_hit_distance = iris::Vector3::distance(hits.front().position, player_->position());
        const auto player_last_hit_distance = iris::Vector3::distance(last_hit->position, player_->position());

        // check if the camera is between the player and the last hit
        if (player_camera_distance < player_last_hit_distance)
        {
            distance = std::min(camera_distance_, player_first_hit_distance);
        }
    }

    // we store the camera position as polar coordinates, which means it moves around a unit sphere centered on
    // the player convert back to cartesian coords
    const iris::Vector3 offset{
        distance * std::sin(pi_2 - altitude_) * std::cos(azimuth_),
        distance * std::cos(pi_2 - altitude_),
        distance * std::sin(pi_2 - altitude_) * std::sin(azimuth_)};

    // update camera position to always follow player
    const iris::Vector3 new_camera_pos = player_->position() + offset;
    camera_.set_position(new_camera_pos);

    // snap player to camera direction if moving
    if ((key_map_[iris::Key::W] == iris::KeyState::DOWN) || (key_map_[iris::Key::A] == iris::KeyState::DOWN) ||
        (key_map_[iris::Key::S] == iris::KeyState::DOWN) || (key_map_[iris::Key::D] == iris::KeyState::DOWN))
    {
        iris::Quaternion player_orientation{{0.0f, 1.0f, 0.0f}, -azimuth_};
        player_orientation.normalise();
        player_->set_orientation(player_orientation);
    }
}

iris::Camera *ThirdPersonCamera::camera()
{
    return &camera_;
}

const iris::Camera *ThirdPersonCamera::camera() const
{
    return &camera_;
}

void ThirdPersonCamera::handle_message(MessageType message_type, const std::any &data)
{
    switch (message_type)
    {
        case MessageType::MOUSE_MOVE:
        {
            static const auto sensitivity = 0.0025f;
            const auto mouse = std::any_cast<iris::MouseEvent>(data);

            //  adjust camera azimuth and ensure we are still pointing at the player
            azimuth_ += mouse.delta_x * sensitivity;
            camera_.adjust_yaw(mouse.delta_x * sensitivity);

            static constexpr auto offset = 0.01f;

            // adjust camera altitude and ensure we are still pointing at the player
            // we clamp the altitude [0, pi/2] to ensure no weirdness happens
            altitude_ += mouse.delta_y * sensitivity;
            altitude_ = std::clamp(altitude_, 0.0f, pi_2 - offset);

            camera_.set_pitch(-altitude_);

            break;
        }
        case MessageType::KEY_PRESS:
        {
            const auto key = std::any_cast<iris::KeyboardEvent>(data);
            key_map_.insert_or_assign(key.key, key.state);

            break;
        }
        case MessageType::SCROLL_WHEEL:
        {
            const auto scroll = std::any_cast<iris::ScrollWheelEvent>(data);
            camera_distance_ += scroll.delta_y * -1.5f;

            static constexpr auto max_distance = 100.0f;
            static constexpr auto min_distance = 5.0f;
            camera_distance_ = std::clamp(camera_distance_, min_distance, max_distance);
        }
        default: break;
    }
}

}
