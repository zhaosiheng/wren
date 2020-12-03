#pragma once
#include <string.h>
#include <vector>
#include <map>
#include <regex>
#include <assert.h>
using namespace std;

typedef map<string, vector<int>> priority_table;
priority_table int_pr = { {"+",{3,2}},{"-",{3,2}},{"*",{5,4}},{"/",{5,4}},{"(",{1,6}},{")",{6,1}} };
priority_table bool_pr = { {"or",{3,2}},{"and",{5,4}},{"(",{1,6}},{")",{6,1}} };
priority_table if_pr = { {"if",{1,6}},{"end if",{6,1}},{"else",{3,2}} };
int op_split(string s,priority_table t){
	vector<pair<int,string>> ch,stack;
	for (auto it = t.begin(); it != t.end(); ++it) {
		int pos = s.find(it->first,0);
		while (pos >= 0) {
			pair<int, string> tmp(pos, it->first);
			ch.push_back(tmp);
			pos = s.find(it->first, pos + 1);
		}
	}
	if (ch.empty())
		return -1;
	sort(ch.begin(), ch.end(), [](pair<int,string> a, pair<int,string> b) {return a.first < b.first; });
	auto test = ch.begin();
	for (auto it = ch.begin(); it != ch.end();) {
		if (stack.empty()) {
			stack.push_back(*it);
			++it;
		}
		else {
			int pre = t.find((stack.end()-1)->second)->second[0];
			int post = t.find(it->second)->second[1];
			if (pre < post) {
				stack.push_back(*it);
				++it;
			}if (pre > post) {
				stack.pop_back();
				continue;
			}if (pre == post) {
				stack.pop_back();
				++it;
			}
		}
	}
	if (stack.empty())
		return -1;
	return stack[0].first;
}


bool be_found(string str,string pattern) {
	if (str.find(pattern) < str.size())
		return true;
	else
		return false;
}
//在字符串str中遍历，当遇到pattern时去除并便分割，可将str分割成数个子串，
vector<string> split(string str, string pattern)
{
	int pos;
	std::vector<string> result;
	str += pattern;//扩展字符串以方便操作
	int size = str.size();
	for (int i = 0; i < size; i++)
	{
		pos = str.find(pattern, i);
		if (pos < size)
		{
			string s = str.substr(i, pos - i);
			result.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}
	return result;
}
//除去字符串s头尾的空格和换行符
void clear_spaces(string &s) {
	if (s.empty()) return;
	for (; s[0] == ' ' || s[0] == '\n'; s = s.substr(1));
	for (; s[s.length() - 1] == ' ' || s[s.length() - 1] == '\n'; s.pop_back());
}
typedef vector<pair<string, int>> all_var;
all_var vars = {};
int var_exist(string s) {
	for (int i = 0; i < vars.size(); i++) {
		if (vars[i].first.compare(s) == 0)
			return i;
	}
	return -1;
}

enum Is_Empty{ nonEmpty,Empty};
enum Type { integer, boolean, program, undefined };
typedef string Name;
typedef vector<Name> Var_list;
typedef map<Name, Type> Symbol_table;



Type lookup_type(Name n,Symbol_table t) {
	Symbol_table::iterator iter;
	iter = t.find(n);
	if (iter != t.end())
		return iter->second;
	else
		return undefined;
}
Is_Empty table_insection(Symbol_table t1, Symbol_table t2) {
	Symbol_table::iterator iter;
	iter = t1.begin();
	if (t1.empty())
		return Empty;
	else if (lookup_type(iter->first, t2) != undefined)
		return nonEmpty;
	else {
		t1.erase(iter);
		return table_insection(t1, t2);
	}
}
Symbol_table table_union(Symbol_table t1, Symbol_table t2) {
	 t2.insert(t1.begin(), t1.end());
	 return t2;
}
Symbol_table add_item(Name n, Type t, Symbol_table s) {
	s.insert({ n,t });
	return s;
}
Symbol_table build_symbol_table(Var_list v,Type t) {
	Symbol_table s = {};
	if (v.empty())
		return s;
	else {
		Var_list ::iterator it;
		for (it = v.begin(); it != v.end(); ++it)
			s=add_item(*it, t, s);
		return s;
	}
}
class Program;
class block;
class declarationsequence;
class declaration;
class variable_list;
class type;
class commandsequence;
class command;
class expr;
class integer_expr;
class term;
class element;
class boolean_expr;
class boolean_term;
class boolean_element;
class comparison;
class variable;
class identifier;
class numeral;
class letter;//疑
class digit;//疑

class digit {
public:
	Name mine;
	digit(char c) {
		assert(c <= '9' && c >= '0');
		string s(1, c);
		mine = s;
	}
};
class letter {
public:
	Name mine;
	letter(char c) {
		assert(c <= 'z' && c >= 'a');
		string s(1, c);
		mine = s;
	}
};
class numeral {
public:
	int f;
	digit* my_digit;
	numeral* my_numeral;
	Name mine;
	int val;
	numeral(string s) {
		clear_spaces(s);
		if (s.length() == 1) {
			f = 0;
			my_digit = new digit(s[0]);
			mine = my_digit->mine;
		}
		else {
			f = 1;
			int pos = s.length() - 1;
			my_digit = new digit(s[pos]);
			my_numeral = new numeral(s.substr(0, pos));
			mine =  my_numeral->mine + my_digit->mine;
		}
		val = atoi(mine.data());
	}
};
class identifier {
public:
	int f;
	letter* my_letter;
	identifier* my_identifier;
	digit* my_digits;
	Name mine;
	identifier(string s) {
		if (s.length() == 1) {
			f = 0;
			my_letter = new letter(s[0]);
			mine = my_letter->mine;
		}
		else {
			int pos = s.length() - 1;
			char c = s[pos];
			if (c <= 'z' && c >= 'a') {
				f = 1;
				my_letter = new letter(c);
				my_identifier = new identifier(s.substr(0, pos));
				mine = my_identifier->mine + my_letter->mine;
			}
			else if (c <= '9' && c >= '0') {
				f = 2;
				my_digits = new digit(c);
				my_identifier = new identifier(s.substr(0, pos));
				mine = my_identifier->mine + my_digits->mine;
			}
		}
		
	}
};
class variable {
public:
	identifier* my_identifier;
	Name mine;
	int val;
	variable(string s) {
		clear_spaces(s);
		my_identifier = new identifier(s);
		mine = my_identifier->mine;
		int tmp = var_exist(mine);
		if(tmp!=-1)
			val = vars[var_exist(mine)].second;
	}
};
class expr {
public:
	int f;
	integer_expr* my_integer_expr;
	boolean_expr* my_boolean_expr;
	Symbol_table mine_s;
	Type mine_t;
	int val;
	expr(string s,Symbol_table st,Type t);
};
class element {
public:
	int f;
	numeral* my_numeral;
	variable* my_variable;
	char open_parenthesis;
	expr* my_expr;
	char close_parenthesis;
	char minus;
	element* my_element;
	Symbol_table mine_s;
	Type mine_t;
	int val;
	element(string s,Symbol_table st,Type t):mine_s(st),mine_t(t) {
		if (regex_match(s, regex("^[0-9]+"))) {
			f = 0;
			my_numeral = new numeral(s);
			val = my_numeral->val;
		}
		else if (regex_match(s, regex("^[a-z0-9]+$"))) {
			f = 1;
			my_variable = new variable(s);
			//condition
			switch (lookup_type(my_variable->mine,mine_s))
			{
			case integer:
				break;
			case undefined:
				cout << "Variable not declared";
				abort();
				break;
			case boolean:
				if(mine_t==undefined){}
				else {
					cout << "Integer variable expected";
					abort();
					break;
				}
			case program:
				if (mine_t == undefined) {}
				else {
					cout << "Integer variable expected";
					abort();
					break;
				}
			default:
				break;
			}
			val = my_variable->val;
		}
		else if (regex_match(s, regex("\\(.*\\)"))) {
			f = 2;
			open_parenthesis = '(';
			close_parenthesis = ')';
			my_expr = new expr(s.substr(1, s.length() - 2),mine_s,mine_t);
			my_expr->mine_t = mine_t;
			my_expr->mine_s = mine_s;
			val = my_expr->val;
		}
		else if (regex_match(s, regex("\\-.*"))) {
			minus = '-';
			my_element = new element(s.substr(1),mine_s,mine_t);
			my_element->mine_t = mine_t;
			my_element->mine_s = mine_s;
			val = my_element->val * (-1);
		}
	}
};
class term {
public:
	int f;
	element* my_element;
	term* my_term;
	char strong_op;
	Symbol_table mine_s;
	Type mine_t;
	int val;
	term(string s, Symbol_table st, Type t) :mine_s(st), mine_t(t) {
		int pos = op_split(s, int_pr);
		if (pos == -1) {
			f = 0;
			my_element = new element(s, mine_s, mine_t);
			val = my_element->val;
		}
		else {
			f = 1;
			my_term = new term(s.substr(0, pos), mine_s, mine_t);
			strong_op = s[pos];
			my_element = new element(s.substr(pos + 1), mine_s, mine_t);
			if (strong_op == '*')
				val = my_term->val * my_term->val;
			if (strong_op == '/')
				val = my_term->val / my_term->val;

		}
		
	}
};
class integer_expr {
public:
	int f;
	term* my_term;
	integer_expr* my_integer_epxr;
	char weak_op;
	Symbol_table mine_s;
	Type mine_t;
	int val;
	integer_expr(string s, Symbol_table st, Type t) :mine_s(st), mine_t(t) {
		clear_spaces(s);
		int pos = op_split(s, int_pr);
		if (pos == -1||s[pos]=='*'||s[pos]=='/') {
			f = 0;
			my_term = new term(s, mine_s, mine_t);
			val = my_term->val;
		}
		else {
			f = 1;
			my_integer_epxr = new integer_expr(s.substr(0, pos), mine_s, mine_t);
			weak_op = s[pos];
			my_term = new term(s.substr(pos + 1),mine_s,mine_t);
			if (weak_op == '+')
				val = my_integer_epxr->val + my_term->val;
			if (weak_op == '-')
				val = my_integer_epxr->val - my_term->val;
		}
	}
};
class comparison {
public:
	integer_expr* my_integer_expr_1;
	integer_expr* my_integer_expr_2;
	string relation;//==,<>,<,<=,>,>=
	integer_expr* anthor_integer_expr;
	Symbol_table mine;
	int val;
	comparison(string s, Symbol_table st) :mine(st) {
		if (regex_search(s, regex("=="))) {
			relation = "==";
			auto tmp = split(s, relation);
			my_integer_expr_1 = new integer_expr(tmp[0], mine, integer);
			my_integer_expr_2 = new integer_expr(tmp[1], mine, integer);
			val = my_integer_expr_1->val == my_integer_expr_2->val;
		}
		else if (regex_search(s, regex("<>"))) {
			relation = "<>";
			auto tmp = split(s, relation);
			my_integer_expr_1 = new integer_expr(tmp[0], mine, integer);
			my_integer_expr_2 = new integer_expr(tmp[1], mine, integer);
			val = my_integer_expr_1->val != my_integer_expr_2->val;
		}
		else if (regex_search(s, regex("<"))) {
			relation = "<";
			auto tmp = split(s, relation);
			my_integer_expr_1 = new integer_expr(tmp[0], mine, integer);
			my_integer_expr_2 = new integer_expr(tmp[1], mine, integer);
			val = my_integer_expr_1->val < my_integer_expr_2->val;
		}
		else if (regex_search(s, regex("<="))) {
			relation = "<=";
			auto tmp = split(s, relation);
			my_integer_expr_1 = new integer_expr(tmp[0], mine, integer);
			my_integer_expr_2 = new integer_expr(tmp[1], mine, integer);
			val = my_integer_expr_1->val <= my_integer_expr_2->val;
		}
		else if (regex_search(s, regex(">"))) {
			relation = ">";
			auto tmp = split(s, relation);
			my_integer_expr_1 = new integer_expr(tmp[0], mine, integer);
			my_integer_expr_2 = new integer_expr(tmp[1], mine, integer);
			val = my_integer_expr_1->val > my_integer_expr_2->val;
		}
		else if (regex_search(s, regex(">="))) {
			relation = ">=";
			auto tmp = split(s, relation);
			my_integer_expr_1 = new integer_expr(tmp[0], mine, integer);
			my_integer_expr_2 = new integer_expr(tmp[1], mine, integer);
			val = my_integer_expr_1->val >= my_integer_expr_2->val;
		}
	}
};
class boolean_element {
public:
	int f;//0-true,1-false,2-variable,3-comparison,4-not(bool expr),5-(bool expr)
	bool T;
	bool F;
	variable* my_variable;
	comparison* my_comparison;
	string NOT;
	char open_parenthesis;
	boolean_expr* my_boolean_expr;
	char close_parenthesis;
	Symbol_table mine_s;
	Type mine_t;
	int val;
	boolean_element(string s, Symbol_table st, Type t);

};
class boolean_term {
public:
	int f;
	boolean_element* my_boolean_element;
	string AND;
	boolean_term* my_boolean_term;
	Symbol_table mine_s;
	Type mine_t;
	int val;
	boolean_term(string s, Symbol_table st, Type t) :mine_s(st), mine_t(t) {
		int pos = op_split(s, bool_pr);
		if (pos == -1) {
			f = 0;
			my_boolean_element = new boolean_element(s, mine_s, mine_t);
			val = my_boolean_element->val;
		}
		else {
			f = 1;
			my_boolean_term = new boolean_term(s.substr(0, pos), mine_s, mine_t);
			AND = s.substr(pos,3);
			my_boolean_element = new boolean_element(s.substr(pos + 3), mine_s, mine_t);
			val = my_boolean_term->val && my_boolean_element->val;
		}
	}
};
class boolean_expr {
public:
	int f;
	boolean_term* my_boolean_term;
	string OR;
	boolean_expr* my_boolean_expr;
	Symbol_table mine_s;
	Type mine_t;
	int val;
	boolean_expr(string s, Symbol_table st, Type t):mine_s(st),mine_t(t) {
		clear_spaces(s); 
		int pos = op_split(s, bool_pr);
		if (pos == -1 || !s.substr(pos,3).compare("and")) {
			f = 0;
			my_boolean_term = new boolean_term(s, mine_s, mine_t);
			val = my_boolean_term->val;
		}
		else {
			f = 1;
			my_boolean_expr = new boolean_expr(s.substr(0, pos), mine_s, mine_t);
			OR = s.substr(pos,2);
			my_boolean_term = new boolean_term(s.substr(pos + 2), mine_s, mine_t);
			val = my_boolean_expr->val || my_boolean_term->val;
		}
	}
};
class type {
public:
	Type a= integer;
	Type b= boolean;
	Type* mine;
	type(string s) {
		if (s.compare("integer") == 0)
			mine = &a;
		if (s.compare("boolean") == 0)
			mine = &b;
	}
};
class variable_list {
public:
	int f;
	variable* my_variable;
	char comma;
	variable_list* my_variable_list;
	Var_list mine;
	variable_list(string s) {
		for (; s[0] == ' '; s = s.substr(1));
		int pos = s.find(",");
		if (pos == -1) {
			f = 0;
			my_variable = new variable(s);
			mine = {};
			mine.push_back(my_variable->mine);
		}
		else {
			f = 1;
			my_variable = new variable(s.substr(0, pos));
			comma = s[pos];
			my_variable_list = new variable_list(s.substr(pos + 1));
			//condition
			auto it = find(my_variable_list->mine.begin(), my_variable_list->mine.end(), my_variable->mine);
			if(it== my_variable_list->mine.end()){}
			else {
				cout << "Duplicate variable in declaration list";
				abort();
			}
			mine = my_variable_list->mine;
			mine.push_back(my_variable->mine);
		}
	}
};
class declaration {
public:
	string v;
	variable_list* my_variable_list;
	char colon;
	type* my_type;
	char semicolon;
	Symbol_table mine;
	declaration(string s) {
		int pos = s.find(" ");
//		assert(tmp.compare("var")==0);
		v = s.substr(0, pos);
		for (s=s.substr(pos); s[0] == ' '; s = s.substr(1));
		pos = s.find(":");
		my_variable_list = new variable_list(s.substr(0,pos));
		colon = s[pos];
		for (s = s.substr(pos+1); s[0] == ' '; s = s.substr(1));
		pos = s.find(";");
		my_type = new type(s.substr(0,pos));
		semicolon = s[pos];
		mine = build_symbol_table(my_variable_list->mine, *(my_type->mine) );
	}
};
class declarationsequence {
public:
	int f;
	declaration* my_declaration;
	declarationsequence* my_declarationsequence;
	Symbol_table mine;
	declarationsequence(string s) :mine({}) {
		f = 0;
		if (!s.empty()) {
			f = 1;
			for (; s[0] == ' '; s = s.substr(1));
			int pos = s.find("\n");
			my_declaration = new declaration(s.substr(0, pos));
			my_declarationsequence = new declarationsequence(s.substr(pos+1));
			//condition
			if(table_insection(my_declaration->mine,my_declarationsequence->mine)==Empty){}
			else {
				cout << "Duplicate declaration of identifier";
				abort();
			}
			mine = table_union(my_declaration->mine, my_declarationsequence->mine);
		}
	}
};
class command {
public:
	int f;//0-read,1-write,2-while,3-:=,4-if_else,5-if
	variable* my_variable;
	string assign;
	expr* my_expr;
	string rd;
	string wt;
	string w;
	boolean_expr* my_boolean_expr;
	string d;
	commandsequence* my_commandsequence;
	string i;
	string t;
	string el;
	string ed_if;
	string ed_wh;
	commandsequence* another_commandsequence;
	Symbol_table mine;
	command(string s, Symbol_table st);
};
class commandsequence {
public:
	int f;
	command* my_command;
	char semicolon;
	commandsequence* my_commandsequence;
	Symbol_table mine;
	commandsequence(string s, Symbol_table st);
};
class block {
public:
	declarationsequence* my_declarationsequence;
	string b;
	commandsequence* my_commandsequence;
	string e;
	Symbol_table mine;
	block(string s, Symbol_table st) :mine(st) {
		int pos = s.find("begin");
		my_declarationsequence = new declarationsequence(s.substr(0,pos));
		s = s.substr(pos);
		int op = s.find("\n");
		b = s.substr(0, op);
		int ed = s.rfind("end");
		e = s.substr(ed);
		//condition
		if (table_insection(mine, my_declarationsequence->mine) == Empty) {}
		else {
			cout << "Program name used as a variable";
			abort();
		}
		my_commandsequence = new commandsequence(s.substr(op, ed - op), table_union(mine, my_declarationsequence->mine));
	}
};
class Program {
public:
	string p;
	identifier* my_identifer;
	string i;
	block* my_block;
	Program(string s) {
		
		assert(s.substr(0, 7).compare("program") == 0);
		p = "program";
		s = s.substr(7);
		int pos = s.find("is");
		assert(pos != -1);
		my_identifer = new identifier(regex_replace(s.substr(0, pos), regex("\\s"), ""));
		s = s.substr(pos);
		i = s.substr(0, 2);
		for (s=s.substr(2); s[0] == ' '||s[0]=='\n'; s = s.substr(1));
		my_block = new block(s, add_item(my_identifer->mine, program, {}));
	}

};


boolean_element::boolean_element(string s, Symbol_table st, Type t) :mine_s(st), mine_t(t) {
	if (regex_match(s, regex("true"))) {
		f = 0;
		T = true;
		val = 1;
	}
	else if (regex_match(s, regex("false"))) {
		f = 1;
		F = false;
		val = 0;
	}
	else if (regex_match(s, regex("^not\\(.*\\)"))) {
		f = 4;
		NOT = s.substr(0,3);
		open_parenthesis = '(';
		close_parenthesis = ')';
		my_boolean_expr = new boolean_expr(s.substr(4, s.length() - 5), mine_s, mine_t);
		val = !my_boolean_expr->val;
	}
	else if (regex_match(s, regex("\\(.*\\)"))) {
		f = 5;
		open_parenthesis = '(';
		close_parenthesis = ')';
		my_boolean_expr = new boolean_expr(s.substr(1, s.length() - 2), mine_s, mine_t);
		val = my_boolean_expr->val;
	}
	else if (regex_search(s, regex("(==)|(<>)|(<)|(>)|(<=)|(>=)"))) {
		f = 3;
		my_comparison = new comparison(s, mine_s);
		val = my_comparison->val;
	}
	else if (regex_match(s, regex("^[a-z0-9]+$"))) {
		f = 2;
		my_variable = new variable(s);
		//condition
		switch (lookup_type(my_variable->mine, mine_s))
		{
		case boolean:
			break;
		case undefined:
			cout << "Variable not declared";
			abort();
			break;
		case integer:
			if (mine_t == undefined) {}
			else {
				cout << "Boolean variable expected";
				abort();
				break;
			}
		case program:
			if (mine_t == undefined) {}
			else {
				cout << "Boolean variable expected";
				abort();
				break;
			}
		default:
			break;
		}
		val = my_variable->val;
	}
}
expr::expr(string s, Symbol_table st, Type t) :mine_s(st), mine_t(t) {
	clear_spaces(s);
	if (mine_t == integer) {
		f = 0;
		my_integer_expr = new integer_expr(s, mine_s, mine_t);
		val = my_integer_expr->val;
	}
	if (mine_t == boolean) {
		f = 1;
		my_boolean_expr = new boolean_expr(s, mine_s, mine_t);
		val = my_boolean_expr->val;
	}
}

command::command(string s, Symbol_table st) :mine(st) {
	clear_spaces(s);
	if (s.substr(0, 4).compare("read") == 0) {
		f = 0;
		rd = s.substr(0, 4);
		my_variable = new variable(s.substr(5));
		//condition
		switch (lookup_type(my_variable->mine,mine))
		{
		case integer:
			break;
		case undefined: 
			cout << "Variable not declared"; 
			abort();
			break;
		case boolean: 
			cout << "Integer variable expected for read";
			abort();
			break; 
		case program:
			cout << "Integer variable expected for read";
			abort();
			break; 
		default:
			break;
		}
		cout << "\n请输入" << my_variable->mine << "的值：";
		int temp;
		cin >> temp;
		int n = var_exist(my_variable->mine);
		if (n != -1)
			vars[n].second = temp;
		else
			vars.push_back({ my_variable->mine,temp });
	}
	else if (s.substr(0, 5).compare("write") == 0) {
		f = 1;
		wt = s.substr(0, 5);
		my_expr = new expr(s.substr(6),mine,integer);
		cout << "\n输出值为：" << my_expr->val;
	}
	else if (s.substr(0, 5).compare("while") == 0) {
		f = 2;
		w = s.substr(0, 5);
		s = s.substr(5);
		int pos = s.find("do");
		d = s.substr(pos, 2);
		string loop_condition = s.substr(0, pos - 1);
		my_boolean_expr = new boolean_expr(loop_condition, mine, boolean);
		s = s.substr(pos + 2);
	    pos = s.rfind("end while");
		ed_wh = s.substr(pos, 9);
		while (my_boolean_expr->val) {
			my_commandsequence = new commandsequence(s.substr(0, pos), mine);
			my_boolean_expr = new boolean_expr(loop_condition, mine, boolean);
		}

	}
	else if (s.substr(0, 2).compare("if") == 0) {
		i = s.substr(0,2);
		s = s.substr(2);
		int pos = s.rfind("end if");
		ed_if = s.substr(pos, 6);
		s = s.substr(0, pos);
		for (; s[0] == ' ' || s[0] == '\n'; s = s.substr(1));
		for (; s[s.length() - 1] == ' ' || s[s.length() - 1] == '\n'; s.pop_back());
	    pos = op_split(s, if_pr);
		if (pos != -1) {
			f = 4;
			int tmp = s.find("then");
			my_boolean_expr = new boolean_expr(s.substr(0, tmp), mine, boolean);
			t = s.substr(tmp, 4);
			if(my_boolean_expr->val)
				my_commandsequence = new commandsequence(s.substr(tmp + 4, pos - tmp - 4), mine);
			el = s.substr(pos, 4);
			s = s.substr(pos + 4);
			tmp = s.rfind("end if");
			if(!my_boolean_expr->val)
				another_commandsequence = new commandsequence(s.substr(0,tmp), mine);
		}
		else {
			f = 5;
			int tmp = s.find("then");
			my_boolean_expr = new boolean_expr(s.substr(0, tmp), mine, boolean);
			t = s.substr(tmp, 4);
			s = s.substr(pos + 4);
			tmp = s.rfind("end if");
			if(my_boolean_expr->val)
				my_commandsequence= new commandsequence(s.substr(0,tmp), mine);
		}
	}
	else if (regex_search(s,regex("\\:\\="))) {
		f = 3;
		int pos = s.find(":=");
		assign = s.substr(pos, 2);
		my_variable = new variable(s.substr(0, pos));
		//condition
		switch (lookup_type(my_variable->mine,mine))
		{
		case integer:
			break;
		case boolean:
			break;
		case undefined:
			cout << "Target variable not declared";
			abort();
			break;
		case program:
			cout << "Target variable same as program name";
			abort();
			break;
		default:
			break;
		}
		my_expr = new expr(s.substr(pos + 2),mine,lookup_type(my_variable->mine,mine));
		int n = var_exist(my_variable->mine);
		if (n != -1)
			vars[n].second = my_expr->val;
		else vars.push_back({ my_variable->mine, my_expr->val });
	}
}

commandsequence::commandsequence(string s, Symbol_table st) :mine(st) {
	clear_spaces(s);
	if (!s.empty()) {
		int pos = s.find(";");
		if (pos != -1) {
			f = 1;
			my_command = new command(s.substr(0, pos), st);
			semicolon = s[pos];
			my_commandsequence = new commandsequence(s.substr(pos + 1), st);
		}
		else {
			f = 0;
			my_command = new command(s.substr(0, pos), st);
		}
	}
}

