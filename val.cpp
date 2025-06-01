// implement the overloaded functions in val.h

// implemented the following functions:
// Operator*(), operator<(), Catenate(), Power()


#include "val.h"

//overloaded less than operator of this with op
Value Value::operator<(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        // int temp < int op
        return Value(Itemp < op.Itemp);
    } else if (IsReal() && op.IsReal()) {
        // real temp < real op
        return Value(Rtemp < op.Rtemp);
    } else if (IsInt() && op.IsReal()) {
        // int temp < real op
        return Value(Itemp < op.Rtemp);
    } else if (IsReal() && op.IsInt()) {
        // real temp < int op
        return Value(Rtemp < op.Itemp);
    } else {
        // not either of these
        return Value(); 
    }
}

// Numeric overloaded multiply this by op
Value Value::operator*(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        // int temp * int op
        return Value(Itemp * op.Itemp);
    } else if (IsReal() && op.IsReal()) {
        // real temp * real op
        return Value(Rtemp * op.Rtemp);
    } else if (IsInt() && op.IsReal()) {
        // int temp * real op
        return Value(Itemp * op.Rtemp);
    } else if (IsReal() && op.IsInt()) {
        // real temp * int op
        return Value(Rtemp * op.Itemp);
    } else {
        // not either of these
        return Value(); 
    }
}

// String concatenation of this with op
Value Value::Catenate(const Value& op) const {
    if (IsString() && op.IsString()) {
        // string temp + string op
        return Value(Stemp + op.Stemp);
    } else {
        // not the above
        return Value(); 
    }
}

// Compute the value of this raised to the exponent op
Value Value::Power(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        // int temp ^ int op, 
        return Value(pow(Itemp, op.Itemp));

    } else if (IsReal() && op.IsReal()) {
        // real temp ^ real op
        return Value(pow(Rtemp, op.Rtemp));
    } else if (IsInt() && op.IsReal()) {
        // int temp ^ real op
        return Value(pow(Itemp, op.Rtemp));
    } else if (IsReal() && op.IsInt()) {
        // real temp ^ int op
        return Value(pow(Rtemp, op.Itemp));
    } else {
        // not either of these
        return Value(); 
    }
}

// operator+()
Value Value::operator+(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        return Value(Itemp + op.Itemp);
    } else if (IsReal() && op.IsReal()) {
        return Value(Rtemp + op.Rtemp);
    } else if (IsInt() && op.IsReal()) {
        return Value(Itemp + op.Rtemp);
    } else if (IsReal() && op.IsInt()) {
        return Value(Rtemp + op.Itemp);
    } else {
        return Value(); // Error or invalid type for addition
    }
}

// operator-()
Value Value::operator-(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        return Value(Itemp - op.Itemp);
    } else if (IsReal() && op.IsReal()) {
        return Value(Rtemp - op.Rtemp);
    } else if (IsInt() && op.IsReal()) {
        return Value(Itemp - op.Rtemp);
    } else if (IsReal() && op.IsInt()) {
        return Value(Rtemp - op.Itemp);
    } else {
        return Value(); // Error or invalid type for subtraction
    }
}

// operator/()

Value Value::operator/(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        return Value(Itemp / op.Itemp);
    } else if (IsReal() && op.IsReal()) {
        return Value(Rtemp / op.Rtemp);
    } else if (IsInt() && op.IsReal()) {
        return Value(Itemp / op.Rtemp);
    } else if (IsReal() && op.IsInt()) {
        return Value(Rtemp / op.Itemp);
    } else {
        return Value(); 
    }
}

// operator==()
Value Value::operator==(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        return Value(Itemp == op.Itemp);
    } else if (IsReal() && op.IsReal()) {
        return Value(Rtemp == op.Rtemp);
    } else if (IsBool() && op.IsBool()) {
        return Value(Btemp == op.Btemp);
    } else if (IsString() && op.IsString()) {
        return Value(Stemp == op.Stemp);
    } else {
        return Value(); 
    }
}

// operator>()
Value Value::operator>(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        return Value(Itemp > op.Itemp);
    } else if (IsReal() && op.IsReal()) {
        return Value(Rtemp > op.Rtemp);
    } else if (IsInt() && op.IsReal()) {
        return Value(Itemp > op.Rtemp);
    } else if (IsReal() && op.IsInt()) {
        return Value(Rtemp > op.Itemp);
    } else {
        return Value(); 
    }
}


// implemented these functions from val.h
// operator+(), operator-(), operator/(), operator==(), operator>(), operator<()