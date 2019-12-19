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
    uint64_t committee_count = 0;
    set<uint32_t> final_to_here;
    set<uint32_t> committee_members;
    unordered_map<char, Node*> children;
};

Node* tree = new Node();

void update_later_final(Node *root, uint32_t member) {
    if (root->final) {
        root->final_to_here.insert(member);
    }

    for (auto char_node_ptr_pair : root->children) {
        Node* child = char_node_ptr_pair.second;
        update_later_final(child, member);
    }
}

void add_to_prefix_tree(string &prefix, uint32_t member) {
    Node *current = tree;
    set<uint32_t> final_along_way;
    final_along_way.insert(member);
    for (size_t idx = 0; idx < prefix.size(); ++idx) {
        Node *next = current->children[prefix[idx]];
        if (next == nullptr) {
            next = new Node();
            current->children[prefix[idx]] = next;
        }
        if (next->committee_members.find(member) == next->committee_members.end()) {
            next->committee_members.insert(member);
            next->committee_count++;
        }
        for (uint32_t memb : next->final_to_here) {
            final_along_way.insert(memb);
        }
        current = next;
    }
    for (uint32_t memb: final_along_way) {
        current->final_to_here.insert(memb);
    }
    current->final = true;
    update_later_final(current, member);
}


uint64_t get_prefix_counts_of_length_k(Node *root, uint64_t cur_depth, uint64_t k, uint64_t alphabet_size, uint64_t committee_approval_count, uint64_t needed_count) {
    size_t final_size_to_here = root->final_to_here.size();
    if (final_size_to_here >= committee_approval_count) {
        uint64_t cnt = 1;
        int64_t remaining = k - cur_depth;
        if (remaining > 0) {
            for (uint64_t i = 0; i < remaining; ++i) {
                cnt = (cnt*alphabet_size)%100000;
            }
        }
        return cnt;
    } else {
        uint64_t cnt = 0;
        for (auto char_node_ptr_pair : root->children) {
            Node* child = char_node_ptr_pair.second;
//            cnt = (cnt + get_prefix_counts_of_length_k(child, cur_depth+1, k, alphabet_size, committee_approval_count, 7))%100000;
            if (!root->final || child->committee_count >= committee_approval_count - final_size_to_here) {
                cnt = (cnt + get_prefix_counts_of_length_k(child, cur_depth+1, k, alphabet_size, committee_approval_count, 7))%100000;
            }
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
                add_to_prefix_tree(cur_string, committee_member);
//                if (cur_count == min_approve_count - 1) {
//
//                }
                cur_count++;
            }
        }
    }
    cout << get_prefix_counts_of_length_k(tree, 0, prefix_length, alphabet.size(), min_approve_count, min_approve_count) << endl;

    return 0;
}