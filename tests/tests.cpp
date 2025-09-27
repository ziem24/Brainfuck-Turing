#include <gtest/gtest.h>
#include "../src/Compiler.hpp"

const std::string CWD = "../../tests/";

class DummyCompiler : public Compiler {
  public:
    void set_state_length(int new_state_length) { state_length = new_state_length; }

    void dummy_create_data(const std::vector<std::string>& config, const std::vector<std::string>& transitions) {
      create_data(config, transitions);
    }
    std::vector<std::string> dummy_read_section(const std::vector<std::string>& data, const std::string& section_name) const {
      return read_section(data, section_name);
    }
    std::string dummy_comment(const std::string& message) const {
      return comment(message);
    }
    template <typename T> int dummy_index(const std::vector<T>& vect, const T& element) const {
      return index(vect, element);
    }
    template <typename T> std::string dummy_state_to_bin(T state) const {
      return state_to_bin(state);
    }
    std::string dummy_mul(const std::string& str, int multiplier) const {
      return mul(str, multiplier);
    }
};

TEST(Compiler, read_from_file) {
  DummyCompiler dc;
  std::vector<std::string> expect = { "Hello", "world", "!", "awawawawa", "!", "!" };
  std::vector<std::string> output = dc.read_from_file(CWD + "file_readtest.txt");
  EXPECT_EQ(expect, output);
  EXPECT_THROW(dc.read_from_file("bad file"), std::invalid_argument);
}

TEST(Compiler, read_section) {
  DummyCompiler dc;
  std::vector<std::string> data {
    ".section", "one", "when your tryna be", "nonchalant on the",
    ".section", "two", "skype call in 2008",
    "/*", "TECHNO IST TRUE", ".section", "one", "GROMMLER", "*/",
    ".section", "one",
    ".section", "two", "but yo hb", "talking bout sum", "\"trololololololo!\"",
    ".section", "one"
  };
  std::vector<std::string>
  section1 = dc.dummy_read_section(data, "one"),
  expect_1 = { "when your tryna be", "nonchalant on the" },
  section2 = dc.dummy_read_section(data, "two"),
  expect_2 = {"skype call in 2008", "but yo hb", "talking bout sum", "\"trololololololo!\""};

  EXPECT_EQ(section1, expect_1);
  EXPECT_EQ(section2, expect_2);
}

TEST(Compiler, create_data) {
  DummyCompiler dc;
  std::vector<std::string>
  data = dc.read_from_file(CWD + "file_config.txt"),
  conf = dc.dummy_read_section(data, "CONFIG"),
  trans = dc.dummy_read_section(data, "TRANSITIONS");

  /*
  Expected header space:
  0 1100 1 1 1001 0 1 0000000     (st_D)
  0 0000 1 1 1100 1 0 0000000     (st_C)
  0 1001 1 0 1011 0 0 0000000     (st_B)
  0 1010 1 1 1010 1 0 0000000     (st_A)
  0 1010 1 1                    (Omega + C)
  */

  std::string hs = "011001110010100000000000011110010000000001001101011000000000010101110101000000000101011";
  EXPECT_NO_THROW(dc.dummy_create_data(conf, trans));
  EXPECT_EQ(dc.get_outfile(), "../file.bf");
  EXPECT_EQ(dc.get_headpos(), 25);
  EXPECT_EQ(dc.get_state_length(), 4);
  EXPECT_EQ(dc.get_header_length(), 20);  // 3 * state_length + 8
  EXPECT_EQ(dc.get_header_space(), hs);
}

TEST(Compiler, create_data_fails) {
  DummyCompiler dc;
  std::vector<std::string> data, conf, trans;

  EXPECT_THROW(dc.dummy_create_data({}, {}), std::invalid_argument);

  data = dc.read_from_file(CWD + "file_config_csfail1.txt");
  conf = dc.dummy_read_section(data, "CONFIG");
  trans = dc.dummy_read_section(data, "TRANSITIONS");
  EXPECT_THROW(dc.dummy_create_data(conf, trans), std::invalid_argument);

  data = dc.read_from_file(CWD + "file_config_csfail2.txt");
  conf = dc.dummy_read_section(data, "CONFIG");
  trans = dc.dummy_read_section(data, "TRANSITIONS");
  EXPECT_THROW(dc.dummy_create_data(conf, trans), std::invalid_argument);

  data = dc.read_from_file(CWD + "file_config_tsfail_bad.txt");
  conf = dc.dummy_read_section(data, "CONFIG");
  trans = dc.dummy_read_section(data, "TRANSITIONS");
  EXPECT_THROW(dc.dummy_create_data(conf, trans), std::invalid_argument);

  data = dc.read_from_file(CWD + "file_config_tsfail_mod8.txt");
  conf = dc.dummy_read_section(data, "CONFIG");
  trans = dc.dummy_read_section(data, "TRANSITIONS");
  EXPECT_THROW(dc.dummy_create_data(conf, trans), std::invalid_argument);

  data = dc.read_from_file(CWD + "file_config_tsfail_redef.txt");
  conf = dc.dummy_read_section(data, "CONFIG");
  trans = dc.dummy_read_section(data, "TRANSITIONS");
  EXPECT_THROW(dc.dummy_create_data(conf, trans), std::invalid_argument);
}

TEST(Compiler, comment) {
  DummyCompiler dc;
  EXPECT_EQ(dc.dummy_comment("trololo!"), "    // trololo!");
}

TEST(Compiler, index) {
  DummyCompiler dc;
  std::vector<int> vect = {1, 3, 3, 7, 6, 9, 4, 2, 0};
  EXPECT_EQ(dc.dummy_index(vect, 9), 5);
  EXPECT_EQ(dc.dummy_index(vect, 3), 1);
  EXPECT_EQ(dc.dummy_index(vect, 8), -1);
}

TEST(Compiler, state_to_bin) {
  DummyCompiler dc;
  dc.set_state_length(7);
  // state length thats too small should not happen
  EXPECT_EQ(dc.dummy_state_to_bin(4), "1000101");
  EXPECT_EQ(dc.dummy_state_to_bin(9), "1001010");
  EXPECT_EQ(dc.dummy_state_to_bin(-1), "0000000");
}

TEST(Compiler, mul) {
  DummyCompiler dc;
  EXPECT_EQ(dc.dummy_mul("awawa!", 3), "awawa!awawa!awawa!");
  EXPECT_EQ(dc.dummy_mul("^>VVV", 0), "");
  EXPECT_EQ(dc.dummy_mul("owo uwu rawr XD", -999), "");
}