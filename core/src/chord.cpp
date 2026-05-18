#include "chord.h"
#include <cmath>
#include <cctype>
#include <stdexcept>
#include <string>

namespace pd::math {

ChordResult chord(const std::function<double(double)>& f, double a, double b, double eps, int maxIter) {
    double fa = f(a), fb = f(b);
    if (fa * fb >= 0)
        throw std::invalid_argument("chord: f(a)*f(b) must be < 0");
    double x = a;
    int it = 0;
    for (; it < maxIter; ++it) {
        x  = a - fa * (b - a) / (fb - fa);
        double fx = f(x);
        if (std::fabs(fx) < eps) return {x, it+1, std::fabs(fx), true};
        if (fa * fx < 0) { b = x; fb = fx; }
        else             { a = x; fa = fx; }
    }
    return {x, it, std::fabs(f(x)), false};
}

// -------- tiny expression parser (recursive descent) --------
namespace {
struct Parser {
    const std::string& s; size_t i; double X;
    Parser(const std::string& s_, double x):s(s_),i(0),X(x){}
    void skip() { while (i<s.size() && std::isspace((unsigned char)s[i])) ++i; }
    bool match(char c){ skip(); if (i<s.size()&&s[i]==c){++i;return true;} return false; }
    bool peekId(const char* id){
        skip(); size_t n=std::string(id).size();
        if (i+n<=s.size() && s.compare(i,n,id)==0 && (i+n==s.size() || !std::isalnum((unsigned char)s[i+n])))
        { i+=n; return true; }
        return false;
    }
    double parseExpr(){ double v=parseTerm(); while(true){ skip(); if(match('+'))v+=parseTerm(); else if(match('-'))v-=parseTerm(); else break;} return v;}
    double parseTerm(){ double v=parsePow(); while(true){ skip(); if(match('*'))v*=parsePow(); else if(match('/'))v/=parsePow(); else break;} return v;}
    double parsePow(){ double v=parseUnary(); skip(); if(match('^')){double e=parseUnary(); v=std::pow(v,e);} return v;}
    double parseUnary(){ skip(); if(match('-'))return -parseUnary(); if(match('+'))return parseUnary(); return parseAtom();}
    double parseAtom(){
        skip();
        if (match('(')) { double v=parseExpr(); match(')'); return v; }
        if (peekId("sin")) { match('('); double v=parseExpr(); match(')'); return std::sin(v); }
        if (peekId("cos")) { match('('); double v=parseExpr(); match(')'); return std::cos(v); }
        if (peekId("exp")) { match('('); double v=parseExpr(); match(')'); return std::exp(v); }
        if (peekId("log")) { match('('); double v=parseExpr(); match(')'); return std::log(v); }
        if (peekId("sqrt")){ match('('); double v=parseExpr(); match(')'); return std::sqrt(v); }
        if (peekId("x"))   return X;
        // number
        size_t st=i;
        while (i<s.size() && (std::isdigit((unsigned char)s[i])||s[i]=='.')) ++i;
        if (st==i) throw std::invalid_argument("expr parse error at: "+s.substr(i));
        return std::stod(s.substr(st,i-st));
    }
};
}

double evalExpr(const std::string& expr, double x) {
    Parser p(expr, x);
    return p.parseExpr();
}

} // namespace pd::math
