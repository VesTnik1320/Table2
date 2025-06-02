#include "ConsoleModel2.h"
#include <iostream>
#include <iomanip>
#include <locale>
#include <clocale>
#include <limits>
#include <algorithm>

#define COLOR_RESET "\033[0m"
#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_BLUE "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN "\033[36m"


void SetRussianLocale() {
    setlocale(LC_ALL, "Russian");
    std::locale::global(std::locale(""));
    std::wcout.imbue(std::locale());
    std::wcin.imbue(std::locale());
}

ConsoleModel::ConsoleModel(int size) {
    SetRussianLocale();
    srand(time(nullptr));

    tablesSize = size;
    tablesItems = 0;
    errCount = 0;

    // Инициализация всех типов таблиц
	tables.push_back(new ScanTable<std::string, int>(tablesSize));//неотсортированная таблица
	tables.push_back(new SortTable<std::string, int>(tablesSize));//Сортированная таблица
	tables.push_back(new ArrayHashTable<std::string, int>(tablesSize));//Хэш таблица с массивом
	tables.push_back(new ListHashTable<std::string, int>(tablesSize)); //Хэш таблица со списком
    tables.push_back(new TreeTable<std::string, int>()); //Дерево
	tables.push_back(new BalTreeTable<std::string, int>()); //Сбалансированное дерево
}

ConsoleModel::~ConsoleModel() {
    for (auto table : tables) {
        delete table;
    }
}
//
//void ConsoleModel::Update(const std::string& message) {
//    system("cls");
//    const int BOX_WIDTH = 40;
//
//    // Верхняя граница
//    std::cout << CreateHorizontalLine(BOX_WIDTH) << "\n";
//
//    // Заголовок
//    std::cout << BOX_VERT << "      Управление таблицами      " << BOX_VERT << "\n";
//
//    // Разделитель
//    std::cout << CreateHorizontalDivider(BOX_WIDTH) << "\n";
//
//    // Содержимое
//    PrintInBox("Размер таблиц: " + std::to_string(tablesSize), BOX_WIDTH);
//    PrintInBox("Элементов: " + std::to_string(tablesItems), BOX_WIDTH);
//    PrintInBox("Ошибок: " + std::to_string(errCount), BOX_WIDTH);
//    PrintInBox("Текущая таблица: " + GetCurrentTableType(), BOX_WIDTH);
//    PrintInBox("Эффективность: " + std::to_string(tables[currentTableIndex]->GetEff()), BOX_WIDTH);
//
//    // Разделитель
//    std::cout << CreateHorizontalDivider(BOX_WIDTH) << "\n";
//
//    // Меню
//    PrintInBox("1: Сменить тип таблицы", BOX_WIDTH);
//    PrintInBox("2: Добавить элемент", BOX_WIDTH);
//	PrintInBox("3: Удалить элемент", BOX_WIDTH);
//	PrintInBox("4: Найти элемент", BOX_WIDTH);
//	PrintInBox("5: Добавить много элементов", BOX_WIDTH);
//	PrintInBox("6: Очистить таблицу", BOX_WIDTH);
//	PrintInBox("7: Показать таблицу", BOX_WIDTH);
//	PrintInBox("8: Сохранить данные в файлы", BOX_WIDTH);
//	PrintInBox("q: Выйти", BOX_WIDTH);
//
//    // Сообщение (если есть)
//    if (!message.empty()) {
//        std::cout << CreateHorizontalDivider(BOX_WIDTH) << "\n";
//        PrintInBox("\033[31m" + message + "\033[0m", BOX_WIDTH);
//    }
//
//    // Нижняя граница
//    std::cout << CreateBottomLine(BOX_WIDTH) << "\n";
//    std::cout << "> ";
//}

void ConsoleModel::Insert(const std::string& key, int value) {
    try {
        tables[currentTableIndex]->Insert( key, static_cast<unsigned __int64>(value) );
        tablesItems++;
        Update("Элемент добавлен");
    }
    catch (const std::exception& e) {
        errCount++;
        Update(std::string("Ошибка: ") + e.what());
    }
}

bool ConsoleModel::Find(const std::string& key) {
    try {
        bool found = tables[currentTableIndex]->Find(key);
        Update(found ? "Элемент найден" : "Элемент не найден");
        return found;
    }
    catch (const std::exception& e) {
        errCount++;
        Update(std::string("Ошибка: ") + e.what());
        return false;
    }
}

void ConsoleModel::Delete(const std::string& key) {
    try {
        tables[currentTableIndex]->Delete(key);
        tablesItems--;
        Update("Элемент удален");
    }
    catch (const std::exception& e) {
        errCount++;
        Update(std::string("Ошибка: ") + e.what());
    }
}

void ConsoleModel::Clear() {
    tables[currentTableIndex]->Clear();
    tablesItems = 0;
    Update("Таблица очищена");
}

void ConsoleModel::InsertMany(int count, int border) {
    if (currentTableIndex < 0 || currentTableIndex >= tables.size() || !tables[currentTableIndex]) {
        std::cout << "Ошибка: текущая таблица не выбрана или не инициализирована.\n";
        return;
    }

    int inserted = 0;
    int step = std::max(1, count / 20);

    std::cout << "Прогресс: [";
    for (int i = 0; i < count; i++) {
        try {
            int r = Random(border);
            tables[currentTableIndex]->Insert(std::to_string(r), static_cast<unsigned __int64>(r));
            inserted++;
        }
        catch (...) {
            errCount++;
        }

        if (i % step == 0) {
            std::cout << "=" << std::flush;
        }
    }
    std::cout << "] 100%\n";

    tablesItems += inserted;
    Update("Добавлено " + std::to_string(inserted) + " элементов");

    std::cout << "Нажмите Enter для продолжения...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void ConsoleModel::CreateTables(int size, int count, int border) {
    tablesSize = size;
    tablesItems = 0;
    errCount = 0;

    // Очищаем старые таблицы
    for (auto table : tables) {
        delete table;
    }
    tables.clear();

    // Создаем новые таблицы
    tables.push_back(new ScanTable<std::string, int>(tablesSize));
    tables.push_back(new SortTable<std::string, int>(tablesSize));
    tables.push_back(new ArrayHashTable<std::string, int>(tablesSize));
    tables.push_back(new ListHashTable<std::string, int>(tablesSize));
    tables.push_back(new TreeTable<std::string, int>()); 
    tables.push_back(new BalTreeTable<std::string, int>());

    // Добавляем элементы
    InsertMany(count, border);
}

void ConsoleModel::SwitchTableType() {
    currentTableIndex = (currentTableIndex + 1) % tables.size();
    Update("Тип таблицы изменен на: " + GetCurrentTableType());
}

std::string ConsoleModel::GetCurrentTableType() const {
    if (dynamic_cast<ScanTable<std::string, int>*>(tables[currentTableIndex])) {
        return "ScanTable";
    }
    if (dynamic_cast<SortTable<std::string, int>*>(tables[currentTableIndex])) {
        return "SortTable";
    }
    if (dynamic_cast<ArrayHashTable<std::string, int>*>(tables[currentTableIndex])) {
        return "ArrayHashTable";
    }
    if (dynamic_cast<TreeTable<std::string, int>*>(tables[currentTableIndex])) {
        return "TreeTable";
    }
    if (dynamic_cast<ListHashTable<std::string, int>*>(tables[currentTableIndex])) {
        return "ListHashTable";
    }
    else if (dynamic_cast<BalTreeTable<std::string, int>*>(tables[currentTableIndex])) {
        return "BalTreeTable";
    }
    return "Unknown";
}

void ConsoleModel::PrintTable() const {
    const int KEY_WIDTH = 12;
    const int VAL_WIDTH = 10;
    const int TOTAL_WIDTH = KEY_WIDTH + VAL_WIDTH + 7;

    std::cout << "\nСодержимое таблицы (" << GetCurrentTableType() << "):\n";
    std::cout << BOX_CORNER_TL
        << std::string(KEY_WIDTH + 2, *BOX_HORIZ)
        << BOX_T_TOP
        << std::string(VAL_WIDTH + 2, *BOX_HORIZ)
        << BOX_CORNER_TR << "\n";

    std::cout << BOX_VERT << " " << std::setw(KEY_WIDTH) << "Ключ" << " "
        << BOX_VERT << " " << std::setw(VAL_WIDTH) << "Значение" << " "
        << BOX_VERT << "\n";

    std::cout << BOX_T_LEFT
        << std::string(KEY_WIDTH + 2, *BOX_HORIZ)
        << BOX_CROSS
        << std::string(VAL_WIDTH + 2, *BOX_HORIZ)
        << BOX_T_RIGHT << "\n";

    tables[currentTableIndex]->Reset();
    while (!tables[currentTableIndex]->IsEnd()) {
        auto record = tables[currentTableIndex]->GetCurr();
        std::cout << BOX_VERT << " " << std::setw(KEY_WIDTH) << record.key << " "
            << BOX_VERT << " " << std::setw(VAL_WIDTH) << record.val << " "
            << BOX_VERT << "\n";
        tables[currentTableIndex]->GoNext();
    }

    std::cout << BOX_CORNER_BL
        << std::string(KEY_WIDTH + 2, *BOX_HORIZ)
        << BOX_T_BOTTOM
        << std::string(VAL_WIDTH + 2, *BOX_HORIZ)
        << BOX_CORNER_BR << "\n";
}

void ConsoleModel::FilesUpdate() {
    std::vector<std::string> filenames = {
        "ScanTable.txt",
        "SortTable.txt",
        "ArrayHashTable.txt",
        "ListHashTable.txt",
        "TreeTable.txt",
        "BalTreeTable.txt"
    };

    for (size_t i = 0; i < tables.size() && i < filenames.size(); i++) {
        std::ofstream out(filenames[i]);
        if (out.is_open()) {
            tables[i]->Reset();
            while (!tables[i]->IsEnd()) {
                auto record = tables[i]->GetCurr();
                out << record.key << " : " << record.val << "\n";
                tables[i]->GoNext();
            }
            out.close();
        }
    }
    Update("Данные сохранены в файлы");
}

int ConsoleModel::Random(int border) {
    return rand() % border;
}

//void ConsoleModel::Run() {
//    Update();
//    char command;
//    while (std::cin >> command && command != 'q') {
//        switch (command) {
//        case '1': SwitchTableType(); break;
//        case '2': {
//            std::string key;
//            int value;
//            std::cin >> key >> value;
//            Insert(key, value);
//            break;
//        }
//        case '3': {
//            std::string key;
//            std::cin >> key;
//            Delete(key);
//            break;
//        }
//        case '4': {
//            std::string key;
//            std::cin >> key;
//            Find(key);
//            break;
//        }
//        case '5': {
//            int count, border;
//            std::cin >> count >> border;
//            InsertMany(count, border);
//            break;
//        }
//        case '6': Clear(); break;
//        case '7':
//            PrintTable();
//            std::cout << "Нажмите Enter для продолжения...";
//            std::cin.ignore();
//            std::cin.get();
//            Update();
//            break;
//        case '8': FilesUpdate(); break;
//        default:
//            Update("Неизвестная команда");
//            break;
//        }
//    }
//    Update("Работа завершена");
//}
void ConsoleModel::Run() {
    Update();
    char command;
    while (std::cin >> command && command != 'q') {
        switch (command) {
        case '1': SwitchTableType(); break;
        case '2': {
            std::string key;
            int value;
            std::cin >> key >> value;
            Insert(key, value);
            break;
        }
        case '3': {
            std::string key;
            std::cin >> key;
            Delete(key);
            break;
        }
        case '4': {
            std::string key;
            std::cin >> key;
            Find(key);
            break;
        }
        case '5': {
            int count, border;
            std::cin >> count >> border;
            InsertMany(count, border);
            break;
        }
        case '6': Clear(); break;
        case '7':
            PrintTable();
            std::cout << "Нажмите Enter для продолжения...";
            std::cin.ignore();
            std::cin.get();
            Update();
            break;
        case '8': FilesUpdate(); break;
        case '9': {
            int newSize;
            std::cin >> newSize;
            ChangeTableSize(newSize);
            break;
        }
        default:
            Update("Неизвестная команда");
            break;
        }
    }
    Update("Работа завершена");
}

void ConsoleModel::DrawHeader(const std::string& title) {
    const int width = 40;
    std::cout << BOX_CORNER_TL << std::string(width - 2, *BOX_HORIZ) << BOX_CORNER_TR << "\n";
    std::cout << BOX_VERT << std::setw((width + title.length()) / 2) << title
        << std::setw(width - (width + title.length()) / 2) << BOX_VERT << "\n";
    std::cout << BOX_T_LEFT << std::string(width - 2, *BOX_HORIZ) << BOX_T_RIGHT << "\n";
}

void ConsoleModel::DrawHorizontalLine() {
    const int width = 40;
    std::cout << BOX_T_LEFT << std::string(width - 2, *BOX_HORIZ) << BOX_T_RIGHT << "\n";
}

void ConsoleModel::DrawMessage(const std::string& message) {
    if (!message.empty()) {
        std::cout << BOX_VERT << " " << COLOR_RED << std::left << std::setw(36) << message
            << COLOR_RESET << BOX_VERT << "\n";
        DrawHorizontalLine();
    }
}

void ConsoleModel::DrawTableInfo() {
    const int width = 40;
    std::cout << BOX_VERT << " Текущая таблица: " << std::left << std::setw(21)
        << GetCurrentTableType() << BOX_VERT << "\n";
    std::cout << BOX_VERT << " Размер таблицы: " << std::setw(21) << tablesSize << BOX_VERT << "\n";
    std::cout << BOX_VERT << " Элементов: " << std::setw(26) << tablesItems << BOX_VERT << "\n";
    std::cout << BOX_VERT << " Ошибок: " << std::setw(29) << errCount << BOX_VERT << "\n";
    std::cout << BOX_VERT << " Эффективность: " << std::setw(22)
        << tables[currentTableIndex]->GetEff() << BOX_VERT << "\n";
    DrawHorizontalLine();
}

void ConsoleModel::DrawMenu() {
    const int width = 40;
    std::cout << BOX_VERT << " 1: Сменить тип таблицы" << std::setw(16) << " " << BOX_VERT << "\n";
    std::cout << BOX_VERT << " 2: Добавить элемент(ключ значение)" << std::setw(18) << " " << BOX_VERT << "\n";
    std::cout << BOX_VERT << " 3: Удалить элемент(ключ)" << std::setw(19) << " " << BOX_VERT << "\n";
    std::cout << BOX_VERT << " 4: Найти элемент(ключ)" << std::setw(21) << " " << BOX_VERT << "\n";
    std::cout << BOX_VERT << " 5: Добавить несколько элементов(количество граница)" << std::setw(18) << " " << BOX_VERT << "\n";
    std::cout << BOX_VERT << " 6: Очистить таблицу" << std::setw(19) << " " << BOX_VERT << "\n";
    std::cout << BOX_VERT << " 7: Вывести таблицу" << std::setw(20) << " " << BOX_VERT << "\n";
    std::cout << BOX_VERT << " 8: Сохранить в файлы" << std::setw(18) << " " << BOX_VERT << "\n";
    std::cout << BOX_VERT << " 9: Изменить размер" << std::setw(19) << " " << BOX_VERT << "\n";
    std::cout << BOX_VERT << " q: Выход" << std::setw(30) << " " << BOX_VERT << "\n";
    std::cout << BOX_CORNER_BL << std::string(width - 2, *BOX_HORIZ) << BOX_CORNER_BR << "\n";
    std::cout << "> ";
}

//void ConsoleModel::Update(const std::string& message) {
//    system("cls");
//    DrawHeader("Управление таблицами");
//    DrawTableInfo();
//    DrawMessage(message);
//    DrawMenu();
//}
void ConsoleModel::Update(const std::string& message) {
    system("cls");
    const int BOX_WIDTH = 60; // Увеличено с 40 до 60 для удобства

    // Верхняя граница
    std::cout << BOX_CORNER_TL << std::string(BOX_WIDTH - 2, *BOX_HORIZ) << BOX_CORNER_TR << "\n";

    // Заголовок
    std::string title = "Управление таблицами";
    int padding = (BOX_WIDTH - 2 - title.size()) / 2;
    std::cout << BOX_VERT << std::string(padding, ' ') << title
        << std::string(BOX_WIDTH - 2 - padding - title.size(), ' ')
        << BOX_VERT << "\n";

    // Разделитель
    std::cout << BOX_T_LEFT << std::string(BOX_WIDTH - 2, *BOX_HORIZ) << BOX_T_RIGHT << "\n";

    // Содержимое
    PrintInBox("Размер таблиц: " + std::to_string(tablesSize), BOX_WIDTH);
    PrintInBox("Элементов: " + std::to_string(tablesItems), BOX_WIDTH);
    PrintInBox("Ошибок: " + std::to_string(errCount), BOX_WIDTH);
    PrintInBox("Текущая таблица: " + GetCurrentTableType(), BOX_WIDTH);
    PrintInBox("Эффективность: " + std::to_string(tables[currentTableIndex]->GetEff()), BOX_WIDTH);

    std::cout << BOX_T_LEFT << std::string(BOX_WIDTH - 2, *BOX_HORIZ) << BOX_T_RIGHT << "\n";

    // Меню
    PrintInBox("1: Сменить тип таблицы", BOX_WIDTH);
    PrintInBox("2: Добавить элемент (ключ значение)", BOX_WIDTH);
    PrintInBox("3: Удалить элемент (ключ)", BOX_WIDTH);
    PrintInBox("4: Найти элемент (ключ)", BOX_WIDTH);
    PrintInBox("5: Добавить несколько элементов (количество граница)", BOX_WIDTH);
    PrintInBox("6: Очистить таблицу", BOX_WIDTH);
    PrintInBox("7: Показать таблицу", BOX_WIDTH);
    PrintInBox("8: Сохранить данные в файлы", BOX_WIDTH);
    PrintInBox("9: Изменить размер таблицы (размер)", BOX_WIDTH);
    PrintInBox("q: Выйти", BOX_WIDTH);

    // Сообщение (если есть)
    if (!message.empty()) {
        std::cout << BOX_T_LEFT << std::string(BOX_WIDTH - 2, *BOX_HORIZ) << BOX_T_RIGHT << "\n";
        PrintInBox(COLOR_RED + message + COLOR_RESET, BOX_WIDTH);
    }

    // Нижняя граница
    std::cout << BOX_CORNER_BL << std::string(BOX_WIDTH - 2, *BOX_HORIZ) << BOX_CORNER_BR << "\n";
    std::cout << "> ";
}

void PrintInBoxLine(const std::string& content, int boxWidth) {
    const int maxContentWidth = boxWidth - 4; // 2 символа рамки + 2 пробела
    std::string trimmed = content;
    if (content.length() > maxContentWidth)
        trimmed = content.substr(0, maxContentWidth - 3) + "...";

    std::cout << BOX_VERT << " " << std::left << std::setw(maxContentWidth) << trimmed << " " << BOX_VERT << "\n";
}

void ConsoleModel::ChangeTableSize(int newSize) {
    if (newSize <= 0) {
        Update("Ошибка: размер должен быть положительным");
        return;
    }

    tablesSize = newSize;
    for (auto table : tables) {
        table->Resize(newSize); // Предполагается, что у ваших таблиц есть метод Resize
    }
    Update("Размер таблицы изменен на " + std::to_string(newSize));
}