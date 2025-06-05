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

    ListHashTable(const ListHashTable& other) : HashTable<TKey, TVal>(other.size) {
        pList = new std::list<Record<TKey, TVal>>[this->size];
        for (int i = 0; i < this->size; i++) {
            pList[i] = other.pList[i];
        }
        this->DataCount = other.DataCount;
        this->Eff = other.Eff;
        CurrList = other.CurrList;
        Reset();
    }

    ListHashTable& operator=(const ListHashTable& other) {
        if (this != &other) {
            delete[] pList;

            this->size = other.size;
            pList = new std::list<Record<TKey, TVal>>[this->size];
            for (int i = 0; i < this->size; i++) {
                pList[i] = other.pList[i];
            }
            this->DataCount = other.DataCount;
            this->Eff = other.Eff;
            CurrList = other.CurrList;
            Reset();
        }
        return *this;
    }

    int GetSize() const {
        return this->size;
    }

    const std::list<Record<TKey, TVal>>& GetList(int index) const {
        if (index < 0 || index >= this->size) {
            throw std::out_of_range("Invalid list index");
        }
        return pList[index];
    }

    typename std::list<Record<TKey, TVal>>::const_iterator GetListBegin(int index) const {
        if (index < 0 || index >= this->size) {
            throw std::out_of_range("Invalid list index");
        }
        return pList[index].begin();
    }

    typename std::list<Record<TKey, TVal>>::const_iterator GetListEnd(int index) const {
        if (index < 0 || index >= this->size) {
            throw std::out_of_range("Invalid list index");
        }
        return pList[index].end();
    }

    bool IsListEmpty(int index) const {
        if (index < 0 || index >= this->size) {
            throw std::out_of_range("Invalid list index");
        }
        return pList[index].empty();
    }

    bool Find(TKey key) {
        this->Eff = 0;
        CurrList = this->HashFunc(key);
        for (currI = pList[CurrList].begin(); currI != pList[CurrList].end(); ++currI) {
            this->Eff++;
            if (currI->key == key)
                return true;
        }
        return false;
    }

    void Insert(Record<TKey, TVal> rec) {
        if (IsFull())
            throw "Error, no space for new record!";

        if (Find(rec.key)) 
            throw "Error, this record already exists!";

        pList[CurrList].push_front(rec);
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
                int newHash = std::hash<TKey>{}(rec.key) % newSize;
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