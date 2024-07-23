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

#ifndef TILLY_PARSER_H
#define TILLY_PARSER_H

#ifndef ASSERT_H_INCLUDED
#define ASSERT_H_INCLUDED
#include <assert.h>
#endif

#ifndef VECTOR_INCLUDED
#define VECTOR_INCLUDED
#include <vector>
#endif

#ifndef STRING_INCLUDED
#define STRING_INCLUDED
#include <string>
#endif

#ifndef LIST_INCLUDED
#define LIST_INCLUDED
#include <list>
#endif

#ifndef MEMORY_INCLUDED
#define MEMORY_INCLUDED
#include <memory>
#endif

#ifndef ALGORITHM_INCLUDED
#define ALGORITHM_INCLUDED
#include <algorithm>
#endif

#ifndef CCTYPE_INCLUDED
#define CCTYPE_INCLUDED
#include <cctype>
#endif

class TillyParser;

#ifndef TILES_DEFS_H_INCLUDED
#define TILES_DEFS_H_INCLUDED
#include "tiles_defs.h"
#endif

#ifndef TILES_H_INCLUDED
#define TILES_H_INCLUDED
#include "tiles.h"
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

class ASTNode {
  public:
  Situs situs_;
  std::string ident_;
  std::vector<std::unique_ptr<ASTNode> > children_;
  
  void semantic_analysis(TillyParser &tp);

  std::string cname() {
    std::string name;
    name = ident_;
    for (char &c: name) c = toupper((unsigned char)c);
    if (type_ == kSymbol) {
      return "SYMBOL_" + name;
    }
    else if (type_ == kLabel) {
      return "LABEL_" + name;
    }
    else {
      return "? unknown ASTNode ?";
    }
  }

  enum {
    kUnknown,
    kLabel,
    kSymbol
  } type_;

  // Index of either label or symbol
  size_t index_ = 0;
};

class ASTTile {
  public:
  Situs situs_;
  std::string label_;
  std::unique_ptr<ASTNode> root_;
  Situs cost_snippet_situs_;
  size_t cost_snippet_start_, cost_snippet_end_;
  Situs action_snippet_situs_;
  size_t action_snippet_start_, action_snippet_end_;
  
  void semantic_analysis(TillyParser &tp);

  // Index of label
  size_t label_index_ = 0;
};

struct AutomatonAccept {
  AutomatonAccept() {}
  AutomatonAccept(int branch_depth, size_t tile_id) 
    : branch_depth_(branch_depth), tile_id_(tile_id) {}
  bool operator==(const AutomatonAccept &other) const {
    return (branch_depth_ == other.branch_depth_) && (tile_id_ == other.tile_id_);
  }
  int branch_depth_ = 0;
  size_t tile_id_ = 0;
};

class AutomatonNode {
  public:
  size_t num_ = 0;

  AutomatonNode *parent_ = nullptr;
  std::vector<AutomatonNode *> children_;
  
  // Transition from parent to this node
  bool is_operand_ = false;

  // If is_operand_, this is the index of the operand, if not, this is the index of the symbol.
  // (The index of the symbol first indexes TillyParser::symbols_, if it is a label, it indexes
  //  TillyParser::labels_ at offset (index_ - TillyParser::symbols_.size()).)
  size_t index_ = 0;
  
  size_t acceptance_offset_ = 0; // offset to the first acceptance for this node, calculated when emitting code.
  std::vector<AutomatonAccept> accepting_ids_;

  AutomatonNode *failure_func_ = nullptr;

  AutomatonNode *follow_opd(size_t operand);
  AutomatonNode *follow_sym(size_t symbol);
  AutomatonNode *follow_node(AutomatonNode *transition);
  AutomatonNode *succ_opd(size_t operand);
  AutomatonNode *succ_sym(TillyParser &tp, size_t symbol);

  void detach();
  void attach(AutomatonNode *parent);

  size_t count_depth();
};

class TillyParser {
  public:
  TillyParser();
  ~TillyParser();

  void add_tile_to_automaton(AutomatonNode *an, ASTNode *tn, int depth, size_t tile_num);
  void find_failure_function(AutomatonNode *an);

  AutomatonNode *make_sym_edge(AutomatonNode *parent, size_t symbol);
  AutomatonNode *make_opd_edge(AutomatonNode *parent, size_t symbol);

  std::string input_filename_;
  bool finished_ = false;

  bool had_failure = false;
  void set_failure() { had_failure = true; }

  std::vector<std::string> labels_;
  std::vector<std::string> symbols_;
  std::list<std::pair<std::string, std::string> > label_types_; /* list and not map to keep the order of declaration intact */

  std::vector<std::unique_ptr<ASTTile> > ast_tiles_;
  
  std::vector<std::unique_ptr<AutomatonNode> > automaton_nodes_;

  void add_tile(Situs &situs, const char *label, std::unique_ptr<ASTNode> &root, Situs &cost_snippet_situs, size_t cost_snippet_start, size_t cost_snippet_end, Situs &action_snippet_situs, size_t action_snippet_start, size_t action_snippet_end);

  /* Switches true/false, if true, section code goes into code_prologue_section_, otherwise it
   * goes into tiles. Switches by the directives parser (process_directives()). */
  bool emit_to_code_prologue_section_ = true;
  std::vector<uint8_t> code_prologue_section_;
  std::vector<uint8_t> current_code_section_;

  tiles_stack          tiles_parser_;
  bool                 have_tiles_data_to_process_ = false;
  std::vector<uint8_t> dis_to_tiles_section_;
  Situs                dis_to_tiles_situs_;

  directives_stack     directives_;
  bool                 have_dis_to_directives_data_to_process_ = false;
  std::vector<uint8_t> dis_to_directives_data_;
  Situs                dis_to_directives_situs_;

  bool                 have_dis_to_section_data_to_process = false;
  Situs                dis_to_section_situs_;
  std::vector<uint8_t> dis_to_section_data_; // this is the processed data, whereas the section should get the raw data.

  Situs                dis_output_situs_;
  std::vector<uint8_t> dis_output_data_;
  dis_stack            dis_;
  bool                 have_dis_data_to_process_ = false;
  std::vector<uint8_t> mlc_to_dis_data_;
  Situs                dis_situs_;

  bool                 mlc_finished_ = false;
  Situs                mlc_output_situs_;
  mlc_stack            mlc_;
  bool                 have_mlc_data_to_process_ = false;
  std::vector<uint8_t> lc_to_mlc_data_;
  Situs                lc_to_mlc_situs_;

  lc_stack             lc_;
  bool                 have_lc_data_to_process_ = false;

  FILE *fp_ = nullptr;

  char buf_[4096];

  // We keep track of all input so we can get random access to it by byte offset
  std::vector<uint8_t> total_input_;

  void process_tiles();
  void process_directives();
  void process_dis();
  void process_mlc();
  void process_lc();
  void process();

  void dump_automaton(AutomatonNode *an, int indent = 0);

  void semantic_analysis();

  void emit_snippet(FILE *fp, ASTTile *tile, bool emit_cost_tile_otherwise_action);
  void write_output(FILE *fp);
  
  void emit_tile_descendant_execution(FILE *fp, ASTTile &tile, ASTNode &node, std::string path_to_node);
};

#endif // TILLY_PARSER_H
