// main.cpp
#include <iostream>
#include "ArrayHashTable.h"
#include "ListHashTable.h"

int main() {
    ArrayHashTable<int, int> arrTable(10);
    //ListHashTable<int, int> listTable(10);

    arrTable.Insert(1, 10);
    arrTable.Insert(2, 12);

    std::string val;
    if (arrTable.Find(1)) {
        std::cout << "Found key 1: " << val << std::endl;
    }
    ArrayHashTable<int, int> table(5, 1);

    table.Insert(1, 10);
    table.Insert(2, 12);
    table.Clear();

    bool a = table.IsFull();
    bool b = table.Find(1);
    bool c = table.Find(2);
    bool d = table.IsEnd();  // после Reset/GoNext не найдёт
	printf("IsFull: %d, Find 1: %d, Find 2: %d, IsEnd: %d\n", a, b, c, d);
    //listTable.Insert(3);
    //listTable.Insert(13); // same bucket as 3 if size = 10

    //if (listTable.Find(13)) {
    //    std::cout << "Found key 13 in list: " << val << std::endl;
    //}

    return 0;
}
