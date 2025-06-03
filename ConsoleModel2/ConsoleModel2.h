#pragma once
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <typeinfo>
#include <iomanip> 
#include "../Table2/Table.h"
#include "../Table2/ScanTable.h"
#include "../Table2/SortTable.h"
#include "../Table2/ArrayHashTable.h"
#include "../Table2/TreeTable.h"
#include "../Table2/BalTreeTable.h"
#include "../Table2/ListHashTable.h"

// Box-drawing characters definitions
#ifdef _WIN32
    // ASCII fallback for Windows
#define BOX_HORIZ "="
#define BOX_VERT "|"
#define BOX_CORNER_TL "+"
#define BOX_CORNER_TR "+"
#define BOX_CORNER_BL "+"
#define BOX_CORNER_BR "+"
#define BOX_T_LEFT "+"
#define BOX_T_RIGHT "+"
#define BOX_T_TOP "+"
#define BOX_T_BOTTOM "+"
#define BOX_CROSS "+"
#endif

#define tableCount 6  

class ConsoleModel {
private:
    int tablesSize;                       // Размер таблиц
    int tablesItems;                      // Количество элементов
    std::vector<Table<std::string, int>*> tables;  // Коллекция таблиц
    int currentTableIndex = 0;
    std::string CreateHorizontalLine(int width) const {
        return BOX_CORNER_TL + std::string(width, *BOX_HORIZ) + BOX_CORNER_TR;
    }
    std::string CreateHorizontalDivider(int width) const {
        return BOX_T_LEFT + std::string(width, *BOX_HORIZ) + BOX_T_RIGHT;
    }
    std::string CreateBottomLine(int width) const {
        return BOX_CORNER_BL + std::string(width, *BOX_HORIZ) + BOX_CORNER_BR;
    }
    void PrintInBox(const std::string& text, int boxWidth) const {
        std::cout << BOX_VERT << " " << std::left << std::setw(boxWidth - 2) << text << " " << BOX_VERT << "\n";
    }
    bool showTableTypeAlways = true; // Флаг для постоянного отображения типа таблицы
    void DrawHeader(const std::string& title);
    void DrawHorizontalLine();
    void DrawMessage(const std::string& message);
    void DrawTableInfo();
    void DrawMenu();
    void Update(const std::string& message = "");
    void FilesUpdate();
    int Random(int border);
public:
    ConsoleModel(int size = 10);
    ~ConsoleModel();
    void Insert(const std::string& key, int value);
    bool Find(const std::string& key) ;
    void Delete(const std::string& key);
    void Clear();
    void InsertMany(int count, int border);
    void CreateTables(int size, int count, int border);
    void SwitchTableType();
    std::string GetCurrentTableType() const;
	void ChangeTableSize(int newSize); 
    void PrintTable() const;
    void Run();
};