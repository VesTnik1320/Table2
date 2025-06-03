#pragma once
#include <list>
#include "HashTable.h"
#include <iterator>
#include "Record.h"

template <typename TKey, typename TVal>
class ListHashTable : public HashTable<TKey, TVal>
{
protected:
    std::list<Record<TKey, TVal>>* pList;
    int CurrList;
    typename std::list<Record<TKey, TVal>>::iterator currI;

public:
    ListHashTable(int _size = 100) : HashTable<TKey, TVal>(_size) {
        pList = new std::list<Record<TKey, TVal>>[_size];
        CurrList = -1;
        this->DataCount = 0;
        this->Eff = 0;
    }

    ~ListHashTable() {
        delete[] pList;
    }

    bool Find(TKey key) {
        CurrList = this->HashFunc(key);
        for (currI = pList[CurrList].begin(); currI != pList[CurrList].end(); ++currI) {
            this->Eff++;
            if (currI->key == key)
                return true;
        }
        return false;
    }

    void Insert(Record<TKey, TVal> rec)   {
        if (IsFull())
            throw "Error, no space for new record!";
        if (Find(rec.key))
            throw "Error, this record already exists!";

        pList[this->HashFunc(rec.key)].push_front(rec); 
        this->DataCount++;
        this->Eff++;
    }

    void Insert(TKey key, TVal val) {
        Record<TKey, TVal> rec(key, val);
        Insert(rec);
    }

    void Delete(TKey key) {
        if (!Find(key))
            throw "Error, key not found!";

        pList[CurrList].erase(currI);
        this->DataCount--;
        this->Eff++;
    }

    void Reset()  {
        CurrList = 0;
        currI = pList[CurrList].begin();
        while (currI == pList[CurrList].end() && CurrList < this->size - 1) {
            CurrList++;
            currI = pList[CurrList].begin();
        }
    }

    bool IsEnd()  {
        return CurrList >= this->size - 1 && currI == pList[CurrList].end();
    }

    void GoNext()  {
        if (IsEnd()) return;

        ++currI;
        while (currI == pList[CurrList].end() && CurrList < this->size - 1) {
            CurrList++;
            currI = pList[CurrList].begin();
        }
    }

    void Clear()  {
        for (int i = 0; i < this->size; i++) {
            pList[i].clear();
        }
        this->DataCount = 0;
        this->Eff = 0;
        CurrList = -1;
    }

    bool IsFull() const {
        return this->DataCount >= this->size;
    }

    Record<TKey, TVal> GetCurr() {
        if (IsEnd())
            throw "Error: iterator at end position";
        return *currI;
    }

    TKey GetCurrKey()   {
        return GetCurr().key;
    }

    TVal GetCurrVal()   {
        return GetCurr().val; 
    }

    int GetEff() {
        return this->Eff;
    }
    void Resize(int newSize) override {
        if (newSize < 1) {
            throw std::invalid_argument("Ќовый размер должен быть положительным");
        }

        std::list<Record<TKey, TVal>>* newLists = new std::list<Record<TKey, TVal>>[newSize];

        for (int i = 0; i < this->size; i++) {
            for (const auto& rec : pList[i]) {
                int newHash = this->HashFunc(rec.key) % newSize;
                newLists[newHash].push_back(rec);
            }
        }

        delete[] pList;
        pList = newLists;
        this->size = newSize;
    }

    std::string GetTypeName() const override {
        return "ListHashTable";
    }

};