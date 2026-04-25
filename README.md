# WaveShooter 🎮

Шутер с видом сверху на C++ и SFML.  
Игрок отбивает волны врагов, собирает ресурсы и прокачивает персонажа.

---

## Содержание

1. [Структура репозитория](#структура-репозитория)
2. [Распределение обязанностей](#распределение-обязанностей)
3. [Установка SFML и сборка](#установка-sfml-и-сборка)
4. [Как работать с Git (пошагово)](#как-работать-с-git-пошагово)
5. [4-недельный план разработки](#4-недельный-план-разработки)
6. [Управление в игре](#управление-в-игре)
7. [Частые проблемы](#частые-проблемы)

---

## Структура репозитория

```
WaveShooter/
├── CMakeLists.txt        # система сборки
├── .gitignore
├── README.md
├── assets/
│   ├── font.ttf          # шрифт (добавить вручную, см. ниже)
│   └── README_font.txt
└── src/
    ├── Constants.h       # все числовые константы игры
    ├── main.cpp          # точка входа
    ├── Game.h / .cpp     # главный игровой цикл           [Разраб. А]
    ├── Player.h / .cpp   # игрок, движение, стрельба      [Разраб. А]
    ├── Bullet.h / .cpp   # пуля                           [Разраб. А]
    ├── SaveSystem.h/.cpp # сохранение/загрузка            [Разраб. А]
    ├── Enemy.h / .cpp    # враги трёх типов               [Разраб. Б]
    ├── Weapon.h / .cpp   # виды оружия                    [Разраб. Б]
    ├── WaveManager.h/.cpp# управление волнами             [Разраб. Б]
    ├── ResourceDrop.h/.cpp  # выпадающие ресурсы          [Разраб. Б]
    ├── UpgradeSystem.h/.cpp # меню прокачки               [Разраб. Б]
    └── HUD.h / .cpp      # интерфейс (HP, волна, очки)    [Разраб. Б]
```

---

## Распределение обязанностей

| Файлы                                | Разработчик А                     | Разработчик Б                      |
|--------------------------------------|-----------------------------------|------------------------------------|
| `Game.h / Game.cpp`                  | ✅ главный цикл, склейка систем   |                                    |
| `Player.h / Player.cpp`              | ✅ движение, прицел, HP           |                                    |
| `Bullet.h / Bullet.cpp`             | ✅ пуля, движение, деактивация    |                                    |
| `SaveSystem.h / SaveSystem.cpp`      | ✅ сохранение ключ=значение        |                                    |
| `Enemy.h / Enemy.cpp`               |                                   | ✅ 3 типа врагов, HP, атака        |
| `Weapon.h / Weapon.cpp`             |                                   | ✅ пистолет / дробовик / пулемёт   |
| `WaveManager.h / WaveManager.cpp`   |                                   | ✅ спавн, очередь, сложность       |
| `ResourceDrop.h / ResourceDrop.cpp` |                                   | ✅ монетки, подбор                 |
| `UpgradeSystem.h / UpgradeSystem.cpp`|                                  | ✅ меню апгрейдов                  |
| `HUD.h / HUD.cpp`                   |                                   | ✅ интерфейс, полоска HP           |
| `Constants.h`                        | ✅ договариваются вместе          | ✅ договариваются вместе           |
| `CMakeLists.txt`                     | ✅ вместе при настройке           |                                    |

> **Правило:** каждый редактирует только свои файлы. Если нужно изменить чужой файл —
> сначала договоритесь и сделайте это в отдельной ветке через Pull Request.

---

## Установка SFML и сборка

### Шаг 1 — Инструменты

Установите (если ещё нет):
- **CMake** ≥ 3.16: https://cmake.org/download/
- **MinGW-w64** (компилятор GCC для Windows): https://winlibs.com/
  → скачайте архив `winlibs-x86_64-...-ucrt-...`, распакуйте, добавьте `bin/` в PATH
- Проверьте установку в cmd:
  ```
  cmake --version
  g++ --version
  ```

### Шаг 2 — SFML

1. Скачайте **SFML 2.6.x** (GCC, MinGW, 64-bit) с https://www.sfml-dev.org/download.php
2. Распакуйте, например, в `C:\SFML`
3. Структура должна быть: `C:\SFML\include\`, `C:\SFML\lib\`, `C:\SFML\bin\`

### Шаг 3 — Шрифт

Скачайте любой `.ttf` шрифт (например DejaVuSans) и положите в `assets/font.ttf`.
Или скопируйте `C:\Windows\Fonts\arial.ttf` → `assets\font.ttf`.

### Шаг 4 — Сборка

Откройте **cmd** или **PowerShell** в папке проекта:

```bat
:: Создать папку сборки
mkdir build
cd build

:: Сгенерировать Makefile, указав путь к SFML
cmake .. -G "MinGW Makefiles" -DSFML_DIR="C:/SFML/lib/cmake/SFML"

:: Скомпилировать
mingw32-make
```

После успешной сборки файл `WaveShooter.exe` появится в папке `build/`.

### Шаг 5 — Запуск

```bat
cd build
WaveShooter.exe
```

> **Если появится ошибка «DLL not found»** — скопируйте все `.dll` из `C:\SFML\bin\`
> в папку `build\` (рядом с .exe). CMake должен делать это автоматически,
> но иногда нужно вручную.

---

## Как работать с Git (пошагово)

### Первый раз (один раз на команду)

**Разработчик А** создаёт репозиторий:
```bash
# В папке проекта
git init
git add .
git commit -m "Initial commit: project skeleton"

# Создайте пустой репозиторий на github.com, затем:
git remote add origin https://github.com/ВАШ_НИК/WaveShooter.git
git branch -M main
git push -u origin main
```

**Разработчик Б** клонирует:
```bash
git clone https://github.com/ВАШ_НИК/WaveShooter.git
cd WaveShooter
```

---

### Ежедневная работа

#### Разработчик А (пример)

```bash
# 1. Получить последние изменения партнёра
git pull origin main

# 2. Создать ветку для своей задачи
git checkout -b feature/player-movement

# 3. Работать, редактировать файлы...

# 4. Добавить изменения
git add src/Player.cpp src/Player.h

# 5. Сохранить коммит с понятным сообщением
git commit -m "Player: add WASD movement and camera clamping"

# 6. Отправить ветку на GitHub
git push origin feature/player-movement

# 7. На GitHub нажать "Compare & pull request" → "Merge"
# 8. После мержа обновить локальный main
git checkout main
git pull origin main
```

#### Разработчик Б — то же самое, но со своими файлами:

```bash
git pull origin main
git checkout -b feature/wave-manager
# ... работа ...
git add src/WaveManager.cpp src/WaveManager.h
git commit -m "WaveManager: implement enemy spawn queue"
git push origin feature/wave-manager
```

---

### Соглашения о коммитах

Формат: `Область: что сделано`

```
Player: add WASD movement
Enemy: fix health bar rendering
WaveManager: increase tank count per wave
HUD: show current weapon name
SaveSystem: handle missing save file gracefully
Fix: bullets not deactivating outside map bounds
```

---

### Если возник конфликт

Конфликт случается, если оба изменили одну строку в одном файле.

```bash
git pull origin main
# Git сообщит о конфликте в файле X
# Откройте файл X — там будет:
# <<<<<<< HEAD
#   ваш код
# =======
#   код партнёра
# >>>>>>> origin/main

# Отредактируйте файл вручную, оставьте нужную версию
git add src/X.cpp
git commit -m "Merge: resolve conflict in X.cpp"
git push origin main
```

> **Как избежать конфликтов:** каждый работает только со своими файлами
> (см. таблицу выше). `Constants.h` — редкое исключение, обсудите изменения голосом.

---

## 4-недельный план разработки

### 📅 Неделя 1 — Скелет проекта (оба)

**Цель:** проект компилируется, окно открывается, игрок виден на экране.

| День   | Разработчик А                             | Разработчик Б                              |
|--------|-------------------------------------------|--------------------------------------------|
| 1–2    | Настройка CMake, SFML, первый `git push`  | Клонировать репо, проверить сборку         |
| 2–3    | `Player`: базовое движение WASD           | `Enemy`: конструктор, 3 типа, отрисовка    |
| 3–4    | `Bullet`: создание, движение              | `WaveManager`: очередь спавна, 1-я волна  |
| 4–5    | `Game`: цикл, камера, фон                 | `ResourceDrop`: появление, отрисовка       |
| 5–7    | Сборка и тест: враги двигаются к игроку   | Код-ревью друг друга через PR              |

**Коммиты недели 1 (А):**
```
git commit -m "Game: create window, main loop, delta time"
git commit -m "Player: WASD movement, map clamping"
git commit -m "Bullet: movement and out-of-bounds deactivation"
```

**Коммиты недели 1 (Б):**
```
git commit -m "Enemy: three types with different stats and colors"
git commit -m "WaveManager: spawn queue, random edge positions"
git commit -m "ResourceDrop: drop on enemy death"
```

---

### 📅 Неделя 2 — Игровой цикл (оба)

**Цель:** стрельба работает, враги умирают, ресурсы подбираются, HP убывает.

| День | Разработчик А                              | Разработчик Б                            |
|------|--------------------------------------------|-----------------------------------------|
| 1–2  | `Player`: прицел к мыши, стрельба ЛКМ     | `Weapon`: пистолет, выстрел, кулдаун    |
| 3–4  | `Game`: проверка столкновений пуль/врагов  | `Enemy`: контактный урон с кулдауном    |
| 4–5  | `Game`: подбор ресурсов, очистка мёртвых   | `WaveManager`: следующая волна, масштаб |
| 5–7  | Полный тест цикла «волна → победа → волна» | Отладка балансировки HP и урона         |

**Коммиты недели 2 (А):**
```
git commit -m "Player: mouse aim, LMB shoot trigger"
git commit -m "Game: bullet-enemy collision detection"
git commit -m "Game: resource pickup radius, dead object cleanup"
```

**Коммиты недели 2 (Б):**
```
git commit -m "Weapon: pistol stats, fire cooldown"
git commit -m "Enemy: contact damage with cooldown timer"
git commit -m "WaveManager: scaling enemy count per wave"
```

---

### 📅 Неделя 3 — Системы прогресса (оба)

**Цель:** три вида оружия, меню апгрейдов, HUD, сохранение.

| День | Разработчик А                              | Разработчик Б                               |
|------|--------------------------------------------|--------------------------------------------|
| 1–2  | `SaveSystem`: сохранение в файл            | `Weapon`: дробовик и пулемёт               |
| 3–4  | `Game`: загрузка сохранения при старте     | `UpgradeSystem`: 3 карточки, клик          |
| 4–5  | `Game`: ESC → сохранить и выйти            | `HUD`: полоска HP, волна, очки, ресурсы    |
| 5–7  | Интеграция: сохранение сохраняет оружие    | Тест всех апгрейдов и правильной смены орудия |

**Коммиты недели 3 (А):**
```
git commit -m "SaveSystem: write/read key=value text format"
git commit -m "Game: load save on startup, apply to player"
git commit -m "Game: ESC saves and exits cleanly"
```

**Коммиты недели 3 (Б):**
```
git commit -m "Weapon: add shotgun and machine gun factories"
git commit -m "UpgradeSystem: random card selection, click handler"
git commit -m "HUD: health bar, wave counter, resource display"
```

---

### 📅 Неделя 4 — Полировка и финал (оба)

**Цель:** игра работает без крашей, понятна без объяснений.

| День | Разработчик А                              | Разработчик Б                               |
|------|--------------------------------------------|--------------------------------------------|
| 1–2  | Game Over экран, перезапуск / выход        | Полоска HP у врагов, флеш при получении урона |
| 3–4  | Тест сохранения: закрыть → открыть → продолжить | Баланс: проверить, что волна 5–8 проходимы |
| 4–5  | Оптимизация: убедиться что 60 FPS стабильны | Финальный README, скриншот в репо          |
| 5–7  | Финальный merge обеих веток в `main`       | Тест на чистой машине (без SFML, установить с нуля) |

**Коммиты недели 4 (А):**
```
git commit -m "Game: game over overlay with score display"
git commit -m "Game: stable 60 FPS, dt clamped to 100ms"
git commit -m "Final: full save/load cycle tested"
```

**Коммиты недели 4 (Б):**
```
git commit -m "Enemy: health bar visible during combat"
git commit -m "Balance: wave 5-8 tuned for fair difficulty"
git commit -m "Docs: README updated with screenshots"
```

---

## Управление в игре

| Клавиша / Кнопка | Действие                  |
|------------------|---------------------------|
| W A S D          | Движение                  |
| Мышь             | Прицел                    |
| ЛКМ              | Стрельба                  |
| 1 / 2 / 3        | Смена оружия              |
| ESC              | Сохранить и выйти         |
| Enter            | Начать заново (Game Over) |

**Ресурсы** подбираются автоматически при приближении к монеткам.

---

## Частые проблемы

### «SFML not found»
```
CMake Error: Could not find SFML
```
→ Убедитесь, что путь `-DSFML_DIR="C:/SFML/lib/cmake/SFML"` указан верно.  
→ Папка должна содержать файл `SFMLConfig.cmake`.

### «sfml-graphics-2.dll not found»
→ Скопируйте все `.dll` из `C:\SFML\bin\` в папку `build\`.

### «font.ttf not found» (текст не отображается)
→ Поместите любой `.ttf` файл в `assets\font.ttf`.  
→ Альтернатива: игра автоматически пробует `C:\Windows\Fonts\arial.ttf`.

### Конфликт в Git
→ Открыть файл, найти `<<<<<<<`, вручную выбрать нужный код, убрать маркеры, `git add`, `git commit`.

### Низкий FPS
→ Проверьте, что `window.setFramerateLimit(60)` вызывается в конструкторе `Game`.  
→ Убедитесь, что сборка в Release-режиме: добавьте `-DCMAKE_BUILD_TYPE=Release` к cmake-команде.
