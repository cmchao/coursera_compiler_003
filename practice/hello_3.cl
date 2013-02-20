class Main {

    io : IO <- new IO;

    main() : Object {

        {
        (* no  1; anymore 

           out_string return IO, and Object is root class type
           
           *)
        io.out_string("Hello World\n");

        }
    };
};
