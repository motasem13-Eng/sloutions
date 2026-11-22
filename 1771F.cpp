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
        delete[] static_cast<unsigned long long *>(curr[i]);
      } else {
        clear_node(static_cast<void **>(curr[i]), depth + 1);
        delete[] static_cast<void **>(curr[i]);
      }
      curr[i] = nullptr;
    }
  }

  void clear() { clear_node(root, 0); }
  ~Nibble_Trie() { clear(); }

  unsigned long long &get_ref(unsigned int x) {
    void **curr = root;
    for (int shift = 28; shift >= 8; shift -= 4) {
      unsigned int idx = (x >> shift) & 15;
      if (!curr[idx])
        curr[idx] = new void *[16]();
      curr = static_cast<void **>(curr[idx]);
    }
    unsigned int d7 = (x >> 4) & 15;
    if (!curr[d7])
      curr[d7] = new unsigned long long[16]();
    return static_cast<unsigned long long *>(curr[d7])[x & 15];
  }

  unsigned long long &operator[](unsigned int x) { return get_ref(x); }

  unsigned long long query(unsigned int x) const {
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
    return static_cast<const unsigned long long *>(curr[d7])[x & 15];
  }
};

struct Node {
  int left_child, right_child;
  unsigned long long xor_sum;
};

vec<Node> seg;
Nibble_Trie hash_map;
mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());

int update_pst(int prev_node, int l, int r, int pos,
               unsigned long long val_hash) {
  int cur_node = static_cast<int>(seg.size());
  seg.push_back(seg[prev_node]);
  seg[cur_node].xor_sum ^= val_hash;

  if (l == r)
    return cur_node;

  int mid = l + (r - l) / 2;
  if (pos <= mid) {
    int nxt_left = update_pst(seg[prev_node].left_child, l, mid, pos, val_hash);
    seg[cur_node].left_child = nxt_left;
  } else {
    int nxt_right =
        update_pst(seg[prev_node].right_child, mid + 1, r, pos, val_hash);
    seg[cur_node].right_child = nxt_right;
  }
  return cur_node;
}

int query_pst(int node_l, int node_r, int l, int r) {
  if ((seg[node_r].xor_sum ^ seg[node_l].xor_sum) == 0)
    return 0;
  if (l == r)
    return l;

  int mid = l + (r - l) / 2;
  unsigned long long left_xor =
      seg[seg[node_r].left_child].xor_sum ^ seg[seg[node_l].left_child].xor_sum;

  if (left_xor != 0) {
    return query_pst(seg[node_l].left_child, seg[node_r].left_child, l, mid);
  }
  return query_pst(seg[node_l].right_child, seg[node_r].right_child, mid + 1,
                   r);
}

void solve() {
  int n;
  cin >> n;

  const int MAX_VAL = 1000000000;
  seg.clear();
  seg.reserve(n * 32 + 5);
  seg.push_back({0, 0, 0ULL}); // Version 0 (empty tree)

  vec<int> version_root(n + 1, 0);
  for (int i = 1; i <= n; i++) {
    unsigned int x;
    cin >> x;
    unsigned long long h = hash_map.query(x);
    if (h == 0) {
      while (h == 0)
        h = rng();
      hash_map[x] = h;
    }
    version_root[i] =
        update_pst(version_root[i - 1], 1, MAX_VAL, static_cast<int>(x), h);
  }

  int q;
  cin >> q;
  unsigned int last_ans = 0;

  for (int i = 0; i < q; i++) {
    unsigned int x, y;
    cin >> x >> y;
    int l = static_cast<int>(x ^ last_ans);
    int r = static_cast<int>(y ^ last_ans);
    last_ans = static_cast<unsigned int>(
        query_pst(version_root[l - 1], version_root[r], 1, MAX_VAL));
    cout << last_ans << (i + 1 == q ? "" : "\n");
  }
}

signed main() {
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