cmake_minimum_required(VERSION 3.16)
project(WaveShooter)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Player.cpp
    src/Enemy.cpp
    src/Bullet.cpp
    src/Weapon.cpp
    src/WaveManager.cpp
    src/ResourceDrop.cpp
    src/HUD.cpp
    src/UpgradeSystem.cpp
    src/SaveSystem.cpp
    src/Particle.cpp
)

add_executable(WaveShooter ${SOURCES})

# ============================================================
#  Поиск SFML.
#  Укажите путь к SFML через cmake-gui или командную строку:
#    -DSFML_DIR="C:/SFML/lib/cmake/SFML"
# ============================================================
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
target_link_libraries(WaveShooter PRIVATE sfml-graphics sfml-window sfml-system)

# ============================================================
#  Копирование DLL SFML и ассетов в папку с exe (Windows)
# ============================================================
if(WIN32)
    # Папка с DLL SFML (измените, если у вас другой путь)
    set(SFML_BIN_DIR "C:/SFML/bin")

    # Копируем sfml-graphics (с -d для Debug)
    add_custom_command(TARGET WaveShooter POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SFML_BIN_DIR}/sfml-graphics$<$<CONFIG:Debug>:-d>-2.dll"
            "$<TARGET_FILE_DIR:WaveShooter>"
        COMMENT "Copying sfml-graphics DLL..."
    )

    # Копируем sfml-window
    add_custom_command(TARGET WaveShooter POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SFML_BIN_DIR}/sfml-window$<$<CONFIG:Debug>:-d>-2.dll"
            "$<TARGET_FILE_DIR:WaveShooter>"
        COMMENT "Copying sfml-window DLL..."
    )

    # Копируем sfml-system
    add_custom_command(TARGET WaveShooter POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SFML_BIN_DIR}/sfml-system$<$<CONFIG:Debug>:-d>-2.dll"
            "$<TARGET_FILE_DIR:WaveShooter>"
        COMMENT "Copying sfml-system DLL..."
    )

    # Копируем папку assets (шрифты и т.п.)
    add_custom_command(TARGET WaveShooter POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:WaveShooter>/assets"
        COMMENT "Copying assets directory..."
    )
else()
    # Для других ОС просто копируем ассеты
    add_custom_command(TARGET WaveShooter POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:WaveShooter>/assets"
    )
endif()