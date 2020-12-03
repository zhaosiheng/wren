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




class Program;
class block;
class declarationsequence;
class declaration;
class variable_list;
class commandsequence;
class command;
class expr;
class boolean_expr;
class variable;
class type;


class type {
public:
	string my_code;
	type(string s) {
		clear_spaces(s);
		if (s.compare("boolean") == 0)
			my_code = "bool";
		if (s.compare("integer") == 0)
			my_code = "int";
	}
};
class variable {
public:

	string my_code;
	variable(string s) {
		clear_spaces(s);
		my_code = s;
	}
};
class expr {
public:

	string my_code;
	expr(string s);
};




class boolean_expr {
public:
	string my_code;
	boolean_expr(string s) {
		clear_spaces(s); 
		s = regex_replace(s, regex("<>"), "!=");
		s = regex_replace(s, regex("or"), "||");
		s = regex_replace(s, regex("and"), "&&");
		s = regex_replace(s, regex("not"), "!");
		my_code = s;
	}
};

class variable_list {
public:
	
	string my_code;
	variable_list(string s) {
		clear_spaces(s);
		my_code = s;
	}
};
class declaration {
public:

	variable_list* my_variable_list;

	type* my_type;

	string my_code;
	declaration(string s) {
		int pos = s.find("var");
		s = s.substr(pos + 3);
		clear_spaces(s);
		pos = s.find(":");
		my_variable_list = new variable_list(s.substr(0,pos));
		s = s.substr(pos+1);
		clear_spaces(s);
		pos = s.find(";");
		my_type = new type(s.substr(0, pos));
		my_code = my_type->my_code + " " + my_variable_list->my_code+";\n";
	}
};
class declarationsequence {
public:
	int f;
	declaration* my_declaration;
	declarationsequence* my_declarationsequence;

	string my_code;
	declarationsequence(string s)  {
		clear_spaces(s);
		f = 0;
		if (!s.empty()) {
			f = 1;
			int pos = s.find(";");
			my_declaration = new declaration(s.substr(0, pos));
			my_declarationsequence = new declarationsequence(s.substr(pos + 1));

			my_code = my_declaration->my_code + my_declarationsequence->my_code;
		}
	}
};
class command {
public:
	int f;//0-read,1-write,2-while,3-:=,4-if_else,5-if
	variable* my_variable;

	expr* my_expr;

	boolean_expr* my_boolean_expr;

	commandsequence* my_commandsequence;

	commandsequence* another_commandsequence;

	string my_code;
	command(string s);
};
class commandsequence {
public:
	int f;
	command* my_command;

	commandsequence* my_commandsequence;

	string my_code;
	commandsequence(string s);
};
class block {
public:
	declarationsequence* my_declarationsequence;
	
	commandsequence* my_commandsequence;
	string my_code;
	block(string s) {
		clear_spaces(s);
		int pos = s.find("begin");
		my_declarationsequence = new declarationsequence(s.substr(0,pos));
		s = s.substr(pos+5);

		pos = s.rfind("end");
		
		my_commandsequence = new commandsequence(s.substr(0, pos));
		my_code = my_declarationsequence->my_code + my_commandsequence->my_code;
	}
};
class Program {
public:
	string my_code;
	block* my_block;
	Program(string s) {
		
		int pos = s.find("is");
		
		s = s.substr(pos+2);

		my_block = new block(s);
		my_code = "int main()\n{\n" + my_block->my_code + "\nreturn 0;\n}";
	}

};


expr::expr(string s) {
	clear_spaces(s);
	s = regex_replace(s, regex("<>"), "!=");
	s = regex_replace(s, regex("or"), "||");
	s = regex_replace(s, regex("and"), "&&");
	s = regex_replace(s, regex("not"), "!");
	my_code = s;
}

command::command(string s)  {
	clear_spaces(s);
	if (s.substr(0, 4).compare("read") == 0) {
		f = 0;
		
		my_variable = new variable(s.substr(5));
		my_code = "scanf(\"%d\",&" + my_variable->my_code + ");";
	}
	else if (s.substr(0, 5).compare("write") == 0) {
		f = 1;
		
		my_expr = new expr(s.substr(6));
		my_code = "printf(\"%d\","+my_expr->my_code+");";
	}
	else if (s.substr(0, 5).compare("while") == 0) {
		f = 2;
		
		s = s.substr(5);
		int pos = s.find("do");
		
		my_boolean_expr = new boolean_expr(s.substr(0, pos - 1));
		s = s.substr(pos + 2);
	    pos = s.rfind("end while");
		
		my_commandsequence = new commandsequence(s.substr(0, pos));
		my_code = "while(" + my_boolean_expr->my_code + ")\n{" + my_commandsequence->my_code + "}";
	}
	else if (s.substr(0, 2).compare("if") == 0) {
		
		s = s.substr(2);
		int pos = s.rfind("end if");
		
		s = s.substr(0, pos);
		clear_spaces(s);
		pos = op_split(s, if_pr);
		if (pos != -1) {
			f = 4;
			int tmp = s.find("then");
			my_boolean_expr = new boolean_expr(s.substr(0, tmp));
			
			my_commandsequence = new commandsequence(s.substr(tmp + 4, pos - tmp - 4));;
			s = s.substr(pos + 4);
			tmp = s.rfind("end if");
			another_commandsequence = new commandsequence(s.substr(0,tmp));
			my_code = "if("+my_boolean_expr->my_code+")\n{"+my_commandsequence->my_code+"}\nelse\n{"+another_commandsequence->my_code+"}";
		}
		else {
			f = 5;
			int tmp = s.find("then");
			my_boolean_expr = new boolean_expr(s.substr(0, tmp));
			
			s = s.substr(pos + 4);
			tmp = s.rfind("end if");
			my_commandsequence= new commandsequence(s.substr(0,tmp));
			my_code = "if("+my_boolean_expr->my_code+")\n{"+my_commandsequence->my_code+"}";
		}
	}
	else if (regex_search(s,regex("\\:\\="))) {
		f = 3;
		int pos = s.find(":=");
		
		my_variable = new variable(s.substr(0, pos));
		
		my_expr = new expr(s.substr(pos + 2));
		my_code = my_variable->my_code + "=" + my_expr->my_code + ";";
	}
}

commandsequence::commandsequence(string s) {
	clear_spaces(s);
	if (!s.empty()) {
		int pos = s.find(";");
		if (pos != -1) {
			f = 1;
			my_command = new command(s.substr(0, pos));
			
			my_commandsequence = new commandsequence(s.substr(pos + 1));
			my_code = my_command->my_code +"\n"+ my_commandsequence->my_code;
		}
		else {
			f = 0;
			my_command = new command(s.substr(0, pos));
			my_code = my_command->my_code;
		}
	}
}

