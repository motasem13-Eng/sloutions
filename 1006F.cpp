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

// 15-level Nibble Trie (60-bit keys up to 1.15e18 >= 10^18)
struct Nibble_Trie {
  void *root[16] = {nullptr};

  void clear_node(void **curr, int depth) {
    for (int i = 0; i < 16; i++) {
      if (!curr[i])
        continue;
      if (depth == 13) {
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

  int &get_ref(unsigned long long x) {
    void **curr = root;
    for (int shift = 56; shift >= 8; shift -= 4) {
      unsigned int idx = (x >> shift) & 15;
      if (!curr[idx])
        curr[idx] = new void *[16]();
      curr = static_cast<void **>(curr[idx]);
    }
    unsigned int d14 = (x >> 4) & 15;
    if (!curr[d14])
      curr[d14] = new int[16]();
    return static_cast<int *>(curr[d14])[x & 15];
  }

  int &operator[](unsigned long long x) { return get_ref(x); }

  int query(unsigned long long x) const {
    void *const *curr = root;
    for (int shift = 56; shift >= 8; shift -= 4) {
      unsigned int idx = (x >> shift) & 15;
      if (!curr[idx])
        return 0;
      curr = static_cast<void *const *>(curr[idx]);
    }
    unsigned int d14 = (x >> 4) & 15;
    if (!curr[d14])
      return 0;
    return static_cast<const int *>(curr[d14])[x & 15];
  }
};

int n, m, mid_steps;
unsigned long long target_xor, ans = 0;
unsigned long long grid[20][20];
Nibble_Trie diag_freq[20];

void dfs_forward(int r, int c, int steps, unsigned long long cur_xor) {
  if (steps == mid_steps) {
    diag_freq[r][cur_xor]++;
    return;
  }
  if (r + 1 < n)
    dfs_forward(r + 1, c, steps + 1, cur_xor ^ grid[r + 1][c]);
  if (c + 1 < m)
    dfs_forward(r, c + 1, steps + 1, cur_xor ^ grid[r][c + 1]);
}

void dfs_backward(int r, int c, int steps, unsigned long long cur_xor) {
  if (steps == (n + m - 2) - mid_steps) {
    ans += diag_freq[r].query(cur_xor ^ target_xor);
    return;
  }
  if (r - 1 >= 0)
    dfs_backward(r - 1, c, steps + 1, cur_xor ^ grid[r][c]);
  if (c - 1 >= 0)
    dfs_backward(r, c - 1, steps + 1, cur_xor ^ grid[r][c]);
}

void solve() {
  cin >> n >> m >> target_xor;
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
      cin >> grid[i][j];
    }
  }

  // Asymmetric split: fewer forward insertions to keep memory under 128 MB
  mid_steps = max(0, (n + m - 2) / 2 - 3);
  dfs_forward(0, 0, 0, grid[0][0]);
  dfs_backward(n - 1, m - 1, 0, 0ULL);

  cout << ans;
}

signed main() {

  ios_base::sync_with_stdio(0);
  cin.tie(nullptr);
#ifndef ONLINE_JUDGE
  freopen("input.txt", "r", stdin);
  freopen("output.txt", "w", stdout);
#endif
  int t = 1;
  // cin >> t;
  while (t--) {
    solve();
    cout << el;
  }
  return 0;
}