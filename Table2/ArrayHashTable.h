#pragma once  
#include "HashTable.h"  
#include "Record.h"
#include <functional> 
enum Status { Free, Deleted, Used };

template <typename TKey, typename TVal>
class ArrayHashTable : public HashTable<TKey, TVal>
{
protected:
    Record<TKey, TVal>* pRec;
    int step, curr;
    Status* status;
    int size;

public:
    ArrayHashTable(int _size = 10, int _step = 1) : size(_size), step(_step), curr(0) {
        if (_size <= 0 || _step < 1)
            throw "Error size or step!";

        // Автоматический подбор шага, если передали 1
        if (step == 1) {
            for (int i = 2; i * i <= size; i++) {
                if (size % i != 0) {
                    step = i;
                    break;
                }
            }
        }
        //для размера 10: проверяет 2 (10 делится на 2), затем 3 (10 не делится на 3) - устанавливает шаг 3
        pRec = new Record<TKey, TVal>[size];
        status = new Status[size];

        for (int i = 0; i < size; i++) {
            status[i] = Free;
        }
    }

    ~ArrayHashTable() {
        delete[] pRec;
        delete[] status;
    }

    ArrayHashTable(const ArrayHashTable& other) :
        size(other.size), step(other.step), curr(other.curr)
    {
        pRec = new Record<TKey, TVal>[size];
        status = new Status[size];

        for (int i = 0; i < size; i++) {
            pRec[i] = other.pRec[i];
            status[i] = other.status[i];
        }
        this->DataCount = other.DataCount;
        this->Eff = other.Eff;
    }

    ArrayHashTable& operator=(const ArrayHashTable& other) {
        if (this != &other) {
            delete[] pRec;
            delete[] status;

            size = other.size;
            step = other.step;
            curr = other.curr;

            pRec = new Record<TKey, TVal>[size];
            status = new Status[size];

            for (int i = 0; i < size; i++) {
                pRec[i] = other.pRec[i];
                status[i] = other.status[i];
            }
            this->DataCount = other.DataCount;
            this->Eff = other.Eff;
        }
        return *this;
    }

    TKey GetCurrKey() { return pRec[curr].key; }
    TVal GetCurrVal() { return pRec[curr].val; }
    Record<TKey, TVal> GetCurr() { return pRec[curr]; }

    bool Find(TKey key) {
        curr = this->HashFunc(key);
        this->Eff = 0;
        int firstDeleted = -1;

        for (int i = 0; i < size; i++) {
            this->Eff++;

            if (status[curr] == Free) {
                if (firstDeleted != -1) curr = firstDeleted; 
                break;
            }
            else if (status[curr] == Deleted && firstDeleted == -1) {
                firstDeleted = curr; 
            }
            else if (status[curr] == Used && pRec[curr].key == key) {
                return true;
            }

            curr = (curr + step) % size;
        }

        return false;
    }

    void Insert(TKey key, TVal val) {
        if (IsFull())
            throw "Table is full!";

        this->Eff = 0;
        if (Find(key))
            throw "Error this rec exists!";

        status[curr] = Used;
        pRec[curr] = Record<TKey, TVal>(key, val);
        this->DataCount++;
        this->Eff++; 
    }

    void Delete(TKey key) {
        if (!Find(key))
            throw "Error this rec was deleted!";

        status[curr] = Deleted;
        this->DataCount--;
        this->Eff++;
    }

    void Reset() {
        curr = 0;
        while (curr < size && (status[curr] == Free || status[curr] == Deleted)) {
            curr++;
        }
    }

    void GoNext() {
        curr++;
        while (curr < size && (status[curr] == Free || status[curr] == Deleted)) {
            curr++;
        }
    }

    bool IsEnd() {
        return curr >= size;
    }

    void Clear() {
        this->DataCount = 0;
        for (int i = 0; i < size; i++) {
            status[i] = Free;
        }
    }

    bool IsFull() const {
        return this->DataCount >= size;
    }

    int HashFunc(TKey key) const {
        std::hash<TKey> hasher; 
        return static_cast<int>(hasher(key) % size);
    }
    void Resize(int newSize) override {
        if (newSize < this->DataCount) {
            throw std::invalid_argument("Новый размер меньше текущего количества данных");
        }

        Record<TKey, TVal>* newRecs = new Record<TKey, TVal>[newSize];
        Status* newStatus = new Status[newSize];

        for (int i = 0; i < size; i++) {
            if (status[i] == Used) {
                int newPos = this->HashFunc(pRec[i].key) % newSize;
                while (newStatus[newPos] != Free) {
                    newPos = (newPos + step) % newSize;
                }
                newRecs[newPos] = pRec[i];
                newStatus[newPos] = Used;
            }
        }

        delete[] pRec;
        delete[] status;

        pRec = newRecs;
        status = newStatus;
        size = newSize;

        for (int i = this->DataCount; i < newSize; i++) {
            status[i] = Free;
        }
    }
    std::string GetTypeName() const override {
        return "ArrayHashTable";
    }
};