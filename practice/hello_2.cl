class Main {

    io : IO <- new IO;

    (* return IO object directly *)
    main() : IO {

        {
        io.out_string("Hello World\n");


    (* create object directly *)
        (new IO).out_string("new object\n");

    (* no  1; anymore *)
        }
    };
};
