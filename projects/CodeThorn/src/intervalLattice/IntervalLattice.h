#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>

// Author: Markus Schordan, 2014.

// represents one interval
// Requirements: Type must support algorithms: min, max; operators: '<','>'; values: -1, +1.
template<typename Type>
class IntervalLattice {
  // creates an interval with a known left and right boundary
 public:
 IntervalLattice():_isLowInf(true),_isHighInf(true) {}
 IntervalLattice(Type left, Type right):_low(left),_high(right),_isLowInf(false),_isHighInf(false) {} 
  static IntervalLattice highInfInterval(Type left) {
    IntervalLattice t;
    t.setIsLowInf(false);
    t._low=left;
    return t;
  }
  static IntervalLattice lowInfInterval(Type right) {
    IntervalLattice t;
    t.setIsHighInf(false);
    t._high=right;
    return t;
  }
  bool isTop() {
    return isLowInf() && isHighInf();
  }
  void setTop() {
    return setIsLowInf(true) && isHighInf(true);
  }
  bool isBot() {
    return isEmpty();
  }
  void setBot() {
    setEmpty();
  }
  bool isEmpty() {
    return !isLowInf()&&!isHighInf()&&_low>_high;
  }
  void setEmpty() {
    // unified empty interval [+1,-1] (also to ensure proper operation of default operator==)
    setIsLowInf(false);
    setIsHighInf(false);
    _low=+1;
    _high=-1;
  }
  bool isConst() {
    return !isLowInf()&&!isHighInf()&&_low==_high;
  }
  bool isLowInf() {
    return _isLowInf;
  }
  bool isHighInf() {
    return _isHighInf;
  }
  void setLow(Type val) {
    setIsLowInf(false);
    _low=val;
    unifyEmptyInterval();
  }
  void setHigh(bool val) {
    setIsHighInf(false);
    _high=val;
    unifyEmptyInterval();
  }
  void setLowInf() {
    setIsLowInf(true);
    _low=-1;
  }
  void setHighInf() {
    setIsHighInf(true);
    _high=+1;
  }
  Type getLow() { assert(!isLowInf); return _low; }
  Type getHigh() { assert(!isHighInf); return _high; }
   
  static IntervalLattice top() {
    IntervalLattice t;
    t.setTop();
    return t;
  }
  static IntervalLattice bot() {
    IntervalLattice t;
    t.setBot();
    return t;
  }
  static IntervalLattice join(IntervalLattice l1, IntervalLattice l2) {
    IntervalLattice l3=l1;
    l3.join(l2);
    return l3;
  }
  static IntervalLattice meet(IntervalLattice l1, IntervalLattice l2) {
    IntervalLattice l3=l1;
    l3.meet(l2);
    return l3;
  }
  static IntervalLattice arithAdd(IntervalLattice l1, IntervalLattice l2) {
    IntervalLattice l3=l1;
    l3.arithAdd(l2);
    return l3;
  }
  static IntervalLattice arithSub(IntervalLattice l1, IntervalLattice l2) {
    IntervalLattice l3=l1;
    l3.arithSub(l2);
    return l3;
  }
  static IntervalLattice arithMul(IntervalLattice l1, IntervalLattice l2) {
    IntervalLattice l3=l1;
    l3.arithMul(l2);
    return l3;
  }
  static IntervalLattice arithDiv(IntervalLattice l1, IntervalLattice l2) {
    IntervalLattice l3=l1;
    l3.arithDiv(l2);
    return l3;
  }
  static IntervalLattice arithMod(IntervalLattice l1, IntervalLattice l2) {
    IntervalLattice l3=l1;
    l3.arithMod(l2);
    return l3;
  }

  void meet(IntervalLattice other) {
    if(isLowInf() && other.isLowInf()) {
      // OK
    } else if(isLowInf() && !other.isLowInf()) {
      setIsLowInf(false);
      _low=other._low;
    } else if(!isLowInf() && other.isLowInf()) {
      // OK
    } else {
      _low=std::max(_low,other._low);
    }
    if(isHighInf() && other.isHighInf()) {
      // OK
    } else if(isHighInf() && !other.isHighInf()) {
      setIsHighInf(false);
      _high=other._high;
    } else if(!isHighInf() && other.isHighInf()) {
      // OK
    } else {
      _high=std::min(_high,other._high);
    }
    unifyEmptyInterval();
  }

  void join(IntervalLattice other) {
    if(isLowInf()||other.isLowInf()) {
      setIsLowInf(true);
    } else {
      _low=std::min(_low,other._low);
    }
    if(isHighInf()||other.isHighInf()) {
      setIsHighInf(true);
    } else {
      _high=std::max(_high,other._high);
    }
  }

  std::string toString() {
    std::stringstream ss;
    if(isTop()) {
      ss<<"top";
      return ss.str();
    } else if(isBot()) {
      ss<<"bot";
      return ss.str();
    }
    if(isLowInf())
      ss<<"inf";
    else
      ss<<_low;
    ss<<",";
    if(isHighInf())
      ss<<"inf";
    else
      ss<<_high;
    return "["+ss.str()+"]";
  }

  void arithAdd(Type val) {
    if(!isLowInf())
      _low+=val;
    if(!isHighInf())
      _high+=val;
  }

  void arithSub(Type val) {
    if(!isLowInf())
      _low-=val;
    if(!isHighInf())
      _high-=val;
  }

  void arithMul(Type val) {
    if(!isLowInf())
      _low*=val;
    if(!isHighInf())
      _high*=val;
  }

  void arithDiv(Type val) {
    if(!isLowInf())
      _low/=val;
    if(!isHighInf())
      _high/=val;
  }

  void arithMod(Type val) {
    if(!isLowInf())
      _low%=val;
    if(!isHighInf())
      _high%=val;
  }

  // [a,b]+[c,d]=[a+c,b+d]
  void arithAdd(IntervalLattice other) {
    if(!isLowInf() && !other.isLowInf())
      _low+=other._low;
    if(!isHighInf() && !other.isHighInf())
      _high+=other._high;
    if(isLowInf()||other.isLowInf())
      setIsLowInf(true);
    if(isHighInf()||other.isHighInf())
      setIsHighInf(true);
  }

  // [a,b]-[c,d]=[a-d,b-c]
  void arithSub(IntervalLattice other) {
    if(!isLowInf() && !other.isHighInf())
      _low-=other._high;
    if(!isHighInf() && !other.isLowInf())
      _high-=other._low;
    if(isLowInf()||other.isHighInf())
      setIsLowInf(true);
    if(isHighInf()||other.isLowInf())
      setIsHighInf(true);
  }

  // [a,b]*[c,d]=[a*c,b*d]
  void arithMul(IntervalLattice other) {
    if(!isLowInf() && !other.isLowInf())
      _low*=other._low;
    if(!isHighInf() && !other.isHighInf())
      _high*=other._high;
    if(isLowInf()||other.isLowInf())
      setIsLowInf(true);
    if(isHighInf()||other.isHighInf())
      setIsHighInf(true);
  }

  // [a,b]/[c,d]=[a/d,b/c]
  void arithDiv(IntervalLattice other) {
    if(!isLowInf() && !other.isHighInf())
      _low/=other._high;
    if(!isHighInf() && !other.isLowInf())
      _high/=other._low;
    if(isLowInf()||other.isHighInf())
      setIsLowInf(true);
    if(isHighInf()||other.isLowInf())
      setIsHighInf(true);
  }

  // [a,b]%[c,d]=[a%d,b%c]
  void arithMod(IntervalLattice other) {
    if(!isLowInf() && !other.isHighInf())
      _low%=other._high;
    if(!isHighInf() && !other.isLowInf())
      _high%=other._low;
    if(isLowInf()||other.isHighInf())
      setIsLowInf(true);
    if(isHighInf()||other.isLowInf())
      setIsHighInf(true);
  }
 private:
  void setIsLowInf(bool val) {
    _isLowInf=val;
  }
  void setIsHighInf(bool val) {
    _isHighInf=val;
  }
  void unifyEmptyInterval() {
    if(_low>_high)
      setEmpty();
  }
 private:
  Type _low;
  Type _high;
  bool _isLowInf;
  bool _isHighInf;
};
