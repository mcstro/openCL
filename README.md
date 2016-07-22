OPENCL HELLO WORLD

ONLINE vs. OFFLINE
There are two folders, online and offline. This refers to the two ways that kernel functions are defined.
In the online case (CUDA users may know this as JIT or "just-in-time"), the code is *read into a string and 
turned into machine-level instructions for a device during execution of the code*. Dear coder, you may be
jaded and experienced to the point where this is a totally normal thing to do, but I still can't get over this.
It seems like such a bizarre and completely terrible idea to me, and makes debugging extremely difficult (go
ahead, throw some gibberish nonsense into your kernel function and see if the compiler ever catches it). There
are tools available of course, but you better believe they all have overhead.

I get the impression that I am alone in this thought, because offline compilation seems to only exist so that
companies can conceal proprietary code. I think it's a much more natural thing to do, even though it creates more
files.

There's more info in the READMEs in each folder. Let me know if you have any luck compiling.

DISCLAIMER
I didn't write most of this code, but I did have to scour the internet to find all the pieces, so I am hoping it
benefits someone else to have all of it in one place. I tried to link to the sources where I found the things.

FINALLY
If you think programming GPUs is easy, then I hate you. You've probably also read James Joyce's Ulysses from cover
to cover. Go cure cancer or something.
