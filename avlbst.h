#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void trickleUp(AVLNode<Key, Value>* node);
    void rebalance(AVLNode<Key, Value>* z);
    void rotateLeft(AVLNode<Key, Value> * y);
    void rotateRight(AVLNode<Key, Value>* y);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    AVLNode<Key, Value>* parent = nullptr;
    AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->root_);

    while(curr != nullptr){
      if(new_item.first == curr->getKey()){
        curr->setValue(new_item.second);
        return;
      }

      parent = curr;
      if(new_item.first < curr -> getKey()){
        curr = curr -> getLeft();
      } else {
        curr = curr -> getRight();
      }

    }

    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);
    
    if(parent == nullptr){
      this->root_ = newNode;
    } else if (new_item.first < parent-> getKey()){
      parent -> setLeft(newNode);
    } else{
      parent -> setRight(newNode);
    }

    trickleUp(newNode);
}

template<class Key, class Value>
void AVLTree<Key, Value>::trickleUp(AVLNode<Key, Value>* node){

  AVLNode<Key, Value>* parent = node->getParent();
  while(parent){
    
    if (node == parent->getLeft()){
      parent -> updateBalance(1);
    } else {
      parent -> updateBalance(-1);
    }

    int balance = parent -> getBalance();

    if(balance == 0){
      break;
    }
    else if(balance == -1 || balance == 1){
      node = parent;
      parent = parent->getParent();
    }
    else{

      rebalance(parent);
      break;
    }
  }

}

template<class Key, class Value>
void AVLTree<Key, Value>::rebalance(AVLNode<Key, Value>* z){
  int balance = z -> getBalance();

  if (balance == 2){
    AVLNode<Key, Value>* y = z -> getLeft();
    if (y->getBalance() >= 0){ // for LL
      rotateRight(z);
    }
    else{ //for LR
      rotateLeft(y);
      rotateRight(z);
    }
  }

  else if (balance == -2){
    AVLNode<Key, Value>* y = z -> getRight();
    if (y->getBalance() <= 0){ // for LL
      rotateLeft(z);
    }
    else{
      rotateRight(y);
      rotateLeft(z);
    }
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* y){
  if (y == nullptr || y->getLeft() == nullptr) return;
  
  AVLNode<Key, Value>* x = y->getLeft();
  AVLNode<Key, Value>* b = x ->getRight();

  x->setRight(y);
  y->setLeft(b);

  if (b != nullptr){
    b->setParent(y);
  }

  x->setParent(y->getParent());

  if(y->getParent() == nullptr){
    this->root_ = x;
  } else if (y==y->getParent()->getLeft()){
    y->getParent()->setLeft(x);
  } else{
    y->getParent()->setRight(x);
  }

  y->setParent(x);


  y -> setBalance(y -> getBalance() - 1 - std::max<int8_t>(x->getBalance(), 0));
  x -> setBalance(x -> getBalance() - 1 + std::min<int8_t>(y->getBalance(), 0));

}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* y){
  if (y == nullptr || y->getRight() == nullptr) return;
  
  AVLNode<Key, Value>* x = y->getRight();
  AVLNode<Key, Value>* b = x ->getLeft();

  x->setLeft(y);
  y->setRight(b);

  if (b){
    b->setParent(y);
  }

  x->setParent(y->getParent());

  if(!y->getParent()){
    this->root_ = x;
  } else if (y == y->getParent()->getLeft()){
    y->getParent()->setLeft(x);
  } else{
    y->getParent()->setRight(x);
  }

  y->setParent(x);

  y -> setBalance(y->getBalance() + 1 - std::min<int8_t>(x->getBalance(), 0));
  x -> setBalance(x->getBalance() + 1 + std::max<int8_t>(y->getBalance(), 0));

}




/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
  AVLNode<Key, Value>* node = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
  if (node == nullptr) return;

  AVLNode<Key, Value>* child = nullptr;
  AVLNode<Key, Value>* parent = nullptr;
  bool removedFromLeft = false;

  if (node->getLeft() != nullptr && node->getRight() != nullptr) {
    AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(node));
    this->nodeSwap(node, pred);
  }

  parent = node -> getParent();
  child = (node->getLeft() != nullptr) ? node->getLeft() : node->getRight();


  if (child != nullptr) {
    child->setParent(parent);
  }

  if (parent == nullptr) {
    this->root_ = child;
    if(child != nullptr){
      child->setParent(nullptr);
    }
  } 
  else if (parent->getLeft() == node) {
    parent->setLeft(child);
    removedFromLeft = true;
  } 
  else {
    parent->setRight(child);
  }

  delete node;

  AVLNode<Key, Value>* curr = parent;
  while (curr != nullptr) {
    if (removedFromLeft) {
      curr->updateBalance(-1);
    } else {
      curr->updateBalance(1);
    }

    int balance = curr->getBalance();

    if (balance == 1 || balance == -1) {
      break;
    } else if (balance == 0) {
      removedFromLeft = (curr->getParent() != nullptr && curr == curr->getParent()->getLeft());
      curr = curr->getParent();
    } 
    else {
      AVLNode<Key, Value>* parent = curr->getParent();
      bool isLeft = (parent != nullptr && curr == parent->getLeft());

      rebalance(curr);

      curr = parent;
      removedFromLeft = isLeft;
    }
  }
}



template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
