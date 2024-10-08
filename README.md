# PSMPL
## the Primordial Simple Machine abstraction Programming Language
### Lilly H. St Claire

PSMPL is the programming language designed in the wake of SMPL being too complex to feasibly
implement in the authors time frame. It exists to be simple both to implement, and to program
in. It requires the following features:

- macro definition
- register manipulation
- conditional if statements
- subroutines
- while loops
- stack manipulation

We will begin with the most basic of features. Subroutines, program entry, and program halt.

```
subrt _start
    halt
end
```

`subrt` is a keyword which begins a subroutine with the name of the identifier which follows
it. Halt is a keyword which enters an infinite loop with no action taken within it. It is
equivalent to assembly: `jmp $`. `end` is a keyword which tells the compiler that whatever
group it is currently in (in this case, a subroutine) has ended.

This compiles to the following hex code:

```
0xEB 0xFE 0xCD
```

which translates to the following 8086 assembly:

```
_start:
    jmp $
    ret
```

The immediate problem here is that this is not executable on an 8086 bios machine. To fix
this we will add the `header` directive. It changes our program to be like so:

```
header
    subrt _start
        halt
    end
end
```

Which will compile to the previous code followed by the number of zeros to pad up to 510 bytes
following it is the word `0xAA55`. The actual contents and output of a header in PSMPL is
implementation defined.
