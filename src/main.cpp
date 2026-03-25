#include <SFML/Graphics.hpp>
#include "game/Game.hpp"
#include "systems/RenderSystem.hpp"
#include "systems/MovementSystem.hpp"
#include "systems/AISystem.hpp"
#include "systems/CollisionSystem.hpp"
#include "systems/AnimationSystem.hpp"
#include "core/Constants.hpp"

int main() {
    sf::RenderWindow window(
        sf::VideoMode({(unsigned)Config::WINDOW_W, (unsigned)Config::WINDOW_H}),
        "ASCII Pacman"
    );
    window.setFramerateLimit(60);

    RenderSystem renderer(window);
    if (!renderer.loadFont("/usr/share/fonts/TTF/DejaVuSansMono.ttf"))
        return -1;

    Game game;
    sf::Clock clock;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
                game.handleInput(key->code, true);
        }

        game.update(dt);

        auto& registry = game.getRegistry();
        MovementSystem::update(registry, game.getMap(), dt);
        AISystem::update(registry, game.getMap(), dt);
        int earned = CollisionSystem::update(registry, const_cast<Map&>(game.getMap()));
        AnimationSystem::update(registry, dt);

        window.clear(sf::Color::Black);
        renderer.drawMap(game.getMap());
        renderer.drawEntities(registry);

        bool powered = false;
	registry.view<TagPacman, Powered>().each([&](auto, auto) { powered = true; });
        renderer.drawHUD(game.getScore() + earned, game.getLives(), powered);

        if (game.getStatus() == GameStatus::Win)
            renderer.drawMessage("YOU WIN!");
        if (game.getStatus() == GameStatus::GameOver)
            renderer.drawMessage("GAME OVER");

        window.display();
    }

    return 0;
}
