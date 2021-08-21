#include "calculator.h"

enum class Calculator::TokenType
{
    NUMBER,
    FUNCTION,
    NOTHING
};

Calculator::Calculator(char const *input_str)
    : operands       {}
    , functions      {}
    , input          {}
    , pos            {}
    , prev_token_type{TokenType::NOTHING}
{
    calc(input_str);
}

Calculator::Calculator()
    : operands       {}
    , functions      {}
    , input          {}
    , pos            {}
    , prev_token_type{TokenType::NOTHING}
{}

Calculator::Calculator(Calculator const &calculator)
    : operands       {}
    , functions      {}
    , input          {}
    , pos            {}
    , prev_token_type{TokenType::NOTHING}
{
    operands.assign(calculator.operands.begin(),
                    calculator.operands.end());
    functions.assign(calculator.functions.begin(),
                     calculator.functions.end());
}

inline std::optional<double> Calculator::result() const
{
    return operands.empty() ? std::nullopt : std::make_optional(operands[0]);
}

double Calculator::calc(char const *input_str)
{
    clear();
    input = "(";
    input += input_str;
    input += ")";

    check_brackets_seq();
    while (pos < input.length())
    {
        if (std::isspace(input[pos]))
        {
            ++pos;
            continue;
        }

        std::string token;
        TokenType   token_type;
        std::tie(token, token_type) = next_token();
        if (token_type == TokenType::NUMBER)
            process_number(token);
        else if (token_type == TokenType::FUNCTION)
            process_function(token);
    }

    return operands.back();
}

void Calculator::clear()
{
    input.clear();
    operands.clear();
    functions.clear();
    pos = 0;
    prev_token_type = TokenType::NOTHING;
}

void Calculator::check_brackets_seq() const
{
    long long brackets_balance{};

    for (size_t i = 0; i < input.length(); ++i)
    {
        brackets_balance += (input[i] == '(');
        brackets_balance -= (input[i] == ')');

        if (brackets_balance < 0)
            throw std::logic_error(make_err_msg("Invalid brackets sequence!",
                                                i));
    }
}

std::string Calculator::make_err_msg(std::string const &msg, size_t pos) const
{
    size_t minpref = std::min(pos, size_t(5));
    size_t minsuf  = std::min(input.length() - pos, size_t(3));

    std::stringstream err_msg;

    bool has_shift = false;
    err_msg << "Error \"" << msg << "\" in position " << pos << ":\n";
    err_msg << std::string(40, '-') << "\n";
    if (pos > 5){
        err_msg << "...";
        has_shift = true;
    }
    if (pos > 0)
        err_msg << input.substr(pos - minpref, minpref);
    err_msg << input.substr(pos, minsuf);
    if (input.length() - pos > 5)
        err_msg << "...";
    err_msg << "\n";
    if (has_shift)
        err_msg << "   ";
    err_msg << std::string(minpref + minsuf, '^') << "\n";
    err_msg << std::string(40, '-');
    return err_msg.str();
}

std::tuple<std::string, Calculator::TokenType> Calculator::next_token()
{
    if (is_func(input[pos]))
    {
        ++pos;
        return std::make_tuple(std::string(1, input[pos - 1]),
                               Calculator::TokenType::FUNCTION);
    }
    else
    {
        size_t start = pos;
        while (!(std::isspace(input[pos]) || is_func(input[pos]))
               && pos < input.length())
        {
            ++pos;
        }

        return std::make_tuple(input.substr(start, pos - start),
                               Calculator::TokenType::NUMBER);
    }
}

bool Calculator::is_func(char fn) const
{
    switch (fn)
    {
    case '(':
    case ')':
    case '+':
    case '-':
    case '*':
    case '/':
    case '^':
        return true;
    default:
        return false;
    }
}

std::optional<double> Calculator::num_parse(std::string const &strnum) const
{
    if (!std::isdigit(strnum[0]))
        return std::nullopt;

    size_t parse_num_end;
    double res = stod(strnum, &parse_num_end);

    if (parse_num_end == strnum.length())
        return res;

    return std::nullopt;
}

void Calculator::process_number(std::string const &token)
{
    std::optional<double> num = num_parse(token);
    if (num.has_value())
        operands.push_back(num.value());
    else
        throw std::logic_error(make_err_msg("Invalid number or "
                                            "unknown function!", 
                                            pos - token.length()));
    
    check_missed_function(token, TokenType::NUMBER);
    prev_token_type = TokenType::NUMBER;
}

void Calculator::process_function(std::string const &token)
{
    char func = token[0];
    
    check_missed_operand(token, TokenType::FUNCTION);
    check_missed_function(token, TokenType::FUNCTION);

    if (is_unary(func))
        operands.push_back(0);

    pop_if_possible(func);

    if (func != ')')
    {
        functions.push_back(func);
        prev_token_type = TokenType::FUNCTION;
    } 
    else
    {
        prev_token_type = TokenType::NUMBER;
    }
        
}

inline void Calculator::check_missed_function(std::string const &token, 
                                              TokenType const &token_type) const
{
    if (is_function_missed(token, token_type))
        throw std::logic_error(make_err_msg("Missing function!", 
                                            pos - token.length()));
}

inline bool Calculator::is_function_missed(std::string const &token, 
                                           TokenType const &token_type) const
{
    return prev_token_type == TokenType::NUMBER && 
           (token == "(" || token_type == TokenType::NUMBER);
}

inline void Calculator::check_missed_operand(std::string const &token, 
                                             TokenType const &token_type) const
{
    if (is_operand_missed(token, token_type))
        throw std::logic_error(make_err_msg("Missing operand!", 
                                            pos - token.length()));
}

inline bool Calculator::is_operand_missed(std::string const &token, 
                                          TokenType const &token_type) const
{
    return !is_unary(token[0]) && 
           prev_token_type == TokenType::FUNCTION &&
           token_type == TokenType::FUNCTION && token != "(";
}

inline bool Calculator::is_unary(char func) const
{
    return (func == '-' || func == '+') && 
           (prev_token_type == TokenType::NOTHING || 
           (prev_token_type == TokenType::FUNCTION && functions.back() == '('));
}

void Calculator::pop_if_possible(char func)
{
    if (func == ')')
    {
        while (functions.back() != '(')
            pop_fn();
        
        functions.pop_back();    
    } 
    else
    {
        while (can_pop(func))
            pop_fn();
    }
}

bool Calculator::can_pop(char func) const
{
    if (functions.empty())
        return false;

    int func_pr = priority(func);
    int last_pr = priority(functions.back());

    return func_pr > 0 && last_pr > 0 && func_pr >= last_pr;
}

int Calculator::priority(char func) const
{
    switch (func)
    {
    case '(':
        return -1;
    case '^':
        return 1;
    case '*': case '/':
        return 2;
    case '+': case '-':
        return 3;
    default:
        throw std::logic_error(make_err_msg("Unknown function!", 
                                            pos - 1));
    }
}

void Calculator::pop_fn()
{
    double op2 = operands.back();
    operands.pop_back();
    double op1 = operands.back();
    operands.pop_back();
    char func = functions.back();
    functions.pop_back();
    
    switch (func)
    {
    case '^':
        op1 = std::pow(op1, op2);
        break;
    case '*': 
        op1 *= op2;
        break;
    case '/':
        op1 /= op2;
        break;
    case '+':
        op1 += op2;
        break;
    case '-':
        op1 -= op2;
        break;
    default:
        throw std::logic_error(make_err_msg("Unknown function!", 
                                            pos - 1));
    }
    
    operands.push_back(op1);
}