#ifndef MYLIST_HPP
#define MYLIST_HPP

#include <cstdlib>

template<class Data>
class ListNode {

  public:
    ListNode *next, *prev;
    Data data;
    ListNode(Data d):data(d),next(NULL),prev(NULL){};

    ~ListNode(){

    };

    //Insert new node at the end of the List
    void Insert(Data data){
      if(next == NULL){
        next = new ListNode(data);
        next->prev = this;
      }
      else{
        next->Insert(data);
      }
    };

    //Delete this node and return the list head
    ListNode* Delete(){
      if(prev != NULL)  //update next node
        prev->next = next;
      if(next != NULL)  //update prev node
        next->prev = prev;
      delete this;
      //now return the new head of the list
      if(next != NULL)
        return next->First();
      else if(prev != NULL)
        return prev->First();
      else
        return NULL;  //the whole list is deleted
    };

    //Returns the first node of the list
    ListNode* First(){
      if(prev != NULL)
        return prev->First(); //go left
      else
        return this;    //found first node, return
    };

    //Called by a node(usually first) deletes all next nodes including itself
    void DeleteList(){
      if(next != NULL)
        next->DeleteList(); //recursive until the rightmost node
      delete this;
    };
};

#endif
