#include <iostream>
#include <cstdint>
#include <algorithm>

struct Node {
    Node *left = nullptr;
    Node *right = nullptr;
    Node *parent = nullptr;
    const uint32_t value;

    explicit Node(const uint32_t value) : value(value) {}

    ~Node() {
        delete left;
        delete right;
    }
};

/**
 * return root of the inserted tree
 * @param tree
 * @param insert
 * @return
 */
Node* insert_bst(Node *tree, Node *insert) {
    if (tree == nullptr) {
        return insert;
    } else {
        Node *current = tree;
        Node *parent = nullptr;
        while (current != nullptr) {
            parent = current;
            current = insert->value < current->value ? current->left : current->right;
        }
        if (insert->value < parent->value) {
            parent->left = insert;
        } else {
            parent->right = insert;
        }
        insert->parent = parent;
        return tree;
    }
}

/**
 * return Node* to the node containing the passed in value
 * must exist in the tree
 * @param tree
 * @param value
 * @return
 */
Node* find_bst(Node *tree, uint32_t value) {
    Node *current = tree;
    while (current != nullptr && current->value != value) {
        current = value < current->value ? current->left : current->right;
    }
    return current;
}

void fix_parent_down_pointer(Node *parent, Node *original, Node *new_child) {
    if (parent != nullptr) {
        if (parent->left == original) {
            parent->left = new_child;
        } else {
            parent->right = new_child;
        }
    }
}

Node* zig_R(Node* lower) {
    Node *higher = lower->parent;
    Node *lower_r = lower->right;
    lower->parent = higher->parent;
    lower->right = higher;
    higher->parent = lower;
    higher->left = lower_r;
    if (lower_r != nullptr) {
        lower_r->parent = higher;
    }
    fix_parent_down_pointer(lower->parent, higher, lower);
    return lower;
}

Node* zig_L(Node* lower) {
    Node *higher = lower->parent;
    Node *lower_l = lower->left;
    lower->parent = higher->parent;
    lower->left = higher;
    higher->parent = lower;
    higher->right = lower_l;
    if (lower_l != nullptr) {
        lower_l->parent = higher;
    }
    fix_parent_down_pointer(lower->parent, higher, lower);
    return lower;
}

Node* zigzig_RR(Node *lowest) {
    zig_R(lowest->parent);
    return zig_R(lowest);
}

Node* zigzig_LL(Node *lowest) {
    zig_L(lowest->parent);
    return zig_L(lowest);
}

Node* zigzag_LR(Node *lowest) {
    zig_L(lowest);
    return zig_R(lowest);
}

Node* zigzag_RL(Node *lowest) {
    zig_R(lowest);
    return zig_L(lowest);
}

Node* propagate_to_root_full(Node *node) {
    while (node->parent != nullptr) {
        if (node->parent->parent == nullptr) {
            if (node->parent->left == node) {
                //right rotate
                zig_R(node);
            } else {
                //left rotate
                zig_L(node);
            }
        } else {
            if (node->parent->left == node) {
                //first rotation right
                if (node->parent->parent->left == node->parent) {
                    //second rotation right
                    zigzig_RR(node);
                } else {
                    //second rotation left
                    zigzag_RL(node);
                }
            } else {
                //first rotation left
                if(node->parent->parent->left == node->parent) {
                    //second rotation right
                    zigzag_LR(node);
                } else {
                    //second rotation left;
                    zigzig_LL(node);
                }
            }
        }
    }
    return node;
}

Node* propagate_to_root_zig_only(Node *node) {
    while (node->parent != nullptr) {
        if (node->parent->left == node) {
            //right rotate
            zig_R(node);
        } else {
            //left rotate
            zig_L(node);
        }
    }
    return node;
}

Node* insert_splay_full(Node *tree, Node *insert) {
    tree = insert_bst(tree, insert);
    return propagate_to_root_full(insert);
}

Node* insert_splay_zig(Node *tree, Node* insert) {
    tree = insert_bst(tree, insert);
    return propagate_to_root_zig_only(insert);
}

/**
 * finds max in tree satisfying the BST property
 * @param tree <- non nullptr
 * @return
 */
Node* find_max(Node *tree) {
    Node *current = tree;
    while (current->right != nullptr) {
        current = current->right;
    }
    return current;
}

Node* find_min(Node *tree) {
    Node *current = tree;
    while (current->left != nullptr) {
        current = current->left;
    }
    return current;
}

Node* delete_splay_full(Node *tree, uint32_t value) {
    Node *to_delete = find_bst(tree, value);
    propagate_to_root_full(to_delete);
    Node *left = to_delete->left;
    Node *right = to_delete->right;

    to_delete->left = to_delete->right = nullptr;

    delete to_delete;

    if (left != nullptr) {
        left->parent = nullptr;
        Node *max_node = find_max(left);
        propagate_to_root_full(max_node);
        max_node->right = right;
        if (right != nullptr) {
            right->parent = max_node;
        }
        return max_node;
    } else if (right != nullptr) {
        right->parent = nullptr;
//        Node *min_node = find_min(right);
//        propagate_to_root_full(min_node);
//        min_node->left = left; // is always nullptr
//        return min_node;
        return right;
    } else {
        return nullptr;
    }
}

Node* delete_splay_zig_only(Node *tree, uint32_t value) {
    Node *to_delete = find_bst(tree, value);
    propagate_to_root_zig_only(to_delete);
    Node *left = to_delete->left;
    Node *right = to_delete->right;

    to_delete->left = to_delete->right = nullptr;

    delete to_delete;

    if (left != nullptr) {
        left->parent = nullptr;
        Node *max_node = find_max(left);
        propagate_to_root_zig_only(max_node);
        max_node->right = right;
        if (right != nullptr) {
            right->parent = max_node;
        }
        return max_node;
    } else if (right != nullptr) {
        right->parent = nullptr;
//        Node *min_node = find_min(right);
//        propagate_to_root_zig_only(min_node);
//        min_node->left = left; // is always nullptr
//        return min_node;
        return right;
    } else {
        return nullptr;
    }
}

int64_t get_height(const Node * tree) {
    if (tree == nullptr) {
        return -1;
    } else {
        return std::max(get_height(tree->left), get_height(tree->right)) + 1;
    }
}

int main() {
    uint32_t N{};
    std::cin >> N;
    Node *full_splay = nullptr;
    Node *zig_splay = nullptr;
    for (uint32_t i = 0; i < N; i++) {
        int32_t num;
        std::cin >> num;
        if (num > 0) {
            full_splay = insert_splay_full(full_splay, new Node(num));
            zig_splay = insert_splay_zig(zig_splay, new Node(num));
        } else {
            full_splay = delete_splay_full(full_splay, std::abs(num));
            zig_splay = delete_splay_zig_only(zig_splay, std::abs(num));
        }
    }
    std::cout << get_height(full_splay) << " " << get_height(zig_splay) << std::endl;
    delete full_splay;
    delete zig_splay;
    return 0;
}