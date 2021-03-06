

#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "semant.h"
#include "utilities.h"


extern int semant_debug;
extern char *curr_filename;

//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
static Symbol 
    arg,
    arg2,
    Bool,
    concat,
    cool_abort,
    copy,
    Int,
    in_int,
    in_string,
    IO,
    length,
    Main,
    main_meth,
    No_class,
    No_type,
    Object,
    out_int,
    out_string,
    prim_slot,
    self,
    SELF_TYPE,
    Str,
    str_field,
    substr,
    type_name,
    val;
//
// Initializing the predefined symbols.
//
static void initialize_constants(void)
{
    arg         = idtable.add_string("arg");
    arg2        = idtable.add_string("arg2");
    Bool        = idtable.add_string("Bool");
    concat      = idtable.add_string("concat");
    cool_abort  = idtable.add_string("abort");
    copy        = idtable.add_string("copy");
    Int         = idtable.add_string("Int");
    in_int      = idtable.add_string("in_int");
    in_string   = idtable.add_string("in_string");
    IO          = idtable.add_string("IO");
    length      = idtable.add_string("length");
    Main        = idtable.add_string("Main");
    main_meth   = idtable.add_string("main");
    //   _no_class is a symbol that can't be the name of any 
    //   user-defined class.
    No_class    = idtable.add_string("_no_class");
    No_type     = idtable.add_string("_no_type");
    Object      = idtable.add_string("Object");
    out_int     = idtable.add_string("out_int");
    out_string  = idtable.add_string("out_string");
    prim_slot   = idtable.add_string("_prim_slot");
    self        = idtable.add_string("self");
    SELF_TYPE   = idtable.add_string("SELF_TYPE");
    Str         = idtable.add_string("String");
    str_field   = idtable.add_string("_str_field");
    substr      = idtable.add_string("substr");
    type_name   = idtable.add_string("type_name");
    val         = idtable.add_string("_val");
}



ClassTable::ClassTable(Classes classes) : semant_errors(0) , error_stream(cerr) {

    /* Fill this in */
    if (semant_debug) {
        cerr << "# Classes: " << classes->len() << endl;
    }

    class_table = new SymbolTable<Symbol, class__class>();
    class_table->enterscope();  //required since there is no scope when initialization

    for (int idx = classes->first(); classes->more(idx); idx = classes->next(idx)) {
        class__class *curclass = static_cast<class__class *>(classes->nth(idx));
        if (semant_debug) {
            curclass->dump(cerr, 0);
        }

        class_map.insert(std::make_pair(curclass->get_parent(), curclass->get_name()));

        if (class_table->probe(curclass->get_name()) != NULL) {
            semant_error(curclass, MSG_DUPLICATE);
        }

        if (curclass->get_name() == No_class
            || curclass->get_name() == Object
            || curclass->get_name() == IO
            || curclass->get_name() == Int
            || curclass->get_name() == Str) {
            semant_error(curclass, MSG_BASE_REDEFINE);
        }

        class_table->addid(curclass->get_name(), curclass);
    }

    install_basic_classes();

    //check parent is defined or not
    for (int idx = classes->first(); classes->more(idx); idx = classes->next(idx)) {
        class__class *curclass = static_cast<class__class *>(classes->nth(idx));

        if (class_table->probe(curclass->get_parent()) == NULL) {
            semant_error(curclass, MSG_UNDEF_INHERIT);
        }

        if (check_cyclic(curclass->get_name(), curclass->get_name())) {
            semant_error(curclass, MSG_CYCLE);
        }
    }
}

bool ClassTable::check_cyclic(Symbol target, Symbol child)  {
    std::pair<std::multimap<Symbol, Symbol>::iterator,
              std::multimap<Symbol, Symbol>::iterator> nodes = class_map.equal_range(child);

    std::multimap<Symbol, Symbol>::iterator iter;

    if (nodes.first == class_map.end()) {
        return false;
    }

    bool status = false;
    bool cur;
    for (iter = nodes.first;  iter != nodes.second; ++iter) {
        if (iter->second == target)
            cur = true;
        else
            cur = check_cyclic(target, iter->second);

        status = status || cur;
        if (status)
            return true;
    }

    return false;
}

void ClassTable::install_basic_classes() {

    // The tree package uses these globals to annotate the classes built below.
   // curr_lineno  = 0;
    Symbol filename = stringtable.add_string("<basic class>");
    
    // The following demonstrates how to create dummy parse trees to
    // refer to basic Cool classes.  There's no need for method
    // bodies -- these are already built into the runtime system.
    
    // IMPORTANT: The results of the following expressions are
    // stored in local variables.  You will want to do something
    // with those variables at the end of this method to make this
    // code meaningful.

    // 
    // The Object class has no parent class. Its methods are
    //        abort() : Object    aborts the program
    //        type_name() : Str   returns a string representation of class name
    //        copy() : SELF_TYPE  returns a copy of the object
    //
    // There is no need for method bodies in the basic classes---these
    // are already built in to the runtime system.

    Class_ Object_class =
	class_(Object, 
	       No_class,
	       append_Features(
			       append_Features(
					       single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
					       single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
			       single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
	       filename);


    class_map.insert(std::make_pair(No_class, Object));
    class_table->addid(Object, static_cast<class__class *>(Object_class));
    // 
    // The IO class inherits from Object. Its methods are
    //        out_string(Str) : SELF_TYPE       writes a string to the output
    //        out_int(Int) : SELF_TYPE            "    an int    "  "     "
    //        in_string() : Str                 reads a string from the input
    //        in_int() : Int                      "   an int     "  "     "
    //
    Class_ IO_class = 
	class_(IO, 
	       Object,
	       append_Features(
			       append_Features(
					       append_Features(
							       single_Features(method(out_string, single_Formals(formal(arg, Str)),
										      SELF_TYPE, no_expr())),
							       single_Features(method(out_int, single_Formals(formal(arg, Int)),
										      SELF_TYPE, no_expr()))),
					       single_Features(method(in_string, nil_Formals(), Str, no_expr()))),
			       single_Features(method(in_int, nil_Formals(), Int, no_expr()))),
	       filename);  

    class_map.insert(std::make_pair(Object, IO));
    class_table->addid(IO, static_cast<class__class *>(IO_class));
    //
    // The Int class has no methods and only a single attribute, the
    // "val" for the integer. 
    //
    Class_ Int_class =
	class_(Int, 
	       Object,
	       single_Features(attr(val, prim_slot, no_expr())),
	       filename);

    class_map.insert(std::make_pair(Object, Int));
    class_table->addid(Int, static_cast<class__class *>(Int_class));
    //
    // Bool also has only the "val" slot.
    //
    Class_ Bool_class =
	class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())),filename);

    class_map.insert(std::make_pair(Object, Bool));
    class_table->addid(Bool, static_cast<class__class *>(Bool_class));

    //
    // The class Str has a number of slots and operations:
    //       val                                  the length of the string
    //       str_field                            the string itself
    //       length() : Int                       returns length of the string
    //       concat(arg: Str) : Str               performs string concatenation
    //       substr(arg: Int, arg2: Int): Str     substring selection
    //       
    Class_ Str_class =
	class_(Str, 
	       Object,
	       append_Features(
			       append_Features(
					       append_Features(
							       append_Features(
									       single_Features(attr(val, Int, no_expr())),
									       single_Features(attr(str_field, prim_slot, no_expr()))),
							       single_Features(method(length, nil_Formals(), Int, no_expr()))),
					       single_Features(method(concat, 
								      single_Formals(formal(arg, Str)),
								      Str, 
								      no_expr()))),
			       single_Features(method(substr, 
						      append_Formals(single_Formals(formal(arg, Int)), 
								     single_Formals(formal(arg2, Int))),
						      Str, 
						      no_expr()))),
	       filename);
    class_map.insert(std::make_pair(Object, Str));
    class_table->addid(Str, static_cast<class__class *>(Str_class));
}

////////////////////////////////////////////////////////////////////
//
// semant_error is an overloaded function for reporting errors
// during semantic analysis.  There are three versions:
//
//    ostream& ClassTable::semant_error()                
//
//    ostream& ClassTable::semant_error(Class_ c)
//       print line number and filename for `c'
//
//    ostream& ClassTable::semant_error(Symbol filename, tree_node *t)  
//       print a line number and filename
//
///////////////////////////////////////////////////////////////////

ostream& ClassTable::semant_error(Class_ c)
{                                                             
    return semant_error(c->get_filename(),c);
}    

ostream& ClassTable::semant_error(Symbol filename, tree_node *t)
{
    error_stream << filename << ":" << t->get_line_number() << ": ";
    return semant_error();
}

ostream& ClassTable::semant_error(Class_ c, const char *msg)
{
    error_stream << c->get_filename() << ":" << c->get_name() << ", " << msg << endl;
    semant_errors++;
    return error_stream;
}

ostream& ClassTable::semant_error(Class_ c, MSG_TYPE msg_type)
{
    error_stream << c->get_filename() << ":" << c->get_line_number() << ":";
    switch(msg_type) {
    case MSG_DUPLICATE:
        error_stream << " Class " << c->get_name() << " was previously defined." << endl;
        break;
    case MSG_BASE_REDEFINE:
        error_stream << " Redefinition of basic class " << c->get_name() << "." << endl;
        break;
    case MSG_UNDEF_INHERIT:
        error_stream << " Class " << c->get_name() 
                     << " inherits from an undefined class " << c->get_parent() << "." << endl;
        break;
    case MSG_CYCLE:
        error_stream << " cycle " << c->get_name() << endl;
        break;
    }

    semant_errors++;
    return error_stream;
}

ostream& ClassTable::semant_error()                  
{                                                 
    semant_errors++;                            
    return error_stream;
} 

void ClassTable::traverse_ast(Symbol symbol) {
    method_table->enterscope();
    object_table->enterscope();
    class__class *curclass = class_table->lookup(symbol);

    curclass->scan(object_table, method_table, class_table);

    std::pair<std::multimap<Symbol, Symbol>::iterator,
              std::multimap<Symbol, Symbol>::iterator>
        range = class_map.equal_range(symbol);

    for (std::multimap<Symbol, Symbol>::iterator it = range.first;
         it != range.second; ++it) {
        traverse_ast((*it).second);
    }

    if (semant_debug) {
        cerr << "For class " << curclass->get_name()->get_string();
        object_table->dump();
        method_table->dump();
    }

    object_table->exitscope();
    method_table->exitscope();
}

//collect id for methods and objects
void ClassTable::collect_all_id() {
    method_table = new SymbolTable<Symbol, method_class>();
    object_table = new SymbolTable<Symbol, Symbol>();
    traverse_ast(Object);
}

/*   This is the entry point to the semantic checker.

     Your checker should do the following two things:

     1) Check that the program is semantically correct
     2) Decorate the abstract syntax tree with type information
        by setting the `type' field in each Expression node.
        (see `tree.h')

     You are free to first do 1), make sure you catch all semantic
     errors. Part 2) can be done in a second stage, when you want
     to build mycoolc.
 */
void program_class::semant()
{
    initialize_constants();

    /* ClassTable constructor may do some semantic analysis */
    ClassTable *classtable = new ClassTable(classes);
    if (classtable->errors()) {
        cerr << "Compilation halted due to static semantic errors." << endl;
        exit(1);
    }

    /* some semantic analysis code may go here */
    classtable->collect_all_id();

    if (this->typecheck(cerr, classtable->get_ctable()) > 0) {
        cerr << "Compilation halted due to static semantic errors." << endl;
        exit(1);
    }
}


