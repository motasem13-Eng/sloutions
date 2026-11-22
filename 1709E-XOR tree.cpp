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

int n, ans = 0, timer = 0;
vec<vec<int>> adj;
vec<unsigned int> node_val, prefix_xor;
vec<int> subtree_size, heavy_child, tin, tout, euler_node;
vec<bool> modified;
Nibble_Trie xor_freq;

void dfs_prep(int u, int parent) {
  subtree_size[u] = 1;
  tin[u] = ++timer;
  euler_node[timer] = u;
  int max_sub = 0;

  for (int v : adj[u]) {
    if (v == parent)
      continue;
    prefix_xor[v] = prefix_xor[u] ^ node_val[v];
    dfs_prep(v, u);
    subtree_size[u] += subtree_size[v];
    if (subtree_size[v] > max_sub) {
      max_sub = subtree_size[v];
      heavy_child[u] = v;
    }
  }
  tout[u] = timer;
}

void dfs_dsu(int u, int parent, bool keep) {
  for (int v : adj[u]) {
    if (v != parent && v != heavy_child[u]) {
      dfs_dsu(v, u, false);
    }
  }
  if (heavy_child[u] != 0) {
    dfs_dsu(heavy_child[u], u, true);
  }

  // Check if any path from u into heavy child's subtree has XOR == 0
  bool need_change = (xor_freq.query(prefix_xor[u] ^ node_val[u]) > 0);
  if (!need_change) {
    xor_freq[prefix_xor[u]]++;
  }

  for (int v : adj[u]) {
    if (v == parent || v == heavy_child[u])
      continue;
    if (!need_change) {
      for (int t = tin[v]; t <= tout[v]; t++) {
        int w = euler_node[t];
        if (modified[w]) {
          t = tout[w];
          continue;
        }
        if (xor_freq.query(prefix_xor[w] ^ node_val[u]) > 0) {
          need_change = true;
          break;
        }
      }
    }
    if (!need_change) {
      for (int t = tin[v]; t <= tout[v]; t++) {
        int w = euler_node[t];
        if (modified[w]) {
          t = tout[w];
          continue;
        }
        xor_freq[prefix_xor[w]]++;
      }
    }
  }

  if (need_change) {
    ans++;
    modified[u] = true;
    xor_freq.clear();
  } else if (!keep) {
    xor_freq.clear();
  }
}

void solve() {
  cin >> n;
  adj.assign(n + 1, {});
  node_val.resize(n + 1);
  prefix_xor.resize(n + 1);
  subtree_size.assign(n + 1, 0);
  heavy_child.assign(n + 1, 0);
  tin.resize(n + 1);
  tout.resize(n + 1);
  euler_node.resize(n + 1);
  modified.assign(n + 1, false);

  for (int i = 1; i <= n; i++)
    cin >> node_val[i];
  for (int i = 0; i < n - 1; i++) {
    int u, v;
    cin >> u >> v;
    adj[u].push_back(v);
    adj[v].push_back(u);
  }

  prefix_xor[1] = node_val[1];
  dfs_prep(1, 0);
  dfs_dsu(1, 0, false);

  cout << ans;
}

signed main() {
#ifndef ONLINE_JUDGE
  freopen("input.in", "r", stdin);
  freopen("output.out", "w", stdout);
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