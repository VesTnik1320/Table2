#include "pch.h"
#include "../Table2/ArrayHashTable.h"
#include "../Table2/Record.h"
#include "../Table2/ScanTable.h"
#include "../Table2/SortTable.h"
#include "../Table2/Table.h"
#include "../Table2/ListHashTable.h"
#include "../Table2/TreeTable.h"
#include "../Table2/BalTreeTable.h"

TEST(ArrayHashTable, InsertFullThrows) {
    ArrayHashTable<int, int> table(10, 1);
    for (int i = 0; i < 10; i++) table.Insert(i, i);
    EXPECT_ANY_THROW(table.Insert(10, 10));
}

TEST(ArrayHashTable, InsertExistingThrows) {
    ArrayHashTable<int, int> table(5, 1);
    table.Insert(10, 100);
    EXPECT_ANY_THROW(table.Insert(10, 10));
}

TEST(ArrayHashTable, DeleteNonExistingThrows) {
    ArrayHashTable<int, int> table(5, 1);
    EXPECT_ANY_THROW(table.Delete(10));
}

TEST(ArrayHashTable, InsertAndFindMultiple) {
    ArrayHashTable<int, int> table(5, 1);
    int keys[] = { 40, 2, 1, 4, 0 };
    for (int key : keys) table.Insert(key, key);
    for (int key : keys) EXPECT_TRUE(table.Find(key));
}

TEST(ArrayHashTable, DeleteAndIterator) {
    ArrayHashTable<int, int> table(5, 1);
    int initial[] = { 44, 5, 1, 4, 0 };
    int expected[] = { 0, 1, 5, 4 };

    for (int key : initial) table.Insert(key, key);
    table.Delete(44);

    std::vector<int> actual;
    for (table.Reset(); !table.IsEnd(); table.GoNext())
        actual.push_back(table.GetCurrVal());

    std::sort(actual.begin(), actual.end());
    std::sort(expected, expected + 4);
    for (size_t i = 0; i < 4; i++) EXPECT_EQ(actual[i], expected[i]);
}

TEST(ArrayHashTable, AssignmentOperator) {
    ArrayHashTable<int, int> table1(5, 1), table2(5, 1);
    for (int i = 0; i < 5; i++) table1.Insert(i, i);
    table2 = table1;
    table2.Delete(4);
    EXPECT_TRUE(table1.Find(4));
    EXPECT_FALSE(table2.Find(4));
}

TEST(ArrayHashTable, FindExistingKey) {
    ArrayHashTable<int, int> table(5, 1);
    table.Insert(123, 456);
    EXPECT_TRUE(table.Find(123));
}

TEST(ArrayHashTable, FindNonExistingKey) {
    ArrayHashTable<int, int> table(5, 1);
    table.Insert(123, 456);
    EXPECT_FALSE(table.Find(321));
}

TEST(ArrayHashTable, ClearTableCheckCountAndEmpty) {
    ArrayHashTable<int, int> table(5, 1);
    table.Insert(1, 2);
    table.Insert(2, 3);
    table.Clear();
    EXPECT_EQ(table.GetDataCount(), 0);
    EXPECT_TRUE(table.IsEmpty());
}

TEST(ArrayHashTable, ChainIntegrityAfterDelete) {
    ArrayHashTable<int, int> table(100, 1);
    int keys[] = { 5, 105, 205, 305, 405 };
    for (int key : keys) table.Insert(key, key);
    table.Delete(205);
    EXPECT_TRUE(table.Find(5));
    EXPECT_TRUE(table.Find(105));
    EXPECT_TRUE(table.Find(305));
    EXPECT_TRUE(table.Find(405));
}

TEST(ArrayHashTable, InsertAndFind) {
    ArrayHashTable<int, int> table(5, 1);
    table.Insert(1, 5);
    table.Insert(3, 6);
    table.Insert(2, 9);
    EXPECT_TRUE(table.Find(1));
    EXPECT_TRUE(table.Find(2));
    EXPECT_TRUE(table.Find(3));
    EXPECT_FALSE(table.Find(99));
}

TEST(ArrayHashTable, InsertOverflowDoesNotThrowEarly) {
    ArrayHashTable<int, int> table(3, 1);
    table.Insert(1, 5);
    table.Insert(3, 6);
    table.Insert(2, 9);
    EXPECT_TRUE(table.IsFull());
}

TEST(ArrayHashTable, DeleteAndFind) {
    ArrayHashTable<int, int> table(5, 1);
    table.Insert(1, 5);
    table.Insert(3, 6);
    table.Insert(2, 9);
    EXPECT_TRUE(table.Find(1));
    table.Delete(1);
    EXPECT_FALSE(table.Find(1));
}

TEST(ArrayHashTable, DeleteNonExistentThrows) {
    ArrayHashTable<int, int> table(5, 1);
    table.Insert(1, 5);
    table.Delete(1);
    EXPECT_ANY_THROW(table.Delete(1));
}

TEST(ArrayHashTable, ClearTable) {
    ArrayHashTable<int, int> table(5, 1);
    table.Insert(1, 5);
    table.Insert(2, 9);
    table.Clear();
    EXPECT_EQ(table.IsFull(), false);
    EXPECT_FALSE(table.Find(1));
    EXPECT_FALSE(table.Find(2));
}

TEST(ArrayHashTable, IteratorWorksAfterInsert) {
    ArrayHashTable<int, int> table(5, 1);
    table.Insert(10, 12);
    table.Insert(20, 22);
    table.Insert(30, 33);

    table.Reset();
    EXPECT_FALSE(table.IsEnd());
    EXPECT_TRUE(table.GetCurrKey() == 10 || table.GetCurrKey() == 20 || table.GetCurrKey() == 30);

    table.GoNext();
    if (!table.IsEnd()) {
        EXPECT_TRUE(table.GetCurrKey() == 10 || table.GetCurrKey() == 20 || table.GetCurrKey() == 30);
    }
}

TEST(ArrayHashTable, TheWorstFindIsLinear) {
    ArrayHashTable<int, int> table(10, 1);
    for (int i = 1; i <= 10; ++i) table.Insert(i, i);
    table.ClearEff();
    table.Find(11);
    EXPECT_LE(table.GetEff(), 11);
}

TEST(ArrayHashTable, TheAverageFindIsConst) {
    ArrayHashTable<int, int> table(10, 1);
    for (int i = 1; i <= 10; ++i) table.Insert(i, i);
    table.ClearEff();
    table.Find(7);
    EXPECT_LE(table.GetEff(), 1);
}

TEST(ArrayHashTable, TheWorstInsertIsLinear) {
    ArrayHashTable<int, int> table(4, 1);

    // Создаём худший случай: все позиции заняты, кроме одной
    table.Insert(0, 0);  // hash(0)=0
    table.Insert(1, 1);  // hash(1)=1
    table.Insert(3, 3);  // hash(3)=3
    // Оставляем позицию 2 свободной

    table.ClearEff();
    table.Insert(2, 2);  // hash(2)=2 → вставляем сразу

    // Ожидаем, что Eff будет небольшим (1-2 операции)
    EXPECT_LE(table.GetEff(), 2);
}

TEST(ArrayHashTable, TheAverageDeleteIsConst) {
    ArrayHashTable<int, int> table(10, 1);
    for (int i = 1; i <= 9; ++i) table.Insert(i, i);
    table.ClearEff();
    table.Delete(7);
    EXPECT_LE(table.GetEff(), 2);
}

TEST(ArrayHashTable, ProbeWrapAround) {
    ArrayHashTable<int, std::string> table(5, 2);  // Размер 5, шаг пробирования 2
    table.Insert(1, "one");

    // 6 % 5 = 1 (коллизия), затем +2 = 3 (новая позиция)
    EXPECT_NO_THROW(table.Insert(6, "six"));

    EXPECT_EQ(table.GetDataCount(), 2);
    EXPECT_TRUE(table.Find(1));
    EXPECT_TRUE(table.Find(6));
}
//-------------------------------------------------------------------------------------------------------------------------------------------
TEST(ScanTable, InsertAndFind) {
    ScanTable<int, int> table(5);

    table.Insert(1, 3);
    EXPECT_TRUE(table.Find(1));

    table.Insert(2, 4);
    EXPECT_TRUE(table.Find(2));
}

TEST(ScanTable, InsertDuplicateThrows) {
    ScanTable<int, int> table(3);

    table.Insert(1, 3);
    EXPECT_ANY_THROW(table.Insert(1, 3));  // дубликат
}

TEST(ScanTable, InsertOverflowThrows) {
    ScanTable<int, int> table(2);

    table.Insert(1, 3);
    table.Insert(2, 4);
    EXPECT_ANY_THROW(table.Insert(3, 1));  // превышение размера
}

TEST(ScanTable, DeleteExisting) {
    ScanTable<int, int> table(3);

    table.Insert(1, 3);
    EXPECT_NO_THROW(table.Delete(1));
    EXPECT_FALSE(table.Find(1));
}

TEST(ScanTable, DeleteNonExistingThrows) {
    ScanTable<int, int> table(2);
    EXPECT_ANY_THROW(table.Delete(42));
}

TEST(ScanTable, ClearResetsCount) {
    ScanTable<int, int> table(3);
    table.Insert(1, 2);
    table.Clear();
    EXPECT_FALSE(table.Find(1));
    EXPECT_TRUE(table.IsEnd());
}

TEST(ScanTable, IterationWorks) {
    ScanTable<int, int> table(3);
    table.Insert(1, 4);
    table.Insert(2, 2);
    table.Reset();

    int count = 0;
    while (!table.IsEnd()) {
        Record<int, int> rec = table.GetCurr();
        EXPECT_TRUE(rec.key == 1 || rec.key == 2);
        table.GoNext();
        count++;
    }
    EXPECT_EQ(count, 2);
}

TEST(ScanTable, GetCurrThrowsAtEnd) {
    ScanTable<int, int> table(1);
    table.Insert(1, 4);
    table.Reset();
    table.GoNext(); // за предел
    EXPECT_ANY_THROW(table.GetCurr());
}

TEST(ScanTable, IsFullCheck) {
    ScanTable<int, int> table(2);
    EXPECT_FALSE(table.IsFull());
    table.Insert(1, 3);
    table.Insert(2, 4);
    EXPECT_TRUE(table.IsFull());
}

TEST(ScanTable, AssignCopiesData) {
    ScanTable<int, int> table1(5);
    for (int i = 0; i < 5; i++) {
        table1.Insert(i, i);
    }

    ScanTable<int, int> table2 = table1;
    table2.Delete(4);

    EXPECT_TRUE(table1.Find(4));   // должно остаться в первой
    EXPECT_FALSE(table2.Find(4));  // должно удалиться из второй
}

TEST(ScanTable, FindReturnsCorrectValue) {
    ScanTable<int, int> table(5);
    table.Insert(1, 100);
    EXPECT_TRUE(table.Find(1));
    Record<int, int> rec = table.GetCurr();
    EXPECT_EQ(rec.val, 100);
}

TEST(ScanTable, IterateGivesCorrectValues) {
    ScanTable<int, int> table(5);
    int values[] = { 40, 2, 1, -1, 0 };

    for (int i = 0; i < 5; ++i) {
        table.Insert(values[i], values[i]);
    }

    table.Reset();
    int i = 0;
    while (!table.IsEnd()) {
        EXPECT_EQ(table.GetCurr().val, values[i]);
        table.GoNext();
        i++;
    }
    EXPECT_EQ(i, 5);
}

TEST(ScanTable, DeleteRemovesCorrectKey) {
    ScanTable<int, int> table(5);
    int before[] = { 40, 2, 1, -1, 0 };
    int after[] = { 40, 2, 1, 0 };

    for (int i = 0; i < 5; ++i)
        table.Insert(before[i], before[i]);

    table.Delete(-1);

    table.Reset();
    int i = 0;
    while (!table.IsEnd()) {
        EXPECT_EQ(table.GetCurr().val, after[i]);
        table.GoNext();
        i++;
    }
    EXPECT_EQ(i, 4);
}

TEST(ScanTable, ClearMakesTableEmpty) {
    ScanTable<int, int> table(5);
    for (int i = 0; i < 5; ++i) {
        table.Insert(i, i);
    }

    table.Clear();
    EXPECT_TRUE(table.IsEmpty());
}

TEST(ScanTable, TheWorstFindIsLinear) {
    ScanTable<int, int> table(10);
    table.Insert(1, 1);
    table.Insert(5, 5);
    table.Insert(9, 9);

    table.ClearEff();
    table.Find(7);
    EXPECT_LE(table.GetEff(), 3);
}

TEST(ScanTable, TheAverageFindIsConst) {
    ScanTable<int, int> table(10);

    table.Insert(1, 1);
    table.Insert(5, 5);
    table.Insert(9, 9);
    table.Insert(7, 7);
    table.Insert(0, 0);

    table.ClearEff();
    table.Find(9);
    EXPECT_LE(table.GetEff(), 3);
}

TEST(ScanTable, TheBestFindIsConst) {
    ScanTable<int, int> table(10);

    table.Insert(5, 5);
    table.Insert(7, 7);
    table.Insert(9, 9);

    table.ClearEff();
    table.Find(5);
    EXPECT_LE(table.GetEff(), 1);
}

TEST(ScanTable, TheBestInsertIsConst) {
    ScanTable<int, int> table(10);

    table.ClearEff();
    table.Insert(0, 0);
    EXPECT_LE(table.GetEff(), 1);
}

TEST(ScanTable, TheAverageInsertIsLinear) {
    ScanTable<int, int> table(10);

    table.Insert(1, 1);
    table.Insert(5, 5);
    table.Insert(9, 9);
    table.Insert(7, 7);

    table.ClearEff();
    table.Insert(0, 0);
    EXPECT_LE(table.GetEff(), 5);
}

TEST(ScanTable, TheWorstInsertIsLinear) {
    ScanTable<int, int> table(10);

    table.Insert(1, 1);
    table.Insert(5, 5);
    table.Insert(7, 7);

    table.ClearEff();
    table.Insert(8, 8);
    EXPECT_LE(table.GetEff(), 4);
}

TEST(ScanTable, TheBestDeleteIsConst2) {
    ScanTable<int, int> table(10);
    table.Insert(1, 1);
    table.Insert(5, 5);
    table.Insert(9, 9);
    table.Insert(7, 7);

    table.ClearEff();
    table.Delete(1);
    EXPECT_LE(table.GetEff(), 2);
}

TEST(ScanTable, TheAverageDeleteIsLinear) {
    ScanTable<int, int> table(10);

    table.Insert(1, 1);
    table.Insert(5, 5);
    table.Insert(9, 9);
    table.Insert(7, 7);
    table.Insert(0, 0);

    table.ClearEff();
    table.Delete(9);
    EXPECT_LE(table.GetEff(), 4);
}

TEST(ScanTable, TheWorstDeleteIsLinear) {
    ScanTable<int, int> table(10);
    table.Insert(1, 1);
    table.Insert(5, 5);
    table.Insert(9, 9);
    table.Insert(7, 7);

    table.ClearEff();
    table.Delete(7);
    EXPECT_LE(table.GetEff(), 5);
}
//-------------------------------------------------------------------------------------------------------------------------------------------
TEST(SortTable, InsertIntoEmpty) {
    SortTable<int, int> table(5);
    EXPECT_NO_THROW(table.Insert(2, 2));
    EXPECT_EQ(table.GetDataCount(), 1);
}

TEST(SortTable, InsertMaintainsOrder) {
    SortTable<int, int> table(5);

    table.Insert(2, 2);
    table.Insert(1, 1);
    table.Insert(3, 3);

    int expectedKeys[] = { 1, 2, 3 };
    table.Reset();
    for (int i = 0; i < 3; i++) {
        EXPECT_EQ(table.GetCurrVal(), expectedKeys[i]);
        table.GoNext();
    }
}

TEST(SortTable, DeleteExistingShiftsElements) {
    SortTable<int, int> table(5);

    table.Insert(1, 1);
    table.Insert(2, 2);
    table.Delete(1);

    table.Reset();
    EXPECT_EQ(table.GetCurrVal(), 2);
    EXPECT_EQ(table.GetDataCount(), 1);
}

TEST(SortTable, AssignOperatorWorks) {
    SortTable<int, int> table1(5);
    for (int i = 0; i < 5; ++i) {
        table1.Insert(i, i);
    }
    SortTable<int, int> table2(5);
    table2 = table1;
    table2.Delete(4);

    EXPECT_TRUE(table1.Find(4));
    EXPECT_FALSE(table2.Find(4));
}

TEST(SortTable, CanIterate) {
    SortTable<int, int> table(5);
    int keys[] = { 40, 2, 1, -1, 0 };
    int sortedKeys[] = { -1, 0, 1, 2, 40 };

    for (int key : keys) {
        table.Insert(key, key);
    }

    table.Reset();
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(table.GetCurrVal(), sortedKeys[i]);
        table.GoNext();
    }
}

TEST(SortTable, CanClearTable) {
    SortTable<int, int> table(5);
    for (int i = 0; i < 5; ++i) {
        table.Insert(i, i);
    }
    table.Clear();
    EXPECT_EQ(table.GetDataCount(), 0);
    EXPECT_TRUE(table.IsEmpty());
}

TEST(SortTable, InsertAndFind) {
    SortTable<int, int> table(5);

    table.Insert(1, 1);
    table.Insert(3, 3);
    table.Insert(2, 2);

    EXPECT_TRUE(table.Find(1));
    EXPECT_TRUE(table.Find(2));
    EXPECT_TRUE(table.Find(3));
    EXPECT_FALSE(table.Find(5));
}

TEST(SortTable, InsertDuplicateThrows) {
    SortTable<int, int> table(3);

    table.Insert(1, 1);
    EXPECT_ANY_THROW(table.Insert(1, 1));  // дубликат
}

TEST(SortTable, InsertOverflowThrows) {
    SortTable<int, int> table(2);

    table.Insert(1, 1);
    table.Insert(2, 2);
    EXPECT_ANY_THROW(table.Insert(3, 3));
}

TEST(SortTable, DeleteExisting) {
    SortTable<int, int> table(3);

    table.Insert(1, 1);
    table.Insert(2, 2);
    EXPECT_NO_THROW(table.Delete(1));
    EXPECT_FALSE(table.Find(1));
}

TEST(SortTable, DeleteNonExistingThrows) {
    SortTable<int, int> table(2);
    table.Insert(1, 1);
    EXPECT_ANY_THROW(table.Delete(42));
}

TEST(SortTable, THeWorstFindIsLinear2) {
    SortTable<int, int> table(10);

    table.Insert(1, 1);
    table.Insert(2, 2);
    table.Insert(3, 3);
    table.Insert(4, 4);
    table.Insert(5, 5);
    table.Insert(6, 6);
    table.Insert(7, 7);
    table.Insert(8, 8);
    table.Insert(9, 9);
    table.Insert(10, 10);

	table.ClearEff();
	table.Find(11);
    EXPECT_LE(table.GetEff(), 11);
}

TEST(SortTable, THeAverageFindIsLinear2) {
    SortTable<int, int> table(10);

    table.Insert(1, 1);
    table.Insert(2, 2);
    table.Insert(3, 3);
    table.Insert(4, 4);
    table.Insert(5, 5);
    table.Insert(6, 6);
    table.Insert(7, 7);
    table.Insert(8, 8);
    table.Insert(9, 9);
    table.Insert(10, 10);

    table.ClearEff();
    table.Find(7);
    EXPECT_LE(table.GetEff(), 5);
}

TEST(SortTable, THeAverageInsertIsLinear2) {
    SortTable<int, int> table(10);

    table.Insert(1, 1);
    table.Insert(2, 2);
    table.Insert(3, 3);
    table.Insert(4, 4);
    table.Insert(5, 5);
    table.Insert(6, 6);
    table.Insert(7, 7);
    table.Insert(9, 9);
    table.Insert(10, 10);

    table.ClearEff();
    table.Insert(8, 8);
    EXPECT_LE(table.GetEff(), 10);
}

TEST(SortTable, THeWorstInsertIsLinear2) {
    SortTable<int, int> table(10);

    table.Insert(1, 1);
    table.Insert(2, 2);
    table.Insert(3, 3);
    table.Insert(4, 4);
    table.Insert(5, 5);
    table.Insert(6, 6);
    table.Insert(7, 7);
    table.Insert(8, 8);
    table.Insert(9, 9);
    

    table.ClearEff();
    table.Insert(10, 10);
    EXPECT_LE(table.GetEff(), 10);
}
//-------------------------------------------------------------------------------------------------------------------------------------------
TEST(TreeTable, CanInsertElementToEmptyTable) {
    TreeTable<int, std::string> table;
    ASSERT_NO_THROW(table.Insert(1, "one"));
}

TEST(TreeTable, ThrowsWhenInsertingDuplicateKey) {
    TreeTable<int, std::string> table;
    table.Insert(1, "one");
    ASSERT_ANY_THROW(table.Insert(1, "duplicate"));
}

TEST(TreeTable, CanDeleteElementFromTable) {
    TreeTable<int, std::string> table;
    table.Insert(1, "one");
    table.Insert(2, "two");
    table.Insert(3, "three");
    ASSERT_NO_THROW(table.Delete(2));
    EXPECT_EQ(table.Find(2), false);
}

TEST(TreeTable, FindReturnsTrueForExistingKey) {
    TreeTable<int, std::string> table;
    table.Insert(5, "five");
    table.Insert(8, "eight");
    table.Insert(15, "fifteen");
    EXPECT_TRUE(table.Find(8));
}

TEST(TreeTable, FindReturnsFalseForNonExistingKey) {
    TreeTable<int, std::string> table;
    table.Insert(5, "five");
    table.Insert(8, "eight");
    table.Insert(15, "fifteen");
    EXPECT_FALSE(table.Find(9));
}

TEST(TreeTable, CanDeleteRootNode) {
    TreeTable<int, std::string> table;
    table.Insert(5, "five");
    EXPECT_NO_THROW(table.Delete(5));
    EXPECT_EQ(table.IsEmpty(), true);
}

TEST(TreeTable, CanDeleteRootWithLeftChild) {
    TreeTable<int, std::string> table;
    table.Insert(5, "five");
    table.Insert(3, "three");
    EXPECT_NO_THROW(table.Delete(5));
    EXPECT_EQ(table.GetDataCount(), 1);
}

TEST(TreeTable, CanDeleteRootWithRightChild) {
    TreeTable<int, std::string> table;
    table.Insert(5, "five");
    table.Insert(7, "seven");
    EXPECT_NO_THROW(table.Delete(5));
    EXPECT_EQ(table.GetDataCount(), 1);
}

TEST(TreeTable, CanDeleteRootWithBothChildren) {
    TreeTable<int, std::string> table;
    table.Insert(5, "five");
    table.Insert(3, "three");
    table.Insert(7, "seven");
    EXPECT_NO_THROW(table.Delete(5));
    EXPECT_EQ(table.GetDataCount(), 2);
}

TEST(TreeTable, CanDeleteNodeWithBothChildren) {
    TreeTable<int, std::string> table;
    table.Insert(4, "four");
    table.Insert(2, "two");
    table.Insert(1, "one");
    table.Insert(3, "three");
    table.Insert(6, "six");
    table.Insert(5, "five");
    table.Insert(7, "seven");

    table.ClearEff();
    EXPECT_NO_THROW(table.Delete(4));
    EXPECT_EQ(table.GetDataCount(), 6);
    EXPECT_LE(table.GetEff(), 4);
}

TEST(TreeTable, CanDeleteNodeWithLeftChildOnly) {
    TreeTable<int, std::string> table;
    table.Insert(5, "five");
    table.Insert(4, "four");
    table.Insert(3, "three");
    table.Insert(2, "two");

    table.ClearEff();
    EXPECT_NO_THROW(table.Delete(4));
    EXPECT_EQ(table.GetDataCount(), 3);
    EXPECT_LE(table.GetEff(), 3);
}

TEST(TreeTable, CanDeleteNodeWithRightChildOnly) {
    TreeTable<int, std::string> table;
    table.Insert(1, "one");
    table.Insert(2, "two");
    table.Insert(3, "three");
    table.Insert(4, "four");

    table.ClearEff();
    EXPECT_NO_THROW(table.Delete(2));
    EXPECT_EQ(table.GetDataCount(), 3);
    EXPECT_LE(table.GetEff(), 3);
}

TEST(TreeTable, IterationInCorrectOrder) {
    TreeTable<int, std::string> table;
    table.Insert(4, "four");
    table.Insert(2, "two");
    table.Insert(1, "one");
    table.Insert(3, "three");
    table.Insert(6, "six");
    table.Insert(5, "five");
    table.Insert(7, "seven");

    table.Reset();
    EXPECT_EQ(table.GetCurrKey(), 1);
    table.GoNext();
    EXPECT_EQ(table.GetCurrKey(), 2);
    table.GoNext();
    EXPECT_EQ(table.GetCurrKey(), 3);
    table.GoNext();
    EXPECT_EQ(table.GetCurrKey(), 4);
    table.GoNext();
    EXPECT_EQ(table.GetCurrKey(), 5);
    table.GoNext();
    EXPECT_EQ(table.GetCurrKey(), 6);
    table.GoNext();
    EXPECT_EQ(table.GetCurrKey(), 7);
    table.GoNext();
    EXPECT_TRUE(table.IsEnd());
}

TEST(TreeTable, DataCountAfterInsert) {
    TreeTable<int, std::string> table;
    table.Insert(5, "five");
    EXPECT_EQ(table.GetDataCount(), 1);
}

TEST(TreeTable, DeleteNonExistentKeyThrows) {
    TreeTable<int, std::string> table;
    table.Insert(5, "five");
    EXPECT_ANY_THROW(table.Delete(42));
}

TEST(TreeTable, IteratorOnEmptyTree) {
    TreeTable<int, std::string> table;
    table.Reset();
    EXPECT_TRUE(table.IsEnd());
}

TEST(TreeTable, SequentialDeletionLeavesTreeEmpty) {
    TreeTable<int, std::string> table;
    table.Insert(5, "five");
    table.Insert(3, "three");
    table.Insert(7, "seven");
    table.Insert(1, "one");
    table.Insert(4, "four");
    table.Insert(6, "six");

    table.Delete(5);
    table.Delete(1);
    table.Delete(7);
    table.Delete(3);
    table.Delete(6);
    table.Delete(4);

    EXPECT_TRUE(table.IsEmpty());
}

TEST(TreeTable, WorstCaseFindIsLinear) {
    TreeTable<int, std::string> table;
    for (int i = 0; i < 100; i++) {
        table.Insert(i, std::to_string(i));
    }

    table.ClearEff();
    table.Find(99);
    EXPECT_LE(table.GetEff(), 100);
}

TEST(TreeTable, AverageCaseFindIsLogN) {
    TreeTable<int, std::string> table;
    table.Insert(8, "eight");
    table.Insert(4, "four");
    table.Insert(12, "twelve");
    table.Insert(2, "two");
    table.Insert(6, "six");
    table.Insert(10, "ten");
    table.Insert(14, "fourteen");
    table.Insert(1, "one");
    table.Insert(3, "three");
    table.Insert(5, "five");
    table.Insert(7, "seven");
    table.Insert(9, "nine");
    table.Insert(11, "eleven");
    table.Insert(13, "thirteen");
    table.Insert(15, "fifteen");

    table.ClearEff();
    table.Find(7);
    EXPECT_LE(table.GetEff(), 4);
}

TEST(TreeTable, WorstCaseInsertIsLinear22) {
    TreeTable<int, std::string> table;
    table.Insert(0, "zero");
    table.Insert(1, "one");
    table.Insert(2, "two");
    table.Insert(3, "three");

    table.ClearEff();
    table.Insert(4, "four");
    EXPECT_LE(table.GetEff(), 5);
}

TEST(TreeTable, AverageCaseInsertIsLogN) {
    TreeTable<int, std::string> table;
    table.Insert(8, "eight");
    table.Insert(4, "four");
    table.Insert(12, "twelve");
    table.Insert(2, "two");
    table.Insert(6, "six");
    table.Insert(10, "ten");
    table.Insert(14, "fourteen");
    table.Insert(1, "one");
    table.Insert(3, "three");
    table.Insert(5, "five");
    table.Insert(7, "seven");

    table.ClearEff();
    table.Insert(13, "therteen");
    EXPECT_LE(table.GetEff(), 6);
}

TEST(TreeTable, AverageCaseDeleteIsLogN) {
    TreeTable<int, std::string> table;
    table.Insert(8, "eight");
    table.Insert(4, "four");
    table.Insert(12, "twelve");
    table.Insert(2, "two");
    table.Insert(6, "six");
    table.Insert(10, "ten");
    table.Insert(14, "fourteen");
    table.Insert(1, "one");
    table.Insert(3, "three");
    table.Insert(5, "five");
    table.Insert(7, "seven");
    

    table.ClearEff();
    table.Delete(6);
    EXPECT_LE(table.GetEff(), 5);
}
//-------------------------------------------------------------------------------------------------------------------------------------------
TEST(ListHashTable, ThrowsWhenInsertingExistingKey) {
    ListHashTable<int, std::string> table;
    table.Insert(10, "ten");
    ASSERT_ANY_THROW(table.Insert(10, "another ten"));
}

TEST(ListHashTable, ThrowsWhenDeletingNonPresentKey) {
    ListHashTable<int, std::string> table;
    ASSERT_ANY_THROW(table.Delete(42));
}

TEST(ListHashTable, CorrectlyInsertsAndFindsElements) {
    ListHashTable<int, std::string> table;
    int keys[] = { 5, 10, 15 };
    std::string values[] = { "five", "ten", "fifteen" };

    for (int i = 0; i < 3; i++) {
        table.Insert(keys[i], values[i]);
    }

    for (int i = 0; i < 3; i++) {
        EXPECT_TRUE(table.Find(keys[i]));
    }
}

TEST(ListHashTable, CorrectlyDeletesElements) {
    ListHashTable<int, std::string> table;
    table.Insert(1, "one");
    table.Insert(2, "two");
    table.Delete(1);

    EXPECT_FALSE(table.Find(1));
    EXPECT_TRUE(table.Find(2));
}

TEST(ListHashTable, CorrectAssignmentOperator) {
    ListHashTable<int, std::string> table1;
    ListHashTable<int, std::string> table2;

    table1.Insert(0, "zero");
    table2 = table1;
    table2.Delete(0);

    EXPECT_TRUE(table1.Find(0));
    EXPECT_FALSE(table2.Find(0));
}

TEST(ListHashTable, CannotFindNonExistingElement) {
    ListHashTable<int, std::string> table;
    table.Insert(123, "one two three");
    EXPECT_FALSE(table.Find(321));
}

TEST(ListHashTable, CanClearTableCompletely) {
    ListHashTable<int, std::string> table;
    table.Insert(1, "one");
    table.Insert(2, "two");
    table.Clear();

    EXPECT_EQ(table.GetDataCount(), 0);
    EXPECT_TRUE(table.IsEmpty());
}

TEST(ListHashTable, HandlesCollision) {
    ListHashTable<int, std::string> table(3);  // маленький размер таблицы для коллизий

    table.Insert(1, "one");
    EXPECT_NO_THROW(table.Insert(4, "four")); // 1 % 3 == 4 % 3

    EXPECT_EQ(table.GetDataCount(), 2);
    EXPECT_TRUE(table.Find(1));
    EXPECT_TRUE(table.Find(4));

    table.Reset();
    bool found1 = false, found4 = false;
    while (!table.IsEnd()) {
        if (table.GetCurrKey() == 1) {
            found1 = true;
            EXPECT_EQ(table.GetCurrVal(), "one");
        }
        if (table.GetCurrKey() == 4) {
            found4 = true;
            EXPECT_EQ(table.GetCurrVal(), "four");
        }
        table.GoNext();
    }
    EXPECT_TRUE(found1 && found4);
}

TEST(ListHashTable, CannotFindDeletedRecord) {
    ListHashTable<int, std::string> table;
    table.Insert(5, "five");
    table.Insert(6, "six");
    table.Insert(7, "seven");
    table.Delete(7);
    EXPECT_FALSE(table.Find(7));
}

TEST(ListHashTable, CanDeleteElementFromTable) {
    ListHashTable<int, std::string> table1;
    ListHashTable<int, std::string> table2;

    table1.Insert(5, "five");
    table1.Insert(6, "six");
    table1.Insert(7, "seven");

    table2.Insert(5, "five");
    table2.Insert(7, "seven");

    EXPECT_NO_THROW(table1.Delete(6));

    table1.Reset();
    table2.Reset();
    while (!table1.IsEnd() && !table2.IsEnd()) {
        EXPECT_EQ(table1.GetCurrKey(), table2.GetCurrKey());
        table1.GoNext();
        table2.GoNext();
    }
    EXPECT_TRUE(table1.IsEnd() && table2.IsEnd());
}

TEST(ListHashTable, WorstCaseFindIsConstant) {
    ListHashTable<int, std::string> st;
    for (int i : {2, 3, 5, 7, 9, 1, 6, 4, 10})
        st.Insert({ i, "v" + std::to_string(i) });
    st.ClearEff();
    st.Find(11);
    EXPECT_LE(st.GetEff(), 1);
}

TEST(ListHashTable, AverageCaseFindIsConstant) {
    ListHashTable<int, std::string> table(10);
    table.Insert(2, "two");
    table.Insert(3, "three");
    table.Insert(5, "five");
    table.Insert(7, "seven");
    table.Insert(9, "nine");
    table.Insert(1, "one");
    table.Insert(6, "six");
    table.Insert(4, "four");
    table.Insert(10, "ten");

    table.ClearEff();
    table.Find(7);
    EXPECT_LE(table.GetEff(), 2);
}

TEST(ListHashTable, AverageCaseInsertIsConstant) {
    ListHashTable<int, std::string> table(10);
    table.Insert(2, "two");
    table.Insert(3, "three");
    table.Insert(5, "five");
    table.Insert(7, "seven");
    table.Insert(9, "nine");
    table.Insert(1, "one");
    table.Insert(6, "six");
    table.Insert(4, "four");
    table.Insert(10, "ten");

    table.ClearEff();
    table.Insert(8, "eight");
    EXPECT_LE(table.GetEff(), 3);
}

TEST(ListHashTable, WorstCaseInsertIsConstant) {
    ListHashTable<int, std::string> table(5);
    table.Insert(0, "zero");
    table.Insert(3, "three");
    table.Insert(1, "one");
    table.Insert(4, "four");

    table.ClearEff();
    table.Insert(2, "two");
    EXPECT_LE(table.GetEff(), 4);
}

TEST(ListHashTable, AverageCaseDeleteIsConstant) {
    ListHashTable<int, std::string> table(10);
    table.Insert(2, "two");
    table.Insert(3, "three");
    table.Insert(5, "five");
    table.Insert(8, "eight");
    table.Insert(9, "nine");
    table.Insert(7, "seven");
    table.Insert(1, "one");
    table.Insert(6, "six");
    table.Insert(4, "four");

    table.ClearEff();
    table.Delete(7);
    EXPECT_LE(table.GetEff(), 3);
}

TEST(ListHashTable, LargeTableKeepsFindEfficient) {
    ListHashTable<int, std::string> table(100);

    for (int i = 0; i < 100; ++i) {
        table.Insert(i, std::to_string(i));
    }

    table.ClearEff();
    table.Find(50);

    EXPECT_LE(table.GetEff(), 3);
}
//-------------------------------------------------------------------------------------------------------------------------------------------
TEST(BalTreeTable, CanInsertElementToTable33) {
    BalTreeTable<int, std::string> table;
    ASSERT_NO_THROW(table.Insert(5, "five"));
}

TEST(BalTreeTable, ThrowsWhenInsertingDuplicateKey33) {
    BalTreeTable<int, std::string> table;
    table.Insert(1, "one");
    ASSERT_ANY_THROW(table.Insert(1, "duplicate"));
}

TEST(BalTreeTable, CanDeleteElementFromTable33) {
    BalTreeTable<int, std::string> table;
    table.Insert(5, "five");
    table.Insert(6, "six");
    table.Insert(7, "seven");

    EXPECT_TRUE(table.Find(6));
    ASSERT_NO_THROW(table.Delete(6));
    EXPECT_FALSE(table.Find(6));
}

TEST(BalTreeTable, FindReturnsTrueForExistingKey33) {
    BalTreeTable<int, std::string> table;
    table.Insert(5, "five");
    table.Insert(8, "eight");
    table.Insert(15, "fifteen");
    EXPECT_TRUE(table.Find(8));
}

TEST(BalTreeTable, FindReturnsFalseForNonExistingKey33) {
    BalTreeTable<int, std::string> table;
    table.Insert(5, "five");
    table.Insert(8, "eight");
    table.Insert(15, "fifteen");
    EXPECT_FALSE(table.Find(9));
}

TEST(BalTreeTable, CanDeleteRootNode33) {
    BalTreeTable<int, std::string> table;
    table.Insert(5, "five");
    EXPECT_NO_THROW(table.Delete(5));
    EXPECT_EQ(table.IsEmpty(), true);
}

TEST(BalTreeTable, CanDeleteRootWithLeftChild33) {
    BalTreeTable<int, std::string> table;
    table.Insert(5, "five");
    table.Insert(3, "three");
    EXPECT_NO_THROW(table.Delete(5));
    EXPECT_EQ(table.GetDataCount(), 1);
}

TEST(BalTreeTable, CanDeleteRootWithRightChild33) {
    BalTreeTable<int, std::string> table;
    table.Insert(5, "five");
    table.Insert(7, "seven");
    EXPECT_NO_THROW(table.Delete(5));
    EXPECT_EQ(table.GetDataCount(), 1);
}

TEST(BalTreeTable, CanDeleteRootWithBothChildren33) {
    BalTreeTable<int, std::string> table;
    table.Insert(5, "five");
    table.Insert(3, "three");
    table.Insert(7, "seven");
    EXPECT_NO_THROW(table.Delete(5));
    EXPECT_EQ(table.GetDataCount(), 2);
}

TEST(BalTreeTable, CanDeleteNodeWithBothChildren33) {
    BalTreeTable<int, std::string> table;
    table.Insert(4, "four");
    table.Insert(2, "two");
    table.Insert(1, "one");
    table.Insert(3, "three");
    table.Insert(6, "six");
    table.Insert(5, "five");
    table.Insert(7, "seven");

    table.ClearEff();
    EXPECT_NO_THROW(table.Delete(4));
    EXPECT_EQ(table.GetDataCount(), 6);
    EXPECT_LE(table.GetEff(), 4);
}

TEST(BalTreeTable, CanDeleteNodeWithLeftChildOnly33) {
    BalTreeTable<int, std::string> table;
    table.Insert(4, "four");
    table.Insert(3, "three");
    table.Insert(5, "five");
    table.Insert(2, "two");
    table.Insert(1, "one");

    table.ClearEff();
    EXPECT_NO_THROW(table.Delete(5));
    EXPECT_EQ(table.GetDataCount(), 4);
    EXPECT_LE(table.GetEff(), 3);
}

TEST(BalTreeTable, CanDeleteNodeWithRightChildOnly33) {
    BalTreeTable<int, std::string> table;
    table.Insert(2, "two");
    table.Insert(3, "three");
    table.Insert(1, "one");
    table.Insert(4, "four");
    table.Insert(6, "six");
    table.Insert(5, "five");
    table.Insert(7, "seven");
    table.Insert(8, "eight");

    table.ClearEff();
    EXPECT_NO_THROW(table.Delete(7));
    EXPECT_EQ(table.GetDataCount(), 7);
    EXPECT_LE(table.GetEff(), 5);
}

TEST(BalTreeTable, IterationInCorrectOrder33) {
    BalTreeTable<int, std::string> table;
    table.Insert(1, "one");
    table.Insert(4, "four");
    table.Insert(5, "five");
    table.Insert(3, "three");
    table.Insert(2, "two");
    table.Insert(6, "six");

    table.Reset();
    EXPECT_EQ(table.GetCurrKey(), 1);
    table.GoNext();
    EXPECT_EQ(table.GetCurrKey(), 2);
    table.GoNext();
    EXPECT_EQ(table.GetCurrKey(), 3);
    table.GoNext();
    EXPECT_EQ(table.GetCurrKey(), 4);
    table.GoNext();
    EXPECT_EQ(table.GetCurrKey(), 5);
    table.GoNext();
    EXPECT_EQ(table.GetCurrKey(), 6);
    table.GoNext();
    EXPECT_TRUE(table.IsEnd());
}

TEST(BalTreeTable, WorstCaseFindIsLogN33) {
    BalTreeTable<int, std::string> table;
    for (int i = 0; i < 100; i++) {
        table.Insert(i, std::to_string(i));
    }

    table.ClearEff();
    table.Find(141); // Поиск несуществующего элемента
    EXPECT_LE(table.GetEff(), 7); // log2(100) ≈ 6.64 + запас
}

TEST(BalTreeTable, AverageCaseFindIsLogN33) {
    BalTreeTable<int, std::string> table;
    for (int i = 0; i < 100; i++) {
        table.Insert(i, std::to_string(i));
    }

    table.ClearEff();
    table.Find(64); // Поиск существующего элемента
    EXPECT_LE(table.GetEff(), 7);
}

TEST(BalTreeTable, AverageCaseInsertIsLogN33) {
    BalTreeTable<int, std::string> table;
    for (int i = 0; i < 100; i++) {
        table.Insert(i, std::to_string(i));
    }
    table.Delete(6);

    table.ClearEff();
    table.Insert(6, "six");
    EXPECT_LE(table.GetEff(), 15); // Поиск + вставка
}

TEST(BalTreeTable, WorstCaseInsertIsLogN333) {
    BalTreeTable<int, std::string> table;
    for (int i = 0; i < 100; i++) {
        table.Insert(i, std::to_string(i));
    }
    table.Delete(64);

    table.ClearEff();
    table.Insert(64, "sixty-four");
    EXPECT_LE(table.GetEff(), 12);
}

TEST(BalTreeTable, AverageCaseDeleteIsLogN3333) {
    BalTreeTable<int, std::string> table;
    for (int i = 0; i < 100; i++) {
        table.Insert(i, std::to_string(i));
    }

    table.ClearEff();
    table.Delete(64);
    EXPECT_LE(table.GetEff(), 7);
}

TEST(BalTreeTable, CorrectAssign3333) {
    BalTreeTable<int, std::string> t1, t2;
    for (int i = 0; i < 5; ++i)
        t1.Insert(i, std::to_string(i));

    t2 = t1;
    t2.Delete(4);

    EXPECT_TRUE(t1.Find(4));
    EXPECT_FALSE(t2.Find(4));
}

TEST(BalTreeTable, ThrowsWhenDeletingNonExistentKey33) {
    BalTreeTable<int, std::string> table;
    ASSERT_ANY_THROW(table.Delete(42));
}

TEST(BalTreeTable, CanClearTable33) {
    BalTreeTable<int, std::string> table;
    for (int i = 0; i < 5; ++i)
        table.Insert(i, std::to_string(i));

    table.Clear();
    EXPECT_EQ(table.GetDataCount(), 0);
    EXPECT_TRUE(table.IsEmpty());
}

TEST(BalTreeTable, CanIterateThroughAllElements33) {
    BalTreeTable<int, std::string> table;
    int keys[5] = { 40, 2, 1, -1, 0 };
    for (int i = 0; i < 5; ++i)
        table.Insert(keys[i], std::to_string(keys[i]));

    int count = 0;
    for (table.Reset(); !table.IsEnd(); table.GoNext())
        ++count;

    EXPECT_EQ(count, 5);
}

TEST(BalTreeTable, EfficiencyUnderControl33) {
    BalTreeTable<int, std::string> table;
    for (int i = 0; i < 1000; ++i) {
        table.Insert(i, std::to_string(i));
    }

    int eff = table.GetEff();
    EXPECT_LE(eff, 50); // разумный запас для log2(1000) ≈ 10
}
