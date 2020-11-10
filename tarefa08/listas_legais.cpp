#include <bits/stdc++.h>

using namespace std;

int main() {
    int n, k;
    cin >> n >> k;

    unordered_map<size_t, size_t> t;
    t.reserve(n);
    for (int i = 0; i < n; i++) {
        size_t m;
        cin >> m;

        auto it = t.find(m);
        if (it == t.end())
            t.emplace(m, 1);
        else
            t[m]++;
    }

    for (int i = 0; i < k; i++) {
        int op;
        cin >> op;

        if (op == 1) {
            size_t j;
            cin >> j;

            auto it = t.find(j);
            if (it == t.end())
                t.emplace(j, 1);
            else
                t[j]++;

        } else if (op == 2) {
            size_t m;
            cin >> m;

            auto it = t.find(m);
            if (it == t.end())
                cout << 0 << endl;
            else
                cout << it->second << endl;

        } else {
            size_t diff = 0;
            for (auto p : t) {
                if (p.second > p.first)
                    diff += p.second - p.first;
                else if (p.second < p.first)
                    diff += p.second;
            }
            cout << diff << endl;
        }
    }

    return 0;
}