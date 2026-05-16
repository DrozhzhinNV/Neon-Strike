#pragma once
#include <SFML/Graphics.hpp>
#include <string>

// ============================================================
//  WeaponShop.h — магазин улучшений оружия (Tab = пауза).
//  Открывается по Tab, игра ставится на паузу.
//  Улучшения покупаются за ресурсы (монетки).
//  Разработчик Б.
// ============================================================

class Player;

class WeaponShop {
public:
    WeaponShop();

    bool isOpen() const { return open; }
    void toggle()       { open = !open; }
    void close()        { open = false; }

    // Обработать клик мыши (экранные координаты).
    // Возвращает true если что-то куплено.
    bool handleClick(sf::Vector2f mousePos, Player& player);

    // Рисовать поверх игры (в экранных координатах)
    void draw(sf::RenderWindow& window, const Player& player) const;

private:
    bool     open = false;
    sf::Font font;
    bool     fontLoaded = false;

    // Рисовать одну кнопку апгрейда.
    // Возвращает FloatRect кнопки (для hit-test).
    sf::FloatRect drawUpgradeButton(sf::RenderWindow& window,
                                    sf::Vector2f pos,
                                    const std::string& label,
                                    int currentLevel,
                                    int cost,
                                    bool canAfford) const;

    void drawWeaponPanel(sf::RenderWindow& window,
                         const Player& player,
                         int weaponIdx,
                         sf::Vector2f panelPos,
                         float panelW) const;

    // Список всех кнопок (заполняется в draw, проверяется в handleClick)
    // Структура: {weaponIdx, upgradeType(0=dmg,1=fr,2=mag), rect}
    struct ButtonInfo { int wIdx; int upType; sf::FloatRect rect; };
    mutable std::vector<ButtonInfo> buttons;
};
