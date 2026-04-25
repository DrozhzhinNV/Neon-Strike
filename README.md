# Neon Strike

Шутер с видом сверху на C++ и SFML.  
Игрок отбивает волны врагов, собирает ресурсы и прокачивает персонажа.

## Структура репозитория

```
WaveShooter/
├── CMakeLists.txt        # система сборки
├── .gitignore
├── README.md
├── assets/
│   ├── font.ttf          # шрифт
│   └── README_font.txt
└── src/
    ├── Constants.h       # все числовые константы игры
    ├── main.cpp          # точка входа
    ├── Game.h / .cpp     # главный игровой цикл          
    ├── Player.h / .cpp   # игрок, движение, стрельба     
    ├── Bullet.h / .cpp   # пуля                           
    ├── SaveSystem.h/.cpp # сохранение/загрузка            
    ├── Enemy.h / .cpp    # враги трёх типов              
    ├── Weapon.h / .cpp   # виды оружия                   
    ├── WaveManager.h/.cpp# управление волнами             
    ├── ResourceDrop.h/.cpp  # выпадающие ресурсы          
    ├── UpgradeSystem.h/.cpp # меню прокачки              
    └── HUD.h / .cpp      # интерфейс (HP, волна, очки) 
```
