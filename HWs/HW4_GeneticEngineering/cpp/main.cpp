#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include <cstdint>

using namespace std;

constexpr int32_t UNREACHABLE = INT32_MAX - 1;

vector<int32_t> find_forward_costs(const string &sequence, const string &basic_unit, size_t sequence_start_idx, uint16_t max_deletions);

int32_t deletions_count_check(int32_t deletions, uint16_t max_deletions) {
    return deletions <= max_deletions ? deletions : UNREACHABLE;
}

vector<int32_t>
find_forward_costs(const string &sequence, const string &basic_unit, const size_t sequence_start_idx, const uint16_t max_deletions) {
    size_t costs_len = min(basic_unit.size()+1, sequence.size() - sequence_start_idx + 1);
    vector<int32_t> costs_cur(costs_len, UNREACHABLE);
    vector<int32_t> costs_next(costs_len, UNREACHABLE);

    costs_cur[0] = 0;

    for (size_t unit_idx = 0; unit_idx < basic_unit.size(); ++unit_idx) {
        fill(costs_next.begin(), costs_next.end(), UNREACHABLE);
        costs_next[0] = deletions_count_check(costs_cur[0] + 1, max_deletions);

        size_t max_idx = min(costs_len, unit_idx+2);

        for (size_t costs_offset = 1; costs_offset < max_idx; ++costs_offset) {

            if (basic_unit[unit_idx] == sequence[sequence_start_idx + costs_offset - 1]){
                if (costs_cur[costs_offset-1] != UNREACHABLE) {
                    costs_next[costs_offset] = costs_cur[costs_offset-1];
                } else {
                    costs_next[costs_offset] = UNREACHABLE;
                }
            }

            if (costs_cur[costs_offset] != UNREACHABLE) {
                costs_next[costs_offset] = min(costs_next[costs_offset], deletions_count_check(costs_cur[costs_offset] + 1, max_deletions));
            }
        }
        costs_cur.swap(costs_next);
    }

    return costs_cur;
}

int main() {
    string sequence{};
    vector<pair<uint16_t, string>> basic_units;
    uint16_t basic_unit_count{}, max_deletions{};
    cin >> sequence >> basic_unit_count >> max_deletions;
    for (uint16_t i = 0; i < basic_unit_count; i++) {
        uint16_t unit_cost{};
        string basic_unit{};
        cin >> unit_cost >> basic_unit;
        basic_units.emplace_back(unit_cost, basic_unit);
    }

    //contains additional index compared to sequence, for initial costs
    vector<pair<uint32_t, uint32_t>> costs(sequence.size()+1, {UINT32_MAX, UINT32_MAX});
    costs[0] = {0, 0};

    for (size_t start_idx = 0; start_idx < sequence.size(); ++start_idx) {
        uint32_t cost_to_here, prev_used_units;
        tie(cost_to_here, prev_used_units) = costs[start_idx];

        if (cost_to_here == UINT32_MAX) {
            //Can't start from this index
            continue;
        }

        for (pair<uint16_t, string> &basic_unit_pair : basic_units) {
            uint16_t basic_unit_cost = basic_unit_pair.first;
            string &basic_unit = basic_unit_pair.second;
            vector<int32_t> forward_costs = find_forward_costs(sequence, basic_unit, start_idx, max_deletions);

            for (size_t forward_offset = 1; forward_offset < forward_costs.size(); forward_offset++) {
                uint32_t &current_cost = costs[start_idx + forward_offset].first;
                uint32_t &current_used_units = costs[start_idx + forward_offset].second;

                if (forward_costs[forward_offset] != UNREACHABLE) {
                    uint32_t potential_cost = ((uint32_t) forward_costs[forward_offset]) + (uint32_t) basic_unit_cost + cost_to_here;
                    if (current_cost == potential_cost) {
                        current_used_units = min(current_used_units, prev_used_units + 1);
                    }

                    if (potential_cost < current_cost) {
                        current_cost = potential_cost;
                        current_used_units = prev_used_units + 1;
                    }

                }
            }

        }
    }

    uint32_t minimal_cost{}, minimal_unit_count{};
    tie(minimal_cost, minimal_unit_count) = costs.back();
    cout << minimal_cost << " " << minimal_unit_count << endl;
    return 0;
}