#include "UpgradeSystem.h"
#include "Player.h"
#include "Weapon.h"
#include "Constants.h"
#include <algorithm>
#include <random>
#include <ctime>


//  UpgradeSystem.cpp — меню прокачек


UpgradeSystem::UpgradeSystem() : active(false), fontLoaded(false) {
    if (font.loadFromFile("assets/font.ttf"))
        fontLoaded = true;
    else if (font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
        fontLoaded = true;
}

std::vector<Upgrade> UpgradeSystem::buildAll(const Player& player) {
    std::vector<Upgrade> all;

    all.push_back({
        "Speed Up",
        "+20% movement speed",
        [](Player& p) { p.speedMult += 0.20f; }
    });

    all.push_back({
        "More Damage",
        "+25% weapon damage",
        [](Player& p) { p.damageMult += 0.25f; }
    });

    all.push_back({
        "Extra HP",
        "Max health +50",
        [](Player& p) {
            p.maxHealthBonus += 50.f;
            p.heal(50.f);
        }
    });

    all.push_back({
        "Medkit",
        "Restore 40 HP",
        [](Player& p) { p.heal(40.f); }
    });

    if (player.getWeaponCount() < 2) {
        all.push_back({
            "Shotgun",
            "Spread shot, high close-range dmg",
            [](Player& p) { p.addWeapon(Weapon::makeShotgun()); }
        });
    }

    if (player.getWeaponCount() < 3) {
        all.push_back({
            "Machine Gun",
            "Fast fire rate, lower damage",
            [](Player& p) { p.addWeapon(Weapon::makeMachineGun()); }
        });
    }

    return all;
}

void UpgradeSystem::show(Player& player) {
    auto all = buildAll(player);

    std::mt19937 rng((unsigned)std::time(nullptr));
    std::shuffle(all.begin(), all.end(), rng);

    offered.clear();
    int count = std::min(3, (int)all.size());
    for (int i = 0; i < count; ++i)
        offered.push_back(all[i]);

    active = true;
}

bool UpgradeSystem::handleClick(sf::Vector2f mousePos, Player& player) {
    if (!active) return false;

    const float cardW = 220.f, cardH = 270.f;
    const float startX = (C::WINDOW_W - (cardW * 3 + 40.f)) / 2.f;
    const float cardY  = (C::WINDOW_H - cardH) / 2.f;

    for (int i = 0; i < (int)offered.size(); ++i) {
        sf::FloatRect rect(startX + i * (cardW + 20.f), cardY, cardW, cardH);
        if (rect.contains(mousePos)) {
            offered[i].apply(player);
            active = false;
            offered.clear();
            return true;
        }
    }
    return false;
}

void UpgradeSystem::drawCard(sf::RenderWindow& window,
                              const Upgrade& u,
                              sf::FloatRect rect,
                              bool hovered) const
{

    sf::RectangleShape card({rect.width, rect.height});
    card.setPosition(rect.left, rect.top);
    card.setFillColor(hovered ? sf::Color(60, 80, 120) : sf::Color(30, 40, 70));
    card.setOutlineThickness(2.f);
    card.setOutlineColor(hovered ? sf::Color(150, 210, 255) : sf::Color(70, 90, 130));
    window.draw(card);

    if (!fontLoaded) return;


    sf::Text title(u.name, font, 22);
    title.setFillColor(sf::Color(200, 230, 255));
    title.setPosition(rect.left + 14.f, rect.top + 20.f);
    window.draw(title);


    sf::Text desc(u.description, font, 15);
    desc.setFillColor(sf::Color(140, 170, 210));
    desc.setPosition(rect.left + 14.f, rect.top + 60.f);
    window.draw(desc);

    if (hovered) {
        sf::Text hint("Click to select", font, 13);
        hint.setFillColor(sf::Color(255, 220, 80));
        hint.setPosition(rect.left + 14.f, rect.top + rect.height - 30.f);
        window.draw(hint);
    }
}

void UpgradeSystem::draw(sf::RenderWindow& window) const {
    if (!active) return;


    sf::RectangleShape overlay({(float)C::WINDOW_W, (float)C::WINDOW_H});
    overlay.setFillColor(sf::Color(0, 0, 0, 170));
    window.draw(overlay);

    if (fontLoaded) {
        sf::Text header("-- Choose an Upgrade --", font, 34);
        header.setFillColor(sf::Color(255, 220, 80));

        header.setPosition(C::WINDOW_W / 2.f - 190.f, 70.f);
        window.draw(header);
    }


    const float cardW = 220.f, cardH = 270.f;
    const float startX = (C::WINDOW_W - (cardW * 3 + 40.f)) / 2.f;
    const float cardY  = (C::WINDOW_H - cardH) / 2.f;

    sf::Vector2i mp = sf::Mouse::getPosition(window);
    sf::Vector2f mousePos((float)mp.x, (float)mp.y);

    for (int i = 0; i < (int)offered.size(); ++i) {
        sf::FloatRect rect(startX + i * (cardW + 20.f), cardY, cardW, cardH);
        drawCard(window, offered[i], rect, rect.contains(mousePos));
    }
}
