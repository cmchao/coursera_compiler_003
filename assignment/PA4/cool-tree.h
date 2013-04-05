#ifndef COOL_TREE_H
#define COOL_TREE_H
//////////////////////////////////////////////////////////
//
// file: cool-tree.h
//
// This file defines classes for each phylum and constructor
//
//////////////////////////////////////////////////////////


#include "tree.h"
#include "cool-tree.handcode.h"
#include "symtab.h"
#include <string>

class class__class;
class method_class;
class formal_class;

static std::string cur_class_filename;
static void semant_error(ostream& error_stream, tree_node *t, char *msg)
{
  error_stream << cur_class_filename << ":" << t->get_line_number()
               << ": " << msg << "\n";
}

// define the class for phylum
// define simple phylum - Program
typedef class Program_class *Program;

class Program_class : public tree_node {
public:
   tree_node *copy()		 { return copy_Program(); }
   virtual Program copy_Program() = 0;

   SymbolTable<Symbol, class__class> claz;
#ifdef Program_EXTRAS
   Program_EXTRAS
#endif
};


// define simple phylum - Class_
typedef class Class__class *Class_;

class Class__class : public tree_node {
public:
   tree_node *copy()		 { return copy_Class_(); }
   virtual Class_ copy_Class_() = 0;
   virtual Symbol get_parent(void) const = 0;
   virtual Symbol get_name(void) const = 0;
   virtual method_class* get_method(Symbol) = 0;

   SymbolTable<Symbol, Symbol> objs;
   SymbolTable<Symbol, method_class> funs;
   SymbolTable<Symbol, class__class> claz;
       
#ifdef Class__EXTRAS
   Class__EXTRAS
#endif
};


// define simple phylum - Feature
typedef class Feature_class *Feature;

class Feature_class : public tree_node {
public:
   tree_node *copy()		 { return copy_Feature(); }
   virtual Feature copy_Feature() = 0;
   virtual Symbol get_name() = 0;
   virtual Boolean is_method() = 0;
   virtual Symbol* get_type_addr() = 0;
   virtual void scan(SymbolTable<Symbol, Symbol>*, 
                     SymbolTable<Symbol, method_class>*,
                     SymbolTable<Symbol, class__class>*) = 0;

   SymbolTable<Symbol, Symbol> objs;
   SymbolTable<Symbol, method_class> funs;
   SymbolTable<Symbol, class__class> claz;

#ifdef Feature_EXTRAS
   Feature_EXTRAS
#endif
};


// define simple phylum - Formal
typedef class Formal_class *Formal;

class Formal_class : public tree_node {
public:
   tree_node *copy()		 { return copy_Formal(); }
   virtual Formal copy_Formal() = 0;
   virtual Symbol get_name() = 0;
   virtual void scan(SymbolTable<Symbol, Symbol>*, 
                    SymbolTable<Symbol, method_class>*,
                    SymbolTable<Symbol, class__class>*) = 0;

   SymbolTable<Symbol, Symbol> objs;
   SymbolTable<Symbol, method_class> funs;
   SymbolTable<Symbol, class__class> claz;

#ifdef Formal_EXTRAS
   Formal_EXTRAS
#endif
};


// define simple phylum - Expression
typedef class Expression_class *Expression;

class Expression_class : public tree_node {
public:
   tree_node *copy()		 { return copy_Expression(); }
   virtual Expression copy_Expression() = 0;
   virtual void scan(SymbolTable<Symbol, Symbol>*, 
                     SymbolTable<Symbol, method_class>*,
                     SymbolTable<Symbol, class__class>*) = 0;

   SymbolTable<Symbol, Symbol> objs;
   SymbolTable<Symbol, method_class> funs;
   SymbolTable<Symbol, class__class> claz;

#ifdef Expression_EXTRAS
   Expression_EXTRAS
#endif
};


// define simple phylum - Case
typedef class Case_class *Case;

class Case_class : public tree_node {
public:
   tree_node *copy()		 { return copy_Case(); }
   virtual Case copy_Case() = 0;
   virtual Symbol get_name() = 0;
   virtual void scan(SymbolTable<Symbol, Symbol>*, 
                     SymbolTable<Symbol, method_class>*,
                     SymbolTable<Symbol, class__class>*) = 0;

   SymbolTable<Symbol, Symbol> objs;
   SymbolTable<Symbol, method_class> funs;
   SymbolTable<Symbol, class__class> claz;

#ifdef Case_EXTRAS
   Case_EXTRAS
#endif
};


// define the class for phylum - LIST
// define list phlyum - Classes
typedef list_node<Class_> Classes_class;
typedef Classes_class *Classes;


// define list phlyum - Features
typedef list_node<Feature> Features_class;
typedef Features_class *Features;


// define list phlyum - Formals
typedef list_node<Formal> Formals_class;
typedef Formals_class *Formals;


// define list phlyum - Expressions
typedef list_node<Expression> Expressions_class;
typedef Expressions_class *Expressions;


// define list phlyum - Cases
typedef list_node<Case> Cases_class;
typedef Cases_class *Cases;


// define the class for constructors
// define constructor - program
class program_class : public Program_class {
protected:
   Classes classes;
   int errors;
public:
   program_class(Classes a1) {
      classes = a1;
      errors = 0;
   }
   Program copy_Program();
   void dump(ostream& stream, int n);
   int typecheck(ostream& stream, SymbolTable<Symbol, class__class>* ctable) {
       claz = *ctable;
       bool has_Main = false;
       bool has_main_meth = false;
       for(int idx = classes->first(); classes->more(idx); idx = classes->next(idx)) {
           Class__class *curclass = classes->nth(idx);

           //check 'Main' Class
           if (curclass->get_name() == idtable.lookup_string("Main")) {
             cur_class_filename = curclass->get_filename()->get_string();
             has_Main = true;
             if (curclass->get_method(idtable.lookup_string("main")))
               has_main_meth = true;
           }
       }

       if (!has_Main) {
          errors++;
          stream << "Class Main is not defined." << endl;
       } else if (!has_main_meth) {
          errors++;
          semant_error(stream, this, "No 'main' method in class Main.");
       }

       return errors;
  }

#ifdef Program_SHARED_EXTRAS
   Program_SHARED_EXTRAS
#endif
#ifdef program_EXTRAS
   program_EXTRAS
#endif
};


// define constructor - class_
class class__class : public Class__class {
protected:
   Symbol name;
   Symbol parent;
   Features features;
   Symbol filename;
public:
   class__class(Symbol a1, Symbol a2, Features a3, Symbol a4) {
      name = a1;
      parent = a2;
      features = a3;
      filename = a4;
   }
   Class_ copy_Class_();
   void dump(ostream& stream, int n);
   Symbol get_parent(void) const { return parent;};
   Symbol get_name(void) const { return name;};
   method_class* get_method(Symbol func_name) {return funs.lookup(func_name);}
   void scan(SymbolTable<Symbol, Symbol>* otable,
             SymbolTable<Symbol, method_class>* ftable, 
             SymbolTable<Symbol, class__class>* ctable) {
       //get all method information first
       for(int idx = features->first(); features->more(idx); idx = features->next(idx)) {
           Feature_class* curfea = (Feature_class*)features->nth(idx);
           if (curfea->is_method())
               ftable->addid(curfea->get_name(), (method_class *)curfea);
           else 
               otable->addid(curfea->get_name(), curfea->get_type_addr());
       }

       for(int idx = features->first(); features->more(idx); idx = features->next(idx))  {
           features->nth(idx)->scan(otable, ftable, ctable);
       }

       objs = *otable;
       funs = *ftable;
       claz = *ctable;
   }

#ifdef Class__SHARED_EXTRAS
   Class__SHARED_EXTRAS
#endif
#ifdef class__EXTRAS
   class__EXTRAS
#endif
};


// define constructor - method
class method_class : public Feature_class {
protected:
   Symbol name;
   Formals formals;
   Symbol return_type;
   Expression expr;
public:
   method_class(Symbol a1, Formals a2, Symbol a3, Expression a4) {
      name = a1;
      formals = a2;
      return_type = a3;
      expr = a4;
   }
   Feature copy_Feature();
   void dump(ostream& stream, int n);
   Symbol get_name() { return name; }
   Symbol* get_type_addr() { return NULL; }
   Boolean is_method() {return true;}
   void scan(SymbolTable<Symbol, Symbol>* otable,
            SymbolTable<Symbol, method_class>* ftable,
            SymbolTable<Symbol, class__class>* ctable) {
       otable->enterscope();
       for (int idx = formals->first(); formals->more(idx); 
           idx = formals->next(idx)) {
           formals->nth(idx)->scan(otable, ftable, ctable);
       }

       expr->scan(otable, ftable, ctable);
       objs = *otable;
       funs = *ftable;
       claz = *ctable;
       otable->exitscope();
  }

#ifdef Feature_SHARED_EXTRAS
   Feature_SHARED_EXTRAS
#endif
#ifdef method_EXTRAS
   method_EXTRAS
#endif
};


// define constructor - attr
class attr_class : public Feature_class {
protected:
   Symbol name;
   Symbol type_decl;
   Expression init;
public:
   attr_class(Symbol a1, Symbol a2, Expression a3) {
      name = a1;
      type_decl = a2;
      init = a3;
   }
   Feature copy_Feature();
   void dump(ostream& stream, int n);
   Boolean is_method() { return false; }
   Symbol* get_type_addr() { return &type_decl; }
   Symbol get_name() { return name; }
   void scan(SymbolTable<Symbol, Symbol>* otable,
               SymbolTable<Symbol, method_class>* ftable,
               SymbolTable<Symbol, class__class>* ctable) {
       if (init) {
           init->scan(otable, ftable, ctable);
       }

       objs = *otable;
       funs = *ftable; 
       claz = *ctable;
   }

#ifdef Feature_SHARED_EXTRAS
   Feature_SHARED_EXTRAS
#endif
#ifdef attr_EXTRAS
   attr_EXTRAS
#endif
};


// define constructor - formal
class formal_class : public Formal_class {
protected:
   Symbol name;
   Symbol type_decl;
public:
   formal_class(Symbol a1, Symbol a2) {
      name = a1;
      type_decl = a2;
   }
   Formal copy_Formal();
   void dump(ostream& stream, int n);
   Symbol get_name() { return name; }
   void scan(SymbolTable<Symbol, Symbol>* otable,
             SymbolTable<Symbol, method_class>* ftable,
             SymbolTable<Symbol, class__class>* ctable) {
       otable->addid(name, &type_decl);

       objs = *otable;
       funs = *ftable; 
       claz = *ctable;
   }

#ifdef Formal_SHARED_EXTRAS
   Formal_SHARED_EXTRAS
#endif
#ifdef formal_EXTRAS
   formal_EXTRAS
#endif
};


// define constructor - branch
class branch_class : public Case_class {
protected:
   Symbol name;
   Symbol type_decl;
   Expression expr;
public:
   branch_class(Symbol a1, Symbol a2, Expression a3) {
      name = a1;
      type_decl = a2;
      expr = a3;
   }
   Case copy_Case();
   void dump(ostream& stream, int n);
   Symbol get_name() { return name; }
   void scan(SymbolTable<Symbol, Symbol>* otable,
             SymbolTable<Symbol, method_class>* ftable,
             SymbolTable<Symbol, class__class>* ctable) {
      otable->enterscope();

      otable->addid(name, &type_decl);
      expr->scan(otable, ftable, ctable);

      objs = *otable;
      funs = *ftable;
      claz = *ctable;
      otable->exitscope();
   }

#ifdef Case_SHARED_EXTRAS
   Case_SHARED_EXTRAS
#endif
#ifdef branch_EXTRAS
   branch_EXTRAS
#endif
};


// define constructor - assign
class assign_class : public Expression_class {
protected:
   Symbol name;
   Expression expr;
public:
   assign_class(Symbol a1, Expression a2) {
      name = a1;
      expr = a2;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   Symbol get_name() { return name; }
   void scan(SymbolTable<Symbol, Symbol>* otable,
             SymbolTable<Symbol, method_class>* ftable,
             SymbolTable<Symbol, class__class>* ctable) {
       expr->scan(otable, ftable, ctable);
       objs = *otable;
       funs = *ftable;
       claz = *ctable;
   }

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef assign_EXTRAS
   assign_EXTRAS
#endif
};


// define constructor - static_dispatch
class static_dispatch_class : public Expression_class {
protected:
   Expression expr;
   Symbol type_name;
   Symbol name;
   Expressions actual;
public:
   static_dispatch_class(Expression a1, Symbol a2, Symbol a3, Expressions a4) {
      expr = a1;
      type_name = a2;
      name = a3;
      actual = a4;
   }
   Symbol get_name() { return name; }
   void scan(SymbolTable<Symbol, Symbol>* otable,
             SymbolTable<Symbol, method_class>* ftable,
             SymbolTable<Symbol, class__class>* ctable) {
       for(int i = actual->first(); actual->more(i); i = actual->next(i)) {
           actual->nth(i)->scan(otable, ftable, ctable);
       }

       expr->scan(otable, ftable, ctable);
       objs = *otable;
       funs = *ftable;
       claz = *ctable;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef static_dispatch_EXTRAS
   static_dispatch_EXTRAS
#endif
};


// define constructor - dispatch
class dispatch_class : public Expression_class {
protected:
   Expression expr;
   Symbol name;
   Expressions actual;
public:
   dispatch_class(Expression a1, Symbol a2, Expressions a3) {
      expr = a1;
      name = a2;
      actual = a3;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   Symbol get_name() { return name; }
   void scan(SymbolTable<Symbol, Symbol>* otable,
             SymbolTable<Symbol, method_class>* ftable,
             SymbolTable<Symbol, class__class>* ctable) {
       for(int i = actual->first(); actual->more(i); i = actual->next(i)) {
           actual->nth(i)->scan(otable, ftable, ctable);
       }

       expr->scan(otable, ftable, ctable);
       objs = *otable;
       funs = *ftable;
       claz = *ctable;
   }

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef dispatch_EXTRAS
   dispatch_EXTRAS
#endif
};


// define constructor - cond
class cond_class : public Expression_class {
protected:
   Expression pred;
   Expression then_exp;
   Expression else_exp;
public:
   cond_class(Expression a1, Expression a2, Expression a3) {
      pred = a1;
      then_exp = a2;
      else_exp = a3;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   void scan(SymbolTable<Symbol, Symbol>* otable,
       SymbolTable<Symbol, method_class>* ftable,
       SymbolTable<Symbol, class__class>* ctable ) {
       return;
   }

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef cond_EXTRAS
   cond_EXTRAS
#endif
};


// define constructor - loop
class loop_class : public Expression_class {
protected:
   Expression pred;
   Expression body;
public:
   loop_class(Expression a1, Expression a2) {
      pred = a1;
      body = a2;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   void scan(SymbolTable<Symbol, Symbol>* otable,
             SymbolTable<Symbol, method_class>* ftable,
             SymbolTable<Symbol, class__class>* ctable) {
       pred->scan(otable, ftable, ctable);
       body->scan(otable, ftable, ctable);

       objs = *otable;
       funs = *ftable;
       claz = *ctable;
   }

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef loop_EXTRAS
   loop_EXTRAS
#endif
};


// define constructor - typcase
class typcase_class : public Expression_class {
protected:
   Expression expr;
   Cases cases;
public:
   typcase_class(Expression a1, Cases a2) {
      expr = a1;
      cases = a2;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   void scan(SymbolTable<Symbol, Symbol>* otable,
             SymbolTable<Symbol, method_class>* ftable,
             SymbolTable<Symbol, class__class>* ctable) {
       expr->scan(otable, ftable, ctable);
       for(int i = cases->first(); cases->more(i); i = cases->next(i)) {
           cases->nth(i)->scan(otable, ftable, ctable);
       }
       objs = *otable;
       funs = *ftable; 
       claz = *ctable;
   }

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef typcase_EXTRAS
   typcase_EXTRAS
#endif
};


// define constructor - block
class block_class : public Expression_class {
protected:
   Expressions body;
public:
   block_class(Expressions a1) {
      body = a1;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   void scan(SymbolTable<Symbol, Symbol>* otable,
             SymbolTable<Symbol, method_class>* ftable,
             SymbolTable<Symbol, class__class>* ctable) {
       for(int i = body->first(); body->more(i); i = body->next(i)) {
           body->nth(i)->scan(otable, ftable, ctable);
       }

       objs = *otable;
       funs = *ftable; 
       claz = *ctable;
   }

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef block_EXTRAS
   block_EXTRAS
#endif
};


// define constructor - let
class let_class : public Expression_class {
protected:
   Symbol identifier;
   Symbol type_decl;
   Expression init;
   Expression body;
public:
   let_class(Symbol a1, Symbol a2, Expression a3, Expression a4) {
      identifier = a1;
      type_decl = a2;
      init = a3;
      body = a4;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   void scan(SymbolTable<Symbol, Symbol>* otable,
             SymbolTable<Symbol, method_class>* ftable, 
             SymbolTable<Symbol, class__class>* ctable) {
       otable->enterscope();

       otable->addid(identifier, &type_decl);
       init->scan(otable, ftable, ctable);
       body->scan(otable, ftable, ctable);

       objs = *otable;
       funs = *ftable; 
       claz = *ctable;
       otable->exitscope();
   }

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef let_EXTRAS
   let_EXTRAS
#endif
};


// define constructor - plus
class plus_class : public Expression_class {
protected:
   Expression e1;
   Expression e2;
public:
   plus_class(Expression a1, Expression a2) {
      e1 = a1;
      e2 = a2;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   void scan(SymbolTable<Symbol, Symbol>* otable,
             SymbolTable<Symbol, method_class>* ftable,
             SymbolTable<Symbol, class__class>* ctable) {
       e1->scan(otable, ftable, ctable);
       e2->scan(otable, ftable, ctable);

       objs = *otable;
       funs = *ftable;
       claz = *ctable;
   }

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef plus_EXTRAS
   plus_EXTRAS
#endif
};


// define constructor - sub
class sub_class : public Expression_class {
protected:
   Expression e1;
   Expression e2;
public:
   sub_class(Expression a1, Expression a2) {
      e1 = a1;
      e2 = a2;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   void scan(SymbolTable<Symbol, Symbol>* otable,
             SymbolTable<Symbol, method_class>* ftable,
             SymbolTable<Symbol, class__class>* ctable) {
       e1->scan(otable, ftable, ctable);
       e2->scan(otable, ftable, ctable);

       objs = *otable;
       funs = *ftable;
       claz = *ctable;
   }

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef sub_EXTRAS
   sub_EXTRAS
#endif
};


// define constructor - mul
class mul_class : public Expression_class {
protected:
   Expression e1;
   Expression e2;
public:
   mul_class(Expression a1, Expression a2) {
      e1 = a1;
      e2 = a2;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   void scan(SymbolTable<Symbol, Symbol>* otable,
             SymbolTable<Symbol, method_class>* ftable,
             SymbolTable<Symbol, class__class>* ctable) {
       e1->scan(otable, ftable, ctable);
       e2->scan(otable, ftable, ctable);

       objs = *otable;
       funs = *ftable;
       claz = *ctable;
   }

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef mul_EXTRAS
   mul_EXTRAS
#endif
};


// define constructor - divide
class divide_class : public Expression_class {
protected:
   Expression e1;
   Expression e2;
public:
   divide_class(Expression a1, Expression a2) {
      e1 = a1;
      e2 = a2;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   void scan(SymbolTable<Symbol, Symbol>* otable,
             SymbolTable<Symbol, method_class>* ftable,
             SymbolTable<Symbol, class__class>* ctable) {
       e1->scan(otable, ftable, ctable);
       e2->scan(otable, ftable, ctable);

       objs = *otable;
       funs = *ftable;
       claz = *ctable;
   }

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef divide_EXTRAS
   divide_EXTRAS
#endif
};


// define constructor - neg
class neg_class : public Expression_class {
protected:
   Expression e1;
public:
   neg_class(Expression a1) {
      e1 = a1;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   void scan(SymbolTable<Symbol, Symbol>* otable,
             SymbolTable<Symbol, method_class>* ftable,
             SymbolTable<Symbol, class__class>* ctable) {
       e1->scan(otable, ftable, ctable);

       objs = *otable;
       funs = *ftable;
       claz = *ctable;
   }

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef neg_EXTRAS
   neg_EXTRAS
#endif
};


// define constructor - lt
class lt_class : public Expression_class {
protected:
   Expression e1;
   Expression e2;
public:
   lt_class(Expression a1, Expression a2) {
      e1 = a1;
      e2 = a2;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   void scan(SymbolTable<Symbol, Symbol>* otable,
             SymbolTable<Symbol, method_class>* ftable,
             SymbolTable<Symbol, class__class>* ctable) {
       e1->scan(otable, ftable, ctable);
       e2->scan(otable, ftable, ctable);

       objs = *otable;
       funs = *ftable;
       claz = *ctable;
   }

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef lt_EXTRAS
   lt_EXTRAS
#endif
};


// define constructor - eq
class eq_class : public Expression_class {
protected:
   Expression e1;
   Expression e2;
public:
   eq_class(Expression a1, Expression a2) {
      e1 = a1;
      e2 = a2;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   void scan(SymbolTable<Symbol, Symbol>* otable,
             SymbolTable<Symbol, method_class>* ftable,
             SymbolTable<Symbol, class__class>* ctable) {
       e1->scan(otable, ftable, ctable);
       e2->scan(otable, ftable, ctable);

       objs = *otable;
       funs = *ftable;
       claz = *ctable;
   }

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef eq_EXTRAS
   eq_EXTRAS
#endif
};


// define constructor - leq
class leq_class : public Expression_class {
protected:
   Expression e1;
   Expression e2;
public:
   leq_class(Expression a1, Expression a2) {
      e1 = a1;
      e2 = a2;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   void scan(SymbolTable<Symbol, Symbol>* otable,
             SymbolTable<Symbol, method_class>* ftable,
             SymbolTable<Symbol, class__class>* ctable) {
       e1->scan(otable, ftable, ctable);
       e2->scan(otable, ftable, ctable);

       objs = *otable;
       funs = *ftable;
       claz = *ctable;
   }

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef leq_EXTRAS
   leq_EXTRAS
#endif
};


// define constructor - comp
class comp_class : public Expression_class {
protected:
   Expression e1;
public:
   comp_class(Expression a1) {
      e1 = a1;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   void scan(SymbolTable<Symbol, Symbol>* otable,
             SymbolTable<Symbol, method_class>* ftable,
             SymbolTable<Symbol, class__class>* ctable) {
       e1->scan(otable, ftable, ctable);

       objs = *otable;
       funs = *ftable;
       claz = *ctable;
   }

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef comp_EXTRAS
   comp_EXTRAS
#endif
};


// define constructor - int_const
class int_const_class : public Expression_class {
protected:
   Symbol token;
public:
   int_const_class(Symbol a1) {
      token = a1;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   void scan(SymbolTable<Symbol, Symbol>* otable,
             SymbolTable<Symbol, method_class>* ftable,
             SymbolTable<Symbol, class__class>* ctable) {
       objs = *otable;
       funs = *ftable;
       claz = *ctable;
   }

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef int_const_EXTRAS
   int_const_EXTRAS
#endif
};


// define constructor - bool_const
class bool_const_class : public Expression_class {
protected:
   Boolean val;
public:
   bool_const_class(Boolean a1) {
      val = a1;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   void scan(SymbolTable<Symbol, Symbol>* otable,
             SymbolTable<Symbol, method_class>* ftable,
             SymbolTable<Symbol, class__class>* ctable) {
       objs = *otable;
       funs = *ftable;
       claz = *ctable;
   }

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef bool_const_EXTRAS
   bool_const_EXTRAS
#endif
};


// define constructor - string_const
class string_const_class : public Expression_class {
protected:
   Symbol token;
public:
   string_const_class(Symbol a1) {
      token = a1;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   void scan(SymbolTable<Symbol, Symbol>* otable,
             SymbolTable<Symbol, method_class>* ftable,
             SymbolTable<Symbol, class__class>* ctable) {
       objs = *otable;
       funs = *ftable;
       claz = *ctable;
   }

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef string_const_EXTRAS
   string_const_EXTRAS
#endif
};


// define constructor - new_
class new__class : public Expression_class {
protected:
   Symbol type_name;
public:
   new__class(Symbol a1) {
      type_name = a1;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   void scan(SymbolTable<Symbol, Symbol>* otable,
             SymbolTable<Symbol, method_class>* ftable,
             SymbolTable<Symbol, class__class>* ctable) {
       objs = *otable;
       funs = *ftable;
       claz = *ctable;
   }

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef new__EXTRAS
   new__EXTRAS
#endif
};


// define constructor - isvoid
class isvoid_class : public Expression_class {
protected:
   Expression e1;
public:
   isvoid_class(Expression a1) {
      e1 = a1;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   void scan(SymbolTable<Symbol, Symbol>* otable,
             SymbolTable<Symbol, method_class>* ftable,
             SymbolTable<Symbol, class__class>* ctable) {
       e1->scan(otable, ftable, ctable);

       objs = *otable;
       funs = *ftable;
       claz = *ctable;
   }

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef isvoid_EXTRAS
   isvoid_EXTRAS
#endif
};


// define constructor - no_expr
class no_expr_class : public Expression_class {
protected:
public:
   no_expr_class() {
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   void scan(SymbolTable<Symbol, Symbol>* otable,
             SymbolTable<Symbol, method_class>* ftable,
             SymbolTable<Symbol, class__class>* ctable) {
       objs = *otable;
       funs = *ftable;
       claz = *ctable;
   }

#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef no_expr_EXTRAS
   no_expr_EXTRAS
#endif
};


// define constructor - object
class object_class : public Expression_class {
protected:
   Symbol name;
public:
   object_class(Symbol a1) {
      name = a1;
   }
   Expression copy_Expression();
   void dump(ostream& stream, int n);
   Symbol get_name() { return name; }
   void scan(SymbolTable<Symbol, Symbol>* otable,
             SymbolTable<Symbol, method_class>* ftable,
             SymbolTable<Symbol, class__class>* ctable) {
       objs = *otable;
       funs = *ftable;
       claz = *ctable;
   }


#ifdef Expression_SHARED_EXTRAS
   Expression_SHARED_EXTRAS
#endif
#ifdef object_EXTRAS
   object_EXTRAS
#endif
};


// define the prototypes of the interface
Classes nil_Classes();
Classes single_Classes(Class_);
Classes append_Classes(Classes, Classes);
Features nil_Features();
Features single_Features(Feature);
Features append_Features(Features, Features);
Formals nil_Formals();
Formals single_Formals(Formal);
Formals append_Formals(Formals, Formals);
Expressions nil_Expressions();
Expressions single_Expressions(Expression);
Expressions append_Expressions(Expressions, Expressions);
Cases nil_Cases();
Cases single_Cases(Case);
Cases append_Cases(Cases, Cases);
Program program(Classes);
Class_ class_(Symbol, Symbol, Features, Symbol);
Feature method(Symbol, Formals, Symbol, Expression);
Feature attr(Symbol, Symbol, Expression);
Formal formal(Symbol, Symbol);
Case branch(Symbol, Symbol, Expression);
Expression assign(Symbol, Expression);
Expression static_dispatch(Expression, Symbol, Symbol, Expressions);
Expression dispatch(Expression, Symbol, Expressions);
Expression cond(Expression, Expression, Expression);
Expression loop(Expression, Expression);
Expression typcase(Expression, Cases);
Expression block(Expressions);
Expression let(Symbol, Symbol, Expression, Expression);
Expression plus(Expression, Expression);
Expression sub(Expression, Expression);
Expression mul(Expression, Expression);
Expression divide(Expression, Expression);
Expression neg(Expression);
Expression lt(Expression, Expression);
Expression eq(Expression, Expression);
Expression leq(Expression, Expression);
Expression comp(Expression);
Expression int_const(Symbol);
Expression bool_const(Boolean);
Expression string_const(Symbol);
Expression new_(Symbol);
Expression isvoid(Expression);
Expression no_expr();
Expression object(Symbol);


#endif
