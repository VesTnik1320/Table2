﻿#pragma once
#include "TreeTable.h"
#include "Record.h"
#include <queue> 
const int H_OK = 0, H_INC = 1, H_DEC = -1;

template <typename TKey, typename TVal>
class BalTreeTable : public TreeTable<TKey, TVal> {
protected:
    int InsBalTree(TreeNode<TKey, TVal>*& pNode, Record<TKey, TVal> rec);
    int BalTreeLeft(TreeNode<TKey, TVal>*& pNode, bool isInsert = true);
    int BalTreeRight(TreeNode<TKey, TVal>*& pNode, bool isInsert = true);
    int DeleteRec(TreeNode<TKey, TVal>*& pNode, TKey key);
    TreeNode<TKey, TVal>* FindMin(TreeNode<TKey, TVal>* pNode);
    int RemoveMin(TreeNode<TKey, TVal>*& pNode);
    TreeNode<TKey, TVal>* CopyTree(TreeNode<TKey, TVal>* pNode) {
        if (!pNode) return nullptr;
        auto* newNode = new TreeNode<TKey, TVal>(pNode->rec.key, pNode->rec.val);
        newNode->bal = pNode->bal;
        newNode->pLeft = CopyTree(pNode->pLeft);
        newNode->pRight = CopyTree(pNode->pRight);
        return newNode;
    }
public:
    BalTreeTable(const BalTreeTable& other) {
        this->pRoot = CopyTree(other.pRoot);
        this->DataCount = other.DataCount;
    }

    BalTreeTable& operator=(const BalTreeTable& other) {
        if (this != &other) {
            this->Clear(); 
            this->pRoot = CopyTree(other.pRoot);
            this->DataCount = other.DataCount;
        }
        return *this;
    }
    BalTreeTable() : TreeTable<TKey, TVal>() {}
    void InsertBalanced(const Record<TKey, TVal>& rec) {
        InsBalTree(this->pRoot, rec); 
    }
    void Inserte(const TKey& key, const TVal& val) {
        Record<TKey, TVal> rec(key, val);
        InsBalTree(this->pRoot, rec); 
    }
    TreeNode<TKey, TVal>* GetRoot() const { return this->pRoot; }
    void Insert(Record<TKey, TVal> rec);
    void Insert(TKey key, TVal val);
    void Delete(TKey key);
    void Reset()  {
        TreeTable<TKey, TVal>::Reset();
    }

    void GoNext()  {
        TreeTable<TKey, TVal>::GoNext();
    }

    bool IsEnd()  {
        return TreeTable<TKey, TVal>::IsEnd();
    }

    Record<TKey, TVal> GetCurr()  {
        return TreeTable<TKey, TVal>::GetCurr();
    }

    bool IsFull() const  {
        return TreeTable<TKey, TVal>::IsFull();
    }

    void Clear()  {
        TreeTable<TKey, TVal>::Clear();
    }

    int GetEff() {
        return TreeTable<TKey, TVal>::GetEff();
    }

    void Resize(int newSize) override {
        TreeTable<TKey, TVal>::Resize(newSize);
    }

    std::string GetTypeName() const override {
        return "BalTreeTable";
    }

};
template <typename TKey, typename TVal>
int BalTreeTable<TKey, TVal>::InsBalTree(TreeNode<TKey, TVal>*& pNode, Record<TKey, TVal> rec) { 
    int res = H_OK;
    if (pNode == nullptr) {
        pNode = new TreeNode<TKey, TVal>(rec.key, rec.val);
        res = H_INC;
        this->DataCount++;
    }
    else if (rec.key < pNode->rec.key) {
        this->Eff++;
        int tmp = InsBalTree(pNode->pLeft, rec);
        if (tmp == H_INC) {
            this->Eff++;
            res = BalTreeLeft(pNode);
        }
    }
    else if (pNode->rec.key < rec.key) {
        this->Eff++;
        int tmp = InsBalTree(pNode->pRight, rec);
        if (tmp == H_INC) {
            this->Eff++;
            res = BalTreeRight(pNode);
        }
    }
    return res;
}

template <typename TKey, typename TVal>
int BalTreeTable<TKey, TVal>::BalTreeLeft(TreeNode<TKey, TVal>*& pNode, bool isInsert) {
    int res = H_OK;
    if (pNode->bal == BAL_RIGHT) {
        pNode->bal = BAL_OK;
        res = isInsert ? H_OK : H_DEC;
    }
    else if (pNode->bal == BAL_OK) {
        pNode->bal = BAL_LEFT;
        res = isInsert ? H_INC : H_OK;
    }
    else if (pNode->bal == BAL_LEFT) {
        TreeNode<TKey, TVal>* p1 = pNode->pLeft;
        if (p1->bal == BAL_LEFT) {
            pNode->pLeft = p1->pRight;
            p1->pRight = pNode;
            pNode->bal = BAL_OK;
            pNode = p1;
            pNode->bal = BAL_OK;
            res = isInsert ? H_OK : H_DEC;
        }
        else {
            TreeNode<TKey, TVal>* p2 = p1->pRight;
            p1->pRight = p2->pLeft;
            p2->pLeft = p1;
            pNode->pLeft = p2->pRight;
            p2->pRight = pNode;

            if (p2->bal == BAL_LEFT) {
                pNode->bal = BAL_RIGHT;
                p1->bal = BAL_OK;
            }
            else if (p2->bal == BAL_RIGHT) {
                pNode->bal = BAL_OK;
                p1->bal = BAL_LEFT;
            }
            else {
                pNode->bal = BAL_OK;
                p1->bal = BAL_OK;
            }

            pNode = p2;
            pNode->bal = BAL_OK;
            res = isInsert ? H_OK : H_DEC;
        }
    }
    return res;
}

template<typename TKey, typename TVal>
int BalTreeTable<TKey, TVal>::BalTreeRight(TreeNode<TKey, TVal>*& pNode, bool isInsert)
{
    int res = H_OK;
    if (pNode->bal == BAL_LEFT) {
        pNode->bal = BAL_OK;
        if (isInsert) res = H_OK;
        else res = H_DEC;
    }
    else if (pNode->bal == BAL_OK) {
        pNode->bal = BAL_RIGHT;
        if (isInsert) res = H_INC;
        else res = H_OK;
    }
    else if (pNode->bal == BAL_RIGHT) {
        TreeNode<TKey, TVal>* p1 = pNode->pRight;
        if (p1->bal == BAL_RIGHT) {
            pNode->pRight = p1->pLeft;
            p1->pLeft = pNode;
            pNode->bal = BAL_OK;
            pNode = p1;
            pNode->bal = BAL_OK;
            if (isInsert) res = H_OK;
            else res = H_DEC;
        }
        else {
            TreeNode<TKey, TVal>* p2 = p1->pLeft;
            p1->pLeft = p2->pRight;
            p2->pRight = p1;
            pNode->pRight = p2->pLeft;
            p2->pLeft = pNode;

            if (p2->bal == BAL_RIGHT) {
                pNode->bal = BAL_LEFT;
                p1->bal = BAL_OK;
            }
            else if (p2->bal == BAL_LEFT) {
                pNode->bal = BAL_OK;
                p1->bal = BAL_RIGHT;
            }
            else {
                pNode->bal = BAL_OK;
                p1->bal = BAL_OK;
            }

            pNode = p2;
            pNode->bal = BAL_OK;

            if (isInsert) res = H_OK;
            else res = H_DEC;
        }
    }
    return res;
}

template<typename TKey, typename TVal>
int BalTreeTable<TKey, TVal>::DeleteRec(TreeNode<TKey, TVal>*& pNode, TKey key)
{
    int res = H_OK;
    this->Eff++;
    if (pNode == nullptr)
        throw std::runtime_error("Key not found");
    if (pNode->rec.key < key)
    {
        int tmp = DeleteRec(pNode->pRight, key);
        if (tmp != H_OK) res = BalTreeLeft(pNode, false);
    }
    else if (key < pNode->rec.key)
    {
        int tmp = DeleteRec(pNode->pLeft, key);
        if (tmp != H_OK) res = BalTreeRight(pNode, false);
    }
    else
    {
        this->DataCount--;
        if (pNode->pLeft == nullptr && pNode->pRight == nullptr)
        {
            delete pNode;
            pNode = nullptr;
            res = H_DEC;
        }
        else if (pNode->pLeft == nullptr)
        {
            TreeNode<TKey, TVal>* tmp = pNode;
            pNode = pNode->pRight;
            delete tmp;
            res = H_DEC;
        }
        else if (pNode->pRight == nullptr)
        {
            TreeNode<TKey, TVal>* tmp = pNode;
            pNode = pNode->pLeft;
            delete tmp;
            res = H_DEC;
        }
        else
        {
            TreeNode<TKey, TVal>* tmpRight = pNode->pRight;
            TreeNode<TKey, TVal>* pMin = FindMin(tmpRight);
            pNode->rec = pMin->rec;
            int tmpRes = RemoveMin(tmpRight);
            pNode->pRight = tmpRight;
            if (tmpRes != H_OK) res = BalTreeLeft(pNode, false);
        }
    }
    return res;
}

template<typename TKey, typename TVal>
TreeNode<TKey, TVal>* BalTreeTable<TKey, TVal>::FindMin(TreeNode<TKey, TVal>* pNode)
{
    TreeNode<TKey, TVal>* res = pNode;
    while (res->pLeft != nullptr)
    {
        this->Eff++;
        res = res->pLeft;
    }
    return res;
}

template<typename TKey, typename TVal>
int BalTreeTable<TKey, TVal>::RemoveMin(TreeNode<TKey, TVal>*& pNode)
{
    this->Eff++;
    int res = H_OK;
    if (pNode->pLeft == nullptr)
    {
        TreeNode<TKey, TVal>* tmp = pNode;
        pNode = pNode->pRight;
        delete tmp;
        res = H_DEC;
    }
    else
    {
        res = RemoveMin(pNode->pLeft);
        if (res != H_OK) res = BalTreeRight(pNode, false);
    }
    return res;
}

template<typename TKey, typename TVal>
void BalTreeTable<TKey, TVal>::Insert(Record<TKey, TVal> rec)
{
    if (this->Find(rec.key)) {
        throw - 1;
    }
    InsBalTree(this->pRoot, rec);
}

template<typename TKey, typename TVal>
void BalTreeTable<TKey, TVal>::Insert(TKey key, TVal val)
{
    if (this->Find(key)) {
        throw - 1;
    }
    Record<TKey, TVal> rec(key, val);
    InsBalTree(this->pRoot, rec);
}

template<typename TKey, typename TVal>
void BalTreeTable<TKey, TVal>::Delete(TKey key)
{
    if (this->IsEmpty())
        throw std::runtime_error("Empty tree");
    DeleteRec(this->pRoot, key);
}
