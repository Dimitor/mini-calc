#include <vector>
#include <string>
#include <optional>
#include <stdexcept>
#include <tuple>
#include <sstream>
#include <cmath>

using func_t = std::vector<char> ;
using operand_t = std::vector<double>;

struct Calculator
{
    Calculator(char const *input_str);

    Calculator();

    Calculator(Calculator const &calculator);

    std::optional<double> result() const;

    double calc(char const *input_str);

private:
    operand_t operands;
    func_t functions;
    enum class TokenType;
    std::string input;
    size_t pos;
    TokenType prev_token_type;

    void clear();

    void check_brackets_seq() const;

    std::string make_err_msg(std::string const &msg, size_t pos) const;

    std::tuple<std::string, TokenType> next_token();

    bool is_func(char fn) const;

    std::optional<double> num_parse(std::string const &strnum) const;

    void process_number(std::string const &token);

    void process_function(std::string const &token);

    void check_missed_function(std::string const &token, 
                               TokenType const &token_type) const;

    bool is_function_missed(std::string const &token, 
                                   TokenType const &token_type) const;

    void check_missed_operand(std::string const &token, 
                              TokenType const &token_type) const;

    bool is_operand_missed(std::string const &token,
                           TokenType const &token_type) const;

    bool is_unary(char func) const;

    void pop_if_possible(char func);

    bool can_pop(char func) const;

    int priority(char func) const;

    void pop_fn();
};
