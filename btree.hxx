//  Created by Dana Groff on 10/11/17.
//  Copyright © 2017 Dana Groff. All rights reserved.
//
// .TODO. create a nice EULA
//
// classes for use by btree
#include <memory>
#include <cassert>
#include <mutex>

namespace btree
{
    using std::shared_ptr;

    // node class for use by btree
    template <class T>
    class node
    {
    private:
        T value;
        
    public:
        shared_ptr< node<T> > left;
        shared_ptr< node<T> > right;
        node(T inVal) : left(nullptr), right(nullptr), value(inVal){};
        node(T inVal, shared_ptr< node<T> > l, shared_ptr< node<T> > r) : left(l), right(r), value(inVal){};
        T getValue() const { return value; }
    };

    // compare takes two comparable node<T> and returns <,>,=
    enum comparison { LT, EQ, GT };
    template <class T>
    comparison compare (node<T> const &node, T const value )
    {
        if (node.getValue()==value)
            return EQ;
        else if (node.getValue()<value)
            return LT;
        else
            return GT;
    }

    // return the left-most node in this (sub)tree
    template <class T>
    shared_ptr< node<T> > leftmost(shared_ptr < node<T> > current)
    {
        while (current.get() != nullptr)
        {
            current = current->left;
        }
        return current;
    }
  
    // return the right-mode node
    template <class T>
    shared_ptr < node<T> > rightmost(shared_ptr < node<T> > current)
    {
        while (current.get() != nullptr)
        {
            current = current->right;
        }
        return current;
    }
    
    // templated btree class
    //
    // T must be comparable
    //
    // .TODO. add syncronization to make thread safe
    template <class T>
    class btree
    {
    private:
        shared_ptr< node<T> > root;
        
        // inserts the provided node in the subtree provided
        // note there is no syncronization provided
        bool                                        // RETURNS: success true/false
        insert_node(shared_ptr< node<T> > ptr,      // IN: subtree where we will insert the node
                    shared_ptr< node<T> > toInsert) // IN: node to insert
        {
            while (ptr.get()!=nullptr)
            {
                switch (compare(*(ptr.get()), toInsert->getValue()))
                {
                    case EQ:
                        return false;
                    case LT:
                    {
                        if (ptr->right.get() == nullptr)
                        {
                            ptr->right = toInsert;
                            return true;
                        }
                        
                        ptr = ptr->right;
                        break;
                    }
                    case GT:
                    {
                        if (ptr->left.get() == nullptr)
                        {
                            ptr->left = toInsert;
                            return true;
                        }
                        
                        ptr = ptr->left;
                        break;
                    }
                }
            } //while
            
            assert(false); // should not get here
        } // insert_node
        
    public:
        btree() : root(nullptr) {}
        btree( shared_ptr< node<T> > n ) : root(n) {}
        
        const node<T> *getRoot() const { return root.get(); }
        
        // find: node that contains T
        bool                                  // RETURNS: boolean indicating node is found
        find(T val,                           // IN: value to find
             shared_ptr< node<T> > &ptr)      // OUT: node found or previous node
        {
            ptr = root;
            if (root.get() == nullptr)
                return false;
            
            while (ptr.get()!=nullptr)
            {
                switch ( compare (*(ptr.get()), val) )
                {
                    case EQ:
                        return true;
                    case LT:
                    {
                        if (ptr->right.get() == nullptr)
                            return false;
                        
                        ptr = ptr->right;
                        break;
                    }
                    case GT:
                    {
                        if (ptr->left.get() == nullptr)
                            return false;
                        
                        ptr = ptr->left;
                        break;
                    }
                }
            }
            return false;
        }
        
        // insert:   provided value in tree
        // returns:  true if it inserted the value
        bool
        insert(const T &inValue)
        {
            shared_ptr< node<T> > ptr;
            
            if (find(inValue, ptr))
            { // then the value is in the tree
                return false;
            }
            // find returns in ptr the root of the last subtree searched
            
            // .TODO. We can add syncronization protection here
            // Consider if we want to protect the read-for-duplicate above
            
            // create the node we will insert
            auto toInsert = shared_ptr< node<T> >(new node<T>(inValue));
            
            // insert created node into tree
            if (root.get() == nullptr) // no root node
            {
                root = toInsert;
                return true;
            }
            else
            {
                assert (ptr.get() != nullptr);
                return insert_node(ptr, toInsert);
            }
        } // insert
        
        // remove:  remove node with the same value as provided
        // returns: true if found and removed, false if not found
        bool
        remove(const T &toRemove)
        {
            shared_ptr< node<T> > ptr = root, prev = root;
            
            // TODO: set a lock here for concurrency safety
            // atomic operations will not work since you need to make multiple writes
            
            while (ptr.get())
            {
                switch ( compare ( *(ptr.get()), toRemove ) )
                {
                    case LT: // toRemove is > current, go right
                    {
                        prev = ptr;
                        ptr = ptr->right;
                        break;
                    }
                    case GT: // toRemove is < current, go left
                    {
                        prev = ptr;
                        ptr = ptr->left;
                        break;
                    }
                    case EQ: // found it
                    {
                        if (ptr == root) // if root
                        {
                            if (ptr->left.get()) // left branch exists
                            {
                                root = ptr->left;
                                shared_ptr< node <T> > right = rightmost(root);
                                if (right.get())
                                    right->right = ptr->right;
                            }
                            else // no left branch, assign right branch
                            {
                                root = ptr->right;
                            }
                        }
                        else if (prev->left == ptr) // on left side
                        {
                            if (ptr->left.get()) // left branch exists
                            {
                                prev->left = ptr->left;
                                shared_ptr< node<T> > right = rightmost(prev->left);
                                if (right.get())
                                    right->right = ptr->right;
                            }
                            else // no left branch
                            {
                                prev->left = ptr->right;
                            }
                        }
                        else // on right side
                        {
                            if (ptr->right.get()) // right branch exists
                            {
                                prev->right = ptr->right;
                                shared_ptr< node<T> > left = leftmost(prev->right);
                                if (left.get())
                                    left->left = ptr->left;
                            }
                            else // no right branch
                            {
                                prev->right = ptr->left;
                            }
                        }
                        return true;
                    } // case EQ
                } // switch
                
            } // while ptr exists
            
            return false; // not found
        } // remove
        
    }; // btree class
    
    // simple depth-first visitor
    // .TODO. clean up this function definition to take anything
    // -- this likely requires using "bind" in the caller
    template<class T>
    void walk_tree ( const node<T>& n,
                     std::function< std::ostream&(const node<T>&) > f)
    {
        if (n.left)
        {
            walk_tree(*n.left, f);
        }
        f(n);
        if (n.right)
        {
            walk_tree(*n.right,f);
        }
        return;
    } // walk_tree
    
} // namespace btree


// ostream helper classes

template<class T>
std::ostream& operator<< (std::ostream &os, const btree::node<T> &n)
{
    os << n.getValue();
    return os;
}


template<class T>
std::ostream& operator<< (std::ostream& os, const btree::btree<T>& tree)  
{
    if (!tree.getRoot())
    {
        os << "Emptry Tree";
        return os;
    }
    
    std::function< std::ostream&(const btree::node<T>&)>  out = [&os](const btree::node<T>& n )->std::ostream& { os << n << " "; return os;};
    btree::walk_tree(*(tree.getRoot()),out);
    
    return os;
}
