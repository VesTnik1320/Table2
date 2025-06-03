#pragma once
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
public:
    void InsertBalanced(const Record<TKey, TVal>& rec) {
        InsBalTree(this->pRoot, rec);  // pRoot уже является TreeNode*&
    }
    void Inserte(const TKey& key, const TVal& val) /*override*/ {
        Record<TKey, TVal> rec(key, val);
        InsBalTree(this->pRoot, rec); // Корректный вызов балансировки
    }
	//void PrintTree(ostream& os) const /*override*/ {
 //       if (this->pRoot == nullptr) {
 //           os << "Дерево пусто" << endl;
 //           return;
 //       }
 //       // Рекурсивная функция для печати с уровнями и балансом
 //       function<void(TreeNode<TKey, TVal>*, int, const string&)> printNode =
 //           [&](TreeNode<TKey, TVal>* node, int level, const string& prefix) {
 //           if (node == nullptr) return;
 //           // Печатаем текущий узел с балансом
 //           os << prefix << "Уровень " << level << ": " << node->rec.key
 //               << " (баланс: " << node->bal << ")" << endl;
 //           // Рекурсивно печатаем поддеревья
 //           printNode(node->pLeft, level + 1, prefix + "  ");
 //           printNode(node->pRight, level + 1, prefix + "  ");
 //           };
 //       printNode(this->pRoot, 0, "");
	//}
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
        this->Eff++;
        pNode = new TreeNode<TKey, TVal>(rec.key, rec.val);
        res = H_INC;
        this->DataCount++;
    }
    else if (rec.key < pNode->rec.key) {
        this->Eff++;
        int tmp = InsBalTree(pNode->pLeft, rec);
        if (tmp == H_INC) {
            this->Eff++;//?
            res = BalTreeLeft(pNode);
        }
    }
    else if (pNode->rec.key < rec.key) {
        this->Eff++;
        int tmp = InsBalTree(pNode->pRight, rec);
        if (tmp == H_INC) {
            this->Eff++;//?
            res = BalTreeRight(pNode);
        }
    }
    return res;
}

template <typename TKey, typename TVal>
int BalTreeTable<TKey, TVal>::BalTreeLeft(TreeNode<TKey, TVal>*& pNode, bool isInsert)
{
    int res = H_OK;
    if (pNode->bal == BAL_RIGHT) {
        pNode->bal = BAL_OK;
        if (isInsert) res = H_OK;
        else res = H_DEC;
    }
    else if (pNode->bal == BAL_OK) {
        pNode->bal = BAL_LEFT;
        if (isInsert) res = H_INC;
        else res = H_OK;
    }
    else if (pNode->bal == BAL_LEFT) {
        TreeNode<TKey, TVal>* p1 = pNode->pLeft;
        if (p1->bal == BAL_LEFT) {
            // Малый правый поворот
            pNode->pLeft = p1->pRight;
            p1->pRight = pNode;
            pNode->bal = BAL_OK;
            pNode = p1;
            pNode->bal = BAL_OK;
        }
        else {
            // Большой правый поворот
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

            if (isInsert) res = H_OK;
            else res = H_DEC;
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
            // Малый левый поворот
            pNode->pRight = p1->pLeft;
            p1->pLeft = pNode;
            pNode->bal = BAL_OK;
            pNode = p1;
            pNode->bal = BAL_OK;
        }
        else {
            // Большой левый поворот
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

            if (!isInsert) res = H_DEC;
        }
    }
    return res;
}


template<typename TKey, typename TVal>
int BalTreeTable<TKey, TVal>::DeleteRec(TreeNode<TKey, TVal>*& pNode, TKey key)
{
    int res = H_OK;
    this->Eff++;
    if (pNode == nullptr) throw - 1;
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
    if (this->IsEmpty()) return;
    DeleteRec(this->pRoot, key);
}
