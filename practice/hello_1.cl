(* This is comment *)

(* Main is speciial class as program entry point *)
class Main {

    (* declare class member and initialize it as object create by new
       the format is 
       variable_name : varible_type <- initialization value
    *)
    io : IO <- new IO;

    (* like java's publice void main or c's main.
       main can't take any argument
       function type format is

       function_name(var1 : var1_type, var2 : var2_type) : return_val_type

       function require brace {} to include contain
    *)
    main() : Int {

    (* need another {} don't know why *)
        {
        io.out_string("Hello World\n");

    (* return type *)
        1;
        }
    };
};
