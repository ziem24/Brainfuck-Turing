#include "Compiler.hpp"

Compiler::Compiler(const std::string& filename) {
  std::vector<std::string> data = read_from_file(filename);
  std::vector<std::string> config = read_section(data, "CONFIG");
  std::vector<std::string> transitions = read_section(data, "TRANSITIONS");
  header_space = create_data(config, transitions);
}

std::vector<std::string> Compiler::read_from_file(const std::string& filename) const {
  std::ifstream is(filename);
  std::vector<std::string> data;
  std::string word;

  if (!is.is_open()) { throw std::invalid_argument("Error: Cannot read file \"" + filename + "\""); }
  while (is >> word) { data.push_back(word); }
  return data;
}

void Compiler::save_to_file(const std::vector<std::string>& result) const {
  std::ofstream os(outfile);
  for (const std::string& s : result) { os << s << std::endl; }
}

std::vector<std::string> Compiler::read_section(const std::vector<std::string>& data, const std::string& section_name) const {
  std::vector<std::string> section;
  bool is_section , is_comment;

  for (int i = 0; i < data.size(); i++) {
    if (data[i] == "/*") { is_comment = true;}
    else if (data[i] == "*/") { is_comment = false; continue; }

    if (data[i] == ".section" && !is_comment && i < data.size() - 1) {
      if (data[i + 1] == section_name) {
        is_section = true;
        i++;
        continue;
      }
      else { is_section = false; }
    }
    if (is_section && !is_comment) { section.push_back(data[i]); }
  }

  return section;
}

std::string Compiler::create_data(const std::vector<std::string>& config, const std::vector<std::string>& transitions) {
  std::vector<std::string> states, encoded_headers;
  std::string header_space, current;

  for (int i = 0; i < config.size() - 1; i++) {
    if (config[i] == "FILE") { outfile = config[i + 1]; }
    else if (config[i] == "HEADPOS") { headpos = std::stoi(config[i + 1]); }
  }
  if (outfile.empty() || headpos <= 0) { throw std::invalid_argument("Incorrect FILE or HEADPOS definition in the CONFIG section."); }

  for (int i = 0; i < transitions.size(); i+=8) {
    for (const std::string& v : states) {
      if (v == transitions[i]) { throw std::invalid_argument("Error: Cannot redefine '" + v + "'."); }
    }
    states.push_back(transitions[i]);
  }

  // k = state_length; header_length = header_length
  int s = states.size() - 1;
  while (s) { s /= 2; state_length++; }
  header_length = 3 * state_length + 8;

  for (int i = 0; i < transitions.size(); i+=8) {
    encoded_headers.push_back(
      "0"
      + state_to_bin(index(states, transitions[i + 2]))
      + transitions[i + 3]
      + std::to_string(index({"L", "R"}, transitions[i + 4]))
      + state_to_bin(index(states, transitions[i + 5]))
      + transitions[i + 6]
      + std::to_string(index({"L", "R"}, transitions[i + 7]))
      + mul("0", state_length + 3)
    );
  }

  for (auto v : encoded_headers) {
    if (v.size() != header_length) { throw std::invalid_argument("Error: badly encoded transition function"); }
  }
  current = "0" + state_to_bin(index(states, transitions[2])) + transitions[3]
    + std::to_string(index({"L", "R"}, transitions[4]));
  for (const std::string& s : encoded_headers) { header_space = s + header_space; }

  return header_space + current;
}

std::string Compiler::_msg(const std::string& message, bool newlines) const {
  if (newlines) { return "\n   // " + message + "\n"; }
  else { return "    // " + message; }
}

// BRAINFUCK INSTRUCTION SEQUENCES

std::vector<std::string> Compiler::compile_all() const {
  // Dependencies: mul, INITIALIZE_HEADERS, SET_Q, SET_S, MOVE_HEAD, LOAD_HEADER, STORE_HEADER
  std::vector<std::string> instructions = {
    "========== TURING_MAIN ==========\n",
    _msg("INITIALIZE_HEADERS"),
    INITIALIZE_HEADERS(),
    _msg("INITIALIZE_TAPE"),
    mul(">>+", headpos),
    mul("<", headpos * 2 + state_length + 2),
    "[-<",
    _msg("SET_Q; SET_S; MOVE_HEAD"),
    SET_Q(),
    SET_S(),
    MOVE_HEAD(),
    _msg("LOAD_HEADER"),
    LOAD_HEADER(),
    _msg("STORE_HEADER"),
    STORE_HEADER(),
    _msg("Next loop"),
    GOTO_T(),
    "[-" + mul(">", header_length) + "]",
    ">]",
    _msg("Post halt handling"),
    mul(">",state_length),
    SET_S(),
    MOVE_HEAD()
  };

  return instructions;
}

std::string Compiler::mul(const std::string& str, int multiplier) const {
  std::string new_str;
  for (int i = 0; i < multiplier; i++) { new_str += str; }
  return new_str;
}

std::string Compiler::INITIALIZE_HEADERS() const {
  // ending point: Theta
  std::string instruction;
  for (const char& c : header_space) {
    if (c == '1') { instruction += "+"; }
    instruction += ">";
  }
  return instruction;
}

std::string Compiler::SET_Q() const {
  // starting/ending point: Omega / sigma
  std::string instruction = ">>";
  for (int i = 2; i <= state_length; i++) {
    instruction += S(i) + ">";
  }
  return instruction;
}

std::string Compiler::SET_S() const {
  // starting/ending point: sigma / mu
  return "[>>>>[>>]<+<[<<]<<-]>";
}

std::string Compiler::MOVE_HEAD() const {
  // starting/ending point: mu / Omega
  return "[>+<-]>[->>[>>]+>>+>>]>>[>>]<<<<[>>]<<-<<[<<]" + mul("<", state_length + 3);
}

std::string Compiler::LOAD_HEADER() const {
  // starting/ending point: Omega
  // dependencies: mul, GOTO_H, COME_FROM_H, GOTO_T, COME_FROM_T, FULLCOPY_0, FULLCOPY_1
  return
  GOTO_T()
  + ">\n"
  + FULLCOPY_0()
  + COME_FROM_T()
  + GOTO_H()
  + "["
  + COME_FROM_H()
  + GOTO_T()
  + mul(">", 2 * state_length + 5)
  + mul("[-]>", state_length + 2)
  + mul("<", 2 * state_length + 4)
  + FULLCOPY_1()
  + COME_FROM_T()
  + GOTO_H()
  + "-]"
  + COME_FROM_H();
}

std::string Compiler::STORE_HEADER() const {
  // starting/ending point: Omega
  // dependencies: mul, GOTO_T, COME_FROM_T
  int offs = 2 * state_length + 5;
  std::string instruction = GOTO_T();
  for (int i = 0; i < state_length + 2; i++) {
    instruction +=
      "\n"
      + mul(">", offs)
      + "["
      + mul(">", header_length - offs)
      + COME_FROM_T()
      + mul(">", i + 1)
      + "+"
      + mul("<", i + 1)
      + GOTO_T()
      + mul(">", offs)
      + "-]"
      + mul("<", offs);
    offs++;
  }
  instruction += "\n" + COME_FROM_T();
  return instruction;
}

std::string Compiler::GOTO_T() const {
  // starting/ending point: Omega / I_T
  // dependencies: mul
  return mul("<", header_length) + "[" + mul("<", header_length) + "]" + mul(">", header_length);
}

std::string Compiler::COME_FROM_T() const {
  // starting/ending point: I_T / Omega
  // dependencies: mul
  return "[" + mul(">", header_length) + "]";
}

std::string Compiler::GOTO_H() const {
  // starting/ending point: Omega / HEAD
  // dependencies: mul
  return mul(">", state_length + 5) + "[>>]<";
}

std::string Compiler::COME_FROM_H() const {
  // starting/ending point: HEAD / Omega
  // dependencies: mul
  return "<[<<]" + mul("<", state_length + 3);
}

std::string Compiler::S(int i) const {
  // starting/ending point: chi_i
  // dependencies: mul
  return
  "["
  + mul("<", header_length + i)
  + "[" + mul("<", header_length) + "]+"
  + mul(mul("<", header_length) + "+", (1 << (state_length - i)) - 1)
  + "[" + mul(">", header_length) + "]"
  + mul(">", i)
  + "-]";
}

std::string Compiler::C(int offset) const {
  // starting/ending point: any
  // dependencies: mul
  return
    "[" + mul(">", offset) + "+>+" + mul("<", offset + 1) + "-]" + mul(">", offset + 1)
  + "[" + mul("<", offset+1) + "+" + mul(">", offset + 1) + "-]" + mul("<", offset + 1);
}

std::string Compiler::FULLCOPY_0() const {
  // starting/ending point: q_01 / I_T
  // dependencies: mul, C
  return mul(C(2 * state_length + 4) + ">\n", state_length + 2) + mul(">", 2 * state_length + 5);
}

std::string Compiler::FULLCOPY_1() const {
  // starting/ending point: q_11 / I_T
  // dependencies: mul, C
  return mul(C(state_length + 2) + ">\n", state_length + 2) + mul(">", state_length + 3);
}
