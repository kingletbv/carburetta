/* Copyright 2023-2025 Kinglet B.V.
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

#ifndef VECTOR_INCLUDED
#define VECTOR_INCLUDED
#include <vector>
#endif

#ifndef STRING_INCLUDED
#define STRING_INCLUDED
#include <string>
#endif

#ifndef STDEXCEPT_INCLUDED
#define STDEXCEPT_INCLUDED
#include <stdexcept>
#endif

#ifndef ALGORITHM_INCLUDED
#define ALGORITHM_INCLUDED
#include <algorithm>
#endif

#ifndef SYSTEM_ERROR_INCLUDED
#define SYSTEM_ERROR_INCLUDED
#include <system_error>
#endif

#ifndef STDEXCEPT_INCLUDED
#define STDEXCEPT_INCLUDED
#include <stdexcept>
#endif

#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED
#include <string.h>
#endif

#ifndef TILLY_PARSER_H_INCLUDED
#define TILLY_PARSER_H_INCLUDED
#include "tilly_parser.h"
#endif

#ifndef DIRECTIVES_DEFS_H_INCLUDED
#define DIRECTIVES_DEFS_H_INCLUDED
#include "directives_defs.h"
#endif

#ifndef DIRECTIVES_H_INCLUDED
#define DIRECTIVES_H_INCLUDED
#include "directives.h"
#endif

#ifndef DIRECTIVE_INPUT_SPLITTER_DEFS_H_INCLUDED
#define DIRECTIVE_INPUT_SPLITTER_DEFS_H_INCLUDED
#include "directive_input_splitter_defs.h"
#endif

#ifndef DIRECTIVE_INPUT_SPLITTER_H_INCLUDED
#define DIRECTIVE_INPUT_SPLITTER_H_INCLUDED
#include "directive_input_splitter.h"
#endif

#ifndef MULTI_LINE_COMMENTS_DEFS_H_INCLUDED
#define MULTI_LINE_COMMENTS_DEFS_H_INCLUDED
#include "multi_line_comments_defs.h"
#endif

#ifndef MULTI_LINE_COMMENTS_H_INCLUDED
#define MULTI_LINE_COMMENTS_H_INCLUDED
#include "multi_line_comments.h"
#endif

#ifndef LINE_CONTINUATION_DEFS_H_INCLUDED
#define LINE_CONTINUATION_DEFS_H_INCLUDED
#include "line_continuation_defs.h"
#endif

#ifndef LINE_CONTINUATION_H_INCLUDED
#define LINE_CONTINUATION_H_INCLUDED
#include "line_continuation.h"
#endif

#ifndef SNIPPETS_H_INCLUDED
#define SNIPPETS_H_INCLUDED
#include "snippets.h"
#endif

#define VERBOSE_OUTPUT 0

void ASTNode::semantic_analysis(TillyParser &tp) {
  type_ = kUnknown;
  size_t sym_index = 0;
  for (auto &sym: tp.symbols_) {
    if (ident_ == sym) {
      type_ = kSymbol;
      index_ = sym_index;
      break;
    }
    ++sym_index;
  }
  if (type_ == kUnknown) {
    for (auto &label: tp.labels_) {
      if (ident_ == label) {
        type_ = kLabel;
        index_ = sym_index;
        break;
      }
      ++sym_index;
    }
  }
  if (type_ == kUnknown) {
    fprintf(stderr, "%s(%d): error: unknown symbol or label '%s'\n", situs_.filename(), situs_.line(), ident_.c_str());
    tp.set_failure();
  }
  if (type_ == kLabel) {
    if (children_.size()) {
      // Labels are like non-terminals and should not have any children (they can be the leaf nodes of a tile, but
      // they should not have children of their own).
      fprintf(stderr, "%s(%d): error: label '%s' should not have children\n", situs_.filename(), situs_.line(), ident_.c_str());
      tp.set_failure();
    }
  }
  for (auto &child: children_) {
    child->semantic_analysis(tp);
  }
}

void ASTTile::semantic_analysis(TillyParser &tp) {
  size_t label_index = 0;
  bool found_label = false;
  for (auto &label: tp.labels_) {
    if (label == label_) {
      found_label = true;
      label_index_ = label_index;
      break;
    }
    ++label_index;
  }
  if (!found_label) {
    fprintf(stderr, "%s(%d): error: unknown label '%s'\n", situs_.filename(), situs_.line(), label_.c_str());
    tp.set_failure();
  }
  root_->semantic_analysis(tp);
}


AutomatonNode *AutomatonNode::follow_opd(size_t operand) {
  for (auto &child : children_) {
    if (child->is_operand_ && child->index_ == operand) {
      return child;
    }
  }
  return nullptr;
}

AutomatonNode *AutomatonNode::follow_sym(size_t symbol) {
  for (auto &child : children_) {
    if (!child->is_operand_ && child->index_ == symbol) {
      return child;
    }
  }
  return nullptr;
}

AutomatonNode *AutomatonNode::follow_node(AutomatonNode *transition) {
  if (transition->is_operand_) {
    return follow_opd(transition->index_);
  }
  else {
    return follow_sym(transition->index_);
  }
}

AutomatonNode *AutomatonNode::succ_opd(size_t operand) {
  AutomatonNode *an = this;
  AutomatonNode *s = follow_opd(operand);
  while (!s) {
    if (!an->failure_func_) {
      // Cannot go via root as root always holds symbols, not operands.
      return nullptr;
    }
    else {
      an = an->failure_func_;
    }
    s = an->follow_opd(operand);
  }
  return s;
}

AutomatonNode *AutomatonNode::succ_sym(TillyParser &tp, size_t symbol) {
  AutomatonNode *an = this;
  AutomatonNode *s = follow_sym(symbol);
  while (!s) {
    if (!an->failure_func_) {
      // Try and go via root
      an = tp.automaton_nodes_[0].get();
      return an->follow_sym(symbol);
    }
    else {
      an = an->failure_func_;
    }
    s = an->follow_sym(symbol);
  }

  return s;
}

void AutomatonNode::detach() {
  if (parent_) {
    parent_->children_.erase(std::remove(parent_->children_.begin(), parent_->children_.end(), this), parent_->children_.end());
    parent_ = nullptr;
  }
}

void AutomatonNode::attach(AutomatonNode *parent) {
  detach();
  parent->children_.push_back(this);
  parent_ = parent;
}

size_t AutomatonNode::count_depth() {
  size_t depth = 0;
  AutomatonNode *p = this;
  for (; p->parent_; p = p->parent_) {
    ++depth;
  }
  return depth;
}

AutomatonNode *TillyParser::make_sym_edge(AutomatonNode *parent, size_t symbol) {
  automaton_nodes_.emplace_back(std::move(std::make_unique<AutomatonNode>()));
  AutomatonNode *an = automaton_nodes_.back().get();
  an->attach(parent);
  an->is_operand_ = false;
  an->index_ = symbol;
  return an;
}

AutomatonNode *TillyParser::make_opd_edge(AutomatonNode *parent, size_t operand) {
  automaton_nodes_.emplace_back(std::move(std::make_unique<AutomatonNode>()));
  AutomatonNode *an = automaton_nodes_.back().get();
  an->attach(parent);
  an->is_operand_ = true;
  an->index_ = operand;
  return an;
}

TillyParser::TillyParser() {
  tiles_stack_init(&tiles_parser_);
  directives_stack_init(&directives_);
  dis_stack_init(&dis_);
  mlc_stack_init(&mlc_);
  lc_stack_init(&lc_);
}

TillyParser::~TillyParser() {
  tiles_stack_cleanup(&tiles_parser_);
  directives_stack_cleanup(&directives_);
  dis_stack_cleanup(&dis_);
  mlc_stack_cleanup(&mlc_);
  lc_stack_cleanup(&lc_);
}

void TillyParser::add_tile(Situs &situs, const char *label, std::unique_ptr<ASTNode> &root, Situs &cost_snippet_situs, size_t cost_snippet_start, size_t cost_snippet_end, Situs &action_snippet_situs, size_t action_snippet_start, size_t action_snippet_end) {
  ast_tiles_.emplace_back(std::make_unique<ASTTile>());
  auto &node = *ast_tiles_.back().get();
  node.situs_ = situs;
  node.label_ = label;
  node.root_ = std::move(root);
  node.cost_snippet_situs_ = cost_snippet_situs;
  node.cost_snippet_start_ = cost_snippet_start;
  node.cost_snippet_end_ = cost_snippet_end;
  node.action_snippet_situs_ = action_snippet_situs;
  node.action_snippet_start_ = action_snippet_start;
  node.action_snippet_end_ = action_snippet_end;

}

void TillyParser::add_tile_to_automaton(AutomatonNode *an, ASTNode *tn, int depth, size_t tile_num) {
  // Find symbol node for tn
  AutomatonNode *sym_node = nullptr;
  if (!an) {
    if (automaton_nodes_.size()) {
      an = automaton_nodes_[0].get();
    }
    else {
      // No root node to add to, create root.
      automaton_nodes_.emplace_back(std::move(std::make_unique<AutomatonNode>()));
      an = automaton_nodes_.back().get();
    }
  }
  
  sym_node = an->follow_sym(tn->index_);

  if (!sym_node) {
    // No symbol node found, add it for tn's symbol in tn->index_
    sym_node = make_sym_edge(an, tn->index_);
  }
  if (!tn->children_.size()) {
    sym_node->accepting_ids_.emplace_back(depth, tile_num);
    return;
  }
  depth++;
  size_t child_index = 0;
  for (auto &child: tn->children_) {
    // Find operand node for this child's index

    AutomatonNode *opd_node = sym_node->follow_opd(child_index);
    if (!opd_node) {
      // Add operand node for this child's index
      opd_node = make_opd_edge(sym_node, child_index);
    }
    add_tile_to_automaton(opd_node, child.get(), depth, tile_num);

    child_index++;
  }
}

void TillyParser::find_failure_function(AutomatonNode *an) {
  auto *root = automaton_nodes_[0].get();
  std::vector<AutomatonNode *> path;

  AutomatonNode *failure_func = nullptr;

  for (auto *n = an; n != root; n = n->parent_) {
    path.resize(an->count_depth() - n->count_depth());
    auto *q = an;
    for (size_t i = 0; i < path.size(); ++i) {
      path[path.size() - i - 1] = q;
      q = q->parent_;
    }
    q = root;
    for (size_t i = 0; i < path.size(); ++i) {
      q = q->follow_node(path[i]);
      if (!q) break;
    }

    if (q && (q != root)) {
      /* This may not be the final (longest) failure function node,
       * but any acceptances on this node will also be on the node
       * we started with. */
      for (auto &x: q->accepting_ids_) {
        if (std::find(an->accepting_ids_.begin(), an->accepting_ids_.end(), x) == an->accepting_ids_.end()) {
          an->accepting_ids_.push_back(x);
        }
      }
      /* If the failure function destination state has no egress that is not
       * also on the source state, then it is not a valid failure function (because
       * there would be no point in following it.) */
      for (auto failure_state_edge : q->children_) {
        if (!an->follow_node(failure_state_edge)) {
          /* Have found an edge on the failure state that is not also on the original
           * state, ergo the failure state is not fully overlapped by the original state
           * and it would make sense, on failure, to try and go there. */
          failure_func = q;
          break;
        }
      }
    }
  }

  an->failure_func_ = failure_func;
}

void TillyParser::process_tiles() {
  while (have_tiles_data_to_process_) {
    int r;
    r = tiles_scan(&tiles_parser_, *this, dis_to_tiles_situs_);

    switch (r) {
      case _TILES_FINISH: {
        finished_ = true;
        have_tiles_data_to_process_ = false;
        break;
      }
      case _TILES_INTERNAL_ERROR:
      case _TILES_OVERFLOW:
        throw std::runtime_error("Internal error");
      case _TILES_NO_MEMORY:
        throw std::bad_alloc();
      case _TILES_FEED_ME:
        have_tiles_data_to_process_ = false;
        dis_to_tiles_section_.resize(0);
        break;
      case _TILES_SYNTAX_ERROR: {
        Situs *situs = (Situs *)tiles_token_common_data(&tiles_parser_);
        const char *msg = "%s(%d): Syntax error at column %d: \"%s\"\n";
        if (situs) {
          fprintf(stderr, msg, situs->filename(), situs->line(), situs->col(), mlc_text(&mlc_));
        }
        else {
          fprintf(stderr, msg, dis_to_tiles_situs_.filename(), dis_to_tiles_situs_.line(), dis_to_tiles_situs_.col(), mlc_text(&mlc_));
        }
        break;
      }
      case _TILES_LEXICAL_ERROR:
        fprintf(stderr, "%s(%d): Lexical error at column %d: \"%s\"\n", dis_to_tiles_situs_.filename(), dis_to_tiles_situs_.line(), dis_to_directives_situs_.col(), mlc_text(&mlc_));
        break;
    }
  }
}

void TillyParser::process_directives() {
  while (have_dis_to_directives_data_to_process_) {
    int r;
    r = directives_scan(&directives_, *this, dis_to_directives_situs_);

    switch (r) {
      case _DIRECTIVES_FINISH: {
        have_dis_to_directives_data_to_process_ = false;
        return;
      }
      case _DIRECTIVES_INTERNAL_ERROR:
      case _DIRECTIVES_OVERFLOW:
        throw std::runtime_error("Internal error");
      case _DIRECTIVES_NO_MEMORY:
        throw std::bad_alloc();
      case _DIRECTIVES_FEED_ME:
        have_dis_to_directives_data_to_process_ = false;
        dis_to_directives_data_.resize(0);
        break;
      case _DIRECTIVES_SYNTAX_ERROR: {
        // XXX: Should access the line and column from the multiline comment scanner at stack_[0].
        // The current token in the lookahead that's causing the syntax error is in slot 0 of the stack_,
        // consequently, access it there.
        Situs *situs = (Situs *)directives_token_common_data(&directives_);
        const char *msg = "%s(%d): Syntax error at column %d: \"%s\"\n";
        if (situs) {
          fprintf(stderr, msg, situs->filename(), situs->line(), situs->col(), directives_text(&directives_));
        }
        else {
          fprintf(stderr, msg, dis_to_directives_situs_.filename(), dis_to_directives_situs_.line(), dis_to_directives_situs_.col(), directives_text(&directives_));
        }
        break;
      }
      case _DIRECTIVES_LEXICAL_ERROR:
        fprintf(stderr, "%s(%d): Lexical error at column %d: \"%s\"\n", dis_to_directives_situs_.filename(), dis_to_directives_situs_.line(), dis_to_directives_situs_.col(), directives_text(&directives_));
        break;
      case DIR_TOGGLE_CODE_SECTION:
        if (emit_to_code_prologue_section_) {
          /* Push last code to prologue */
          code_prologue_section_.insert(code_prologue_section_.end(), current_code_section_.begin(), current_code_section_.end());
          current_code_section_.resize(0);
        }
        emit_to_code_prologue_section_ = !emit_to_code_prologue_section_;
        break;
    }
  }
}

void TillyParser::process_dis() {
  while (have_dis_data_to_process_) {
    int r;
    r = dis_scan(&dis_, dis_situs_, dis_output_data_, dis_output_situs_);

    switch (r) {
      case _DIS_FINISH: {
        have_dis_data_to_process_ = false;
        mlc_to_dis_data_.resize(0);
        dis_situs_.reset();

        if (mlc_finished_) {
          directives_set_input(&directives_, NULL, 0, /* is_final_input: */1);
          have_dis_to_directives_data_to_process_ = true;

          tiles_set_input(&tiles_parser_, NULL, 0, /* is_final_input: */1);
          have_tiles_data_to_process_ = true;
        }
        
        return;
      }
      case _DIS_INTERNAL_ERROR:
      case _DIS_OVERFLOW:
        throw std::runtime_error("Internal error");
      case _DIS_NO_MEMORY:
        throw std::bad_alloc();
      case _DIS_FEED_ME:
        have_dis_data_to_process_ = false;
        mlc_to_dis_data_.resize(0);
        break;
      case _DIS_SYNTAX_ERROR: {
        // XXX: Should access the line and column from the multiline comment scanner at stack_[0].
        // The current token in the lookahead that's causing the syntax error is in slot 0 of the stack_,
        // consequently, access it there.
        Situs *situs = (Situs *)dis_token_common_data(&dis_);
        const char *msg = "%s(%d): Syntax error at column %d: \"%s\"\n";
        if (situs) {
          fprintf(stderr, msg, situs->filename(), situs->line(), situs->col(), dis_text(&dis_));
        }
        else {
          fprintf(stderr, msg, dis_situs_.filename(), dis_situs_.line(), dis_situs_.col(), dis_text(&dis_));
        }
        break;
      }
      case _DIS_LEXICAL_ERROR:
        fprintf(stderr, "%s(%d): Lexical error at column %d: \"%s\"\n", dis_situs_.filename(), dis_situs_.line(), dis_situs_.col(), dis_text(&dis_));
        break;

      case DIS_DIRECTIVE_LINE_AVAILABLE: {
#if VERBOSE_OUTPUT
        size_t len;

        fprintf(stdout, "DIS Directive Line available \"");
        len = dis_output_data_.size();
        if (len && (dis_output_data_[len - 1] == '\n')) {
          len--;
          if (len && (dis_output_data_[len - 1] == '\r')) {
            len--;
          }
        }
        fwrite(dis_output_data_.data(), sizeof(uint8_t), len, stdout);
        fprintf(stdout, "\"\n");
        fprintf(stdout, "DIS range from %zu to %zu\n", dis_output_situs_.first_span()->start_, (dis_output_situs_.first_span() + dis_output_situs_.num_spans_ - 1)->end_);
#endif
        //mlc_set_input(&mlc_, (const char *)mlc_to_splitter_data_.data(), mlc_to_splitter_data_.size(), 0);
        dis_to_directives_data_ = dis_output_data_;
        dis_to_directives_situs_.concat(&dis_output_situs_);
        directives_set_input(&directives_, (const char *)dis_to_directives_data_.data(), dis_to_directives_data_.size(), /* is_final_input: */1);
        have_dis_to_directives_data_to_process_ = true;
        dis_output_data_.resize(0);
        dis_output_situs_.reset();
        return; /* Force process of data prior to continue */
      }
      case DIS_SECTION_LINE_AVAILABLE: {
        // Specific value returned for each line, caller is to pop the input in lc_to_mc_data and
        // lc_to_mc_situs and pass to the multiline comment scanner.
        size_t start_offset, end_offset;
        start_offset = dis_output_situs_.first_span()->start_;
        end_offset = (dis_output_situs_.first_span() + dis_output_situs_.num_spans_ - 1)->end_;

#if VERBOSE_OUTPUT
        size_t len;
        fprintf(stdout, "DIS Section Line available \"");
        len = dis_output_data_.size();
        if (len && (dis_output_data_[len - 1] == '\n')) {
          len--;
          if (len && (dis_output_data_[len - 1] == '\r')) {
            len--;
          }
        }
        fwrite(dis_output_data_.data(), sizeof(uint8_t), len, stdout);
        fprintf(stdout, "\"\n");
        fprintf(stdout, "DIS range from %zu to %zu\n", start_offset, end_offset);
        fprintf(stdout, ">>>\n");
        fwrite(total_input_.data() + start_offset, sizeof(uint8_t), end_offset - start_offset, stdout);
        fprintf(stdout, "<<<\n");
#endif
        if (emit_to_code_prologue_section_) {
          current_code_section_.insert(current_code_section_.end(), total_input_.begin() + start_offset, total_input_.begin() + end_offset);
          // code_prologue_section_.insert(code_prologue_section_.end(), total_input_.begin() + start_offset, total_input_.begin() + end_offset);
        }
        else {
          dis_to_tiles_section_.insert(dis_to_tiles_section_.end(), total_input_.begin() + start_offset, total_input_.begin() + end_offset);
          Situs original_span;
          original_span.clone_as_single_span_all_gaps_filled(&dis_output_situs_);
          dis_to_tiles_situs_.concat(&original_span);
          tiles_set_input(&tiles_parser_, (const char *)dis_to_tiles_section_.data(), dis_to_tiles_section_.size(), /* is_final_input: */0);
          have_tiles_data_to_process_ = true;
        }
        //mlc_set_input(&mlc_, (const char *)mlc_to_splitter_data_.data(), mlc_to_splitter_data_.size(), 0);
        dis_to_section_data_.insert(dis_to_section_data_.end(), dis_output_data_.begin(), dis_output_data_.end());
        dis_to_section_situs_.concat(&dis_output_situs_);
        have_dis_to_section_data_to_process = true;
        dis_output_data_.resize(0);
        dis_output_situs_.reset();
        return; /* Force process of data prior to continue */
      }
    }
  }
}

void TillyParser::process_mlc() {
  while (have_mlc_data_to_process_) {
    int r;
    r = mlc_scan(&mlc_, lc_to_mlc_situs_, mlc_to_dis_data_, mlc_output_situs_);

    switch (r) {
      case _MLC_FINISH: {
#if VERBOSE_OUTPUT
        size_t len = mlc_to_dis_data_.size();
        if (len) {
          // Have trailing data to pass to the multiline comment scanner.
          // (The last line does not end with a newline and so ends up here...)
          fprintf(stdout, "Last line available \"");
          fwrite(mlc_to_dis_data_.data(), sizeof(uint8_t), len, stdout);
          fprintf(stdout, "\"\n");
        }
#endif
        dis_set_input(&dis_, (const char *)mlc_to_dis_data_.data(), mlc_to_dis_data_.size(), 1);
        have_mlc_data_to_process_ = false;
        mlc_finished_ = true;
        have_dis_data_to_process_ = true;

        lc_to_mlc_data_.resize(0);
        lc_to_mlc_situs_.reset();
        break;
      }
      case _MLC_INTERNAL_ERROR:
      case _MLC_OVERFLOW:
        throw std::runtime_error("Internal error");
      case _MLC_NO_MEMORY:
        throw std::bad_alloc();
      case _MLC_FEED_ME:
        have_mlc_data_to_process_ = false;
        lc_to_mlc_data_.resize(0);
        break;
      case _MLC_SYNTAX_ERROR: {
        // XXX: Should access the line and column from the multiline comment scanner at stack_[0].
        // The current token in the lookahead that's causing the syntax error is in slot 0 of the stack_,
        // consequently, access it there.
        Situs *situs = (Situs *)mlc_token_common_data(&mlc_);
        const char *msg = "%s(%d): Syntax error at column %d: \"%s\"\n";
        if (situs) {
          fprintf(stderr, msg, situs->filename(), situs->line(), situs->col(), mlc_text(&mlc_));
        }
        else {
          fprintf(stderr, msg, lc_to_mlc_situs_.filename(), lc_to_mlc_situs_.line(), lc_to_mlc_situs_.col(), mlc_text(&mlc_));
        }
        break;
      }
      case _MLC_LEXICAL_ERROR:
        fprintf(stderr, "%s(%d): Lexical error at column %d: \"%s\"\n", lc_to_mlc_situs_.filename(), lc_to_mlc_situs_.line(), lc_to_mlc_situs_.col(), mlc_text(&mlc_));
        break;

      case MLC_LINE_AVAILABLE:
        // Specific value returned for each line, caller is to pop the input in lc_to_mc_data and
        // lc_to_mc_situs and pass to the multiline comment scanner.
#if VERBOSE_OUTPUT
        fprintf(stdout, "MLC Line available \"");
        size_t len = mlc_to_dis_data_.size();
        if (len && (mlc_to_dis_data_[len-1] == '\n')) {
          len--;
          if (len && (mlc_to_dis_data_[len-1] == '\r')) {
            len--;
          }
        }
        fwrite(mlc_to_dis_data_.data(), sizeof(uint8_t), len, stdout);
        fprintf(stdout, "\"\n");
        fprintf(stdout, "MLC range from %zu to %zu\n", mlc_output_situs_.first_span()->start_, (mlc_output_situs_.first_span() + mlc_output_situs_.num_spans_ - 1)->end_);
#endif
        dis_situs_.concat(&mlc_output_situs_);
        mlc_output_situs_.reset();
        dis_set_input(&dis_, (const char *)mlc_to_dis_data_.data(), mlc_to_dis_data_.size(), 1);
        have_dis_data_to_process_ = true;
        return;
    }
  }
}

void TillyParser::process_lc() {
  while (have_lc_data_to_process_) {
    int r;
    r = lc_scan(&lc_, input_filename_.c_str(), lc_to_mlc_data_, lc_to_mlc_situs_);

    switch (r) {
      case _LC_FINISH: {
        mlc_set_input(&mlc_, (const char *)lc_to_mlc_data_.data(), lc_to_mlc_data_.size(), /* is_final_input: */1);
        have_mlc_data_to_process_ = true;
        have_lc_data_to_process_ = false;
        break;
      }
      case _LC_INTERNAL_ERROR:
      case _LC_OVERFLOW:
        throw std::runtime_error("Internal error");
      case _LC_NO_MEMORY:
        throw std::bad_alloc();
      case _LC_FEED_ME:
        have_lc_data_to_process_ = false;
        break;
      case _LC_SYNTAX_ERROR:
        fprintf(stderr, "%s(%d): Syntax error at column %d: \"%s\"\n", input_filename_.c_str(), lc_line(&lc_), lc_column(&lc_), lc_text(&lc_));
        break;
      case _LC_LEXICAL_ERROR:
        fprintf(stderr, "%s(%d): Lexical error at column %d: \"%s\"\n", input_filename_.c_str(), lc_line(&lc_), lc_column(&lc_), lc_text(&lc_));
        break;

      case LC_LINE_AVAILABLE:
        // Specific value returned for each line, caller is to pop the input in lc_to_mc_data and
        // lc_to_mc_situs and pass to the multiline comment scanner.
        mlc_set_input(&mlc_, (const char *)lc_to_mlc_data_.data(), lc_to_mlc_data_.size(), 0);
        have_mlc_data_to_process_ = true;
        return;
        // XXX: Who resets, and when ?
        //      MLC resets when have_mlc_data_to_process_ hits false.
        // lc_to_mc_data.resize(0);
        // lc_to_mc_situs.reset();
        break;
    }
  }
}


void TillyParser::process() {
  for (;;) {
    if (finished_) return;
    if (have_tiles_data_to_process_) {
      process_tiles();
    }
    else if (have_dis_to_section_data_to_process) {
      // XXX: Give meaningful content
      have_dis_to_section_data_to_process = false;
      if (emit_to_code_prologue_section_) {
      }
      else {
      }
    }
    else if (have_dis_to_directives_data_to_process_) {
      while (have_dis_to_directives_data_to_process_) {
        process_directives();
      }
    }
    else if (have_dis_data_to_process_) {
      // note: exits at each line so downstream can process immediately; this is important
      // as the directives might redirect where the section data is to be sent.
      process_dis();
    }
    else if (have_mlc_data_to_process_) {
      process_mlc();
    }
    else if (have_lc_data_to_process_) {
      process_lc();
    }
    else if (feof(fp_)) {
      /* Processed to the end of it.. ? */
      lc_set_input(&lc_, NULL, 0, 1);
    }
    else {
      size_t num_bytes_read;
      num_bytes_read = fread(buf_, sizeof(*buf_), sizeof(buf_) / sizeof(*buf_), fp_);

      total_input_.insert(total_input_.end(), buf_, buf_ + num_bytes_read);

      lc_set_input(&lc_, buf_, num_bytes_read, feof(fp_));
      have_lc_data_to_process_ = true;
    }
  }
}

static int num_digits(size_t num) {
  int num_digits = 1;
  while (num >= 10) {
    num_digits++;
    num /= 10;
  }
  return num_digits;
}

void TillyParser::dump_automaton(AutomatonNode *an, int indent) {
  printf("%zu", an->num_);
  if (an->failure_func_ || an->accepting_ids_.size()) {
    printf(" (");
    bool is_first = true;
    if (an->failure_func_) {
      printf("fail: %zu", an->failure_func_->num_);
      is_first = false;
    }
    if (an->accepting_ids_.size()) {
      printf("%saccept: ", is_first ? "" : ", ");
      is_first = true;
    }
    for (auto &aid: an->accepting_ids_) {
      printf("%s%zu", is_first ? "" : ", ", aid.tile_id_);
      is_first = false;
    }
    printf(")");
  }
  printf("\n");
  for (auto &child: an->children_) {
    printf("%*s+-", indent, "");
    if (child->is_operand_) {
      printf("%zu-> ", child->index_+1);
      int child_indent = indent + 2 + num_digits(child->index_+1) + 3;
      dump_automaton(child, child_indent);
    }
    else {
      if (child->index_ < symbols_.size()) {
        // Child is symbol transition
        printf(" %s -> ", symbols_[child->index_].c_str());
        int child_indent = indent + 3 + int(symbols_[child->index_].size()) + 4;
        dump_automaton(child, child_indent);
      }
      else {
        // Child is label transition
        size_t label_index = child->index_ - symbols_.size();
        printf(" %s -> ", labels_[label_index].c_str());
        int child_indent = indent + 3 + int(labels_[label_index].size()) + 4;
        dump_automaton(child, child_indent);
      }
    }
  }
}

void TillyParser::semantic_analysis() {
  for (auto &tile: ast_tiles_) {
    tile->semantic_analysis(*this);
  }

  size_t tile_index = 0;
  for (auto &tile: ast_tiles_) {
    add_tile_to_automaton(nullptr, tile->root_.get(), 0, tile_index++);
  }

  size_t num = 0;
  for (auto &an: automaton_nodes_) {
    an->num_ = num++;
    find_failure_function(an.get());
  }

  if (automaton_nodes_.size()) {
    dump_automaton(automaton_nodes_[0].get());
  }
}


void TillyParser::emit_snippet(FILE *fp, ASTTile *tile, bool emit_cost_tile_otherwise_action) {
  Situs snippet;
  snippet.clone_as_single_span_all_gaps_filled(emit_cost_tile_otherwise_action ? &tile->cost_snippet_situs_ : &tile->action_snippet_situs_);
  size_t snippet_start = 0;
  size_t snippet_end = 0;
  snippet.get_byte_range(&snippet_start, &snippet_end);

  snippets_stack snip;
  snippets_stack_init(&snip);
  try {
    // XXX: This snippets_stack_reset() is called so the reset doesn't occur inside snippets_scan(). If it were to
    //      occur in snippets_scan(), we'd lose the location information we set with snippets_set_location().
    snippets_stack_reset(&snip);
    snippets_set_input(&snip, (const char *)total_input_.data() + snippet_start, snippet_end - snippet_start, 1);
    snippets_set_location(&snip, snippet.line(), snippet.col(), snippet_start);
    int r;
    r = snippets_scan(&snip, *this, snippet, tile, fp);
    if (r == _SNIPPETS_SYNTAX_ERROR) {
      fprintf(stderr, "%s(%d,%d): syntax error\n", input_filename_.c_str(), snippets_line(&snip), snippets_column(&snip));
      set_failure();
    }
    else if (r == _SNIPPETS_LEXICAL_ERROR) {
      fprintf(stderr, "%s(%d): lexical error\n", input_filename_.c_str(), snippets_line(&snip));
      set_failure();
    }
    else if (r) {
      fprintf(stderr, "Error: snippets_scan returned %d\n", r);
      set_failure();
    }
  }
  catch (...) {
    snippets_stack_cleanup(&snip);
    throw;
  }
  snippets_stack_cleanup(&snip);
}

void TillyParser::write_output(FILE *fp) {
  if (code_prologue_section_.size() != fwrite(code_prologue_section_.data(), 1, code_prologue_section_.size(), fp)) {
    fprintf(stderr, "Failed to write: %s\n", strerror(errno));
    throw std::system_error(errno, std::generic_category());
  }

  fprintf(fp, "#include <stdint.h>\n"
              "#include <limits.h>\n"
              "\n");

  fprintf(fp, "#define NUM_SYMS   %zu\n", symbols_.size());
  fprintf(fp, "#define NUM_LABELS %zu\n", labels_.size());
  fprintf(fp, "\n");

  size_t sym_index = 0;
  for (auto &symbol: symbols_) {
    std::string upper_sym = symbol;
    for (auto &c: upper_sym) {
      c = toupper((unsigned char )c);
      if (c == '-') c = '_';
    }
    fprintf(fp, "#define SYM_%s %zu\n", upper_sym.c_str(), sym_index++);
  }

  for (auto &label: labels_) {
    std::string upper_label = label;
    for (auto &c: upper_label) {
      c = toupper((unsigned char)c);
      if (c == '-') c = '_';
    }
    fprintf(fp, "#define LABEL_%s %zu\n", upper_label.c_str(), sym_index++);
  }

  fprintf(fp, "\n\n");

  size_t num_transitions = symbols_.size() + labels_.size();

  // First size the columns for each transition.
  std::vector<int> column_widths;
  column_widths.resize(num_transitions);
  size_t num_rows = 0;
  int num_num_automaton_node_digits = num_digits(automaton_nodes_.size());
  int max_num_failure_digits = 0;
  int max_num_num_acceptances_digits = 0;
  for (auto &an: automaton_nodes_) {
    num_rows++;
    int num_failure_digits = an->failure_func_ ? num_digits(an->failure_func_->num_) : 1;
    if (max_num_failure_digits < num_failure_digits) {
      max_num_failure_digits = num_failure_digits;
    }
    for (auto *child: an->children_) {
      int num_column_digits = num_digits(child->num_);
      if (num_column_digits > column_widths[child->index_]) {
        column_widths[child->index_] = num_column_digits;
      }
    }
    int num_num_acceptances_digits = num_digits(an->accepting_ids_.size());
    if (num_num_acceptances_digits > max_num_num_acceptances_digits) {
      max_num_num_acceptances_digits = num_num_acceptances_digits;
    }
  }

  fprintf(fp, "struct automaton_acceptance {\n"
              "  size_t tile_id_;\n"
              "  int branch_depth_;\n"
              "} acceptances_[] = {\n");
  size_t current_acceptance_index = 0;
  size_t automaton_node_index = 0;
  int max_num_acceptances_digits = 0;
  for (auto &an: automaton_nodes_) {
    automaton_node_index++;
    an->acceptance_offset_ = current_acceptance_index;
    int num_acceptances_digits = num_digits(current_acceptance_index);
    if (num_acceptances_digits > max_num_acceptances_digits) {
      max_num_acceptances_digits = num_acceptances_digits;
    }
    current_acceptance_index += an->accepting_ids_.size();
    size_t accepting_id_index = 0;
    for (auto &id: an->accepting_ids_) {
      accepting_id_index++;
      fprintf(fp, "  %zu, %d%s", id.tile_id_, id.branch_depth_, 
              ((automaton_node_index == automaton_nodes_.size()) && (accepting_id_index == an->accepting_ids_.size())) ? "\n" : ",\n");
    }
  }
  fprintf(fp, "};\n\n");

  fprintf(fp, "struct automaton_node {\n"
              "  struct automaton_node *failure_func_;\n"
              "  size_t num_acceptances_;\n"
              "  struct automaton_acceptance *acceptances_;\n"
              "  struct automaton_node *out_[%zu];\n"
              "} nodes_[] ={\n", num_transitions);
  size_t row_num = 0;
  for (auto &an: automaton_nodes_) {
    fprintf(fp, "  /* %*zu */ { ", num_num_automaton_node_digits, row_num);
    row_num++;
    bool last_row = row_num == num_rows;

    if (an->failure_func_) {
      fprintf(fp, "nodes_ + %*zu, ", max_num_failure_digits, an->failure_func_->num_);
    }
    else {
      fprintf(fp, "%*s0, ", 9 + max_num_failure_digits - 1, "");
    }
    
    fprintf(fp, "%*zu, ", max_num_num_acceptances_digits, an->accepting_ids_.size());
    if (an->accepting_ids_.size()) {
      fprintf(fp, "acceptances_ + %*zu, ", max_num_acceptances_digits, an->acceptance_offset_);
    }
    else {
      fprintf(fp, "%*s0, ", max_num_acceptances_digits + 15 - 1, "");
    }

    fprintf(fp, "{ ");
    for (size_t n = 0; n < num_transitions; ++n) {
      bool found_transition = false;
      int max_num_digits = column_widths[n];
      for (auto *child: an->children_) {
        if (child->index_ == n) {
          found_transition = true;
          fprintf(fp, "nodes_ + %*zu%s", max_num_digits, child->num_, ((n + 1) == column_widths.size()) ? "" : ", ");
          break;
        }
      }
      if (!found_transition) {
        fprintf(fp, "0%*s%s", 9 + max_num_digits - 1, "", ((n + 1) == column_widths.size()) ? "" : ", ");
      }
    }
    fprintf(fp, "}");

    fprintf(fp, "  }%s\n", last_row ? "" : ",");
  }
  fprintf(fp, "};\n\n");

  size_t max_num_operands = 0;
  for (auto &an: automaton_nodes_) {
    if (an->is_operand_ && (an->index_ >= max_num_operands)) {
      max_num_operands = an->index_ + 1;
    }
  }

  size_t num_match_words = ast_tiles_.size();

  fprintf(fp, "struct subject_node {\n"
              "  size_t symbol_index_;\n"
              "  size_t operand_index_in_parent_;\n"
              "  uint64_t match_[%zu];\n"
              "  int reduction_cost_[%zu];\n"
              "  size_t reduction_tile_[%zu];\n"
              "  struct subject_node *parent_;\n"
              "  struct subject_node *children_[%zu];\n"
              "  struct automaton_node *state_;\n", num_match_words, labels_.size(), labels_.size(), max_num_operands);
  if (label_types_.size()) {
    fprintf(fp, "  struct {\n");
    for (auto &ltp: label_types_) {
      fprintf(fp, "    %s %s_;\n", ltp.second.c_str(), ltp.first.c_str());
    }
    fprintf(fp, "  } v_;\n");
  }
  fprintf(fp, "};\n\n");

  fprintf(fp, "size_t index_of_label_tile_reduces_to_[] = {\n");
  for (size_t n = 0; n < ast_tiles_.size(); ++n) {
    auto &tile = ast_tiles_[n];
    bool last = ((n + 1) == ast_tiles_.size());
    fprintf(fp, "  %zu%s\n", tile->label_index_, last ? "" : ",");
  }
  fprintf(fp, "};\n\n");


  fprintf(fp, "\n"
              "struct automaton_node *automaton_node_succ_sym(struct automaton_node *an, size_t sym_index) {\n"
              "  struct automaton_node *s = an->out_[sym_index];\n"
              "  while (!s) {\n"
              "    if (!an->failure_func_) {\n"
              "      return nodes_->out_[sym_index];\n"
              "    }\n"
              "    an = an->failure_func_;\n"
              "    s = an->out_[sym_index];\n"
              "  }\n"
              "  return s;\n"
              "}\n"
              "\n"
              "struct automaton_node *automaton_node_succ_opd(struct automaton_node *an, size_t opd_index) {\n"
              "  struct automaton_node *s = an->out_[opd_index];\n"
              "  while (!s) {\n"
              "    if (!an->failure_func_) {\n"
              "      return NULL;\n"
              "    }\n"
              "    an = an->failure_func_;\n"
              "    s = an->out_[opd_index];\n"
              "  }\n"
              "  return s;\n"
              "}\n"
              "\n"
              "void subject_node_visit(struct subject_node *sn) {\n"
              "  if (!sn->parent_) {\n"
              "    /* At root */\n"
              "    sn->state_ = nodes_->out_[sn->symbol_index_];\n"
              "  }\n"
              "  else {\n"
              "    sn->state_ = automaton_node_succ_opd(sn->parent_->state_, sn->operand_index_in_parent_);\n"
              "    if (sn->state_) sn->state_ = automaton_node_succ_sym(sn->state_, sn->symbol_index_);\n"
              "  }\n"
              "  /* Reset costs at this node, then visit children. If this is a leaf, expect cost to \n"
              "   * already be initialized and don't overwrite. */\n"
              "  size_t n;\n"
              "  if (sn->children_[0]) {\n"
              "    for (n = 0; n < (sizeof(sn->reduction_cost_) / sizeof(*sn->reduction_cost_)); ++n) {\n"
              "      sn->reduction_cost_[n] = INT_MAX;\n"
              "      sn->reduction_tile_[n] = SIZE_MAX;\n"
              "    }\n"
              "  }\n"
              "  for (n = 0; (n < (sizeof(sn->children_) / sizeof(*sn->children_))) && sn->children_[n]; ++n) {\n"
              "    struct subject_node *child = sn->children_[n];\n"
              "    subject_node_visit(child);\n"
              "  }\n"
              "\n"
              "  /* post process */\n"
              "  for (n = 0; (n < (sizeof(sn->children_) / sizeof(*sn->children_))) && sn->children_[n]; ++n) {\n"
              "    struct subject_node *child = sn->children_[n];\n"
              "    size_t k;\n"
              "    if (!n) {\n"
              "      for (k = 0; k < sizeof(sn->match_) / sizeof(*sn->match_); ++k) {\n"
              "        sn->match_[k] = child->match_[k] >> 1;\n"
              "      }\n"
              "    }\n"
              "    else {\n"
              "      for (k = 0; k < sizeof(sn->match_) / sizeof(*sn->match_); ++k) {\n"
              "        sn->match_[k] &= child->match_[k] >> 1;\n"
              "      }\n"
              "    }\n"
              "  }\n"
              "  if (!sn->children_[0]) {\n"
              "    for (n = 0; n < sizeof(sn->match_) / sizeof(*sn->match_); ++n) {\n"
              "      sn->match_[n] = 0;\n"
              "    }\n"
              "  }\n"
              "  if (sn->state_) {\n"
              "    for (n = 0; n < sn->state_->num_acceptances_; ++n) {\n"
              "      struct automaton_acceptance *acc = sn->state_->acceptances_ + n;\n"
              "      sn->match_[acc->tile_id_] |= 1ULL << acc->branch_depth_;\n"
              "    }\n"
              "  }\n"
              "\n"
              "  /* do reduce */\n");
  fprintf(fp, "  char tile_already_reduced[%zu] = {0};\n", num_match_words);
  fprintf(fp, "  int did_work;\n"
              "  do {\n"
              "    did_work = 0;\n");
  fprintf(fp, "    for (n = 0; n < sizeof(sn->match_) / sizeof(*sn->match_); ++n) {\n"
              "      if ((sn->match_[n] & 1) && !tile_already_reduced[n]) {\n"
              "        tile_already_reduced[n] = 1;\n"
              "        did_work = 1;\n"
              "        int cost = INT_MAX - 1;\n"
              "        switch (n) {\n"
              "          // cases here, index by tile index.\n");
  size_t tile_index;
  for (tile_index = 0; tile_index < ast_tiles_.size(); ++tile_index) {
    auto &tile = ast_tiles_[tile_index];
    fprintf(fp, "          case %zu:\n", tile_index);
    // emit tile cost snippet
    emit_snippet(fp, tile.get(), true);
    fprintf(fp, "\n            break;\n");
  }
  fprintf(fp, "          default:\n"
              "            break;\n"
              "        }\n"
              "        size_t index_of_label_reduced_to = index_of_label_tile_reduces_to_[n];\n"
              "        int pre_existing_cost = sn->reduction_cost_[index_of_label_reduced_to];\n"
              "        if (pre_existing_cost > cost) {\n"
              "          sn->reduction_cost_[index_of_label_reduced_to] = cost;\n"
              "          sn->reduction_tile_[index_of_label_reduced_to] = n;\n"
              "\n"
              "          /* Find the new state we arrive at on transitioning from the parent state through\n"
              "           * the index_of_label_reduced_to. Check if the resulting state is accepting, and,\n"
              "           * if so, make sure the appropriate bits are cascaded up. */\n"
              "          struct automaton_node *sigma = NULL;\n"
              "          if (!sn->parent_ /* root */) {\n"
              "            sigma = automaton_node_succ_sym(nodes_, index_of_label_reduced_to + NUM_SYMS);\n"
              "          }\n"
              "          else {\n"
              "            sigma = automaton_node_succ_opd(sn->parent_->state_, sn->operand_index_in_parent_);\n"
              "            if (sigma) sigma = automaton_node_succ_sym(sigma, index_of_label_reduced_to + NUM_SYMS);\n"
              "          }\n"
              "\n"
              "          if (sigma && sigma->num_acceptances_) {\n"
              "            size_t k;\n"
              "            for (k = 0; k < sigma->num_acceptances_; ++k) {\n"
              "              struct automaton_acceptance *acc = sigma->acceptances_ + k;\n"
              "              sn->match_[acc->tile_id_] |= 1ULL << acc->branch_depth_;\n"
              "            }\n"
              "          }\n"
              "        }\n"
              "      }\n"
              "    }\n"
              "  } while (did_work);\n"
              "}\n"
              "\n");
  fprintf(fp, "void subject_node_execute(struct subject_node *sn, size_t symbol_index) {\n"
              "  if (symbol_index < NUM_SYMS) {\n"
              "    /* symbol as opposed to label, at leaf. Syms don't execute as they don't reduce, as such. */\n"
              "    return;\n"
              "  }\n"
              "  size_t tile_reduced = sn->reduction_tile_[symbol_index - NUM_SYMS];\n"
              "  if (tile_reduced == SIZE_MAX) {\n"
              "    return;\n"
              "  }\n"
              "  switch (tile_reduced) {\n"
              "    // cases here, index by tile index; for each tile, recursively invoke children\n"
              "    // (skipping any intermediate subject nodes).\n"
              "    // After this, invoke the tile's action.\n");
  for (tile_index = 0; tile_index < ast_tiles_.size(); ++tile_index) {
    auto &tile = ast_tiles_[tile_index];
    fprintf(fp, "    case %zu:\n", tile_index);
    emit_tile_descendant_execution(fp, *tile, *tile->root_, "sn");
    // emit tile action snippet
    emit_snippet(fp, tile.get(), false);
    fprintf(fp, "\n      break;\n");
  }
  fprintf(fp, "    default:\n"
              "      break;\n");
  fprintf(fp, "  }\n"
              "}\n");
  if (current_code_section_.size() != fwrite(current_code_section_.data(), 1, current_code_section_.size(), fp)) {
    fprintf(stderr, "Failed to write: %s\n", strerror(errno));
    throw std::system_error(errno, std::generic_category());
  }
}

void TillyParser::emit_tile_descendant_execution(FILE *fp, ASTTile &tile, ASTNode &node, std::string path_to_node) {
  size_t child_index = 0;
  for (auto &child: node.children_) {
    emit_tile_descendant_execution(fp, tile, *child, path_to_node + "->children_[" + std::to_string(child_index) + "]");
    child_index++;
  }
  if (node.index_ >= symbols_.size()) {
    fprintf(fp, "      subject_node_execute(%s, %zu);\n", path_to_node.c_str(), node.index_);
  }
}

