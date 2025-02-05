= "kc", C99 interpreter/compiler front-end example (WIP)

This is an example that shows how to implement a C compiler (C99 standard
to be specific) using Carburetta, through all of its 7 translation phases.

It currently does not have a backend (although the "Tilly example" elsewhere
might be useful for this); and instead can execute the compiled code through
interpretation.

Because of function pointers being what they are, there is some JIT 
trampoline code in invoke_x64.c/h and invoke_call_x64.asm that ties this,
otherwise portable, code to the Windows x64 ABI and MASM. Consequently, the
example will not work on non-Windows platforms. (The nix_invoke_XXX is not
where it needs to be.)

Finally, there is a C preprocessor here that the preprocessor from Aex-GL was
based on (see https://github.com/kingletbv/aex-gl src/pp in particular.) The
one in Aex-GL is seperated out into its own module, it would be nice to re-
integrate it, or at least backport the fixes there into here. Note however
that OpenGL ES 2.0 has extensions to a standard preprocessor, so it is not
trivial to blank these out conditionally (or desirable to have those 
extensions here as dormant code.)
