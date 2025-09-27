#include "Compiler.hpp"

template <typename T> int Compiler::index(const std::vector<T>& vect, const T& element) const {
  for (int i = 0; i < vect.size(); i++) {
    if (vect[i] == element) return i;
  }
  return -1;
}

template <typename T> std::string Compiler::state_to_bin(T state) const {
  std::string encoded_state;
  state++;

  if (!state) { return mul("0", state_length); }
  while (state > 0) {
    encoded_state = std::to_string(state % 2) + encoded_state;
    state /= 2;
  }
  while (encoded_state.size() < state_length - 1) {
    encoded_state = "0" + encoded_state;
  }
  return "1" + encoded_state;
}