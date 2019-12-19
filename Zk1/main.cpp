#include <iostream>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <map>
#include <set>
#include <memory>

using namespace std;

struct Node {
    bool final = false;
    unordered_map<char, Node*> children;
};

Node* tree = new Node();

void add_to_prefix_tree(string &prefix) {
    Node *current = tree;
    for (size_t idx = 0; idx < prefix.size(); ++idx) {
        Node *next = current->children[prefix[idx]];
        if (next == nullptr) {
            next = new Node();
            current->children[prefix[idx]] = next;
        }
        current = next;
    }
    current->final = true;
}

uint64_t get_prefix_counts_of_length_k(Node *root, uint64_t cur_depth, uint64_t k, uint64_t alphabet_size) {
    if (root->final) {
        uint64_t cnt = 1;
        uint64_t remaining = k - cur_depth;
        for (uint64_t i = 0; i < remaining; ++i) {
            cnt = (cnt*alphabet_size)%100000;
        }
        return cnt;
    } else {
        uint64_t cnt = 0;
        for (auto char_node_ptr_pair : root->children) {
            Node* child = char_node_ptr_pair.second;
            cnt = (cnt + get_prefix_counts_of_length_k(child, cur_depth+1, k, alphabet_size))%100000;
        }
        return cnt;
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);
    map<string, uint64_t> counts;

    string alphabet;
    uint32_t committee_members, min_approve_count, prefix_length;

    cin >> alphabet >> committee_members >> min_approve_count >> prefix_length;
    //cout << alphabet << endl << commitee_members << endl << min_approve_count << endl;

    for (size_t committee_member = 0; committee_member < committee_members; ++committee_member) {
        set<string> member_used;
        uint32_t member_list_length;
        cin >> member_list_length;
        for (size_t member_string_idx = 0; member_string_idx < member_list_length; ++member_string_idx) {
            string cur_string;
            cin >> cur_string;
            if (member_used.find(cur_string) == member_used.end()) {
                member_used.insert(cur_string);
                uint64_t &cur_count = counts[cur_string];
                if (cur_count == min_approve_count - 1) {
                    add_to_prefix_tree(cur_string);
                    cur_count++;
                } else if (cur_count >= min_approve_count) {
                    ;
                } else {
                    cur_count++;
                }
            }
        }
    }
    cout << get_prefix_counts_of_length_k(tree, 0, prefix_length, alphabet.size()) << endl;

    return 0;
}