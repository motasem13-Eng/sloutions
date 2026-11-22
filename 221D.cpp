#include <algorithm>
#include <bits/stdc++.h>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
#include <numeric>

using namespace std;
using namespace __gnu_pbds;

// #define int long long
#define ll long long
#define vec vector
#define el '\n'
#define all(v) v.begin(), v.end()
#define rall(v) v.rbegin(), v.rend()
const ll INF = 1e18;

struct Nibble_Trie {
  void *root[16] = {nullptr};

  void clear_node(void **curr, int depth) {
    for (int i = 0; i < 16; i++) {
      if (!curr[i])
        continue;
      if (depth == 6) {
        delete[] static_cast<int *>(curr[i]);
      } else {
        clear_node(static_cast<void **>(curr[i]), depth + 1);
        delete[] static_cast<void **>(curr[i]);
      }
      curr[i] = nullptr;
    }
  }

  void clear() { clear_node(root, 0); }
  ~Nibble_Trie() { clear(); }

  int &get_ref(unsigned int x) {
    void **curr = root;
    for (int shift = 28; shift >= 8; shift -= 4) {
      unsigned int idx = (x >> shift) & 15;
      if (!curr[idx])
        curr[idx] = new void *[16]();
      curr = static_cast<void **>(curr[idx]);
    }
    unsigned int d7 = (x >> 4) & 15;
    if (!curr[d7])
      curr[d7] = new int[16]();
    return static_cast<int *>(curr[d7])[x & 15];
  }

  int &operator[](unsigned int x) { return get_ref(x); }

  int query(unsigned int x) const {
    void *const *curr = root;
    for (int shift = 28; shift >= 8; shift -= 4) {
      unsigned int idx = (x >> shift) & 15;
      if (!curr[idx])
        return 0;
      curr = static_cast<void *const *>(curr[idx]);
    }
    unsigned int d7 = (x >> 4) & 15;
    if (!curr[d7])
      return 0;
    return static_cast<const int *>(curr[d7])[x & 15];
  }
};

struct Query {
  int l, r, id, block;
  bool operator<(const Query &other) const {
    if (block != other.block)
      return block < other.block;
    return (block & 1) ? (r > other.r) : (r < other.r);
  }
};

void solve() {
  int n, m;
  cin >> n >> m;
  vec<unsigned int> a(n + 1);
  for (int i = 1; i <= n; i++)
    cin >> a[i];

  int block_sz = max(1, static_cast<int>(sqrt(n)));
  vec<Query> queries(m);
  for (int i = 0; i < m; i++) {
    int l, r;
    cin >> l >> r;
    queries[i] = {l, r, i, l / block_sz};
  }
  sort(all(queries));

  Nibble_Trie freq;
  int good_count = 0;

  auto add_val = [&](unsigned int x) {
    int &f = freq[x];
    if (static_cast<unsigned int>(f) == x)
      good_count--;
    f++;
    if (static_cast<unsigned int>(f) == x)
      good_count++;
  };

  auto remove_val = [&](unsigned int x) {
    int &f = freq[x];
    if (static_cast<unsigned int>(f) == x)
      good_count--;
    f--;
    if (static_cast<unsigned int>(f) == x)
      good_count++;
  };

  vec<int> answers(m);
  int cur_l = 1, cur_r = 0;

  for (const auto &qry : queries) {
    while (cur_l > qry.l)
      add_val(a[--cur_l]);
    while (cur_r < qry.r)
      add_val(a[++cur_r]);
    while (cur_l < qry.l)
      remove_val(a[cur_l++]);
    while (cur_r > qry.r)
      remove_val(a[cur_r--]);
    answers[qry.id] = good_count;
  }

  for (int i = 0; i < m; i++) {
    cout << answers[i] << (i + 1 == m ? "" : "\n");
  }
}

signed main() {
#ifndef ONLINE_JUDGE
  freopen("input.txt", "r", stdin);
  freopen("output.txt", "w", stdout);
#endif
  ios_base::sync_with_stdio(0);
  cin.tie(nullptr);

  int t = 1;
  // cin >> t;
  while (t--) {
    solve();
    cout << el;
  }
  return 0;
}