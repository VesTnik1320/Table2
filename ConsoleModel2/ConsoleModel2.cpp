#include "ConsoleModel2.h"
#include <iostream>
#include <iomanip>
#include <locale>
#include <clocale>
#include <limits>
#include <algorithm>
#include <sstream>
#include <string>
#include <fstream>  

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

    // Создаем таблицы с числовыми ключами
    tables.push_back(new ScanTable<int, int>(tablesSize));
    tables.push_back(new SortTable<int, int>(tablesSize));
    tables.push_back(new ArrayHashTable<int, int>(tablesSize));
    tables.push_back(new ListHashTable<int, int>(tablesSize));
    tables.push_back(new TreeTable<int, int>());
    tables.push_back(new BalTreeTable<int, int>());
}

ConsoleModel::~ConsoleModel() {
    for (auto table : tables) {
        delete table;
    }
}

void ConsoleModel::Insert(const int& key, int value) {
    try {
        tables[currentTableIndex]->Insert(key, static_cast<unsigned __int64>(value));
        tablesItems++;
        Update("Элемент добавлен");
    }
    catch (const std::exception& e) {
        Update(std::string("Ошибка: ") + e.what());
    }
}

bool ConsoleModel::Find(const int& key) {
    try {
        bool found = tables[currentTableIndex]->Find(key);
        Update(found ? "Элемент найден" : "Элемент не найден");
        return found;
    }
    catch (const std::exception& e) {
        Update(std::string("Ошибка: ") + e.what());
        return false;
    }
}

void ConsoleModel::Delete(const int& key) {
    try {
        tables[currentTableIndex]->Delete(key);
        tablesItems--;
        Update("Элемент удален");
    }
    catch (const std::exception& e) {
        Update(std::string("Ошибка: ") + e.what());
    }
}

void ConsoleModel::Clear() {
    tables[currentTableIndex]->Clear();
    tablesItems = 0;
    Update("Таблица очищена");
}

void ConsoleModel::InsertMany(int count, int border) {
    int inserted = 0;
    int step = std::max(1, count / 20);

    std::cout << "Прогресс: [";
    while (inserted < count) {
        try {
            int key = Random(border);
            int value = Random(border * 10);

            if (auto balTree = dynamic_cast<BalTreeTable<int, int>*>(tables[currentTableIndex])) {
                balTree->Inserte(key, value);
            }
            else {
                tables[currentTableIndex]->Insert(key, static_cast<unsigned __int64>(value));
            }

            inserted++;
            if (inserted % step == 0) {
                std::cout << "=" << std::flush;
            }
        }
        catch (...) {
            // Повтор при конфликте
        }
    }
    std::cout << "] 100%\n";
    tablesItems += inserted;
    Update("Добавлено " + std::to_string(inserted) + " элементов");
}

void ConsoleModel::CreateTables(int size, int count, int border) {
    tablesSize = size;
    tablesItems = 0;

    for (auto table : tables) delete table;
    tables.clear();

    tables.push_back(new ScanTable<int, int>(tablesSize));
    tables.push_back(new SortTable<int, int>(tablesSize));
    tables.push_back(new ArrayHashTable<int, int>(tablesSize));
    tables.push_back(new ListHashTable<int, int>(tablesSize));
    tables.push_back(new TreeTable<int, int>());
    tables.push_back(new BalTreeTable<int, int>());

    InsertMany(count, border);
}

void ConsoleModel::SwitchTableType() {
    currentTableIndex = (currentTableIndex + 1) % tables.size();
    Update("Тип таблицы изменен на: " + GetCurrentTableType());
}

std::string ConsoleModel::GetCurrentTableType() const {
    // BalTreeTable проверяется до TreeTable
    if (dynamic_cast<BalTreeTable<int, int>*>(tables[currentTableIndex])) return "BalTreeTable";
    if (dynamic_cast<TreeTable<int, int>*>(tables[currentTableIndex])) return "TreeTable";
    if (dynamic_cast<SortTable<int, int>*>(tables[currentTableIndex])) return "SortTable";
    if (dynamic_cast<ScanTable<int, int>*>(tables[currentTableIndex])) return "ScanTable";
    if (dynamic_cast<ArrayHashTable<int, int>*>(tables[currentTableIndex])) return "ArrayHashTable";
    if (dynamic_cast<ListHashTable<int, int>*>(tables[currentTableIndex])) return "ListHashTable";
    return "Unknown";
}

void ConsoleModel::PrintTable() const {
    const int KEY_WIDTH = 12;
    const int VAL_WIDTH = 10;

    for (const auto& table : tables) {
        const int TOTAL_WIDTH = KEY_WIDTH + VAL_WIDTH + 7;

        std::cout << "\nСодержимое таблицы (" << table->GetTypeName() << "):\n";
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

        table->Reset();
        while (!table->IsEnd()) {
            auto record = table->GetCurr();
            std::cout << BOX_VERT << " " << std::setw(KEY_WIDTH) << record.key << " "
                << BOX_VERT << " " << std::setw(VAL_WIDTH) << record.val << " "
                << BOX_VERT << "\n";
            table->GoNext();
        }

        std::cout << BOX_CORNER_BL
            << std::string(KEY_WIDTH + 2, *BOX_HORIZ)
            << BOX_T_BOTTOM
            << std::string(VAL_WIDTH + 2, *BOX_HORIZ)
            << BOX_CORNER_BR << "\n";
    }
}

template<typename Key, typename Value>
void PrintTreeToFile(TreeNode<Key, Value>* node, std::ofstream& out, const std::string& prefix = "", bool isLeft = true) {
    if (!node) return;

    out << prefix;
    out << (isLeft ? "|-- " : "`-- ");
    out << "[" << node->rec.key << "] : " << node->rec.val;
    if (node->bal != BAL_OK) {
        out << " (баланс: " << node->bal << ")";
    }
    out << "\n";

    std::string newPrefix = prefix + (isLeft ? "|   " : "    ");
    if (node->pLeft || node->pRight) {
        if (node->pLeft)  PrintTreeToFile(node->pLeft, out, newPrefix, true);
        if (node->pRight) PrintTreeToFile(node->pRight, out, newPrefix, false);
    }
}

void ConsoleModel::FilesUpdate() {
    std::vector<std::string> paths = {
        "ScanTable.txt", "SortTable.txt", "ArrayHashTable.txt",
        "ListHashTable.txt", "TreeTable.txt", "BalTreeTable.txt"
    };

    for (size_t i = 0; i < tables.size(); i++) {
        std::ofstream out(paths[i]);
        if (!out) {
            std::cerr << "Ошибка открытия файла: " << paths[i] << "\n";
            continue;
        }

        if (i == 4 || i == 5) { // TreeTable и BalTreeTable
            if (auto tree = dynamic_cast<TreeTable<int, int>*>(tables[i])) {
                PrintTreeToFile(tree->GetRoot(), out);
            }
        }
        else {
            tables[i]->Reset();
            while (!tables[i]->IsEnd()) {
                auto record = tables[i]->GetCurr();
                out << record.key << " : " << record.val << "\n";
                tables[i]->GoNext();
            }
        }
    }
    Update("Данные сохранены в файлы");
}

int ConsoleModel::Random(int border) {
    return rand() % border;
}

void ConsoleModel::Run() {
    Update();
    char command;
    while (std::cin >> command && command != 'q') {
        switch (command) {
        case '1': SwitchTableType(); break;
        case '2': {
            int key, value;
            std::cin >> key >> value;
            Insert(key, value);
            break;
        }
        case '3': {
            int key;
            std::cin >> key;
            Delete(key);
            break;
        }
        case '4': {
            int key;
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
        case 's': {
            if (auto sortTable = dynamic_cast<SortTable<int, int>*>(tables[currentTableIndex])) {
                std::cout << "Выберите метод сортировки:\n"
                    << "1. SelectSort\n"
                    << "2. QSortRec\n"
                    << "3. MergeSort\n> ";
                char method;
                std::cin >> method;

                switch (method) {
                case '1':
                    sortTable->SelectSort();
                    Update("Отсортировано SelectSort");
                    break;
                case '2':
                    sortTable->QSortRec(0, sortTable->GetDataCount() - 1);
                    Update("Отсортировано QSortRec");
                    break;
                case '3':
                    sortTable->MergeSort(0, sortTable->GetDataCount() - 1);
                    Update("Отсортировано MergeSort");
                    break;
                default:
                    Update("Неизвестный метод сортировки");
                }
            }
            else {
                Update("Ошибка: текущая таблица не является SortTable");
            }
            break;
        }
        default:
            Update("Неизвестная команда");
            break;
        }
    }
    Update("Работа завершена");
}

void ConsoleModel::ChangeTableSize(int newSize) {
    if (newSize <= 0) {
        Update("Ошибка: размер должен быть положительным");
        return;
    }

    tablesSize = newSize;
    for (auto table : tables) {
        table->Resize(newSize);
    }
    Update("Размер таблицы изменен на " + std::to_string(newSize));
}

void ConsoleModel::Update(const std::string& message) {
    system("cls");
    const int BOX_WIDTH = 60;

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
    PrintInBox("s: Сортировать SortTable", BOX_WIDTH);
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