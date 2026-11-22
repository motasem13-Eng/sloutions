//#pragma GCC optimize("O3,unroll-loops")
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

// 13-level Nibble Trie (52-bit keys up to 4.5e15, 4 bits per level)
struct Nibble_Trie {
  void *root[16] = {nullptr};

  void clear_node(void **curr, int depth) {
    for (int i = 0; i < 16; i++) {
      if (!curr[i])
        continue;
      if (depth == 11) {
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

  inline int &get_ref(unsigned long long x) {
    void **curr = root;
    for (int shift = 48; shift >= 8; shift -= 4) {
      unsigned int idx = (x >> shift) & 15;
      if (!curr[idx])
        curr[idx] = new void *[16]();
      curr = static_cast<void **>(curr[idx]);
    }
    unsigned int d12 = (x >> 4) & 15;
    if (!curr[d12])
      curr[d12] = new int[16]();
    return static_cast<int *>(curr[d12])[x & 15];
  }

  inline int &operator[](unsigned long long x) { return get_ref(x); }

  inline int query(unsigned long long x) const {
    void *const *curr = root;
    for (int shift = 48; shift >= 8; shift -= 4) {
      unsigned int idx = (x >> shift) & 15;
      if (!curr[idx])
        return 0;
      curr = static_cast<void *const *>(curr[idx]);
    }
    unsigned int d12 = (x >> 4) & 15;
    if (!curr[d12])
      return 0;
    return static_cast<const int *>(curr[d12])[x & 15];
  }
};

// Hilbert curve order to minimize Mo's pointer movements
ll hilbert_order(int x, int y, int pow, int rotate) {
  if (pow == 0)
    return 0;
  int hpow = 1 << (pow - 1);
  int seg = (x < hpow) ? ((y < hpow) ? 0 : 3) : ((y < hpow) ? 1 : 2);
  seg = (seg + rotate) & 3;
  const int rotate_delta[4] = {3, 0, 0, 1};
  int nx = x & (x ^ hpow), ny = y & (y ^ hpow);
  int nrot = (rotate + rotate_delta[seg]) & 3;
  ll sub_square_size = 1LL << (2 * pow - 2);
  ll ans = seg * sub_square_size;
  ll add = hilbert_order(nx, ny, pow - 1, nrot);
  ans += (seg == 1 || seg == 2) ? add : (sub_square_size - add - 1);
  return ans;
}

struct Query {
  int l, r, id;
  ll order;
  bool operator<(const Query &other) const { return order < other.order; }
};

void solve() {
  int n;
  ll k;
  cin >> n >> k;
  vec<int> type(n + 1);
  for (int i = 1; i <= n; i++)
    cin >> type[i];

  vec<ll> prefix_sum(n + 1, 0);
  for (int i = 1; i <= n; i++) {
    ll val;
    cin >> val;
    prefix_sum[i] = prefix_sum[i - 1] + (type[i] == 1 ? val : -val);
  }

  int q;
  cin >> q;
  vec<Query> queries(q);
  for (int i = 0; i < q; i++) {
    int l, r;
    cin >> l >> r;
    queries[i] = {l - 1, r, i, hilbert_order(l - 1, r, 17, 0)};
  }
  sort(all(queries));

  // Offset by 3e14 to map signed prefix sums into [1e14, 5e14] < 2^52
  const unsigned long long OFFSET = 300000000000000ULL;
  Nibble_Trie freq;
  ll cur_ans = 0;

  auto add_right = [&](int idx) {
    cur_ans +=
        freq.query(static_cast<unsigned long long>(prefix_sum[idx] - k) + OFFSET);
    freq[static_cast<unsigned long long>(prefix_sum[idx]) + OFFSET]++;
  };

  auto add_left = [&](int idx) {
    cur_ans +=
        freq.query(static_cast<unsigned long long>(prefix_sum[idx] + k) + OFFSET);
    freq[static_cast<unsigned long long>(prefix_sum[idx]) + OFFSET]++;
  };

  auto remove_right = [&](int idx) {
    freq[static_cast<unsigned long long>(prefix_sum[idx]) + OFFSET]--;
    cur_ans -=
        freq.query(static_cast<unsigned long long>(prefix_sum[idx] - k) + OFFSET);
  };

  auto remove_left = [&](int idx) {
    freq[static_cast<unsigned long long>(prefix_sum[idx]) + OFFSET]--;
    cur_ans -=
        freq.query(static_cast<unsigned long long>(prefix_sum[idx] + k) + OFFSET);
  };

  vec<ll> answers(q);
  int cur_l = 0, cur_r = -1;

  for (const auto &qry : queries) {
    while (cur_l > qry.l)
      add_left(--cur_l);
    while (cur_r < qry.r)
      add_right(++cur_r);
    while (cur_l < qry.l)
      remove_left(cur_l++);
    while (cur_r > qry.r)
      remove_right(cur_r--);
    answers[qry.id] = cur_ans;
  }

  for (int i = 0; i < q; i++) {
    cout << answers[i] << (i + 1 == q ? "" : "\n");
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
 
}