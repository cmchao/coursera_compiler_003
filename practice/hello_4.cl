(* inherits other class
class Main inherits IO{

    io : IO <- new IO;

    main() : Object {

        {
        (* out_string become member function *)
        out_string("Hello World, call by member function\n");

        (* self like C++ this pointer *)
        self.out_string("Hello World, call by this pointer\n");
        }
    };
};
