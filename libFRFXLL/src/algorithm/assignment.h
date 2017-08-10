/*
    FingerJetFX OSE -- Fingerprint Feature Extractor, Open Source Edition

    Copyright (c) 2011 by DigitalPersona, Inc. All rights reserved.

    DigitalPersona, FingerJet, and FingerJetFX are registered trademarks 
    or trademarks of DigitalPersona, Inc. in the United States and other
    countries.

    FingerJetFX OSE is open source software that you may modify and/or
    redistribute under the terms of the GNU Lesser General Public License
    as published by the Free Software Foundation, either version 3 of the 
    License, or (at your option) any later version, provided that the 
    conditions specified in the COPYRIGHT.txt file provided with this 
    software are met.
 
    For more information, please visit digitalpersona.com/fingerjetfx.
*/ 
/*
      LIBRARY: FRFXLL - Fingerprint Feature Extractor - Low Level API

      ALGORITHM:      Alexander Ivanisov
                      Yi Chen
                      Salil Prabhakar
      IMPLEMENTATION: Alexander Ivanisov
                      Jacob Kaminsky
                      Lixin Wei
      DATE:           11/08/2011
*/

#ifndef __assignment_h
#define __assignment_h

/// Solve linear assignment maximization problem
/// http://en.wikipedia.org/wiki/Assignment_problem

#include <algorithm>
#include <integer_limits.h>
#include <intmath.h>
#include <bitset>

namespace FingerJetFxOSE {
  /// Partial solution to linear assignment problem on sparse boolean data
  /// Partial in a sense that it addresses only 1-1 and 1-many cases, but not (all) many-to-many
  template <class pair_t, uint8 n>
  struct assignment_1_to_1 {
    //struct pair_t {
    //  pair_t * next;
    //  uint8 left;
    //  uint8 right;
    //};
    uint8 cnt_left[n];
    uint8 cnt_right[n];
    pair_t * first;            // initial list of pairs
    pair_t * corr;             // list of correspondencies
    uint8 count;               // number of correspondances

    assignment_1_to_1() : first(NULL), corr(NULL), count(0) {
      std::fill_n(cnt_left,  n, 0);
      std::fill_n(cnt_right, n, 0);
    }
  private:
    void insert(pair_t & pair, pair_t ** ppcur) {
      pair.next = *ppcur;
      *ppcur = &pair;
    }
    pair_t & remove(pair_t ** ppcur) {
      pair_t * pair = *ppcur;
      *ppcur = pair->next;
      return *pair;
    }
  public:
    void add(pair_t & pair) {
      cnt_left [pair.left ]++;
      cnt_right[pair.right]++;
      insert(pair, &first);
    }
    pair_t * collect() {
      for (pair_t ** ppcur = &first; *ppcur; ) {
        pair_t & cur = **ppcur;
        if (cnt_left[cur.left] == 1 && cnt_right[cur.right] == 1) {
          insert(remove(ppcur), &corr);
          count++;
          continue;
        }
        ppcur = &((*ppcur)->next);
      }
      return corr;
    }
  };

  /// Find the number of linear assignments on sparse boolean data without extablishing correspondences
  template <class pair_t, uint8 n>
  struct assignment_counter {
    //struct pair_t {
    //  pair_t * next_left;
    //  pair_t * next_right;
    //  pair_t * next;
    //  pair_t * prev;
    //  uint8 left;
    //  uint8 right;
    //};

    pair_t * first_left[n];
    pair_t * first_right[n];
    pair_t * first;             // initial first of pairs
    //pair_t * corr;             // first of correspondencies
    //uint8 left_counts[n];
    //uint8 right_counts[n];
    assignment_counter() : first(NULL) {
      std::fill_n(first_left,  n, reinterpret_cast<pair_t*>(NULL));
      std::fill_n(first_right, n, reinterpret_cast<pair_t*>(NULL));
    }
  private:
    void insert(pair_t & pair, pair_t ** ppcur, pair_t * pair_t::* next) {
      pair.*next = *ppcur;
      *ppcur = &pair;
    }
    //void insert_sorted(pair_t & pair, pair_t ** ppcur, pair_t * pair_t::* next) {
    //  for (; *ppcur && pair.cost < (*ppcur)->cost; ppcur = &((*ppcur)->*next)) {}
    //  insert(pair, ppcur, next);
    //}
    void remove(pair_t & pair, pair_t ** ppcur, pair_t * pair_t::* next) {
      for (; *ppcur; ppcur = &((*ppcur)->*next)) {
        if (*ppcur == &pair) {
          *ppcur = pair.*next;
          return;
        }
      }
    }
    // remove one pair
    void remove(pair_t & pair) {
      if (pair.next) {
        pair.next->prev = pair.prev;
      }
      if (pair.prev) {
        pair.prev->next = pair.next;
      } else {
        first = pair.next;
      }
      remove(pair, &first_left[pair.left],   &pair_t::next_left );
      remove(pair, &first_right[pair.right], &pair_t::next_right);
    }
    // remove row or column
    void remove_left(uint8 ind) {
      for (pair_t * pcur = first_left[ind]; pcur; pcur = pcur->next_left) {
        remove(*pcur);
      }
    }
    void remove_right(uint8 ind) {
      for (pair_t * pcur = first_right[ind]; pcur; pcur = pcur->next_right) {
        remove(*pcur);
      }
    }
  public:
    void add(pair_t & pair) {
      insert(pair, &first, &pair_t::next);
      pair.prev = NULL;
      if (pair.next) {
        pair.next->prev = &pair;
      }
      insert(pair, &first_left[pair.left],   &pair_t::next_left );
      insert(pair, &first_right[pair.right], &pair_t::next_right);
    }
    uint8 count() {
      uint8 cnt = 0;
      while (true) {
        uint8 c0;
        do {
          c0 = cnt;
          for (pair_t * pcur = first; pcur; pcur = pcur->next) {
            if (pcur->next_left == NULL && first_left[pcur->left] == pcur) {
              remove_right(pcur->right);
              cnt++;
            } else if (pcur->next_right == NULL && first_right[pcur->right] == pcur) {
              remove_left(pcur->left);
              cnt++;
            }
          }
        } while (c0 < cnt);
        if (first == NULL) break;
        pair_t * cur = first;
        remove_right(cur->right);
        remove_left (cur->left);
        cnt++;
      }
      return cnt;
    }
  };

  template <
    class weight_t,
    class index_t,
    class weight_f,
    index_t M,
    index_t N,
    weight_t  WEIGHT_INF = integer_limits<weight_t>::max,
    index_t INDEX_UNASSIGNED = (index_t)(-1),
    index_t INDEX_EOL = (index_t)(-2)
  >
  struct hungarian_algorithm {
    const static index_t MAX_MN = M > N ? M : N;

    STATIC_ASSERT(INDEX_EOL < 0 || N < INDEX_EOL);
    STATIC_ASSERT(INDEX_UNASSIGNED < 0 || N < INDEX_UNASSIGNED);

    const weight_f & weight;
    index_t m, n;
    weight_t lx[M];  //xy[x] - vertex that is matched with x,
    weight_t ly[N];  //yx[y] - vertex that is matched with y

    hungarian_algorithm(const weight_f & weight_)
      : weight(weight_)
      , m(weight_.m())
      , n(weight_.n())
    {
    }

    weight_t operator () (index_t (&xy)[M], index_t (&yx)[N]) {
      std::fill_n(xy, m, INDEX_UNASSIGNED); // (size_t) for case when index_t is int64
      std::fill_n(yx, n, INDEX_UNASSIGNED);
      if (n == 0 || m == 0) { //nothing more to do, really
        return 0;
      }
      init_labels();
      if (!hungarian_main(xy, yx)) {
        return WEIGHT_INF;
      }
      weight_t ret = 0;                       // weight of the optimal matching
      for (index_t x = 0; x < m; x++) {       // forming answer there
        if (xy[x] >= n) {
          xy[x] = INDEX_UNASSIGNED;//-xy[x];
        }
        if (xy[x] != INDEX_UNASSIGNED) {
          ret += weight(x, xy[x]);
        }
      }
      return ret;
    }

    // main function of the algorithm
    bool hungarian_main(index_t (&xy)[M], index_t (&yx)[N]) {
      index_t max_m_n = m; // max(m, n);// min(m, n);
      for (index_t match_level = 0; match_level < max_m_n; match_level++) { // number of vertices in current matching
        index_t n_m = max(n, match_level + 1);
        index_t root;                             // root vertex
        index_t prev[M];                          // array for memorizing alternating paths
        std::fill_n(prev, m, INDEX_UNASSIGNED);   // init set prev - for the alternating tree

        root = find_root(xy);

        prev[root] = INDEX_EOL;
        index_t x, y;                             // last found new matching
        if (!hungarian_main_loop(xy, yx, prev, root, n_m, x, y)) {
          return false;
        }

        // in this cycle we inverse edges along augmenting path
        //index_t x1 = x, y1 = y;
        for (; x != INDEX_EOL; x = prev[x]) {
          if (y < n) {
            yx[y] = x;
          }
          std::swap(xy[x], y);
        }

      }
      return true;
    }

    void init_labels() {
      std::fill_n(ly, n, 0);
      for (index_t x = 0; x < m; x++) {
        //lx[x] = *std::max_element(weight.weight[x], weight.weight[x]+n);
        lx[x] = integer_limits<weight_t>::min;
        for (index_t y = 0; y < n; y++) {
          lx[x] = std::max(lx[x], weight(x, y));
        }
      }
    }

    index_t find_root(index_t (&xy)[M]) {
      index_t x;
      // finding root of the tree - first unassigned
      for (x = 0; x < m && xy[x] != INDEX_UNASSIGNED; x++) ;

      if (x >= m) {
        return INDEX_UNASSIGNED;
      }

      // finding root of the tree - be a little greedy - looking for max unassigned
      index_t root = x;
      for (weight_t maxlx = lx[x++]; x < m; x++) {
        if (xy[x] != INDEX_UNASSIGNED)  continue;
        if (maxlx >= lx[x])             continue;
        maxlx = lx[x];
        root = x;
      }
      return root;
    }

    bool update_labels(bool (&S)[M], bool (&T)[MAX_MN], weight_t (&slack)[MAX_MN], index_t n_m, index_t (&new_slack_zero)[MAX_MN+2]) {
      index_t x, y;
      weight_t delta = WEIGHT_INF;                    // init delta as infinity
      for (y = 0; y < n_m; y++) {                     // calculate delta using slack
        if (!T[y]) {
          delta = std::min(delta, slack[y]);
        }
      }
      if (delta == WEIGHT_INF || delta == 0) {        // should never be true
        return false;
      }
      for (x = 0; x < m; x++) {                       // update X labels
        if (S[x]) {
          lx[x] -= delta;
        }
      }
      for (y = 0; y < n; y++) {                       // update Y labels
        if (T[y]) {
          ly[y] += delta;
        }
      }
      index_t i = 0, n_non_zero = 0;
      for (y = n_m; y-- != 0; ) {                     // update slack array and fill new_slack_zero in reverse order
        if (!T[y]) {
          slack[y] -= delta;
          if (slack[y] == 0) {
            new_slack_zero[i++] = y;                  // in reverse order
          }
        }
        if (slack[y] != 0) {
          n_non_zero++;
        }
      }
      new_slack_zero[i] = INDEX_UNASSIGNED;
      new_slack_zero[MAX_MN+1] = n_non_zero;
      return true;
    }

    /// x - current vertex
    bool update_slacks(index_t x, weight_t (&slack)[MAX_MN], index_t (&slackx)[MAX_MN], index_t n_m) {
      bool bNewZero = false;
      for (index_t y = 0; y < n; y++) {               // update slacks, because we add new vertex to S
        weight_t new_slack = lx[x] + ly[y] - weight(x, y);
        if (slack[y] > new_slack) {
          slack[y] = new_slack;
          slackx[y] = x;
          if (slack[y] == 0) {
            bNewZero = true;
          }
        }
      }
      for (index_t y = n; y < n_m; y++) {             // update slacks, because we add new vertex to S
        if (slack[y] > lx[x]) { // ly_tail[y] == 0 weight(*, y) == 0
          slack[y] = lx[x];
          slackx[y] = x;
          if (slack[y] == 0) {
            bNewZero = true;
          }
        }
      }
      return bNewZero;
    }

    // main loop of the algorithm
    bool hungarian_main_loop(index_t (&xy)[M], index_t (&yx)[N], index_t (&prev)[M], index_t root, index_t n_m, index_t& x_ext, index_t& y_ext) {
      struct saver {
        index_t &ext, &loc;
        saver(index_t& _ext, index_t& _loc) : ext(_ext), loc(_loc) { }
        ~saver() {ext = loc;}
      };
//    index_t prev[M];            // array for memorizing alternating paths
//    index_t root;               // root vertex
//    index_t x_ext, y_ext;       // last found new matching
      index_t x, y;               // local counters
      saver svx(x_ext, x);        // we save references to x_ext and x in constuctor and
      saver svy(y_ext, y);        // assing x to x_ext in destructor by references (same for y_ext and y)
      index_t q[M];               // q - queue for bfs, // ??? [M] ??? [N]
      int wr = 0, rd = 0;         // wr,rd - write and read pos in queue
      bool S[M], T[MAX_MN];       // sets S and T in algorithm
      weight_t slack[MAX_MN];     // as in the algorithm description
      index_t slackx[MAX_MN];     // slackx[y] such a vertex, that lx[slackx[y]] + ly[y] - weight[slackx[y],y] = slack[y]

      std::fill_n(S, m, false);   // init set S
      std::fill_n(T, n, false);   // init set T
      S[root] = true;

      for (y = 0; y < n; y++) {   // initializing slack array
        slack[y] = lx[root] + ly[y] - weight(root, y);
        slackx[y] = root;
      }
      std::fill_n(slack + n, n_m -n, lx[root]); // m
      std::fill_n(slackx + n, n_m -n, root);
      std::fill_n(T + n, n_m -n, false);

      q[wr++] = root;
      index_t n_non_zero;
      do {                                                    // main cycle
        do {                                                  // building tree with bfs cycle
          x = q[rd++];                                        // current vertex from X part

          if (0 == lx[x]) {                                   // weight(x, y)==0 ly_tail[y]==0 ; T[y] == true here
            if (n < n_m) {
              y = n_m-1;                                      // exposed vertex in Y found - augmenting path exists!
              return true;                                    // augmenting path exists!
            }
          }

          for (y = 0; y < n; y++) {                           // iterate through all edges in equality graph
            // if (!T[y] && weight(x, y) == lx[x] + ly[y]) {  // This is original condition of equality graph
            // It is true that (weight(x, y) == lx[x] + ly[y]) == (slack[y] == 0 && slackx[y] == x)
            if (!T[y] && slack[y] == 0 && slackx[y] == x) {   // This condition is equivalent to the original one
              if (yx[y] == INDEX_UNASSIGNED) {                // an exposed vertex in Y found, so
                return true;                                  // augmenting path exists!
              }
              T[y] = true;                                    // else just add y to T,
              index_t x1 = yx[y];
              S[x1] = true;                                   // add edges (x,y) and (y,yx[y]) to the tree
              prev[x1] = x;                                   // we need this when augmenting
              if (update_slacks(x1, slack, slackx, n_m)) {
                q[wr++] = x1;                                 // add vertex yx[y], which is matched with y, to the queue
              }
            }
          }
        } while (rd < wr);

        wr = rd = 0;
        do {
          index_t new_slack_zero[MAX_MN+2] = {INDEX_UNASSIGNED}; // in reverse order, because for y > n the target item is the last
          // augmenting path not found, so improve labeling
          if (!update_labels(S, T, slack, n_m, new_slack_zero)) {
            return false;                                     // never should happen
          }
          n_non_zero = new_slack_zero[MAX_MN+1];
          // in this cycle we add edges that were added to the equality graph as a
          // result of improving the labeling, we add edge (slackx[y], y) to the tree if
          // and only if !T[y] &&  slack[y] == 0, also with this edge we add another one
          // (y, yx[y]) or augment the matching, if y was exposed
          //for (y = 0; y < n; y++) {
            //if (!T[y] && slack[y] == 0) {
          y = new_slack_zero[0];
          if (y >= n) {
            x = slackx[y];                                    // exposed vertex in Y found - augmenting path exists!
            return true;
          }
          //for (y = 0; y < n; y++) {
          // if (!T[y] && slack[y] == 0) {
          index_t i;
          for (i = 0 ; y != INDEX_UNASSIGNED; y = new_slack_zero[++i]) {
            if (yx[y] == INDEX_UNASSIGNED) {                  // exposed vertex in Y found - augmenting path exists!
              x = slackx[y];
              return true;
            } else {
              T[y] = true;                                    // else just add y to T,
              index_t x1 = yx[y];
              if (!S[x1]) {
                S[x1] = true;                                 // and add edges (slackx[y],y) and (y,yx[y]) to the tree
                prev[x1] = slackx[y];                         // we need this when augmenting
                if (update_slacks(x1, slack, slackx, n_m)) {
                  q[wr++] = x1;                               // add vertex yx[y], which is matched with y, to the queue
                }
              }
            }
          }
        } while (wr == 0 && n_non_zero != 0);
      } while (n_non_zero != 0);

      // we did not find augmenting path ????
      return false;                                           // never should happen
    }
  };

  template <class weight_t, class index_t, index_t M, index_t N>
  struct weight_f {
    const weight_t (&weight)[M][N];
    const index_t _m, _n;
#ifdef TRANSPOSE_TALL_MATRIX
    const bool transpose;
#endif
    weight_f(const weight_t (&weight_)[M][N], index_t m_, index_t n_, bool transpose_ = false) 
      : weight(weight_)
#ifdef TRANSPOSE_TALL_MATRIX
      , _m(transpose_ ? n_ : m_)
      , _n(transpose_ ? m_ : n_)
      , transpose(transpose_)
#else
      , _m(m_)
      , _n(n_)
#endif // TRANSPOSE_TALL_MATRIX
    {
    }

    index_t m() const { return _m; }
    index_t n() const { return _n; }
    weight_t operator () (index_t x, index_t y) const {
#ifdef TRANSPOSE_TALL_MATRIX
      return transpose ? weight[y][x] : weight[x][y];
#endif // TRANSPOSE_TALL_MATRIX
      return weight[x][y];
    }
  };

  template <class weight_t, class index_t, size_t M, size_t N>
  inline weight_t linear_assignment(const weight_t (&weight_)[M][N], size_t m, size_t n, index_t (&xy)[M], index_t (&yx)[N], bool allow_tr = false) {
    typedef weight_f<weight_t, index_t, M, N> weight_f_t;
#ifdef TRANSPOSE_TALL_MATRIX
    bool transpose = allow_tr ? m > n : false;
#else
    const static bool transpose = false;
#endif // TRANSPOSE_TALL_MATRIX
    weight_f_t weight(weight_, index_t(m), index_t(n), transpose);
    weight_t ret;
#ifdef TRANSPOSE_TALL_MATRIX
    if (transpose) {
      hungarian_algorithm<weight_t, index_t, weight_f_t, N, M> ha(weight);
      ret = ha(yx, xy);
    } else
#endif // TRANSPOSE_TALL_MATRIX
    {
      hungarian_algorithm<weight_t, index_t, weight_f_t, M, N> ha(weight);
      ret = ha(xy, yx);
    }
    return ret;
  }

  // Workaround: ARM compiler has a hard time with reference to array of const
  template <class weight_t, class index_t, size_t M, size_t N>
  inline weight_t linear_assignment(weight_t (&weight_)[M][N], size_t m, size_t n, index_t (&xy)[M], index_t (&yx)[N], bool allow_tr = true) {
    typedef weight_f<weight_t, index_t, M, N> weight_f_t;
#ifdef TRANSPOSE_TALL_MATRIX
    bool transpose = allow_tr ? m > n : false;
#else
    const static bool transpose = false;
#endif // TRANSPOSE_TALL_MATRIX
    weight_f_t weight(weight_, index_t(m), index_t(n), transpose);
    weight_t ret;
#ifdef TRANSPOSE_TALL_MATRIX
    if (transpose) {
      hungarian_algorithm<weight_t, index_t, weight_f_t, N, M> ha(weight);
      ret = ha(yx, xy);
    } else
#endif // TRANSPOSE_TALL_MATRIX
    {
      hungarian_algorithm<weight_t, index_t, weight_f_t, M, N> ha(weight);
      ret = ha(xy, yx);
    }
    return ret;
  }
};

#endif // __assignment_h
