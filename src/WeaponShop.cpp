#include "WeaponShop.h"
#include "Player.h"
#include "Constants.h"
#include <sstream>
#include <iomanip>

// ============================================================
//  WeaponShop.cpp — магазин оружия.
//  Разработчик Б.
// ============================================================

WeaponShop::WeaponShop() {
    fontLoaded = font.loadFromFile("assets/font.ttf");
    if (!fontLoaded)
        fontLoaded = font.loadFromFile("C:/Windows/Fonts/arial.ttf");
}

// ── Вспомогательная отрисовка текста ─────────────────────────
static void drawText(sf::RenderWindow& w, const sf::Font& f,
                     const std::string& s, sf::Vector2f pos,
                     unsigned sz, sf::Color col)
{
    sf::Text t(s, f, sz);
    t.setFillColor(col);
    t.setPosition(pos);
    w.draw(t);
}

// ── Нарисовать одну кнопку апгрейда ──────────────────────────
sf::FloatRect WeaponShop::drawUpgradeButton(sf::RenderWindow& window,
                                             sf::Vector2f pos,
                                             const std::string& label,
                                             int currentLevel,
                                             int cost,
                                             bool canAfford) const
{
    const float BW = 170.f, BH = 48.f;
    sf::FloatRect rect(pos.x, pos.y, BW, BH);

    // Фон кнопки
    sf::RectangleShape bg({BW, BH});
    bg.setPosition(pos);
    bool maxed = (currentLevel >= C::SHOP_MAX_LEVEL);
    if (maxed)
        bg.setFillColor(sf::Color(40,60,40,200));
    else if (canAfford)
        bg.setFillColor(sf::Color(40,55,80,220));
    else
        bg.setFillColor(sf::Color(55,35,35,200));
    bg.setOutlineThickness(1.5f);
    bg.setOutlineColor(maxed  ? sf::Color(60,160,60,200) :
                       canAfford ? sf::Color(80,130,200,200) :
                                   sf::Color(160,60,60,200));
    window.draw(bg);

    if (!fontLoaded) return rect;

    // Название
    drawText(window, font, label,
             {pos.x+8.f, pos.y+5.f}, 14, sf::Color(200,220,255));

    // Уровень (звёздочки)
    std::string stars;
    for (int i=0; i<C::SHOP_MAX_LEVEL; ++i)
        stars += (i < currentLevel) ? "*" : ".";
    drawText(window, font, stars,
             {pos.x+8.f, pos.y+22.f}, 13, sf::Color(255,200,50));

    // Цена или MAX
    if (maxed) {
        drawText(window, font, "MAX",
                 {pos.x+BW-40.f, pos.y+BH/2.f-8.f}, 16,
                 sf::Color(80,220,80));
    } else {
        std::string priceStr = "$" + std::to_string(cost);
        sf::Color pc = canAfford ? sf::Color(100,220,100) : sf::Color(220,80,80);
        drawText(window, font, priceStr,
                 {pos.x+BW-50.f, pos.y+BH/2.f-8.f}, 15, pc);
    }
    return rect;
}

// ── Нарисовать панель одного оружия ──────────────────────────
void WeaponShop::drawWeaponPanel(sf::RenderWindow& window,
                                  const Player& player,
                                  int weaponIdx,
                                  sf::Vector2f panelPos,
                                  float panelW) const
{
    const auto& weapons = player.getWeapons();
    if (weaponIdx >= (int)weapons.size()) {
        // Оружие не разблокировано
        sf::RectangleShape locked({panelW, 180.f});
        locked.setPosition(panelPos);
        locked.setFillColor(sf::Color(30,30,30,180));
        locked.setOutlineThickness(1.f);
        locked.setOutlineColor(sf::Color(80,80,80,150));
        window.draw(locked);
        if (fontLoaded) {
            drawText(window, font, "Locked",
                     {panelPos.x + panelW/2.f - 25.f, panelPos.y + 70.f},
                     18, sf::Color(100,100,100));
        }
        return;
    }

    const Weapon& w = weapons[weaponIdx];
    int res = player.getResources();

    // Фон панели
    sf::RectangleShape panel({panelW, 210.f});
    panel.setPosition(panelPos);
    bool isActive = (weaponIdx == player.getCurrentWeaponIdx());
    panel.setFillColor(sf::Color(25,35,55,210));
    panel.setOutlineThickness(isActive ? 2.f : 1.f);
    panel.setOutlineColor(isActive ? sf::Color(100,160,255,230)
                                   : sf::Color(60,80,120,180));
    window.draw(panel);

    if (!fontLoaded) return;

    float x = panelPos.x + 10.f;
    float y = panelPos.y + 10.f;

    // Название оружия
    drawText(window, font, w.name,
             {x, y}, 20,
             isActive ? sf::Color(120,180,255) : sf::Color(180,200,230));
    y += 28.f;

    // Характеристики
    auto stat = [&](const std::string& label, float val){
        std::ostringstream ss;
        ss << label << ": " << std::fixed << std::setprecision(1) << val;
        drawText(window, font, ss.str(), {x, y}, 13, sf::Color(160,180,210));
        y += 16.f;
    };
    stat("Damage",    w.effectiveDamage());
    stat("Fire rate", w.effectiveFireRate());
    stat("Mag",       (float)w.effectiveMagSize());
    stat("Ammo left", (float)(w.currentAmmo + w.totalAmmo));
    y += 6.f;

    // Три кнопки апгрейда
    struct UpBtn { std::string label; int level; int cost; int type; };
    std::vector<UpBtn> btns = {
        {"+ Damage",    w.damageLevel,   w.damageCost(),   0},
        {"+ Fire Rate", w.fireRateLevel, w.fireRateCost(), 1},
        {"+ Magazine",  w.magLevel,      w.magCost(),      2},
    };

    for (auto& btn : btns) {
        bool affordable = (res >= btn.cost) && (btn.cost > 0);
        sf::FloatRect r = drawUpgradeButton(window,
                          {x, y}, btn.label,
                          btn.level, btn.cost, affordable);
        buttons.push_back({weaponIdx, btn.type, r});
        y += 54.f;
    }
}

// ── Главная отрисовка ─────────────────────────────────────────
void WeaponShop::draw(sf::RenderWindow& window, const Player& player) const {
    if (!open) return;
    buttons.clear();

    // СОХРАНИТЬ текущий вид и переключиться на дефолтный (экран)
    sf::View savedView = window.getView();
    window.setView(window.getDefaultView());  // ← ДОБАВИТЬ ЭТУ СТРОКУ

    // Затемнение фона
    sf::RectangleShape overlay({(float)C::WINDOW_W, (float)C::WINDOW_H});
    overlay.setFillColor(sf::Color(0,0,0,180));
    window.draw(overlay);

    // Заголовок
    if (fontLoaded) {
        drawText(window, font, "WEAPON SHOP",
                 {C::WINDOW_W/2.f - 90.f, 18.f}, 30,
                 sf::Color(255,200,60,240));
        drawText(window, font,
                 "$ " + std::to_string(player.getResources()),
                 {C::WINDOW_W/2.f - 30.f, 54.f}, 20,
                 sf::Color(0,220,170,230));
        drawText(window, font, "[TAB] Close  |  Click to buy",
                 {C::WINDOW_W/2.f - 130.f, (float)C::WINDOW_H - 28.f},
                 14, sf::Color(120,130,150,200));
    }

    // Три панели (по одной на каждое оружие)
    const float panelW = 210.f;
    const float gap    = 18.f;
    float totalW = 3*panelW + 2*gap;
    float startX = (C::WINDOW_W - totalW) / 2.f;
    float startY = 90.f;

    for (int i = 0; i < 3; ++i)
        drawWeaponPanel(window, player, i,
                        {startX + i*(panelW+gap), startY}, panelW);

    // ВОССТАНОВИТЬ предыдущий вид
    window.setView(savedView);  // ← ДОБАВИТЬ ЭТУ СТРОКУ
}

// ── Обработка кликов ─────────────────────────────────────────
bool WeaponShop::handleClick(sf::Vector2f mouse, Player& player) {
    if (!open) return false;

    for (auto& btn : buttons) {
        if (!btn.rect.contains(mouse)) continue;

        auto& weapons = player.getWeapons();
        if (btn.wIdx >= (int)weapons.size()) continue;
        Weapon& w = weapons[btn.wIdx];

        // Определить цену и уровень
        int cost  = 0;
        int level = 0;
        if      (btn.upType == 0) { cost = w.damageCost();   level = w.damageLevel;   }
        else if (btn.upType == 1) { cost = w.fireRateCost(); level = w.fireRateLevel; }
        else                      { cost = w.magCost();       level = w.magLevel;      }

        if (cost == 0 || level >= C::SHOP_MAX_LEVEL) return false;
        if (player.getResources() < cost) return false;

        // Снять деньги и применить апгрейд
        player.addResource(-cost);
        if      (btn.upType == 0) w.damageLevel++;
        else if (btn.upType == 1) w.fireRateLevel++;
        else {
            w.magLevel++;
            // Сразу добавить пули от увеличенной обоймы
            int bonus = w.effectiveMagSize() - w.currentAmmo;
            if (bonus > 0) w.currentAmmo = std::min(w.effectiveMagSize(),
                                                     w.currentAmmo + bonus);
        }
        return true;
    }
    return false;
}
