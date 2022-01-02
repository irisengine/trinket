////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#include "third_person_camera.h"

#include <any>
#include <cstdint>

#include "iris/core/camera_type.h"
#include "iris/core/vector3.h"
#include "iris/events/event.h"
#include "iris/events/keyboard_event.h"

#include "maths.h"
#include "player.h"

namespace trinket
{

ThirdPersonCamera::ThirdPersonCamera(Player *player, std::uint32_t width, std::uint32_t height)
    : player_(player)
    , camera_(iris::CameraType::PERSPECTIVE, width, height, 10000u)
    , key_map_()
    , azimuth_(pi_2)
    , altitude_(pi_4 / 2.0f)
{
    subscribe(MessageType::MOUSE_MOVE);
    subscribe(MessageType::KEY_PRESS);

    key_map_ = {
        {iris::Key::W, iris::KeyState::UP},
        {iris::Key::A, iris::KeyState::UP},
        {iris::Key::S, iris::KeyState::UP},
        {iris::Key::D, iris::KeyState::UP},
    };

    camera_.set_position({0.0f, 0.0f, 800.0f});
    camera_.set_pitch(-altitude_);
}

void ThirdPersonCamera::update()
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

    static constexpr auto distance = 30.0f;

    // we store the camera position as polar coordinates, which means it moves around a unit sphere centered on
    // the player convert back to cartesian coords
    const iris::Vector3 offset{
        distance * std::sin(pi_2 - altitude_) * std::cos(azimuth_),
        distance * std::cos(pi_2 - altitude_),
        distance * std::sin(pi_2 - altitude_) * std::sin(azimuth_)};

    // update camera position to always follow player
    const iris::Vector3 new_camera_pos = player_->position() + offset;
    camera_.set_position(new_camera_pos);

    // make player always face in direction of camera
    iris::Quaternion player_orientation{{0.0f, 1.0f, 0.0f}, -azimuth_};
    player_orientation.normalise();
    player_->set_orientation(player_orientation);
}

iris::Camera *ThirdPersonCamera::camera()
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

            //  adjust camera altitude and ensure we are still pointing at the player
            // we clamp the altitude [0, pi/2] to ensure no weirdness happens
            altitude_ += mouse.delta_y * sensitivity;
            if (altitude_ <= 0.0f)
            {
                altitude_ = 0.0f;
            }
            else if (altitude_ >= pi_2)
            {
                altitude_ = pi_2;
            }
            else
            {

                camera_.adjust_pitch(-mouse.delta_y * sensitivity);
            }

            break;
        }
        case MessageType::KEY_PRESS:
        {
            const auto key = std::any_cast<iris::KeyboardEvent>(data);
            key_map_.insert_or_assign(key.key, key.state);

            break;
        }
        default: break;
    }
}

}
