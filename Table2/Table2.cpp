// main.cpp
#include <iostream>
#include "ArrayHashTable.h"
#include "ListHashTable.h"

int main() {
    const int TABLE_SIZE = 10;
    ArrayHashTable<int, string> table(TABLE_SIZE, 1);

    int testKey1 = 15;
    int position1 = table.HashFunc(testKey1);

    table.Insert(testKey1, "value1");

    table.Reset();
    bool found = false;
    for (int i = 0; i < TABLE_SIZE && !table.IsEnd(); i++) {
        if (table.GetCurrKey() == testKey1) {
            printf("Index: %d, Position: %d\n", i, position1); 
            found = true;
            break;
        }
        table.GoNext();
    }
    printf("%s", found ? "true" : "false");
    return 0;
}
