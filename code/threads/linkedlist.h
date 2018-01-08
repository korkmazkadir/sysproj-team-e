/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   LinkedList.h
 * Author: kadir
 *
 * Created on December 19, 2017, 11:05 AM
 */

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

class LinkedList {
public:
    LinkedList();
    
    void add(void *item);
    void remove(void *item);
    void *find(void *item);
    int getSize();
    
    LinkedList *get(int index);
    
    virtual ~LinkedList();
    
private:
    int size = 0;
    void *payload;
    LinkedList *parent;
    LinkedList *child;
    
    LinkedList * findContainer(void *item);
    
};

#endif /* LINKEDLIST_H */

