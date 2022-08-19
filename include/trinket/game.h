////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <any>
#include <memory>
#include <string>
#include <vector>

#include "iris/graphics/window.h"
#include "iris/physics/rigid_body.h"

#include "config.h"
#include "message_type.h"
#include "subscriber.h"
#include "zone_loader.h"

namespace trinket
{

/**
 * Enumeration of possible game states.
 */
enum class GameState
{
    PLAYING,
    DEAD
};

/**
 * This class is the root of the game.
 */
class Game : public Subscriber
{
  public:
    /**
     * Construct a new Game object.
     *
     * @param config
     *   Config to use for the game.
     *
     * @param zone_loaders
     *   Zone loaders for all possible zones.
     */
    Game(std::unique_ptr<Config> config, std::vector<std::unique_ptr<ZoneLoader>> &&zone_loaders);

    /**
     * Blocks and runs the game. Will return when the game exits.
     */
    void run();

    /**
     * Message handler.
     *
     * @param message_type
     *   Type of message being sent.
     *
     * @param data
     *   Any data for the message.
     */
    void handle_message(MessageType message_type, const std::any &data) override;

  private:
    /**
     * Execute the game for the current selected zone.
     */
    void run_zone();

    /** Flag indicating if the game should keep running or exit. */
    bool running_;

    /** Config for the game. */
    std::unique_ptr<Config> config_;

    /** Loaders for all possible game zones. */
    std::vector<std::unique_ptr<ZoneLoader>> zone_loaders_;

    /** Current game zone. */
    ZoneLoader *current_zone_;

    /** Next zone to load, when set the game will load this zone and reset to nullptr. */
    ZoneLoader *next_zone_;

    /** Game window. */
    iris::Window *window_;

    /** Portal to next zone (for collisions). */
    iris::RigidBody *portal_;

    /** Name of zone to load when colliding with portal. */
    std::string portal_destination_;

    /** Current game state. */
    GameState state_;
};

}
