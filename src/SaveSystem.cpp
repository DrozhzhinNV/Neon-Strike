#include "SaveSystem.h"
#include <fstream>
#include <iostream>
#include <string>

//  SaveSystem.cpp — сохранение в текстовый файл.


bool SaveSystem::save(const SaveData& d, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[Save] Не удалось открыть " << filename << "\n";
        return false;
    }

    file << "wave="         << d.wave          << "\n"
         << "score="        << d.score         << "\n"
         << "resources="    << d.resources     << "\n"
         << "playerHP="     << d.playerHP      << "\n"
         << "hasShotgun="   << (int)d.hasShotgun    << "\n"
         << "hasMachineGun="<< (int)d.hasMachineGun << "\n"
         << "speedLevel="   << d.speedLevel    << "\n"
         << "damageLevel="  << d.damageLevel   << "\n"
         << "healthLevel="  << d.healthLevel   << "\n";

    std::cout << "[Save] Сохранено в " << filename << "\n";
    return true;
}

bool SaveSystem::load(SaveData& d, const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false; // файл не найден — первый запуск

    std::string line;
    while (std::getline(file, line)) {
        auto sep = line.find('=');
        if (sep == std::string::npos) continue;

        std::string key   = line.substr(0, sep);
        std::string value = line.substr(sep + 1);

        // Разбор каждого поля
        if      (key == "wave")          d.wave          = std::stoi(value);
        else if (key == "score")         d.score         = std::stoi(value);
        else if (key == "resources")     d.resources     = std::stoi(value);
        else if (key == "playerHP")      d.playerHP      = std::stof(value);
        else if (key == "hasShotgun")    d.hasShotgun    = (value == "1");
        else if (key == "hasMachineGun") d.hasMachineGun = (value == "1");
        else if (key == "speedLevel")    d.speedLevel    = std::stoi(value);
        else if (key == "damageLevel")   d.damageLevel   = std::stoi(value);
        else if (key == "healthLevel")   d.healthLevel   = std::stoi(value);
    }

    std::cout << "[Save] Загружено из " << filename << "\n";
    return true;
}
