#pragma once
#include <array>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class Compiler {
  public:
    Compiler() {}
    Compiler(const std::string& filename);
    std::vector<std::string> read_from_file(const std::string& filename) const;
    std::vector<std::string> compile_all() const;
    void save_to_file(const std::vector<std::string>& result) const;

    int get_headpos() const { return headpos; }
    int get_state_length() const { return state_length; }
    int get_header_length() const { return header_length; }
    std::string get_outfile() const { return outfile; }
    std::string get_header_space() const { return header_space; }

  protected:
    int headpos = 0, state_length = 2, header_length = 0;
    std::string outfile, header_space;

    void create_data(const std::vector<std::string>& config, const std::vector<std::string>& transitions);
    std::vector<std::string> read_section(const std::vector<std::string>& data, const std::string& section_name) const;
    std::string comment(const std::string& message) const;
    template <typename T> int index(const std::vector<T>& vect, const T& element) const;
    template <typename T> std::string state_to_bin(T state) const;

    // Brainfuck instructions; not tested (except mul) because there's no point
    std::string mul(const std::string& str, int multiplier) const;
    std::string INITIALIZE_HEADERS() const;
    std::string SET_Q() const;
    std::string SET_S() const;
    std::string MOVE_HEAD() const;
    std::string LOAD_HEADER() const;
    std::string STORE_HEADER() const;
    std::string GOTO_T() const;
    std::string COME_FROM_T() const;
    std::string GOTO_H() const;
    std::string COME_FROM_H() const;
    std::string S(int i) const;
    std::string C(int offset) const;
    std::string FULLCOPY_0() const;
    std::string FULLCOPY_1() const;
};

#include "Compiler_impl.hpp"