#include "TileMap.h"
#include "Constants.h"
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>

// ============================================================
//  TileMap.cpp — генерация и отрисовка карты.
//  Разработчик Б.
// ============================================================

TileMap::TileMap() {
    tilesX = (int)(C::MAP_W / TILE) + 1;
    tilesY = (int)(C::MAP_H / TILE) + 1;
    grid.assign(tilesY, std::vector<int>(tilesX, 0));
}

// ─────────────────────────────────────────────────────────────
bool TileMap::loadTextures() {
    bool ok = true;
    auto load = [&](sf::Texture& t, const std::string& p) {
        if (!t.loadFromFile(p)) {
            std::cerr << "[TileMap] Cannot load " << p << "\n";
            ok = false;
        } else {
            t.setSmooth(false);
            t.setRepeated(false);
        }
    };
    load(texGround,     "assets/tile_ground.png");
    load(texGrass,      "assets/tile_grass.png");
    load(texTree,       "assets/tree.png");
    load(texBuildSmall, "assets/building_small.png");
    load(texBuildLarge, "assets/building_large.png");
    load(texCar,        "assets/car.png");
    load(texBarrier,    "assets/barrier.png");
    return ok;
}

// ─────────────────────────────────────────────────────────────
//  Главная функция генерации
// ─────────────────────────────────────────────────────────────
void TileMap::generate(unsigned int seed) {
    std::srand(seed);

    // --- Заполнить сетку ---
    // 0 = асфальт, 1 = трава
    float mapCX = C::MAP_W / 2.f;
    float mapCY = C::MAP_H / 2.f;
    float roadW = 3;   // ширина дороги в тайлах

    for (int row = 0; row < tilesY; ++row) {
        for (int col = 0; col < tilesX; ++col) {
            float wx = col * TILE + TILE / 2.f;
            float wy = row * TILE + TILE / 2.f;

            // Центральная площадь (400×300) — асфальт
            bool inCenter = (std::abs(wx - mapCX) < 200.f &&
                             std::abs(wy - mapCY) < 150.f);

            // Горизонтальная дорога через центр
            int centerRow = (int)(mapCY / TILE);
            bool onHRoad  = (row >= centerRow - roadW/2 &&
                             row <= centerRow + roadW/2);

            // Вертикальная дорога через центр
            int centerCol = (int)(mapCX / TILE);
            bool onVRoad  = (col >= centerCol - roadW/2 &&
                             col <= centerCol + roadW/2);

            if (inCenter || onHRoad || onVRoad)
                grid[row][col] = 0; // асфальт
            else
                grid[row][col] = 1; // трава
        }
    }

    buildVertexArray();
    placeObjects(seed);
}

// ─────────────────────────────────────────────────────────────
//  Сборка VertexArray — один вызов draw для всей земли
// ─────────────────────────────────────────────────────────────
void TileMap::buildVertexArray() {
    // Будем рисовать оба тайла из одного массива,
    // но разбитого на два слоя — сначала асфальт, потом трава.
    // Проще: два отдельных VertexArray (один draw каждый).
    // Для учебного проекта — просто один общий массив с двумя текстурами
    // будет работать через два прохода в drawGround().
    groundVerts.setPrimitiveType(sf::Quads);
    groundVerts.resize(tilesX * tilesY * 4);

    int idx = 0;
    auto texSz = texGround.getSize(); // все тайлы одного размера
    float tw = (float)texSz.x;
    float th = (float)texSz.y;

    for (int row = 0; row < tilesY; ++row) {
        for (int col = 0; col < tilesX; ++col) {
            float x = col * TILE;
            float y = row * TILE;

            sf::Vertex* q = &groundVerts[idx * 4];
            q[0].position = {x,        y       };
            q[1].position = {x + TILE, y       };
            q[2].position = {x + TILE, y + TILE};
            q[3].position = {x,        y + TILE};

            // UV — берём весь тайл
            q[0].texCoords = {0,  0 };
            q[1].texCoords = {tw, 0 };
            q[2].texCoords = {tw, th};
            q[3].texCoords = {0,  th};

            // Цвет: трава немного темнее для визуального разделения
            sf::Color c = (grid[row][col] == 1)
                        ? sf::Color(230, 240, 220)
                        : sf::Color(240, 240, 245);
            for (int v = 0; v < 4; ++v) q[v].color = c;

            ++idx;
        }
    }
}

// ─────────────────────────────────────────────────────────────
//  Рисовать землю
// ─────────────────────────────────────────────────────────────
void TileMap::drawGround(sf::RenderWindow& window) const {
    // Два прохода: асфальт, потом трава
    // (для корректного наложения достаточно одного прохода,
    //  но текстуры разные — рисуем отдельными группами)
    for (int pass = 0; pass < 2; ++pass) {
        sf::Texture& tex = (pass == 0) ? const_cast<sf::Texture&>(texGround)
                                       : const_cast<sf::Texture&>(texGrass);
        sf::VertexArray va;
        va.setPrimitiveType(sf::Quads);

        for (int row = 0; row < tilesY; ++row) {
            for (int col = 0; col < tilesX; ++col) {
                if (grid[row][col] != pass) continue;

                float x = col * TILE;
                float y = row * TILE;
                auto sz = tex.getSize();
                float tw = (float)sz.x;
                float th = (float)sz.y;

                sf::Vertex q[4];
                q[0].position = {x,        y       };
                q[1].position = {x + TILE, y       };
                q[2].position = {x + TILE, y + TILE};
                q[3].position = {x,        y + TILE};
                q[0].texCoords = {0,  0 };
                q[1].texCoords = {tw, 0 };
                q[2].texCoords = {tw, th};
                q[3].texCoords = {0,  th};
                for (auto& v : q) va.append(v);
            }
        }
        sf::RenderStates st;
        st.texture = &tex;
        window.draw(va, st);
    }

    // Граница карты (зелёный контур)
    sf::RectangleShape border({C::MAP_W, C::MAP_H});
    border.setPosition(0.f, 0.f);
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineThickness(6.f);
    border.setOutlineColor(sf::Color(60, 160, 60, 200));
    window.draw(border);
}

// ─────────────────────────────────────────────────────────────
//  Рисовать объекты
// ─────────────────────────────────────────────────────────────
void TileMap::drawObjects(sf::RenderWindow& window) const {
    // Рисуем в порядке Y — дальние объекты позади ближних
    // (сортировка уже сделана в placeObjects)
    for (const auto& obj : objects)
        obj.draw(window);
}

// ─────────────────────────────────────────────────────────────
//  Проверка: не перекрывает ли новый объект существующие
// ─────────────────────────────────────────────────────────────
bool TileMap::overlapsExisting(sf::FloatRect r, float margin) const {
    // Расширить прямоугольник на margin
    r.left   -= margin; r.top    -= margin;
    r.width  += margin * 2; r.height += margin * 2;

    for (const auto& obj : objects) {
        if (r.intersects(obj.sprite.getGlobalBounds()))
            return true;
    }
    return false;
}

// ─────────────────────────────────────────────────────────────
//  Расстановка объектов на карте
// ─────────────────────────────────────────────────────────────
void TileMap::placeObjects(unsigned int seed) {
    objects.clear();
    std::srand(seed + 1);

    float mapCX = C::MAP_W / 2.f;
    float mapCY = C::MAP_H / 2.f;

    // Зона старта игрока — не ставить объекты рядом
    sf::FloatRect safeZone(mapCX - 220.f, mapCY - 170.f, 440.f, 340.f);

    // Вспомогательная функция: попытаться поставить объект
    auto tryPlace = [&](MapObjType type, sf::Texture& tex,
                        float x, float y, float scale, bool solid) -> bool {
        // Не в безопасной зоне
        if (safeZone.contains(x, y)) return false;
        // Не за границей карты
        if (x < 40 || y < 40 || x > C::MAP_W-40 || y > C::MAP_H-40) return false;

        MapObject obj(type, {x, y}, tex, scale, solid);
        if (overlapsExisting(obj.sprite.getGlobalBounds(), 12.f)) return false;

        objects.push_back(obj);
        return true;
    };

    // ── Здания по периметру и в «кварталах» ────────────────
    // Большие здания — 8 штук, по углам кварталов
    std::vector<sf::Vector2f> buildingPos = {
        {140,  120}, {380,  100}, {620,  130}, {860,  110},
        {140,  600}, {400,  650}, {700,  620}, {900,  580},
        {150, 1100}, {500, 1200}, {800, 1150}, {1000, 1100},
        {1200, 300}, {1400, 500}, {1300, 800}, {1500, 200},
        {200,  1400},{600,  1450},{1000,1400}, {1400,1200},
    };
    for (auto& p : buildingPos) {
        // чередуем большие и маленькие
        bool large = (std::rand() % 3 != 0);
        sf::Texture& t = large ? texBuildLarge : texBuildSmall;
        float sc = large ? 1.1f : 1.0f;
        tryPlace(large ? MapObjType::BUILDING_LARGE : MapObjType::BUILDING_SMALL,
                 t, p.x, p.y, sc, true);
    }

    // ── Машины — брошены на дорогах и рядом ────────────────
    std::vector<sf::Vector2f> carPos = {
        {320, 750}, {750, 320}, {500, 400}, {900, 750},
        {300, 300}, {1100,400}, {700, 900}, {400,1100},
        {1300,700}, {600,1300}, {850, 500}, {1200,900},
    };
    for (auto& p : carPos) {
        float angle = (std::rand() % 4) * 90.f; // кратно 90°
        if (tryPlace(MapObjType::CAR, texCar, p.x, p.y, 1.0f, true)) {
            objects.back().sprite.setRotation(angle);
            objects.back().updateBounds();
        }
    }

    // ── Деревья — на траве, много ─────────────────────────
    int treesPlaced = 0;
    int attempts    = 0;
    while (treesPlaced < 80 && attempts < 2000) {
        ++attempts;
        float x = 60.f + (float)(std::rand() % (int)(C::MAP_W - 120));
        float y = 60.f + (float)(std::rand() % (int)(C::MAP_H - 120));

        // Только на траве (проверяем тайл)
        int col = (int)(x / TILE);
        int row = (int)(y / TILE);
        if (col < 0 || col >= tilesX || row < 0 || row >= tilesY) continue;
        if (grid[row][col] != 1) continue; // не трава

        float scale = 0.9f + (std::rand() % 4) * 0.08f;
        if (tryPlace(MapObjType::TREE, texTree, x, y, scale, true))
            ++treesPlaced;
    }

    // ── Баррикады — группами по 3-5 штук ──────────────────
    std::vector<sf::Vector2f> barrierGroups = {
        {500, 500}, {900, 400}, {400, 900}, {1100, 600},
        {700, 700}, {300, 700}, {1200, 500},{600, 1100},
    };
    for (auto& gp : barrierGroups) {
        int count = 3 + std::rand() % 3;
        for (int i = 0; i < count; ++i) {
            float ox = gp.x + (std::rand() % 80) - 40;
            float oy = gp.y + (std::rand() % 30) - 15;
            float angle = (std::rand() % 2) * 90.f;
            if (tryPlace(MapObjType::BARRIER, texBarrier, ox, oy, 1.0f, false)) {
                objects.back().sprite.setRotation(angle);
                objects.back().updateBounds();
            }
        }
    }

    // Отсортировать по Y — объекты ниже рисуются поверх
    std::sort(objects.begin(), objects.end(),
              [](const MapObject& a, const MapObject& b){
                  return a.sprite.getPosition().y < b.sprite.getPosition().y;
              });
}
