/* Copyright 2023-2024 Kinglet B.V.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#define TILLY_VERSION_STR "0.01"

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

#include <map>
#include <memory>
#include <vector>
#include <system_error>
#include <stdexcept>
#include <algorithm>

#include "tilly_parser.h"

struct automaton_node;

struct tile_node;

struct tile {
  char reduces_to_symbol_ = 0;
  std::unique_ptr<tile_node> root_;
};

struct tile_node {
  char symbol_ = 0;
  std::vector<std::unique_ptr<tile_node> > children_;

  template<typename Head, typename... Tail>
  void insert_child(Head &&head, Tail&&... tail) {
    children_.push_back(std::move(head));
    insert_child(std::forward<Tail>(tail)...);
  }
  void insert_child() {
    // base case.
  }
};

struct subject_node {
  char symbol_ = 0;

  std::vector<std::unique_ptr<subject_node> > children_;
  std::map<char, int> non_terminal_cost_;
  std::map<char, int> non_terminal_match_;

  subject_node *parent_ = nullptr;
  int operand_index_in_parent_ = 0;
  automaton_node *state_ = nullptr;
  uint64_t matches_[3] = {0};

  template<typename Head, typename... Tail>
  void insert_child(Head &&head, Tail&&... tail) {
    head->operand_index_in_parent_ = (int)children_.size();
    children_.push_back(std::move(head));
    children_.back()->parent_ = this;
    insert_child(std::forward<Tail>(tail)...);
  }
  void insert_child() {
    // base case.
  }
  void visit();
  void post_process();
  void do_reduce();
};

struct automaton_accept {
  bool operator==(const automaton_accept &rhs) const {
    return tile_id_ == rhs.tile_id_ && branch_depth_ == rhs.branch_depth_;
  }
  int tile_id_ = 0;
  int branch_depth_ = 0;
};

struct automaton_node {
  automaton_node *sibling_ = nullptr;
  automaton_node *parent_ = nullptr;
  automaton_node *egress_ = nullptr;

  /* Transition from parent to this node. */
  bool is_operand_ = false;
  char symbol_ = 0;
  int operand_ = 0;

  /* Details about this node */
  bool is_accepting_ = false;
  size_t failure_func_ = 0;

  std::vector<automaton_accept> accepting_ids_;
  
  automaton_node *follow_opd(int operand);
  automaton_node *follow_sym(char symbol);

  automaton_node *succ_opd(int operand);
  automaton_node *succ_sym(char symbol);

  bool edge_match(const automaton_node &other) const {
    if (is_operand_ != other.is_operand_) return false;
    if (is_operand_) {
      return operand_ == other.operand_;
    }
    else {
      return symbol_ == other.symbol_;
    }
  }
};

std::vector<std::unique_ptr<tile> > tiles_;
std::vector<std::unique_ptr<automaton_node> > automaton_nodes_;

// Create new tile_node, arguments are the children of the node and are stored in children_
template <typename... Args>
std::unique_ptr<tile_node> make_tile_node(char symbol, Args... args) {
  auto n = std::make_unique<tile_node>();
  n->symbol_ = symbol;
  n->insert_child(std::move(args)...);

  return n;
}

// Create new subject_node, arguments are the children of the node and are stored in children_
template <typename... Args>
std::unique_ptr<subject_node> make_subject(char symbol, Args... args) {
  auto n = std::make_unique<subject_node>();
  n->symbol_ = symbol;
  n->insert_child(std::move(args)...);

  return n;
}

automaton_node *automaton_node::follow_opd(int operand) {
  if (egress_) {
    auto *sib = egress_;
    do {
      sib = sib->sibling_;

      if (sib->is_operand_ && sib->operand_ == operand) {
        return sib;
      }

    } while (sib != egress_);
  }

  return nullptr;
}

automaton_node *automaton_node::follow_sym(char symbol) {
  if (egress_) {
    auto *sib = egress_;
    do {
      sib = sib->sibling_;

      if (!sib->is_operand_ && sib->symbol_ == symbol) {
        return sib;
      }

    } while (sib != egress_);
  }

  return nullptr;
}

automaton_node *automaton_node::succ_opd(int operand) {
  automaton_node *an = this;
  automaton_node *s = follow_opd(operand);
  while (!s) {
    if (!an->failure_func_) {
      // Cannot go via root as root always holds symbols, not operands.
      return nullptr;
    }
    else {
      an = automaton_nodes_[an->failure_func_].get();
    }
    s = an->follow_opd(operand);
  }
  return s;
}

automaton_node *automaton_node::succ_sym(char symbol) {
  automaton_node *an = this;
  automaton_node *s = follow_sym(symbol);
  while (!s) {
    if (!an->failure_func_) {
      an = automaton_nodes_[0].get();
      return an->follow_sym(symbol);
    }
    else {
      an = automaton_nodes_[an->failure_func_].get();
    }
    s = an->follow_sym(symbol);
  }
  return s;
}

// Implement visit() function for subject_node
void subject_node::visit() {
  if (!parent_) {
    // We're at the root.
    automaton_node *root = automaton_nodes_[0].get();
    state_ = root->succ_sym(symbol_);

  }
  else {
    state_ = parent_->state_->succ_opd(operand_index_in_parent_)->succ_sym(symbol_);
  }
  for (auto &x: children_) {
    x->visit();
  }
  post_process();
}

void subject_node::post_process() {
  bool first = true;
  for (auto &child: children_) {
    for (size_t n = 0; n < sizeof(matches_) / sizeof(*matches_); ++n) {
      if (first) {
        matches_[n] = child->matches_[n] >> 1;
      }
      else {
        matches_[n] &= child->matches_[n] >> 1;
      }
    }
    first = false;
  }
  for (auto &x: state_->accepting_ids_) {
    matches_[x.tile_id_] |= 1ULL << x.branch_depth_;
  }

  do_reduce();
}

void subject_node::do_reduce() {
  for (size_t n = 0; n < sizeof(matches_) / sizeof(*matches_); ++n) {
    if (matches_[n] & 1) {
      int computed_cost = INT_MAX;
      // Compute costs, note that, at this point, the tiles already
      // are known to match. We therefore don't need to check if paths or values
      // exist.
      switch (n) {
        case 0: {
          // r <- +(r, i(+(c, r)))
          computed_cost = 3 +
                          children_[0]->non_terminal_cost_['r'] + 
                          children_[1]->children_[0]->children_[1]->non_terminal_cost_['r'];
          break;
        }
        case 1: {
          // r <- +(r, r)
          computed_cost = 1 +
                          children_[0]->non_terminal_cost_['r'] + 
                          children_[1]->non_terminal_cost_['r'];
          break;
        }
        case 2: {
          // r <- c
          computed_cost = 1;
          break;
        }
      }
      char symbol_reduced_to = tiles_[n]->reduces_to_symbol_;
      int pre_existing_cost = non_terminal_cost_.count(symbol_reduced_to) ? non_terminal_cost_[symbol_reduced_to] : INT_MAX;
      if (pre_existing_cost > computed_cost) {
        non_terminal_cost_[symbol_reduced_to] = computed_cost;
        non_terminal_match_[symbol_reduced_to] = (int)n;

        // Find the new state we arrive at on transitioning from the parent state through the symbol_reduced_to
        // state. Check if it is accepting and, if so, make sure the appropriate bits are cascaded up.
        automaton_node *sigma = nullptr;
        if (!parent_ /* root */) {
          automaton_node *root = automaton_nodes_[0].get();
          sigma = root->succ_sym(symbol_reduced_to);
        }
        else {
          sigma = parent_->state_->succ_opd(operand_index_in_parent_);
          if (sigma) sigma = sigma->succ_sym(symbol_reduced_to);
        }

        if (sigma && sigma->accepting_ids_.size()) {
          for (auto &x: sigma->accepting_ids_) {
            matches_[x.tile_id_] |= 1ULL << x.branch_depth_;
          }
        }
      }
    }
  }
}



void detach_child(automaton_node *n) {
  if (!n->parent_) return;
  if (n->parent_->egress_ == n) {
    /* n is last child, update parent's egress_ to point to predecessor */
    automaton_node *pred = n->sibling_;
    while (pred->sibling_ != n) {
      pred = pred->sibling_;
    }
    if (pred != n) {
      pred->sibling_ = n->sibling_;
      n->parent_->egress_ = n->sibling_;
    }
    else {
      n->parent_->egress_ = nullptr;
    }
    return;
  }
  /* n is not last child, just find it and update predecessor's sibling_ */
  automaton_node **ppred = &n->parent_->egress_;
  while (*ppred != n) {
    ppred = &(*ppred)->sibling_;
  }
  *ppred = n->sibling_;
}

void make_edge(automaton_node *parent, automaton_node *child) {
  detach_child(child);

  child->parent_ = parent;
  if (parent->egress_) {
    child->sibling_ = parent->egress_->sibling_;
    parent->egress_->sibling_ = child;
    parent->egress_ = child;
  }
  else {
    child->sibling_ = child;
    parent->egress_ = child;
  }
}

void make_sym_edge(automaton_node *parent, automaton_node *child, char symbol) {
  make_edge(parent, child);
  child->is_operand_ = false;
  child->symbol_ = symbol;
}

void make_sym_edge(size_t parent_index, size_t child_index, char symbol) {
  auto *child = automaton_nodes_[child_index].get();
  auto *parent = automaton_nodes_[parent_index].get();
  make_sym_edge(parent, child, symbol);
}

void make_opd_edge(automaton_node *parent, automaton_node *child, int operand) {
  make_edge(parent, child);
  child->is_operand_ = true;
  child->operand_ = operand;
}

void make_opd_edge(size_t parent_index, size_t child_index, int operand) {
  auto *child = automaton_nodes_[child_index].get();
  auto *parent = automaton_nodes_[parent_index].get();
  make_opd_edge(parent, child, operand);
}

automaton_node *follow_node(automaton_node *p, automaton_node *transition) {
  if (!p) return nullptr;
  if (transition->is_operand_) {
    return p->follow_opd(transition->operand_);
  } else {
    return p->follow_sym(transition->symbol_);
  }
}

size_t count_depth(automaton_node *p) {
  size_t depth = 0;
  for (; p->parent_; p = p->parent_) {
    ++depth;
  }
  return depth;
}

void find_failure_function(size_t node_index) {
  auto *root = automaton_nodes_[0].get();
  auto *p = automaton_nodes_[node_index].get();
  std::vector<automaton_node *> path;
  
  size_t failure_func = 0;

  for (auto *n = p; n != root; n = n->parent_) {
    path.resize(count_depth(p) - count_depth(n));
    auto *q = p;
    for (size_t i = 0; i < path.size(); ++i) {
      path[path.size() - i - 1] = q;
      q = q->parent_;
    }
    q = root;
    for (size_t i = 0; i < path.size(); ++i) {
      q = follow_node(q, path[i]);
      if (!q) break;
    }
    if (q && q != root) {
      /* Have a failure function, find index to it. */
      size_t index;
      for (index = 0; index < automaton_nodes_.size(); ++index) {
        if (automaton_nodes_[index].get() == q) break;
      }
      /* This may not be the final (longest) failure function node,
       * but any acceptances on this node will also be on the node
       * we started with. */
      for (auto &x: q->accepting_ids_) {
        if (std::find(p->accepting_ids_.begin(), p->accepting_ids_.end(), x) == p->accepting_ids_.end()) {
          p->accepting_ids_.push_back(x);
        }
      }
      /* If the failure function destination state has no egress that is not
       * also on the source state, then it is not a valid failure function (because
       * there would be no point in following it). */
      if (q->egress_) {
        automaton_node *failure_state_edge = q->egress_;
        do {
          failure_state_edge = failure_state_edge->sibling_;

          automaton_node *original_edge = p->egress_;
          bool found = false;
          if (original_edge) {
            do {
              original_edge = original_edge->sibling_;

              if (original_edge->edge_match(*failure_state_edge)) {
                found = true;
                break;
              }

              if (original_edge == failure_state_edge) break;
            } while (original_edge != p->egress_);
          }
          if (!found) {
            /* Have at least one edge on the failure state that is not also
             * on the original state, so this is a valid failure function. */
            failure_func = index;
            break;
          }

        } while (failure_state_edge != q->egress_);
      }
    }
  }
  if (failure_func) {
    if (failure_func == automaton_nodes_.size()) {
      fprintf(stderr, "Error: could not find failure function after we found it\n");
      exit(1);
    }

    if (p->failure_func_ && (p->failure_func_ != failure_func)) {
      fprintf(stderr, "Error: failure function already set yet mismatches\n");
      exit(1);
    }

    p->failure_func_ = failure_func;
  }
  else {
    if (p->failure_func_) {
      fprintf(stderr, "Error: failure function already set yet expecting no such failure function\n");
      exit(1);
    }
    p->failure_func_ = 0;
  }
}

void add_tile_to_automaton(automaton_node *n, tile_node *t, int depth, int tile_num) {
  // Find symbol node for t
  automaton_node *sym_node = nullptr;
  if (!n) {
    if (automaton_nodes_.size()) {
      n = automaton_nodes_[0].get();
    }
    else {
      // No root node to add to, create root.
      automaton_nodes_.emplace_back(std::move(std::make_unique<automaton_node>()));
      n = automaton_nodes_.back().get();
    }
  }
  if (n->egress_) {
    auto *sib = n->egress_;
    do {
      sib = sib->sibling_;
        
      if (!sib->is_operand_ && (sib->symbol_ == t->symbol_)) {
        sym_node = sib;
        break;
      }
    } while (sib != n->egress_);
  }

  if (!sym_node) {
    // No symbol node found, add it for t->symbol_.
    automaton_nodes_.emplace_back(std::move(std::make_unique<automaton_node>()));
    sym_node = automaton_nodes_.back().get();
    make_sym_edge(n, sym_node, t->symbol_);
  }
  if (!t->children_.size()) {
    sym_node->is_accepting_ = true;
    struct automaton_accept a;
    a.branch_depth_ = depth;
    a.tile_id_ = tile_num;
    sym_node->accepting_ids_.push_back(a);
    return ;
  }
  depth++;
  int child_index = 0;
  for (auto &child: t->children_) {
    // Find operand node for this child's index
    bool got_it = false;
    if (sym_node->egress_) {
      auto *sib = sym_node->egress_;
      do {
        sib = sib->sibling_;
        
        if (sib->is_operand_ && sib->operand_ == child_index) {
          add_tile_to_automaton(sib, child.get(), depth, tile_num);
          got_it = true;
          break;
        }
      } while (sib != sym_node->egress_);
    }
    if (!got_it) {
      // Add the operand node for this child's index 
      automaton_nodes_.emplace_back(std::move(std::make_unique<automaton_node>()));
      auto *opd_node = automaton_nodes_.back().get();
      make_opd_edge(sym_node, opd_node, child_index);
      add_tile_to_automaton(opd_node, child.get(), depth, tile_num);
    }
    child_index++;
  }
}


struct cli_args {
  char short_;
  const char *long_;
  const char *arg_line_;
  const char *description_;
  int has_value_;
} g_args_[] = {
  { 'H', "help", NULL, "Print this help message.", 0 },
  { 'v', "version", NULL, "Print version information.", 0 },
  { 'c', "c", "[<c_filename>]", "Generate a C file and output it to c_filename. If no filename is specified, then output will be to stdout.", 1 },
};

int process_option(int argc, const char **argv, int *arg_index, int permit_default_arg) {
  ++(*arg_index);
  size_t n;
  if ((*arg_index) == argc) {
    return 0; // end reached.
  }
  const char *arg = argv[*arg_index];
  for (n = 0; n < sizeof(g_args_) / sizeof(*g_args_); ++n) {
    if (arg[0] == '-' && arg[1] == g_args_[n].short_ && !arg[2]) {
      if (g_args_[n].has_value_) {
        ++(*arg_index);
      }
      return g_args_[n].short_;
    }
    else if (arg[0] == '-' && arg[1] == '-' && !strcmp(arg + 2, g_args_[n].long_)) {
      if (g_args_[n].has_value_) {
        ++(*arg_index);
      }
      return g_args_[n].short_;
    }
  }
  if (arg[0] != '-') {
    // default argument
    if (permit_default_arg) return '-';
    else {
      // Either default argument not permitted, or default argument already specified.
      return -2;
    }
  }
  else {
    fprintf(stderr, "Invalid option %s\n", arg);
    return -1;
  }
}

static void puts_wrapped(FILE *fp, const char *str, int indent, size_t linesize) {
  size_t len = strlen(str);
  size_t line_start = 0;
  size_t line_end = 0;
  while (line_end < len) {
    size_t line_scan = line_end;
    while ((line_scan < len) && (str[line_scan] != '\n')) {
      if (str[line_scan] == ' ') line_end = line_scan;
      if ((line_scan - line_start) >= linesize) break;
      line_scan++;
    }
    if (line_end == line_start) {
      /* no progress made, force wrap in middle of word */
      line_end = line_scan;
    }
    if (line_scan == len) {
      line_end = line_scan;
    }
    fprintf(fp, "%*s%.*s\n", indent, "", (int)(line_end - line_start), str + line_start);
    line_start = line_end;
    if (str[line_start] == '\n') line_start++;
    else if (str[line_start] == ' ') line_start++;
    line_end = line_start;
  }
}

static void print_usage(FILE *fp) {
  fprintf(fp, "Tilly instruction tile pattern matcher generator (C) 2024 Kinglet B.V.\n"
              "version " TILLY_VERSION_STR " \n"
              "Tilly is one of the examples of the Carburetta scanner and parser generator.\n"
              "https://carburetta.com/\n"
              "\n"
              "tilly <inputfile.tilly> <flags>\n"
              "\n"
              "<inputfile.tilly>\n"
              "         the input file containing the pattern tiles (mandatory)\n"
              "\n"
              "<flags>\n"
  );
  size_t n;
  for (n = 0; n < sizeof(g_args_) / sizeof(*g_args_); ++n) {
    fprintf(fp, "%s-%c, --%s %s\n", n ? "\n" : "", g_args_[n].short_, g_args_[n].long_, g_args_[n].arg_line_ ? g_args_[n].arg_line_ : "");
    puts_wrapped(fp, g_args_[n].description_, 9, 79 - 9);
  }

  fprintf(fp, "\nReport bugs to: carburetta@kinglet.nl\n"
              "Sourcecode repository at: <https://github.com/kingletbv/carburetta>\n"
  );

}

static void print_version(FILE *fp) {
  fprintf(fp, "Tilly " TILLY_VERSION_STR "\n");
}

int main(int argc, char **argv) {
  int r = EXIT_SUCCESS;

  const char *input_filename = NULL;
  const char *output_filename = NULL;

  FILE *fp = NULL;

  int option_index = 0;
  int have_input_file = 0;
  int option;
  do {
    option = process_option(argc, (const char **)argv, &option_index, !have_input_file);
    switch (option) {
      case 0:
        break;
      case 'H':
        print_usage(stdout);
        goto exit_program_success;
      case 'v':
        print_version(stdout);
        goto exit_program_success;
      case 'c':
        if ((option_index < argc) && (argv[option_index])[0] != '-') {
          /* filename specified */
          if (output_filename) {
            fprintf(stderr, "Error: only one C output file permitted\n");
            print_usage(stderr);
            goto exit_arg_eval;
          }
          output_filename = strdup(argv[option_index]);
          if (!output_filename) {
            fprintf(stderr, "Error: no memory\n");
            goto exit_arg_eval;
          }
        }
        else {
          /* no filename specified, back up */
          option_index--;
        }
        break;
      case '?':
        print_usage(stdout);
        goto exit_program_success;
      case '-':
        /* default argument */
        input_filename = strdup(argv[option_index]);
        if (!input_filename) {
          fprintf(stderr, "Error: no memory\n");
          goto exit_arg_eval;
        }
        have_input_file = 1;
        break;
      default:
        fprintf(stderr, "Error: unknown option\n");
        print_usage(stderr);
        goto exit_arg_eval;
    }
  } while (option);

  if (!have_input_file) {
    fprintf(stderr, "Error: expected input file\n");
    print_usage(stderr);
    goto exit_arg_eval;
  }

  try {
    fp = fopen(input_filename, "rb");
    if (!fp) {
      fprintf(stderr, "Failed to open file \"%s\": %s\n", input_filename, strerror(errno));
      throw std::system_error(errno, std::generic_category());
    }
    TillyParser tp;
    tp.fp_ = fp;
    tp.input_filename_ = input_filename ? input_filename : "(stdin)";
    tp.process();
    tp.semantic_analysis();
    if (tp.had_failure) {
      r = EXIT_FAILURE;
    }
    else {
      // Produce output
      FILE *output;
      if (output_filename) {
        output = fopen(output_filename, "wb");
        if (!output) {
          fprintf(stderr, "Failed to open file for output \"%s\": %s\n", output_filename, strerror(errno));
          throw std::system_error(errno, std::generic_category());
        }
      }
      else {
        output = stdout;
      }
      tp.write_output(output);
      if (output != stdout) fclose(output);
      if (tp.had_failure) {
        r = EXIT_FAILURE;
      }
    }
  }
  catch (std::exception &e) {
    fprintf(stderr, "Error: %s\n", e.what());
    r = EXIT_FAILURE;
  }
  if (fp && (fp != stdin)) {
    fclose(fp);
  }

  if (0) {
  exit_program_success:
    r = EXIT_SUCCESS;
    goto exit_program;
  exit_arg_eval:
    r = EXIT_FAILURE;
    goto exit_program;
  }
  exit_program:
  return r;
}
