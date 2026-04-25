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
#  Копировать DLL SFML рядом с .exe (только Windows)
# ============================================================
if(WIN32)
    add_custom_command(TARGET WaveShooter POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "C:/SFML/bin/sfml-graphics-2.dll" $<TARGET_FILE_DIR:WaveShooter>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "C:/SFML/bin/sfml-window-2.dll" $<TARGET_FILE_DIR:WaveShooter>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "C:/SFML/bin/sfml-system-2.dll" $<TARGET_FILE_DIR:WaveShooter>
        # Копирование ассетов (если папка существует)
        COMMAND ${CMAKE_COMMAND} -E copy_directory 
            "${CMAKE_SOURCE_DIR}/assets" 
            "$<TARGET_FILE_DIR:WaveShooter>/assets"
    )
endif()

# Копировать папку assets рядом с .exe
add_custom_command(TARGET WaveShooter POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:WaveShooter>/assets"
)
