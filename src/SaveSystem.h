#pragma once
#include <string>


//  SaveSystem.h — сохранение/загрузка прогресса.


// Всё, что сохраняется между сессиями
struct SaveData {
    int   wave        = 1;
    int   score       = 0;
    int   resources   = 0;
    float playerHP    = 100.f;

    // Флаги разблокированного оружия
    bool  hasShotgun    = false;
    bool  hasMachineGun = false;

    // Уровни апгрейдов (0 = не куплен)
    int   speedLevel  = 0;
    int   damageLevel = 0;
    int   healthLevel = 0;
};

class SaveSystem {
public:
    // Записать данные в текстовый файл. Возвращает true при успехе.
    static bool save(const SaveData& data,
                     const std::string& filename = "save.dat");

    // Прочитать данные. Возвращает false если файл не найден.
    static bool load(SaveData& data,
                     const std::string& filename = "save.dat");
};
