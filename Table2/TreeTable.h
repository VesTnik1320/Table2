#pragma once
#include "Record.h"
#include "Table.h"
#include <stack>
#include <vector>
const int BAL_OK = 0; const int BAL_LEFT = -1; const int BAL_RIGHT = 1;
template <class TKey, class TVal>
struct TreeNode
{
    Record<TKey, TVal> rec;
    TreeNode* pLeft, * pRight;
    int bal;
public:
    TreeNode(TKey k, TVal v) : rec(k, v), pLeft(nullptr), pRight(nullptr), bal(BAL_OK) {}
    bool operator<(TreeNode<TKey, TVal> r) { return rec.key < r.rec.key; };
    bool operator==(TreeNode<TKey, TVal> r) { return rec.key == r.rec.key; };
};
template <class TKey, class TVal>
class TreeTable : public Table<TKey, TVal>
{
    int pos, level;
protected:
    TreeNode<TKey, TVal>* pRoot, * pCurr, * pPrev;
    std::stack<TreeNode<TKey, TVal>*> st;
    void DelNode(TreeNode<TKey, TVal>* node);
public:
    TreeTable() : pRoot(nullptr), pCurr(nullptr), pPrev(nullptr), pos(0), level(0) {};
    bool IsEmpty() const override {
        return pRoot == nullptr;  // Таблица пуста, если корень nullptr
    }
    bool Find(TKey key);
    virtual void Insert(TKey key, TVal val);
    virtual void Insert(Record<TKey, TVal> rec);
    void Delete(TKey key);
    bool IsFull() const;
    void Clear();
    virtual int GetEff() const;
    virtual void Reset();
    virtual void GoNext();
    virtual bool IsEnd();


    virtual TKey GetCurrKey() { return pCurr->rec.key; }
    virtual TVal GetCurrVal() { return pCurr->rec.val; }
    virtual Record<TKey, TVal> GetCurr() { return pCurr->rec; }

    void PrintRec(ostream& os, TreeNode<TKey, TVal>* p);
    void PrintTree(ostream& os);

    ~TreeTable()
    {
        DelNode(pRoot);
    }

    void Resize(int newSize) override {
        if (newSize < 1) {
            throw std::invalid_argument("Новый размер меньше 1");
        }
        this->DataCount = newSize;
    }

};

template<class TKey, class TVal>
void TreeTable<TKey, TVal>::DelNode(TreeNode<TKey, TVal>* node)
{
    if (node == nullptr) return;
    DelNode(node->pLeft);
    DelNode(node->pRight);
    delete node;
}

template<class TKey, class TVal>
bool TreeTable<TKey, TVal>::Find(TKey key)
{
    this->Eff = 0;
    pCurr = pRoot;
    while (pCurr != nullptr) {
        this->Eff++;
        if (key > pCurr->rec.key) {
            pPrev = pCurr;
            pCurr = pCurr->pRight;
        }
        else if (key < pCurr->rec.key) {
            pPrev = pCurr;
            pCurr = pCurr->pLeft;
        }
        else if (key == pCurr->rec.key) {
            return true;
        }
    }
    //this->Eff++;
    pCurr = pPrev;
    return false;
}

template<class TKey, class TVal>
void TreeTable<TKey, TVal>::Insert(TKey key, TVal val) 
{
    if (Find(key)) throw - 1;
    TreeNode<TKey, TVal>* tmp = new TreeNode<TKey, TVal>(key, val);
    if (pCurr == nullptr) {
        pRoot = tmp;
    }
    else if (key > pCurr->rec.key) {
        pCurr->pRight = tmp;
    }
    else if (key < pCurr->rec.key) {
        pCurr->pLeft = tmp;
    }
    this->Eff++;
    this->DataCount++;
}

template<class TKey, class TVal>
void TreeTable<TKey, TVal>::Insert(Record<TKey, TVal> rec)
{
    if (Find(rec.key)) {
        throw - 1;
    }
    TreeNode<TKey, TVal>* tmp = new TreeNode<TKey, TVal>(rec.key, rec.val);
    if (pCurr == nullptr) {
        pRoot = tmp;
    }
    else if (rec.key > pCurr->rec.key) {
        pCurr->pRight = tmp;
    }
    else if (rec.key < pCurr->rec.key) {
        pCurr->pLeft = tmp;
    }
    this->Eff++;
    this->DataCount++;
}

template<class TKey, class TVal>
void TreeTable<TKey, TVal>::Delete(TKey key) {
    if (!Find(key)) {
        throw - 1;
    }
    TreeNode<TKey, TVal>* nodeToDelete = pCurr;
    //Один потомок слева
    if (pCurr->pRight == nullptr && pCurr->pLeft != nullptr) {
        this->Eff++;
        TreeNode<TKey, TVal>* child = pCurr->pLeft;
        if (nodeToDelete == pRoot) {
            pRoot = child;
        }
        else {
            if (pPrev->pLeft == nodeToDelete) {
                pPrev->pLeft = child;
            }
            else {
                pPrev->pRight = child;
            }
        }
    }
    //Один потомок справа
    else if (pCurr->pLeft == nullptr && pCurr->pRight != nullptr) {
        this->Eff++;
        TreeNode<TKey, TVal>* child = pCurr->pRight;
        if (nodeToDelete == pRoot) {
            pRoot = child;
        }
        else {
            if (pPrev->pLeft == nodeToDelete) {
                pPrev->pLeft = child;
            }
            else {
                pPrev->pRight = child;
            }
        }
    }
    //Нет потомков - Лист
    else if (pCurr->pLeft == nullptr && pCurr->pRight == nullptr) {
        this->Eff++;
        if (nodeToDelete == pRoot) {
            pRoot = nullptr;
        }
        else {
            if (pPrev->pLeft == nodeToDelete) {
                pPrev->pLeft = nullptr;
            }
            else {
                pPrev->pRight = nullptr;
            }
        }
    }
    //Оба потомка
    else {
        TreeNode<TKey, TVal>* beforeMaxLeft = nodeToDelete;
        TreeNode<TKey, TVal>* maxLeft = nodeToDelete->pLeft;
        while (maxLeft->pRight != nullptr) {
            this->Eff++;
            beforeMaxLeft = maxLeft;
            maxLeft = maxLeft->pRight;
        }
        nodeToDelete->rec = maxLeft->rec;
        if (nodeToDelete->pLeft == maxLeft) {
            nodeToDelete->pLeft = maxLeft->pLeft;
        }
        else {
            beforeMaxLeft->pRight = maxLeft->pLeft;
        }
        this->Eff++;
        nodeToDelete = maxLeft;
    }
    delete nodeToDelete;
    this->DataCount--;
}

template<class TKey, class TVal>
bool TreeTable<TKey, TVal>::IsFull() const
{
    try
    {
        Record<TKey, TVal>* tmp = new Record<TKey, TVal>;
        delete tmp;
        return true;
    }
    catch (...)
    {
        return false;
    }
}

template<class TKey, class TVal>
void TreeTable<TKey, TVal>::Clear()
{
    vector < TreeNode<TKey, TVal>*> toDel;
    int sz = this->DataCount, i = 0;
    for (Reset(); i < sz; GoNext(), i++)
    {

        toDel.push_back(pCurr);
    }
    for (i = 0; i < sz; i++)
    {
        delete toDel[i];
    }
    pCurr = nullptr;
    pPrev = nullptr;
    pRoot = nullptr;
    this->DataCount = 0;
    pos = 0;
}

template<class TKey, class TVal>
void TreeTable<TKey, TVal>::Reset() {
    pos = 0;
    if (IsEnd()) return;
    pCurr = pRoot;
    while (!st.empty()) {
        st.pop();
    }
    while (pCurr->pLeft != nullptr) {
        st.push(pCurr);
        pCurr = pCurr->pLeft;
    }
    pos = 0;
}
template<class TKey, class TVal>
void TreeTable<TKey, TVal> ::GoNext() {
    pCurr = pCurr->pRight;
    pos++;
    if (IsEnd()) return;
    if (pCurr == nullptr && !st.empty()) {
        pCurr = st.top();
        st.pop();
    }
    else {
        while (pCurr->pLeft != nullptr) {
            st.push(pCurr);
            pCurr = pCurr->pLeft;
        }
    }
}
template<class TKey, class TVal>
bool TreeTable<TKey, TVal>::IsEnd() {
    return pos >= this->DataCount;
}
template<class TKey, class TVal>
void TreeTable<TKey, TVal>::PrintRec(ostream& os, TreeNode<TKey, TVal>* p) {
    if (p == nullptr) {
        return;
    }
    for (int i = 0; i < level; i++) {
        os << " ";
    }
    os << level << " level: " << p->rec.key << endl;
    level++;
    PrintRec(os, p->pRight);
    PrintRec(os, p->pLeft);
    level--;
}
template<class TKey, class TVal>
void TreeTable<TKey, TVal>::PrintTree(ostream& os) {
    level = 0;
    PrintRec(os, pRoot);
}

template<class TKey, class TVal>
int TreeTable<TKey, TVal>::GetEff() const {
    return this->Eff;
}
