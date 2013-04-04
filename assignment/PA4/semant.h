#ifndef SEMANT_H_
#define SEMANT_H_

#include <assert.h>
#include <iostream>  
#include <map>
#include "cool-tree.h"
#include "stringtab.h"
#include "symtab.h"
#include "list.h"

#define TRUE 1
#define FALSE 0

class ClassTable;
typedef ClassTable *ClassTableP;

// This is a structure that may be used to contain the semantic
// information such as the inheritance graph.  You may use it or not as
// you like: it is only here to provide a container for the supplied
// methods.

class ClassTable {
private:
  int semant_errors;
  void install_basic_classes();
  ostream& error_stream;

  //extra method
  void check_inheritance(void);

  //extra information
  std::multimap<Symbol, Symbol>     class_map; //keep parent information for each class
  SymbolTable<Symbol, class__class> *class_table;  //class scope
  SymbolTable<Symbol, method_class> *method_table; //method scope
  SymbolTable<Symbol, Symbol>       *object_table; //object scope

public:
  ClassTable(Classes);
  int errors() { return semant_errors; }
  ostream& semant_error();
  ostream& semant_error(Class_ c);
  ostream& semant_error(Class_ c, const char *msg);
  ostream& semant_error(Symbol filename, tree_node *t);
};


#endif

