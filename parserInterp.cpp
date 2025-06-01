/* Implementation of Recursive-Descent Parser
 * for the SFort95 Language
 * parser(SP24).cpp
 * Programming Assignment 2
 * Spring 2024
 */

// for reference, the ParseTree (or print statement debugs) are all commented out in this file

#include "parserInterp.h"
#include <stack>

// given
map<string, bool> defVar;
map<string, Token> SymTable;
queue<Value> *ValQue;			 // declare a pointer variable to a queue of Value objects
map<string, Value> TempsResults; // Container of temporary locations of Value objects for results of expressions, variables values and constants

// global queue for the SymTable and stack for the TempsResults
queue<string> SymTableQueue;
stack<string> TempResultStack;

// global strlen variable that will get propogated in varlist recursively
int strLengthtest3 = 1;

namespace Parser
{
	bool pushed_back = false;
	LexItem pushed_token;

	static LexItem GetNextToken(istream &in, int &line)
	{
		if (pushed_back)
		{
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem &t)
	{
		if (pushed_back)
		{
			abort();
		}
		pushed_back = true;
		pushed_token = t;
	}

}

static int error_count = 0;

int ErrCount()
{
	return error_count;
}

void ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}

bool IdentList(istream &in, int &line);

// Program is: Prog = PROGRAM IDENT {Decl} {Stmt} END PROGRAM IDENT
bool Prog(istream &in, int &line)
{
	bool dl = false, sl = false;
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok.GetToken() == PROGRAM)
	{
		tok = Parser::GetNextToken(in, line);
		// 	cout << "Prog -> ";
		if (tok.GetToken() == IDENT)
		{
			// 	cout << "DECL -> ";
			dl = Decl(in, line);
			if (!dl)
			{
				ParseError(line, "Incorrect Declaration in Program");
				return false;
			}
			// 	cout << "STMT -> ";

			sl = Stmt(in, line);

			if (!sl)
			{
				ParseError(line, "Incorrect Statement in program");
				return false;
			}
			tok = Parser::GetNextToken(in, line);

			if (tok.GetToken() == END)
			{
				tok = Parser::GetNextToken(in, line);

				if (tok.GetToken() == PROGRAM)
				{
					tok = Parser::GetNextToken(in, line);

					if (tok.GetToken() == IDENT)
					{
						cout << "(DONE)" << endl;
						return true;
					}
					else
					{
						ParseError(line, "Missing Program Name");
						return false;
					}
				}
				else
				{
					ParseError(line, "Missing PROGRAM at the End");
					return false;
				}
			}
			else
			{
				ParseError(line, "Missing END of Program");
				return false;
			}
		}
		else
		{
			ParseError(line, "Missing Program name");
			return false;
		}
	}
	else if (tok.GetToken() == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}

	else
	{
		ParseError(line, "Missing Program keyword");
		return false;
	}
}

// DONE?
// Decl ::= Type :: VarList
// Type ::= INTEGER | REAL | CHARARACTER [(LEN = ICONST)]
bool Decl(istream &in, int &line)
{
	bool status = false;
	LexItem tok;
	string strLen;

	LexItem t = Parser::GetNextToken(in, line);

	if (t.GetToken() == INTEGER || t.GetToken() == REAL || t.GetToken() == CHARACTER)
	{
		tok = t;
		// 	cout << t << " IN DECL (GOING IN VARLIST) -> ";

		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == DCOLON)
		{
			// t is the type of the variable
			status = VarList(in, line, t);

			if (status)
			{
				status = Decl(in, line);
				if (!status)
				{
					ParseError(line, "Declaration Syntactic Error.");
					return false;
				}
				return status;
			}
			else
			{
				ParseError(line, "Missing Variables List.");
				return false;
			}
		}
		else if (t == CHARACTER && tok.GetToken() == LPAREN)
		{
			tok = Parser::GetNextToken(in, line);

			if (tok.GetToken() == LEN)
			{
				tok = Parser::GetNextToken(in, line);

				if (tok.GetToken() == ASSOP)
				{
					tok = Parser::GetNextToken(in, line);

					if (tok.GetToken() == ICONST)
					{
						strLen = tok.GetLexeme();
						// change the global variable here

						strLengthtest3 = stoi(strLen);
						tok = Parser::GetNextToken(in, line);
						if (tok.GetToken() == RPAREN)
						{
							tok = Parser::GetNextToken(in, line);
							if (tok.GetToken() == DCOLON)
							{
								status = VarList(in, line, t, strLengthtest3);

								if (status)
								{
									status = Decl(in, line);
									if (!status)
									{
										ParseError(line, "Declaration Syntactic Error.");
										return false;
									}
									return status;
								}
								else
								{
									ParseError(line, "Missing Variables List.");
									return false;
								}
							}
						}
						else
						{
							ParseError(line, "Missing Right Parenthesis for String Length definition.");
							return false;
						}
					}
					else
					{
						ParseError(line, "Incorrect Initialization of a String Length");
						return false;
					}
				}
			}
		}
		else
		{
			ParseError(line, "Missing Double Colons");
			return false;
		}
	}

	Parser::PushBackToken(t);
	return true;
} // End of Decl

// Stmt ::= AssigStmt | BlockIfStmt | PrintStmt | SimpleIfStmt
bool Stmt(istream &in, int &line)
{
	bool status;

	LexItem t = Parser::GetNextToken(in, line);

	switch (t.GetToken())
	{

	case PRINT:
		status = PrintStmt(in, line);

		if (status)
			status = Stmt(in, line);
		break;

	case IF:
		status = BlockIfStmt(in, line);
		if (status)
			status = Stmt(in, line);
		break;

	case IDENT:
		Parser::PushBackToken(t);
		status = AssignStmt(in, line);
		if (status)
			status = Stmt(in, line);
		break;

	default:
		Parser::PushBackToken(t);
		return true;
	}

	return status;
} // End of Stmt

bool SimpleStmt(istream &in, int &line)
{
	bool status;

	LexItem t = Parser::GetNextToken(in, line);

	switch (t.GetToken())
	{

	case PRINT:
		status = PrintStmt(in, line);

		if (!status)
		{
			ParseError(line, "Incorrect Print Statement");
			return false;
		}
		break;

	case IDENT:
		Parser::PushBackToken(t);
		status = AssignStmt(in, line);
		if (!status)
		{
			ParseError(line, "Incorrect Assignent Statement");
			return false;
		}

		break;

	default:
		Parser::PushBackToken(t);
		return true;
	}

	return status;
} // End of SimpleStmt

// VarList ::= Var [= Expr] {, Var [= Expr]}
//  idtok is the type of the variable, passed
//  strlen is globally 1 by default and will be updated in the decl function
bool VarList(istream &in, int &line, LexItem &idtok, int strlen)
{
	bool status = false, exprstatus = false;
	string ident;
	Value retVal;

	// SymTable will be updated here as well as initialized values.
	LexItem tok = Parser::GetNextToken(in, line);
	// 	cout << tok << " IN VARLIST (map stuff lel) -> ";

	if (tok == IDENT)
	{

		ident = tok.GetLexeme();
		if (!(defVar.find(ident)->second))
		{
			SymTableQueue.push(ident);
			// cout << "SymTableQueue.push(" << identstr << ")" << endl;
			defVar[ident] = true;
			// cout << "defVar[" << identstr << "] = true" << endl;
			TempResultStack.push(ident);
			// cout << "TempResultStack.push(" << identstr << ")" << endl;

			while (!SymTableQueue.empty())
			{
				SymTable[SymTableQueue.front()] = idtok.GetToken();
				SymTableQueue.pop();
			}
			//	cout << SymTable[identstr] << " (enum position) in symTable for " << identstr << " idtok is " << idtok << "-> ";
		}
		else
		{
			ParseError(line, "Variable Redefinition");
			return false;
		}
	}
	else
	{

		ParseError(line, "Missing Variable Name");
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok == ASSOP)
	{
		exprstatus = Expr(in, line, retVal);
		if (!exprstatus)
		{
			ParseError(line, "Incorrect initialization for a variable.");
			return false;
		}
		// for strlen maybe?
		// s.insert(s.end(), strlen - s.size(), ' ');
		// found a better fix

		if (idtok.GetToken() == CHARACTER)
		{
			string retvalstring = retVal.GetString();
			strLengthtest3 = strlen;
			if (retvalstring.length() > strlen)
			{
				retvalstring = retvalstring.substr(0, strlen);
			}
			else
			{
				retvalstring += string(strlen - retvalstring.length(), ' ');
			}
			retVal.SetString(retvalstring);
		}

		if (SymTable[ident] == REAL && retVal.GetType() == VINT)
		{
			retVal.SetReal(static_cast<double>(retVal.GetInt()));
			retVal.SetType(VREAL);
		}

		if (idtok.GetToken() == CHARACTER && strlen == 1)
		{
			string truncatedString = retVal.GetString().substr(0, 1);
			retVal.SetString(truncatedString);
		}

		TempsResults[ident] = retVal;

		while (!TempResultStack.empty())
		{
			TempsResults[TempResultStack.top()] = retVal;
			TempResultStack.pop();
		}

		tok = Parser::GetNextToken(in, line);

		if (tok == COMMA)
		{
			// cout << " more recursive Varlist stuff >_< -> ";
			// cout << idtok << " is being passed for more recursive Varlist stuff >_< -> ";
			status = VarList(in, line, idtok, strLengthtest3);
		}
		else
		{
			Parser::PushBackToken(tok);
			return true;
		}
	}
	else if (tok == COMMA)
	{
		// 	cout << " more recursive Varlist stuff >_< -> ";

		status = VarList(in, line, idtok);
	}
	else if (tok == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");

		return false;
	}
	else
	{
		// default value of 1, only one space
		if (idtok.GetToken() == CHARACTER)
		{
			string defaultValue(strlen, ' ');
			TempsResults[ident] = Value(defaultValue);
		}
		while (!TempResultStack.empty())
		{
			TempResultStack.pop();
		}
		Parser::PushBackToken(tok);
		return true;
	}

	return status;

} // End of VarList

// GIVEN
// PrintStmt:= PRINT *, ExprList

bool PrintStmt(istream &in, int &line)
{
	LexItem t;
	ValQue = new queue<Value>;

	t = Parser::GetNextToken(in, line);

	if (t != DEF)
	{

		ParseError(line, "Print statement syntax error.");
		return false;
	}
	t = Parser::GetNextToken(in, line);

	if (t != COMMA)
	{

		ParseError(line, "Missing Comma.");
		return false;
	}
	bool ex = ExprList(in, line);

	if (!ex)
	{
		ParseError(line, "Missing expression after Print Statement");
		return false;
	}

	while (!(*ValQue).empty())
	{
		Value nextVal = (*ValQue).front();
		cout << nextVal;
		ValQue->pop();
	}
	cout << endl;
	return ex;
} // End of PrintStmt

// BlockIfStmt ::= IF (RelExpr) THEN {Stmt} [ELSE {Stmt}] END IF
//  SimpleIfStmt ::= IF (RelExpr) SimpleStmt
bool BlockIfStmt(istream &in, int &line)
{
	bool ex = false, status = true;
	LexItem t;
	Value retVal;
	bool foundElseOrEnd = false;

	t = Parser::GetNextToken(in, line);
	if (t != LPAREN)
	{
		ParseError(line, "Missing Left Parenthesis");
		return false;
	}

	ex = RelExpr(in, line, retVal);
	if (!ex)
	{
		ParseError(line, "Missing if statement condition");
		return false;
	}

	t = Parser::GetNextToken(in, line);
	if (t != RPAREN)
	{
		ParseError(line, "Missing Right Parenthesis");
		return false;
	}

	t = Parser::GetNextToken(in, line);
	if (t != THEN)
	{
		Parser::PushBackToken(t);
		return SimpleStmt(in, line);
	}

	if (!retVal.GetBool())
	{
		// If retVal is false (test 4,5), skip to ELSE or END IF
		while (!foundElseOrEnd)
		{
			t = Parser::GetNextToken(in, line);
			if (t == ELSE || t == END)
			{
				foundElseOrEnd = true;
				Parser::PushBackToken(t);
			}
		}
	}

	if (!foundElseOrEnd || t == THEN)
	{
		status = Stmt(in, line);
		if (!status)
		{
			ParseError(line, "Error in THEN part of IF statement");
			return false;
		}
	}

	t = Parser::GetNextToken(in, line);
	if (t == ELSE)
	{
		status = Stmt(in, line);
		if (!status)
		{
			ParseError(line, "Error in ELSE part of IF statement");
			return false;
		}
		t = Parser::GetNextToken(in, line);
	}

	if (t != END)
	{
		ParseError(line, "Missing END of IF");
		return false;
	}

	t = Parser::GetNextToken(in, line);
	if (t != IF)
	{
		ParseError(line, "Missing IF at End of IF statement");
		return false;
	}

	return true;
}

// Var:= ident
bool Var(istream &in, int &line, LexItem &idtok)
{
	string identstr;

	idtok = Parser::GetNextToken(in, line);

	if (idtok == IDENT)
	{
		identstr = idtok.GetLexeme();
		// checks symTable for the variable
		// idtok is the variable type
		if (!(SymTable.find(identstr)->second))
		{
			ParseError(line, "Undeclared Variable");
			return false;
		}
		return true;
	}
	else if (idtok.GetToken() == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << idtok.GetLexeme() << ")" << endl;
		return false;
	}
	return false;
} // End of Var

// AssignStmt:= Var = Expr
bool AssignStmt(istream &in, int &line)
{

	bool varstatus = false, status = false;
	LexItem idtok;
	varstatus = Var(in, line, idtok);

	string id = idtok.GetLexeme();

	if (varstatus)
	{

		idtok = Parser::GetNextToken(in, line);
		// 	cout << idtok << " (idtok) in AssignStmt -> ";

		if (idtok == ASSOP)
		{
			Value retVal;

			status = Expr(in, line, retVal);
			if (!status)
			{
				ParseError(idtok.GetLinenum(), "Missing Expression in Assignment Statment");
				return status;
			}
			if (SymTable[id] == CHARACTER)
			{
				// type for characters is VSTRING
				if (retVal.GetType() != VSTRING)
				{
					ParseError(idtok.GetLinenum(), "Illegal mix match operation");
					return false;
				}
				else
				{
					// add id key to the TempsResults map and assign retVal
					TempsResults[id] = retVal;
				}
			}

			// varlist logic, for test 4, 5
			if (SymTable[id] == CHARACTER)
			{
				string value = retVal.GetString();
				if (value.length() > strLengthtest3)
				{
					value = value.substr(0, strLengthtest3);
				}
				else
				{
					value += string(strLengthtest3 - value.length(), ' ');
				}
				retVal.SetString(value);
			}

			if (SymTable[id] == INTEGER && retVal.GetType() == VREAL)
			{

				int convertedValue = static_cast<int>(retVal.GetReal());
				retVal.SetType(VINT);
				retVal.SetInt(convertedValue);
			}

			// Assign retVal to TempsResults in all cases
			TempsResults[id] = retVal;
		}
		else if (idtok.GetToken() == ERR)
		{
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << idtok.GetLexeme() << ")" << endl;
			return false;
		}
		else
		{
			ParseError(line, "Missing Assignment Operator");
			return false;
		}
	}
	else
	{
		ParseError(line, "Missing Left-Hand Side Variable in Assignment statement");
		return false;
	}
	return status;
} // End of AssignStmt

// GIVEN
// ExprList:= Expr {,Expr}
bool ExprList(istream &in, int &line)
{
	bool status = false;
	Value retVal;

	status = Expr(in, line, retVal);
	if (!status)
	{
		ParseError(line, "Missing Expression");
		return false;
	}
	ValQue->push(retVal);
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok == COMMA)
	{

		status = ExprList(in, line);
	}
	else if (tok.GetToken() == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else
	{
		Parser::PushBackToken(tok);
		return true;
	}
	return status;
} // End of ExprList

// RelExpr ::= Expr  [ ( == | < | > ) Expr ]
bool RelExpr(istream &in, int &line, Value &retVal)
{
	Value lval, rval;

	bool t1 = Expr(in, line, lval);
	LexItem tok;
	if (!t1)
	{
		return false;
	}
	retVal = lval;
	//	cout << retVal << " (retVal) in RelExpr after lval = retval ->";

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	if (tok == EQ || tok == LTHAN || tok == GTHAN)
	{
		t1 = Expr(in, line, rval);

		if (!t1)
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}

		if (tok == LTHAN)
		{
			retVal = (retVal < rval);
		}
		else if (tok == GTHAN)
		{
			retVal = (retVal > rval);
			if (retVal.IsErr())
			{

				ParseError(line, "Illegal Greater Than Operator");
				return false;
			}
		}
		// convert to int if real is equal to int. MUST DO THIS FOR TEST CASE 3
		else if (tok == EQ)
		{
			if (retVal.GetType() == VREAL && retVal.GetReal() == static_cast<int>(retVal.GetReal()))
			{
				retVal.SetInt(static_cast<int>(retVal.GetReal()));
				retVal.SetType(VINT);
			}
			if (rval.GetType() == VREAL && rval.GetReal() == static_cast<int>(rval.GetReal()))
			{
				rval.SetInt(static_cast<int>(rval.GetReal()));
				rval.SetType(VINT);
			}
			retVal = (retVal == rval);
			if (retVal.IsErr())
			{
				ParseError(line, "Illegal Equal Operator");
				return false;
			}
		}
	}
	return true;
} // End of RelExpr

// Expr ::= MultExpr { ( + | - | ) MultExpr }
bool Expr(istream &in, int &line, Value &retVal)
{
	LexItem tok;
	Value lval, rval;
	//	cout << "Expr -> ";

	bool t1 = MultExpr(in, line, lval);

	if (!t1)
	{
		return false;
	}
	retVal = lval;
	// 	cout << retVal << " (retVal) in EXPR after lval = retval -> ";

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while (tok == PLUS || tok == MINUS || tok == CAT)
	{
		if (tok == PLUS)
		{
			t1 = MultExpr(in, line, rval);
			retVal = retVal + rval;
			if (retVal.IsErr())
			{
				ParseError(tok.GetLinenum(), "Illegal addition operator");
				return false;
			}
		}
		else if (tok == MINUS)
		{
			t1 = MultExpr(in, line, rval);
			retVal = retVal - rval;
			if (retVal.IsErr())
			{
				ParseError(tok.GetLinenum(), "Illegal Subtraction operator");
				return false;
			}
		}
		else if (tok == CAT)
		{
			t1 = MultExpr(in, line, rval);
			retVal = retVal.Catenate(rval);
			if (retVal.IsErr())
			{
				ParseError(tok.GetLinenum(), "Illegal Concatenation operator");
				return false;
			}
		}

		if (!t1)
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}

		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == ERR)
		{
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
	}
	Parser::PushBackToken(tok);
	return true;
}

// MultExpr ::= TermExpr { ( * | / ) TermExpr }
bool MultExpr(istream &in, int &line, Value &retVal)
{
	LexItem tok;
	Value lval, rval;
	// cout << "MultExpr -> ";

	bool t1 = TermExpr(in, line, lval);

	if (!t1)
	{
		return false;
	}
	retVal = lval;
	// 	cout << retVal << " (retVal) in MultExpr after lval = retval -> ";

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while (tok == MULT || tok == DIV)
	{
		if (tok == MULT)
		{
			t1 = TermExpr(in, line, rval);
			retVal = retVal * rval;

			if (retVal.IsErr())
			{
				ParseError(tok.GetLinenum(), "Illegal Operand in Multiplication");
				return false;
			}
		}

		else if (tok == DIV)
		{
			t1 = TermExpr(in, line, rval);
			if (rval.IsInt())
			{
				int itemp = rval.GetInt();
				if (itemp == 0)
				{
					ParseError(tok.GetLinenum(), "RunTime Error, Illegal INT Division by Zero");
					return false;
				}
				retVal = retVal / rval;
				if (retVal.IsErr())
				{
					ParseError(line, "Runtime ERR, Illegal Int Division");
					return false;
				}
			}
			else if (rval.IsReal())
			{
				float rtemp = rval.GetReal();
				if (rtemp == 0.0)
				{
					ParseError(tok.GetLinenum(), "RunTime ERR, REAL Division by Zero");
					return false;
				}
				retVal = retVal / rval;
				if (retVal.IsErr())
				{
					ParseError(tok.GetLinenum(), "Runtime ERR, Illegal Real Division");
					return false;
				}
			}
		}

		if (!t1)
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}

		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == ERR)
		{
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
	}
	Parser::PushBackToken(tok);
	return true;
} // End of MultExpr

// TermExpr ::= SFactor { ** SFactor }
bool TermExpr(istream &in, int &line, Value &retVal)
{
	stack<Value> precedencevalues;
	Value lval, rval;
	// 	cout << "TermExpr -> ";

	bool t1 = SFactor(in, line, lval);

	LexItem tok;

	if (!t1)
	{
		return false;
	}
	precedencevalues.push(lval);

	retVal = lval;

	// 	cout << retVal << " (retVal) in TermExpr after lval = retval -> ";

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while (tok.GetToken() == POW)
	{
		t1 = SFactor(in, line, rval);
		// 		cout << rval << " (rval) in TermExpr, before pow retval -> ";

		if (!t1)
		{
			ParseError(line, "Missing exponent operand");
			return false;
		}
		precedencevalues.push(rval);
		// exponentian only on real
		if (!(retVal.GetType() == VREAL || rval.GetType() == VREAL))
		{
			ParseError(line, "Runtime ERR, Illegal Pow Division on NON REALS");
			return false;
		}

		if (tok.GetToken() == POW)
		{
			retVal = retVal.Power(rval);
			if (retVal.IsErr())
			{
				ParseError(line, "Runtime ERR, Illegal POW");
				return false;
			}
			// 		cout << retVal << " (retVal) in TermExpr, pow check -> ";
		}

		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == ERR)
		{
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
	}
	Parser::PushBackToken(tok);

	while (precedencevalues.size() > 1)
	{
		Value exponent = precedencevalues.top();
		precedencevalues.pop();
		Value base = precedencevalues.top();
		precedencevalues.pop();

		// Perform the exponentiation, check types to handle errors
		if (!(base.GetType() == VREAL || exponent.GetType() == VREAL))
		{
			ParseError(line, "Runtime ERR, Illegal Mixed Type POW");
			return false;
		}

		Value result = base.Power(exponent);
		precedencevalues.push(result); // Assume Value can hold doubles, adapt type as needed
	}

	// The final result on the stack is the result of the term expression
	if (!precedencevalues.empty())
	{
		retVal = precedencevalues.top();
	}

	return true;
} // End of TermExpr

// SFactor = Sign Factor | Factor
//  done
bool SFactor(istream &in, int &line, Value &retVal)
{
	LexItem t = Parser::GetNextToken(in, line);
	bool status;
	// default positive sign
	int sign = 1;
	// 	cout << t << " (token) in SFactor -> ";

	if (t.GetToken() == MINUS)
	{
		sign = -1;
	}
	else if (t.GetToken() == PLUS)
	{
		sign = 1;
	}
	else
		Parser::PushBackToken(t);

	status = Factor(in, line, sign, retVal);
	// 	cout << retVal << " (retVal) in SFactor w/ " << "sign " << sign << " -> ";

	return status;
} // End of SFactor

// Factor := ident | iconst | rconst | sconst | (Expr)
bool Factor(istream &in, int &line, int sign, Value &retVal)
{

	LexItem tok = Parser::GetNextToken(in, line);
	string lexeme = tok.GetLexeme();
	// 	cout << tok << " (Tok) in Factor " << lexeme << " (lexeme) " << tok.GetToken() << " (token) in Factor -> ";

	if (tok.GetToken() == IDENT)
	{

		if (!(defVar.find(lexeme)->second))
		{
			ParseError(line, "Using Undefined Variable");
			return false;
		}
		if (SymTable[lexeme] == CHARACTER && sign != 1)
		{
			ParseError(line, "CHAR type has an illegal sign");
			return false;
		}

		// make retVal equal to the value of the variable
		retVal = TempsResults[lexeme];
		//  cout << retVal << " (retVal) IN FACTOR -> ";

		if (retVal.GetType() == VINT)
		{
			retVal.SetInt(sign * retVal.GetInt());
		}
		else if (retVal.GetType() == VREAL)
		{
			retVal.SetReal(sign * retVal.GetReal());
		}

		return true;
	}
	// test case 1 goes here
	else if (tok.GetToken() == ICONST)
	{
		// cout << SymTable[lexeme] << " (enum position) in SymTable for " << lexeme << " -> ";
		retVal = new Value(sign * stoi(lexeme));
		// so r is the lexeme
		if (SymTable[lexeme] == REAL)
		{
			retVal.SetType(VREAL);
			retVal.SetInt(sign * stof(lexeme));
		}
		else
		{
			retVal.SetType(VINT);
			retVal.SetInt(sign * stoi(lexeme));
			// retVal has a value in test1 here
			// 		cout << retVal << " (retVal) IN FACTOR ICONST check -> ";
		}
		return true;
	}
	else if (tok.GetToken() == SCONST)
	{
		retVal = Value(tok.GetLexeme());
		if (sign != 1)
		{
			ParseError(line, "Illegal MINUS operator to string");
			return false;
		}
		return true;
	}
	else if (tok.GetToken() == RCONST)
	{
		retVal.SetType(VREAL);
		if (sign == -1)
		{
			retVal.SetReal(-(stof(tok.GetLexeme())));
		}
		else
		{
			retVal.SetReal(stof(tok.GetLexeme()));
		}
		return true;
	}
	else if (tok.GetToken() == LPAREN)
	{
		// 		cout << "FACTOR -> ";

		bool ex = Expr(in, line, retVal);
		if (!ex)
		{
			ParseError(line, "Missing expression after (");
			return false;
		}
		if (Parser::GetNextToken(in, line) == RPAREN)
			return ex;
		else
		{
			Parser::PushBackToken(tok);
			ParseError(line, "Missing ) after expression");
			return false;
		}
	}
	else if (tok.GetToken() == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}

	return false;
}