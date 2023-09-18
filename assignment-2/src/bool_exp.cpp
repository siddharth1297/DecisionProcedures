#include "dpll.hpp"

/*
 * https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c#:~:text=147-,For%20string%20delimiter,-Split%20string%20based
 */
static vector<string> Split(const string &s, const string &delimiter) {
  size_t pos_start = 0, pos_end, delim_len = delimiter.length();
  std::string token;
  std::vector<std::string> res;

  while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
    token = s.substr(pos_start, pos_end - pos_start);
    pos_start = pos_end + delim_len;
    if (!token.empty())
      res.push_back(token);
  }
  res.push_back(s.substr(pos_start));
  return res;
}

BoolFunction::BoolFunction(const string &file_name) {
  ParseDimacsFile(file_name);
  assert(n_var_ && n_clauses_);
  assignment_.resize(n_var_ + 1, -1);
  frequency_sign_.resize(n_var_ + 1, {0, 0});
  for (auto &clause : functions_) {
    for (auto &lit : clause) {
      assert(lit != 0);
      if (lit > 0) {
        frequency_sign_[lit].first++;
      } else {
        frequency_sign_[-1 * lit].second++;
      }
    }
  }
}

BoolFunction::BoolFunction(const BoolFunction &bool_func)
    : n_clauses_(bool_func.n_clauses_), n_var_(bool_func.n_var_),
      functions_(bool_func.functions_), assignment_(bool_func.assignment_),
      frequency_sign_(bool_func.frequency_sign_) {}

void BoolFunction::ParseDimacsFile(const string &file_name) {
  cout << "File: " << file_name << endl;
  size_t cluses_read = 0;
  fstream ip_stream(file_name, std::fstream::in);
  string line;
  while (getline(ip_stream, line)) {
    if (line.empty() || line[0] == 'c') {
      continue;
    }
    vector<string> words = Split(line, " ");
    if (line[0] == 'p') {
      int n_clauses = atoi(words[3].c_str());
      int n_var = atoi(words[2].c_str());
      assert(n_clauses > 0);
      assert(n_var > 0);
      n_var_ = n_var;
      cluses_read = n_clauses_ = n_clauses;
      continue;
    }

    assert(words.size() > 0 && words.back() == "0");

    vector<int> clause;
    for (int i = 0; i < words.size() - 1; i++) {
      int num = atoi(words[i].c_str());
      if (num == 0) {
        cout << "Word: " << words[i] << " " << (words[i].c_str()) << endl;
        cout << line << endl;
        for (auto word : words) {
          cout << "\"" << word << "\" ";
        }
        cout << endl;
        assert(false);
      }
      clause.push_back(num);
    }
    functions_.push_back(clause);

    cluses_read--;
    if (cluses_read == 0) {
      break;
    }
  }
  ip_stream.close();
}

void BoolFunction::DumpAssignment() {
  cout << "ASSIGNMENT: ";
  for (size_t i = 1; i < assignment_.size(); i++) {
    if (true || assignment_[i] != -1) {
      cout << "(" << i << "->" << assignment_[i] << ") ";
    }
  }
  cout << endl;
}
