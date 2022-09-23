%option yylineno

Dt			[0-9]
Le			[a-zA-Z_]
H			[a-fA-F0-9]
E			([Ee][+-]?{Dt}+)
P           ([Pp][+-]?{Dt}+)
FS			(f|F|le|Le)
IS          ((u|U)|(u|U)?(le|Le|lele|LeLe)|(le|Le|lele|LeLe)(u|U))

%{
#include <stdio.h>
#include "compiler.hpp"
#include "node.h"
void count_column(void);
void My_comment();
int check_type();
void yyerror(char const *s);
%}

%%
"/*"			{ My_comment(); }
"//"[^\n]*      { /* consume //-My_comment */ }
[ \t\v\n\f]		{ count_column(); }


"auto"			{ count_column(); yylval.Element = new element(yylineno,"storage_class_specifier","auto");return(AUTO); }
"_Bool"			{ count_column(); yylval.Element = new element(yylineno,"type_specifier","_Bool");return(BOOL); }
"break"			{ count_column(); yylval.Element = new element(yylineno,"reserved_keywords","break");return(BREAK); }
"case"			{ count_column(); return(CASE); }
"char"			{ count_column(); yylval.Element = new element(yylineno,"type_specifier","char"); return(CHAR); }
"_Complex"		{ count_column(); /*I abandon this. @txt*/return(COMPLEX); }
"const"			{ count_column(); yylval.Element = new element(yylineno,"type_qualifier","const");return(CONST); }
"continue"		{ count_column(); yylval.Element = new element(yylineno,"reserved_keywords","continue");return(CONTINUE); }
"default"		{ count_column(); return(DEFAULT); }
"do"			{ count_column(); yylval.Element = new element(yylineno, "iteration_specifier", "do"); return(DO); }
"double"		{ count_column(); yylval.Element = new element(yylineno,"type_specifier","double"); return(DOUBLE); }
"else"			{ count_column(); yylval.Element = new element(yylineno, "selection_specifier", "else"); return(ELSE); }
"enum"			{ count_column(); return(ENUM); }
"extern"		{ count_column(); yylval.Element = new element(yylineno,"storage_class_specifier","extern");return(EXTERN); }
"float"			{ count_column(); yylval.Element = new element(yylineno,"type_specifier","float"); return(FLOAT); }
"for"			{ count_column(); yylval.Element = new element(yylineno, "iteration_specifier", "for"); return(FOR); }
"goto"			{ count_column(); return(GOTO); }
"if"			{ count_column(); yylval.Element = new element(yylineno, "selection_specifier", "if"); return(IF); }
"_Imaginary"		{ count_column(); /*I abandon this. @txt*/return(IMAGINARY); }
"inline"		{ count_column(); yylval.Element = new element(yylineno,"function_specifier","inline");return(INLINE); }
"int"			{ count_column(); yylval.Element = new element(yylineno,"type_specifier","int"); return(INT); }
"long"			{ count_column(); yylval.Element = new element(yylineno,"type_specifier","long"); return(LONG); }
"register"		{ count_column(); yylval.Element = new element(yylineno,"storage_class_specifier","register");return(REGISTER); }
"restrict"		{ count_column(); yylval.Element = new element(yylineno,"type_qualifier","restrict");return(RESTRICT); }
"return"		{ count_column(); yylval.Element = new element(yylineno,"reserved_keywords","return");return(RETURN); }
"short"			{ count_column(); yylval.Element = new element(yylineno,"type_specifier","short"); return(SHORT); }
"signed"		{ count_column(); yylval.Element = new element(yylineno,"type_specifier","signed");return(SIGNED); }
"sizeof"		{ count_column(); return(SIZEOF); }
"static"		{ count_column(); yylval.Element = new element(yylineno,"storage_class_specifier","static");return(STATIC); }
"struct"		{ count_column(); return(STRUCT); }
"switch"		{ count_column(); yylval.Element = new element(yylineno, "selection_specifier", "swtich"); return(SWITCH); }
"typedef"		{ count_column(); yylval.Element = new element(yylineno,"storage_class_specifier","typedef");return(TYPEDEF); }
"union"			{ count_column(); return(UNION); }
"unsigned"		{ count_column(); yylval.Element = new element(yylineno,"type_specifier","unsigned");return(UNSIGNED); }
"void"			{ count_column(); yylval.Element = new element(yylineno,"type_specifier","void"); return(VOID); }
"volatile"		{ count_column(); yylval.Element = new element(yylineno,"type_qualifier","volatile");return(VOLATILE); }
"while"			{ count_column(); yylval.Element = new element(yylineno, "iteration_specifier", "while"); return(WHILE); }

{Le}({Le}|{Dt})*		{ count_column();yylval.Element = new element(yylineno,"IDENTIFIER",yytext); return(check_type()); }

0[xX]{H}+{IS}?		{ count_column(); yylval.Element = new element(yylineno,"CONSTANT",yytext);return(CONSTANT); }
0[0-7]*{IS}?		{ count_column(); yylval.Element = new element(yylineno,"CONSTANT",yytext);return(CONSTANT); }
[1-9]{Dt}*{IS}?		{ count_column(); yylval.Element = new element(yylineno,"CONSTANT",yytext);return(CONSTANT); }
Le?'(\\.|[^\\'\n])+'	{ count_column(); yylval.Element = new element(yylineno,"CONSTANT",yytext);return(CONSTANT); }

{Dt}+{E}{FS}?		{ count_column(); yylval.Element = new element(yylineno,"CONSTANT",yytext);return(CONSTANT); }
{Dt}*"."{Dt}+{E}?{FS}?	{ count_column(); yylval.Element = new element(yylineno,"CONSTANT",yytext);return(CONSTANT); }
{Dt}+"."{Dt}*{E}?{FS}?	{ count_column(); yylval.Element = new element(yylineno,"CONSTANT",yytext);return(CONSTANT); }
0[xX]{H}+{P}{FS}?	{ count_column(); yylval.Element = new element(yylineno,"CONSTANT",yytext);return(CONSTANT); }
0[xX]{H}*"."{H}+{P}{FS}?     { count_column(); yylval.Element = new element(yylineno,"CONSTANT",yytext);return(CONSTANT); }
0[xX]{H}+"."{H}*{P}{FS}?     { count_column(); yylval.Element = new element(yylineno,"CONSTANT",yytext);return(CONSTANT); }


Le?\"(\\.|[^\\"\n])*\"	{ count_column(); yylval.Element = new element(yylineno,"STRING_LITERAL",yytext);return(STRING_LITERAL); }

"..."			{ count_column(); yylval.Element = new element(yylineno,"ELLIPSIS",yytext);return(ELLIPSIS); }
">>="			{ count_column(); yylval.Element = new element(yylineno,"assignment_operator",yytext);return(RIGHT_ASSIGN); }
"<<="			{ count_column(); yylval.Element = new element(yylineno,"assignment_operator",yytext);return(LEFT_ASSIGN); }
"+="			{ count_column(); yylval.Element = new element(yylineno,"assignment_operator",yytext);return(ADD_ASSIGN); }
"-="			{ count_column(); yylval.Element = new element(yylineno,"assignment_operator",yytext);return(SUB_ASSIGN); }
"*="			{ count_column(); yylval.Element = new element(yylineno,"assignment_operator",yytext);return(MUL_ASSIGN); }
"/="			{ count_column(); yylval.Element = new element(yylineno,"assignment_operator",yytext);return(DIV_ASSIGN); }
"%="			{ count_column(); yylval.Element = new element(yylineno,"assignment_operator",yytext);return(MOD_ASSIGN); }
"&="			{ count_column(); yylval.Element = new element(yylineno,"assignment_operator",yytext);return(AND_ASSIGN); }
"^="			{ count_column(); yylval.Element = new element(yylineno,"assignment_operator",yytext);return(XOR_ASSIGN); }
"|="			{ count_column(); yylval.Element = new element(yylineno,"assignment_operator",yytext);return(OR_ASSIGN); }
">>"			{ count_column(); yylval.Element = new element(yylineno,"RIGHT_OP",yytext);return(RIGHT_OP); }
"<<"			{ count_column(); yylval.Element = new element(yylineno,"LEFT_OP",yytext);return(LEFT_OP); }
"++"			{ count_column(); yylval.Element = new element(yylineno,"INC_OP",yytext);return(INC_OP); }
"--"			{ count_column(); yylval.Element = new element(yylineno,"DEC_OP",yytext);return(DEC_OP); }
"->"			{ count_column(); yylval.Element = new element(yylineno,"PTR_OP",yytext);return(PTR_OP); }
"&&"			{ count_column(); yylval.Element = new element(yylineno,"AND_OP",yytext);return(AND_OP); }
"||"			{ count_column(); yylval.Element = new element(yylineno,"OR_OP",yytext);return(OR_OP); }
"<="			{ count_column(); yylval.Element = new element(yylineno,"LE_OP",yytext);return(LE_OP); }
">="			{ count_column(); yylval.Element = new element(yylineno,"GE_OP",yytext);return(GE_OP); }
"=="			{ count_column(); yylval.Element = new element(yylineno,"EQ_OP",yytext);return(EQ_OP); }
"!="			{ count_column(); yylval.Element = new element(yylineno,"NE_OP",yytext);return(NE_OP); }
";"			{ count_column(); return(';'); }
("{"|"<%")		{ count_column(); return('{'); }
("}"|"%>")		{ count_column(); return('}'); }
","			{ count_column(); return(','); }
":"			{ count_column(); return(':'); }
"="			{ count_column(); yylval.Element = new element(yylineno,"assignment_operator",yytext);return('='); }
"("			{ count_column(); return('('); }
")"			{ count_column(); return(')'); }
("["|"<:")		{ count_column(); return('['); }
("]"|":>")		{ count_column(); return(']'); }
"."			{ count_column(); return('.'); }
"&"			{ count_column(); yylval.Element = new element(yylineno,"unary_operator",yytext);return('&'); }
"!"			{ count_column(); yylval.Element = new element(yylineno,"unary_operator",yytext);return('!'); }
"~"			{ count_column(); yylval.Element = new element(yylineno,"unary_operator",yytext);return('~'); }
"-"			{ count_column(); yylval.Element = new element(yylineno,"unary_operator",yytext);return('-'); }
"+"			{ count_column(); yylval.Element = new element(yylineno,"unary_operator",yytext);return('+'); }
"*"			{ count_column(); yylval.Element = new element(yylineno,"unary_operator",yytext);return('*'); }
"/"			{ count_column(); yylval.Element = new element(yylineno,"unary_operator",yytext);return('/'); }
"%"			{ count_column(); yylval.Element = new element(yylineno,"unary_operator",yytext);return('%'); }
"<"			{ count_column(); yylval.Element = new element(yylineno,"unary_operator",yytext);return('<'); }
">"			{ count_column(); yylval.Element = new element(yylineno,"unary_operator",yytext);return('>'); }
"^"			{ count_column(); yylval.Element = new element(yylineno,"unary_operator",yytext);return('^'); }
"|"			{ count_column(); yylval.Element = new element(yylineno,"unary_operator",yytext);return('|'); }
"?"			{ count_column(); yylval.Element = new element(yylineno,"unary_operator",yytext);return('?'); }
s
.			{ /* Add code to complain about unmatched characters */ }

%%

int result = 1;
int column = 0;


int yywrap(void)
{
	
	return result;
}


void My_comment(void)
{
	char c = 0;
	char cc = 0;
  
	while ((c = yyinput()) != 0)      /* (EOF maps to 0) */
	{
		if (c == '/' && cc == '*')
			return;
		cc = c;
	}
	yyerror("unterminated My_comment");
}



void count_column(void)
{
	int i;

	for (i = 0; yytext[i] != '\0'; i++)
		if (yytext[i] == '\n')
			column = 0;
		else if (yytext[i] == '\t')
			column += 8 - (column % 8);
		else
			column++;

	ECHO;
}


int check_type(void)
{
	return IDENTIFIER;
}
