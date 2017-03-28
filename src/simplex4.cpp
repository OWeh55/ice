/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2002 FSU Jena, Digital Image Processing Group
 * Contact: ice@pandora.inf.uni-jena.de
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <stdio.h>

#include "simplex1.h"
#include "simplex4.h"


namespace ice
{
  typedef struct tnode
  {
    int           data;
    struct tnode* parent;
    struct tnode* left;
    struct tnode* right;
  } TNODE;

  extern TNODE* si4_root;

  static void killnode(TNODE* node)
  {
    if (node->left  != nullptr) killnode(node->left);;

    if (node->right != nullptr) killnode(node->right);;

    if (node != nullptr) FREE(node);
  }

  void killtree()
  {
    if (si4_root != nullptr) killnode(si4_root);

    si4_root = nullptr;
  }

  void addtree(int data)
  {
    TNODE* node = si4_root;
    TNODE* parent;

    if (si4_root == nullptr)
      {
        MALLOC(si4_root, 1, TNODE);
        si4_root->data   = data;
        si4_root->parent = nullptr;
        si4_root->left   = nullptr;
        si4_root->right  = nullptr;
        return;
      }

    while (node != nullptr)
      {
        if (node->data > data)
          {
            parent = node;
            node = node->left;
          }
        else if (node->data < data)
          {
            parent = node;
            node = node->right;
          }
        else
          {
            return;  /* already there */
          }
      }

    MALLOC(node, 1, TNODE);
    node->data   = data;
    node->parent = parent;
    node->left   = nullptr;
    node->right  = nullptr;

    if (parent->data > data)
      {
        parent->left  = node;
      }
    else
      {
        parent->right = node;
      }
  }

  void deltree(int data)
  {
    TNODE* node   = si4_root;
    TNODE* parent = nullptr;
    TNODE* node1, *node2;

    while (node != nullptr && node->data != data)
      {
        if (node->data > data)
          {
            node = node->left;
          }
        else if (node->data < data)
          {
            node = node->right;
          }
      }

    if (node == nullptr)
      {
        return;  /* not there anyway */
      }

    parent = node->parent;

    if (node->right == nullptr)
      {
        node1 = node->left;

        if (node1 != nullptr)
          {
            node1->parent = parent;
          }
      }
    else if (node->left == nullptr)
      {
        node1 = node->right;
        node1->parent = parent;
      }
    else
      {
        node1 = node->left;

        for (node2 = node1; node2->right != nullptr; node2 = node2->right) { }

        node2->right = node->right;
        node->right->parent = node2;

        node->left->parent = parent;
      }

    if (parent == nullptr)
      {
        si4_root = node1;
      }
    else
      {
        if (parent->data > data)
          {
            parent->left  = node1;
          }
        else
          {
            parent->right = node1;
          }
      }

    FREE(node);
  }

  static TNODE* curnode = nullptr;

  int getfirst()
  {
    TNODE* node;
    TNODE* parent;

    if (si4_root == nullptr)
      {
        /* printf("getfirst: empty tree \n"); */ return -1;
      }

    for (node = si4_root; node != nullptr; parent = node, node = node->left) { }

    curnode = parent;

    return curnode->data;
  }

  int getnext()
  {
    TNODE* node;
    TNODE* par;

    if (curnode == nullptr)
      {
        /*printf("getnext: no current node \n");*/ return -1;
      }

    if (curnode->right != nullptr)
      {
        for (node = curnode->right; node != nullptr; par = node, node = node->left) { }

        curnode = par;
        return curnode->data;
      }

    for (node = curnode->parent; node != nullptr; node = node->parent)
      {
        if (node->data > curnode->data) break;
      }

    curnode = node;

    if (curnode != nullptr)
      {
        return curnode->data;
      }
    else
      {
        return -1;    /* no more */
      }
  }

  int getlast()
  {
    TNODE* node;
    TNODE* parent;

    if (si4_root == nullptr)
      {
        /* printf("getlast: empty tree \n"); */ return -1;
      }

    for (node = si4_root; node != nullptr; parent = node, node = node->right) { }

    curnode = parent;

    return curnode->data;
  }

  int getprev()
  {
    TNODE* node;
    TNODE* par;

    if (curnode == nullptr)
      {
        /*printf("getnext: no current node \n");*/ return -1;
      }

    if (curnode->left != nullptr)
      {
        for (node = curnode->left; node != nullptr; par = node, node = node->right) { }

        curnode = par;
        return curnode->data;
      }

    for (node = curnode->parent; node != nullptr; node = node->parent)
      {
        if (node->data < curnode->data) break;
      }

    curnode = node;

    if (curnode != nullptr)
      {
        return curnode->data;
      }
    else
      {
        return -1;    /* no more */
      }
  }

  static void printnode(TNODE* node)
  {
    int parent, left, right;

    if (node != nullptr)
      {
        if (node->parent != nullptr)
          {
            parent = node->parent->data;
          }
        else
          {
            parent = -1;
          }

        if (node->left  != nullptr)
          {
            left  = node->left  ->data;
          }
        else
          {
            left   = -1;
          }

        if (node->right != nullptr)
          {
            right = node->right ->data;
          }
        else
          {
            right  = -1;
          }

        printnode(node->left);
        printf("    %4d %4d %4d %4d\n", node->data, parent, left, right);
        printnode(node->right);
      }
  }

  void printtree()
  {
    if (si4_root != nullptr) printf("root node is %4d\n", si4_root->data);

    printf("  data parent left right\n");
    printnode(si4_root);
  }

}
