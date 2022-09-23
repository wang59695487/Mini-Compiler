#include <iostream>
#include <vector>
#include <fstream>
#include "node.h"
#include "compiler.hpp"
#define FILE_PATH "../test/test.c"
#define RESULT_PATH "../result/"

using namespace std;


extern FILE *yyin;
extern node* root;
extern bool IsError;
extern int errorNum;
int yyparse();

char* itoa(int i){
    static char str[20];
    sprintf(str,"%d",i);
    return str;
}

typedef struct VAR* ptr_VAR;

struct VAR{
    bool state;
    string name;
    string type;
};

class Data{
private:
    
    bool *temps;//false no use
    int num;
    ptr_VAR vars;
    ptr_VAR ARG;
public:
    void operator=(const Data& d){
        num=d.num;
        temps = new bool [num];
        vars = new VAR [num];
        ARG = new VAR [num];
        for(int i=0;i<num;i++)
        {
            temps[i]=d.temps[i];
            vars[i]=d.vars[i];
        }
    }
    Data(const Data& d){
        num=d.num;
        temps = new bool [num];
        vars = new VAR [num];
        for(int i=0;i<num;i++)
        {
            temps[i]=d.temps[i];
            vars[i]=d.vars[i];
        }
    }
    Data(int num = 32){
        this->num=num;
        temps = new bool [num];
        vars = new VAR [num];
        for(int i=0;i<num;i++)
        {
            temps[i]=false;
            vars[i].state=false;
        }
    }
    ~Data(){
        delete [] temps;
        delete [] vars;
    }
    int getTemp(){
        for(int i=0;i<num;i++)
        {
            if(temps[i]==false)
            {
                temps[i]=true;
                //cout<<"debug----"<<i<<endl;
                return i;
            }
        }
        return -1;
    }
    void release_Temp(int i){
        bool flag = false;
        temps[i]=flag;
    }
    int setVar(string name,string type){
        bool flag  = false;
        bool flag1 = true;
        for(int i=0;i<num;i++)
        {
            if(vars[i].state==flag)
            {
                vars[i].state=flag1;
                vars[i].name=name;
                vars[i].type=type;
                return i;
            }
        }
        return -1;
    }
    int getVar(string name){
        bool flag = true;
        for(int i=num-1;i>=0;i--)//find the recent one
        {
            if(vars[i].state==flag)
            {
                if(vars[i].name==name)
                return i;
            }
        }
        return -1;
    }
    void releaseVar(int i){
        bool flag = false;
        vars[i].state=flag;
    }
    
};


/*----------------------------ErrogLog----------------------------*/

struct line{
    string content;
    int linenum;
};

class ErrorLog{
    vector<struct line> log;
public:
    
    void addLog(string content, int linenum){
        struct line t;
        t.content = content;
        t.linenum = linenum;
        log.push_back(t);
    }
    void printErrorLog(){
        string s = "ErrorLog.txt";
        ofstream mycout(RESULT_PATH+s);
        for(int i=0; i<log.size();i++){
            mycout<<"line "<<log[i].linenum;
            mycout<<" : " << log[i].content << endl;
        }
        mycout.close();
    }
};

/*----------------------------Function----------------------------*/
struct para{
    string paratype;
    string paraname;
};


struct funcrecord{
    string functype;
    string funcname;
    vector<para> funcpara;
    int getParaNum(){
        return funcpara.size();
    }
};

class Function{
public:
    vector<funcrecord> Record;
    int size = 0;
    void addfunc(struct funcrecord record){
        Record.push_back(record);
        size++;
    }
    
    void printAllFunc(){
        cout << endl <<  "-------------- Function-Table ----------------" << endl << endl;
        for(int i=0;i<size;i++){
            cout << Record[i].functype << " " << Record[i].funcname <<"(";
            for(int j=0;j<Record[i].getParaNum();j++){
                if(j!=Record[i].getParaNum()-1)
                cout<<Record[i].funcpara[j].paratype<<" "<<Record[i].funcpara[j].paraname<<" , ";
                else
                cout<<Record[i].funcpara[j].paratype<<" "<<Record[i].funcpara[j].paraname;
            }
            cout<<")";
            cout<<endl;
        }
    }
    
    bool funcExist(string funcname){
        if(funcname.compare("print")==0) return true;
        else if(funcname.compare("read")==0) return true;
        for(int i=0;i<Record.size();i++){
            string name = Record[i].funcname;
            if(funcname.compare(name) == 0){
                return true;
            }
        }
        return false;
    }
};

/*----------------------------IR-Tree----------------------------*/
class IRGenerator{
    node* root;
    bool *labels;
    const int MAX_LABELS = 1000;
    Data r = Data(32);
    bool isloop;
    int startlabel;
    int endlabel;
    bool func_call_visual = true;
    Function FunctionRecord;
    ErrorLog Log;
public:
    IRGenerator(node* root){
        this->root = root;
        labels = new bool[MAX_LABELS];
        for(int i=0;i<MAX_LABELS;i++)
        {
            labels[i]=false;
        }
        stratTranslate(this->root,r);
        cout << endl;
        Log.printErrorLog();
    }
    
    ~IRGenerator(){
        delete[] labels;
    }
    
    int getLabel(){
        for(int i=0;i<MAX_LABELS;i++)
        {
            if(labels[i]==false)
            {
                labels[i]=true;
                return i;
            }
        }
        return -1;
    }
    void releaseLabel(int i){
        labels[i]=false;
    }
    
    
    string stratTranslate(node* root,Data* r) {
        return stratTranslate(root, *r);
    }
    
    
    string stratTranslate(node* root,Data &r) {
        node* t = root;
        string value="";
        while (t) {
            
            if(t->type.compare("function_definition")==0){
                cout << "FUNCTION " << t->contents[1]->content << " :" << endl;
                struct funcrecord f;
                f.functype = t->contents[0]->content;
                f.funcname = t->contents[1]->content;
                if(t->sub.size()>1){
                    node* psub = t->sub[0];
                    while(psub){
                        struct para p;
                        p.paratype = psub->contents[0]->content;
                        p.paraname = psub->contents[1]->content;
                        //cout << "-----" << p.paratype  << "----" << p.paraname << endl;
                        f.funcpara.push_back(p);
                        if(psub->next){
                            psub = psub->next;
                        }
                        else{
                            break;
                        }
                    }
                }
                FunctionRecord.addfunc(f);
                loop(t,r);
            }
            
            else if(t->type.compare("parameter_declaration")==0){
                r.setVar(t->contents[1]->content,t->contents[0]->content);
                cout << "PARAM var" <<  r.getVar(t->contents[1]->content) << endl;
                loop(t,r);
            }
            else if (t->type.compare("declaration")==0){
                string ttype = t->sub[0]->contents[0]->content;
                for(int i=0;i<t->sub[1]->sub.size();i++){
                    node* tt = t->sub[1]->sub[i];
                    r.setVar(tt->contents[0]->content,ttype);
                    loop(tt,r);
                }
            }
            else if (t->type.find("expression")!=-1&&t->type.compare("conditional_expression2")!=0){
                node* tt=t->sub[0];
                string r1,r2,r3,l;
                if(tt->sub.size()==0){
                    if(tt->contents[0]->name.compare("IDENTIFIER")==0)
                    r1 = string("var").append(itoa(r.getVar(tt->contents[0]->content)));
                    else if(tt->contents[0]->name.compare("CONSTANT")==0){
                        int i = r.getTemp();
                        cout<<"Temp"<<itoa(i)<<" = "<<"#"<<tt->contents[0]->content<<endl;
                        r1 = string("Temp").append(itoa(i));
                    }
                }
                else
                r1 = stratTranslate(tt,new Data(r));
                
                tt=t->sub[2];
                if(tt->sub.size()==0){
                    if(tt->contents[0]->name.compare("IDENTIFIER")==0)
                    r3 = string("var").append(itoa(r.getVar(tt->contents[0]->content)));
                    else if(tt->contents[0]->name.compare("CONSTANT")==0){
                        int i = r.getTemp();
                        cout<<"Temp"<<itoa(i)<<" = "<<"#"<<tt->contents[0]->content<<endl;
                        r3 = string("Temp").append(itoa(i));
                    }
                }
                else{
                    if(tt->type == "function_call"){
                        func_call_visual = false;
                        stratTranslate(tt,r);
                        int i = r.getTemp();
                        cout << "Temp" << i << " = CALL " << tt->sub[0]->contents[0]->content << endl;
                        r3 = string("Temp") + to_string(i);
                        r.release_Temp(i);
                    }
                    else  r3 = stratTranslate(tt,new Data(r));
                }
                
                tt = t->sub[1];
                r2 = tt->contents[0]->content;
                if(tt->contents[0]->name.compare("assignment_operator")==0){
                    cout<<r1<<" "<<r2<<" "<<r3<<endl;
                    value = r1;
                }
                else{
                    l = string("Temp").append(itoa(r.getTemp()));
                    value = l;
                    cout<< l<<" = "<<r1<<" "<<r2<<" "<<r3<<endl;
                }
            }
            
            else if(t->type == "if_statement"){
                string resReg = stratTranslate(t->sub[0],r);
                int label_true = getLabel();
                int label_false = getLabel();
                cout << "IF " << resReg << " == 0 GOTO label_" << label_true << endl;
                cout << "GOTO label_" << label_false << endl;
                cout << "LABEL label_" << label_true<<" :" << endl;
                stratTranslate(t->sub[1],r);
                cout << "LABEL label_" << label_false <<" :" << endl;
            }
            
            
            else if(t->type == "if_else_statement"){
                int label_true = getLabel();
                int label_false = getLabel();
                int label_next = getLabel();
                string resReg = stratTranslate(t->sub[0],r);
                cout << "IF " << resReg << " != 0 GOTO label_" << label_true << endl;
                cout << "GOTO label_" << label_false << endl;
                cout << "LABEL label_" << label_true<<" :" << endl;
                stratTranslate(t->sub[1],r);
                cout << "GOTO label_" << label_next << endl;
                cout << "LABEL label_" << label_false <<" :" << endl;
                stratTranslate(t->sub[2],r);
                cout << "LABEL label_" << label_next <<" :" << endl;
            }
            
            else if(t->type == "break"){
                cout << "GOTO label_" << this->endlabel << endl;
            }
            
            else if(t->type == "continue"){
                cout << "GOTO label_" << this->startlabel << endl;
            }
            
            else if(t->type == "while_statement"){
                int label_while = getLabel();
                int label_start = getLabel();
                int label_end = getLabel();
                isloop = true;
                startlabel = label_while;
                endlabel = label_end;
                
                cout << "LABEL label_" << label_while <<" :" << endl;
                string resReg = stratTranslate(t->sub[0],r);
                cout << "IF " << resReg << " != 0 GOTO label_" << label_start << endl;
                cout << "GOTO label_" << label_end << endl;
                cout << "LABEL label_" << label_start <<" :" << endl;
                stratTranslate(t->sub[1],r);
                cout << "GOTO label_" << label_while << endl;
                cout << "LABEL label_" << label_end <<" :" << endl;
            }
            
            else if(t->type == "do_while_statement"){
                int beginlabel = getLabel();
                startlabel = getLabel();
                endlabel = getLabel();
                cout << "LABEL label_" << beginlabel <<" :" << endl;
                stratTranslate(t->sub[0],r);
                cout << "LABEL label_" << startlabel <<" :" << endl;
                string resReg = stratTranslate(t->sub[1],r);
                cout << "IF " << resReg << " != 0 GOTO label_" << beginlabel << endl;
                cout << "LABEL label_" << endlabel <<" :" << endl;
                
            }
            
            else if(t->type == "for_statement_exp3"){
                int label_judge = getLabel();
                int label_start = getLabel();
                
                
                startlabel = getLabel();
                endlabel = getLabel();
                
                stratTranslate(t->sub[0],r);
                cout << "LABEL label_" << label_judge <<" :" << endl;
                string resReg = stratTranslate(t->sub[1],r);
                cout << "IF " << resReg << " != 0 GOTO label_" << label_start << endl;
                cout << "GOTO label_" << endlabel << endl;
                cout << "LABEL label_" << label_start <<" :" << endl;
                stratTranslate(t->sub[3],r);
                cout << "LABEL label_" << startlabel <<" :" << endl;
                stratTranslate(t->sub[2],r);
                cout << "GOTO label_" << label_judge << endl;
                cout << "LABEL label_" << endlabel <<" :" << endl;
            }
            
            else if(t->type == "function_call"){
                string res;
                string funcname = t->sub[0]->contents[0]->content;
                if (FunctionRecord.funcExist(funcname)== false){
                    string hint = "No function named with ";
                    hint += funcname;
                    Log.addLog(hint,t->sub[0]->contents[0]->lineNum);
                }
                // IF have parameter
                if(t->sub.size()==2){
                    node* s = t->sub[1];
                    vector<string> ARG;
                    vector<int> TEMP;
                    int j = 0;
                    while(s!=nullptr){
                        if(s->type == "primary_expression"){
                            if(s->contents[0]->name == "CONSTANT"){
                                int i = r.getTemp();
                                cout << "Temp" << i << " = #" << s->contents[0]->content <<endl;
                                //cout << "ARG" << "  " << "Temp" << i  << endl;
                                string argname = "Temp";
                                argname += to_string(i);
                                ARG.push_back(argname);
                                TEMP.push_back(i);
                            }
                            if(s->contents[0]->name == "IDENTIFIER"){
                                string varname = s->contents[0]->content;
                                //cout << "ARG" << "  Var" << r.getVar(s->contents[0]->content)  << endl;
                                string argname = "var";
                                argname += to_string(r.getVar(s->contents[0]->content));
                                ARG.push_back(argname);
                            }
                        }
                        else if(s->type == "function_call"){
                            stratTranslate(s,r);
                            int i = r.getTemp();
                            res = "Temp";
                            res += to_string(i);
                            cout << res << " = " << "CALL " << s->sub[0]->contents[0]->content << endl;
                            func_call_visual = false;
                            cout << "ARG " << res << endl;
                            ARG.push_back(res);
                            r.release_Temp(i);
                        }
                        else{
                            string res = stratTranslate(s,r);
                            ARG.push_back(res);
                        }
                        
                        s = s->next;
                    }
                    for(j = ARG.size()-1;j>=0;j--) cout << "ARG" << " "  << ARG[j] << endl;
                }
                if(func_call_visual == true)
                cout << "CALL " << t->sub[0]->contents[0]->content << endl;
                else func_call_visual = true;
            }
            
            else if(t->type == "return_statement"){
                if(t->sub.size()==0) cout << "RETURN" << endl;
                else{
                    node* s = t->sub[0];
                    string res;
                    if(s->type == "primary_expression"){
                        if(s->contents[0]->name == "CONSTANT"){
                            int i = r.getTemp();
                            cout << "Temp" << i << " = #" << s->contents[0]->content << endl;
                            res = "Temp";
                            res = res + to_string(i);
                        }
                        if(s->contents[0]->name == "IDENTIFIER"){
                            res = "var";
                            res += to_string(r.getVar(s->contents[0]->content));
                        }
                    }
                    else res = stratTranslate(s,r);
                    cout << "RETURN " << res << endl;
                }
            }
            t = t->next;
        }
        return value;
    }
    
    void loop(node* t,Data* r){
        for (int i = 0; i < t->sub.size(); i++){
            stratTranslate(t->sub[i],r);
        }
    }
    
    void loop(node* t,Data r){
        for (int i = 0; i < t->sub.size(); i++){
            stratTranslate(t->sub[i],r);
        }
    }
};

/*----------------------------AST----------------------------*/
class ASTGenerator{
    node* root;
    bool *labels;
    const int MAX_LABELS = 1000;
    Data r = Data(32);
    bool isloop;
    int startlabel;
    int endlabel;
    bool func_call_visual = true;
    Function FunctionRecord;
    //ErrorLog Log;
public:
    ASTGenerator(node* root){
        this->root = root;
        labels = new bool[MAX_LABELS];
        for(int i=0;i<MAX_LABELS;i++)
        {
            labels[i]=false;
        }
        stratTranslate(this->root,r);
        cout << endl;
        FunctionRecord.printAllFunc();
        //Log.printErrorLog();
    }
    
    ~ASTGenerator(){
        delete[] labels;
    }
    
    int getLabel(){
        for(int i=0;i<MAX_LABELS;i++)
        {
            if(labels[i]==false)
            {
                labels[i]=true;
                return i;
            }
        }
        return -1;
    }
    void releaseLabel(int i){
        labels[i]=false;
    }
    
    string stratTranslate(node* root,Data* r) {
        return stratTranslate(root, *r);
    }
    
    string stratTranslate(node* root,Data &r) {
        node* t = root;
        string value="";
        while (t) {
            
            if(t->type.compare("function_definition")==0){
                cout << " —— " << t->type << " —— FUNCTION" << " —— " << t->contents[1]->content << endl << "|" << endl;
                struct funcrecord f;
                f.functype = t->contents[0]->content;
                f.funcname = t->contents[1]->content;
                if(t->sub.size()>1){
                    node* psub = t->sub[0];
                    while(psub){
                        struct para p;
                        p.paratype = psub->contents[0]->content;
                        p.paraname = psub->contents[1]->content;
                        //cout << "-----" << p.paratype  << "----" << p.paraname << endl;
                        f.funcpara.push_back(p);
                        if(psub->next){
                            psub = psub->next;
                        }
                        else{
                            break;
                        }
                    }
                }
                FunctionRecord.addfunc(f);
                loop(t,r);
            }
            
            else if(t->type.compare("parameter_declaration")==0){
                // cout << "PARAM " <<  t->contents[0]->content << endl;
                r.setVar(t->contents[1]->content,t->contents[0]->content);
                cout << " —— " << t->type << " —— PARAM var —— " <<  r.getVar(t->contents[1]->content) << endl << "|" << endl;
                loop(t,r);
            }
            
            else if (t->type.compare("declaration")==0){
                string ttype = t->sub[0]->contents[0]->content;
                for(int i=0;i<t->sub[1]->sub.size();i++){
                    node* tt = t->sub[1]->sub[i];
                    //cout<<"debug----"<<tt->contents[0]->content<<endl;
                    r.setVar(tt->contents[0]->content,ttype);
                    loop(tt,r);
                }
            }
            
            else if (t->type.find("expression")!=-1&&t->type.compare("conditional_expression2")!=0){
                cout << " —— " << t->type << endl << "|" << endl;
                node* tt=t->sub[0];
                string r1,r2,r3,l;
                if(tt->sub.size()==0){
                    if(tt->contents[0]->name.compare("IDENTIFIER")==0)
                    r1 = string("var").append(itoa(r.getVar(tt->contents[0]->content)));
                    else if(tt->contents[0]->name.compare("CONSTANT")==0){
                        int i = r.getTemp();
                        cout<<" —— Temp —— "<<t->contents[0]->content<< endl << "|" << endl;
                        r1 = string("Temp").append(itoa(i));
                    }
                }
                else
                r1 = stratTranslate(tt,new Data(r));
                
                
                tt=t->sub[2];
                if(tt->sub.size()==0){
                    if(tt->contents[0]->name.compare("IDENTIFIER")==0)
                    r3 = string("var").append(itoa(r.getVar(tt->contents[0]->content)));
                    else if(tt->contents[0]->name.compare("CONSTANT")==0){
                        int i = r.getTemp();
                        cout<<" —— Temp —— "<<tt->contents[0]->content<<endl << "|" << endl;
                        r3 = string("Temp").append(itoa(i));
                    }
                }
                else{
                    if(tt->type == "function_call"){
                        func_call_visual = false;
                        stratTranslate(tt,r);
                        int i = r.getTemp();
                        cout << " —— Temp —— " << " CALL " << " —— " << tt->sub[0]->contents[0]->content << endl << "|" << endl;
                        r3 = string("Temp") + to_string(i);
                        r.release_Temp(i);
                    }
                    else{
                        r3 = stratTranslate(tt,new Data(r));
                    }
                }
                
                tt = t->sub[1];
                r2 = tt->contents[0]->content;
                if(tt->contents[0]->name.compare("assignment_operator")==0){
                    value = r1;
                }
                else{
                    l = string("Temp").append(itoa(r.getTemp()));
                    value = l;
                }
                
            }
            
            else if(t->type == "if_statement"){
                cout << " —— " << t->type << endl << "|" << endl;
                string resReg = stratTranslate(t->sub[0],r);
                int label_true = getLabel();
                int label_false = getLabel();
                cout << " —— IF —— " << resReg << "== 0" << " —— GOTO label  —— " << label_true << endl << "|" << endl;
                cout << " —— GOTO label —— " << label_false << endl << "|" << endl;
                cout << " —— LABEL label —— " << label_true << endl << "|" << endl;
                stratTranslate(t->sub[1],r);
                cout << " —— LABEL label —— " << label_false << endl << "|" << endl;
            }
            
            
            else if(t->type == "if_else_statement"){
                cout << " —— " << t->type << endl << "|" << endl;
                int label_true = getLabel();
                int label_false = getLabel();
                int label_next = getLabel();
                string resReg = stratTranslate(t->sub[0],r);
                cout << " —— IF —— " << resReg << "!= 0" << " —— GOTO label —— " << label_true << endl << "|" << endl;
                cout << " —— GOTO label —— " << label_false << endl << "|" << endl;
                cout << " —— LABEL label —— " << label_true << endl << "|" << endl;
                stratTranslate(t->sub[1],r);
                cout << " —— GOTO label —— " << label_next << endl << "|" << endl;
                cout << " —— LABEL label —— " << label_false << endl << "|" << endl;
                stratTranslate(t->sub[2],r);
                cout << " —— LABEL label —— " << label_next << endl << "|" << endl;
            }
            
            else if(t->type == "break"){
                cout << " —— " << t->type << endl << "|" << endl;
                cout << " —— GOTO label —— " << this->endlabel << endl;
            }
            
            else if(t->type == "continue"){
                cout << " —— " << t->type << endl << "|" << endl;
                cout << " —— GOTO label —— " << this->startlabel << endl << "|" << endl;
            }
            
            else if(t->type == "while_statement"){
                cout << " —— " << t->type << endl << "|" << endl;
                int label_while = getLabel();
                int label_start = getLabel();
                int label_end = getLabel();
                isloop = true;
                startlabel = label_while;
                endlabel = label_end;
                
                cout << " —— LABEL label —— " << label_while << endl << "|" << endl;
                string resReg = stratTranslate(t->sub[0],r);
                cout << " —— IF —— " << resReg << "!= 0" << " —— GOTO label —— " << label_start << endl << "|" << endl;
                cout << " —— GOTO label —— " << label_end << endl << "|" << endl;
                cout << " —— LABEL label —— " << label_start << endl << "|" << endl;
                stratTranslate(t->sub[1],r);
                cout << " —— GOTO label —— " << label_while << endl << "|" << endl;
                cout << " —— LABEL label —— " << label_end << endl << "|" << endl;
            }
            
            else if(t->type == "do_while_statement"){
                cout << " —— " << t->type << endl << "|" << endl;
                int beginlabel = getLabel();
                startlabel = getLabel();
                endlabel = getLabel();
                cout << " —— LABEL label —— " << beginlabel << endl << "|" << endl;
                stratTranslate(t->sub[0],r);
                cout << "  ——  LABEL label —— " << startlabel << endl << "|" << endl;
                string resReg = stratTranslate(t->sub[1],r);
                cout << "  ——  IF —— " << resReg << "!= 0" << " —— GOTO label —— " << beginlabel << endl << "|" << endl;
                cout << "  ——  LABEL label —— " << endlabel << endl << "|" << endl;
            }
            
            else if(t->type == "for_statement_exp3"){
                cout << " —— " << t->type << endl << "|" << endl;
                int label_judge = getLabel();
                int label_start = getLabel();
                
                
                startlabel = getLabel();
                endlabel = getLabel();
                
                stratTranslate(t->sub[0],r);
                cout << " —— LABEL label —— " << label_judge << endl << "|" << endl;
                string resReg = stratTranslate(t->sub[1],r);
                cout << " —— IF —— " << resReg << "!= 0" << " —— GOTO label —— " << label_start << endl << "|" << endl;
                cout << " —— GOTO label —— " << endlabel << endl << "|" << endl;
                cout << " —— LABEL label —— " << label_start << endl << "|" << endl;
                stratTranslate(t->sub[3],r);
                cout << " —— LABEL label —— " << startlabel << endl << "|" << endl;
                stratTranslate(t->sub[2],r);
                cout << " —— GOTO label —— " << label_judge << endl << "|" << endl;
                cout << " —— LABEL label —— " << endlabel << endl << "|" << endl;
            }
            
            else if(t->type == "function_call"){
                cout << " —— " << t->type << endl << "|" << endl;
                string res;
                string funcname = t->sub[0]->contents[0]->content;
                if (FunctionRecord.funcExist(funcname)== false){
                    string hint = "No function named with ";
                    hint += funcname;
                    //Log.addLog(hint,t->sub[0]->contents[0]->lineNum);
                }
                // have parameter
                if(t->sub.size()==2){
                    node* s = t->sub[1];
                    vector<string> ARG;
                    vector<int> TEMP;
                    while(s!=nullptr){
                        if(s->type == "primary_expression"){
                            cout << " —— " << t->type << endl << "|" << endl;
                            if(s->contents[0]->name == "CONSTANT"){
                                int i = r.getTemp();
                                cout << " —— Temp —— " << s->contents[0]->content <<endl << "|" << endl;
                                //cout << "ARG" << "  " << "Temp" << i  << endl;
                                string argname = "Temp";
                                argname += to_string(i);
                                ARG.push_back(argname);
                                TEMP.push_back(i);
                            }
                            if(s->contents[0]->name == "IDENTIFIER"){
                                string varname = s->contents[0]->content;
                                //cout << "ARG" << "  Var" << r.getVar(s->contents[0]->content)  << endl;
                                string argname = "var";
                                argname += to_string(r.getVar(s->contents[0]->content));
                                ARG.push_back(argname);
                            }
                        }
                        else if(s->type == "function_call"){
                            cout << " —— " << t->type << endl << "|" << endl;
                            stratTranslate(s,r);
                            int i = r.getTemp();
                            res = " —— Temp —— ";
                            res += to_string(i);
                            cout << res << " —— CALL —— " << s->sub[0]->contents[0]->content << endl << "|" << endl;
                            func_call_visual = false;
                            cout << " —— ARG" << res << endl << "|" << endl;
                            ARG.push_back(res);
                            r.release_Temp(i);
                        }
                        else{
                            string res = stratTranslate(s,r);
                            ARG.push_back(res);
                        }
                        
                        s = s->next;
                    }
                    for(int j=ARG.size()-1;j>=0;j--){
                        cout << " —— ARG" << ARG[j] << endl << "|" << endl;
                    }
                    
                    
                }
                if(func_call_visual == true)
                cout << " —— CALL —— " << t->sub[0]->contents[0]->content << endl << "|" << endl;
                else{
                    func_call_visual = true;
                }
            }
            
            else if(t->type == "return_statement"){
                cout << " —— " << t->type << endl << "|" << endl;
                if(t->sub.size()==0){
                    cout << "—— RETURN —— " << endl << "|" << endl;
                }
                else{
                    node* s = t->sub[0];
                    string res;
                    if(s->type == "primary_expression"){
                        cout << " —— " << t->type << endl << "|" << endl;
                        if(s->contents[0]->name == "CONSTANT"){
                            int i = r.getTemp();
                            cout << " —— Temp —— " << s->contents[0]->content << endl << "|" << endl;
                            res = "Temp";
                            res = res + to_string(i);
                        }
                        if(s->contents[0]->name == "IDENTIFIER"){
                            res = "var";
                            res += to_string(r.getVar(s->contents[0]->content));
                        }
                    }
                    else{
                        res = stratTranslate(s,r);
                    }
                    cout << " —— RETURN —— " << res << endl << "|" << endl;
                }
            }
            t = t->next;
        }
        return value;
    }
    
    void loop(node* t,Data* r){
        for (int i = 0; i < t->sub.size(); i++){
            stratTranslate(t->sub[i],r);
        }
    }
    
    void loop(node* t,Data r){
        for (int i = 0; i < t->sub.size(); i++){
            stratTranslate(t->sub[i],r);
        }
    }
};


int main()
{
    
    if(!fopen(FILE_PATH,"r")){
        cout << "Open file failure" << endl;
        exit(0);
    }else{
        yyin = fopen(FILE_PATH,"r");
    }
    yyparse();
    cout<<endl;cout<<endl;
    if(IsError){
        cout<<"There are "<<errorNum<<" syntax errors here!"<<endl;
    }else {
        cout << "IRTREE\n";
        IRGenerator IRtranslator(root);
        cout << "\n-------------- Abstract Syntax Tree --------------\n\n";
        ASTGenerator ASTtranslator(root);
    }
    return 0;
}
