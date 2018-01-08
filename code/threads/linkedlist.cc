/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   LinkedList.cpp
 * Author: kadir
 * 
 * Created on December 19, 2017, 11:05 AM
 */
#include <cstddef>

#include "linkedlist.h"

LinkedList::LinkedList() {
    payload = NULL;
    parent = NULL;
    child = NULL;
}

void LinkedList::add(void *item) {
    LinkedList *node = new LinkedList();
    node->payload = item;
    node->parent = this;
    node->child = NULL;
    size++;
}

void LinkedList::remove(void *item) {
    LinkedList *container = findContainer(item);
    if (container != NULL) {
        container->parent->child = container->child;
        if (container->child != NULL) {
            container->child->parent = container->parent;
        }
        delete container;
        size--;
    }
}

void *LinkedList::find(void *item) {
    LinkedList *container = findContainer(item);
    if (container == NULL) {
        return NULL;
    }
    
    return container->payload;
}

int LinkedList::getSize(){
    return size;
}


LinkedList * LinkedList::get(int index){
    LinkedList *node;
    if(index == 0){
        return this;
    }else{
        node = this->get(index--);
    }
    return node;
}


LinkedList *LinkedList::findContainer(void *item) {
    if (item == this->payload) {
        return this;
    }

    if (this->child != NULL) {
        return this->child->findContainer(item);
    } else {
        return NULL;
    }
}

LinkedList::~LinkedList() {
}

