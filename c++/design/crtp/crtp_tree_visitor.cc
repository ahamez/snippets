
#include <cassert>
#include <iostream>

struct TreeNode
{
    enum Kind {RED, BLUE};

    TreeNode(Kind kind_, TreeNode* left_ = NULL, TreeNode* right_ = NULL)
        : kind(kind_), left(left_), right(right_)
    {}

    Kind kind;
    TreeNode *left, *right;
};

template <typename Derived>
class GenericVisitor
{
public:
    void visit_preorder(TreeNode* node)
    {
        if (node) {
            dispatch_node(node);
            visit_preorder(node->left);
            visit_preorder(node->right);
        }
    }

    void visit_inorder(TreeNode* node)
    {
        if (node) {
            visit_inorder(node->left);
            dispatch_node(node);
            visit_inorder(node->right);
        }
    }

    void visit_postorder(TreeNode* node)
    {
        if (node) {
            visit_postorder(node->left);
            visit_postorder(node->right);
            dispatch_node(node);
        }
    }

    // It's possible to have default implementations:
    void handle_RED(TreeNode* node)
    {}

    void handle_BLUE(TreeNode* node)
    {}

private:

    // Convenience method for CRTP
    Derived& derived()
    {
        return *static_cast<Derived*>(this);
    }

    void dispatch_node(TreeNode* node)
    {
        switch (node->kind) {
            case TreeNode::RED:
                derived().handle_RED(node);
                break;
            case TreeNode::BLUE:
                derived().handle_BLUE(node);
                break;
            default:
                assert(0);
        }
    }
};


class SpecialVisitor
  : public GenericVisitor<SpecialVisitor>
{
public:
    void handle_RED(TreeNode* node)
    {
    }
};

// Without CRTP:
template <typename T>
class GenericVisitor2
{
public:
    void visit_preorder(TreeNode* node)
    {
        if (node) {
            dispatch_node(node);
            visit_preorder(node->left);
            visit_preorder(node->right);
        }
    }

    void visit_inorder(TreeNode* node)
    {
        if (node) {
            visit_inorder(node->left);
            dispatch_node(node);
            visit_inorder(node->right);
        }
    }

    void visit_postorder(TreeNode* node)
    {
        if (node) {
            visit_postorder(node->left);
            visit_postorder(node->right);
            dispatch_node(node);
        }
    }

private:

    void dispatch_node(TreeNode* node)
    {
        switch (node->kind) {
            case TreeNode::RED:
                T{}.handle_RED(node);
                break;
            case TreeNode::BLUE:
                T{}.handle_BLUE(node);
                break;
            default:
                assert(0);
        }
    }
};

// We have to define all methods:
class SpecialVisitor2Impl
{
public:
    void handle_RED(TreeNode* node)
    {
        cerr << "RED is special\n";
    }

    void handle_BLUE(TreeNode* node)
    {
        cerr << "BLUE is special\n";
    }
};

// And we have to define an alias:
using SpecialVisitor2 = GenericVisitor2<SpecialVisitor2Impl>;


//  However, usage is the same:
int main ()
{
  TreeNode leaf{TreeNode::RED};
  TreeNode root{TreeNode::BLUE, &leaf};

  SpecialVisitor{}.visit_preorder(&root);
  SpecialVisitor2{}.visit_preorder(&root);
}
