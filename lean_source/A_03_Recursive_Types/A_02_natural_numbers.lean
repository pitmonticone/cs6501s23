
/- TEXT: 
***************
Natural Numbers
***************

Data Type
---------
TEXT. -/

-- QUOTE:
#check nat
-- QUOTE.

/- TEXT
The definition of the type, nat, the terms/values of which we
use to represent natural numbers.

inductive nat
| zero : nat
| succ (n : nat) : nat

The set of terms is defined inductively. First, *zero* is a term
of type *nat*. Second, if *n'* is any term of type *nat*, then so
is *nat.succ n'* (which you can also write as n'.succ or n' + 1).
TEXT. -/

-- QUOTE: 
-- notations for writing succ applications
def three'  := (nat.succ (nat.succ (nat.succ (nat.zero))))
def three  := nat.zero.succ.succ.succ
-- QUOTE.


/- TEXT:
Operations
----------

Having seen how the *nat* data type is defined, we now look 
at how to define functions taking *nat* values as arguments.
As we've seen before, many such functions here will again be
defined by case analysis on an incoming nat argument value. 
That means considering two cases separately: the incoming value
is either zero or non-zero: that is, either *nat.zero,* or 
*nat.succ n'* for some "one-smaller" value, *n'*. For example,
if the incoming argument is *succ(succ(succ zero))*, i.e., 3, 
then (a) it does not match *nat.zero*, but (b) it does match 
*nat.succ n'*, with *n'* is bound to *succ(succ zero)*, i.e., 2.
TEXT. -/

-- QUOTE:
-- increment is just succ application 
def inc (n' : nat) : nat := n'.succ
def three'' := inc(inc(inc nat.zero))
-- QUOTE.

/- TEXT:
A predecessor (one less than) function can be defined by 
case analysis on a nat argument. Here we'll define *pred'*
to return 0 when applied to 0, and otherwise to return the
one smaller value, *n'*, when applied to any non-zero value,
*nat.succ n'*. 

Rather than "implementing a function" think "proving a function 
type." A "proof" of function type, *nat → nat,* is any function 
that converts any given nat into some resulting nat. 

When proving a proposition (a type in Prop), any proof (value
of that type) will do. When proving function or other data type, 
however, the particular value of the type that you construct is
usually important. Here, for example, we don't want any function
that takes and returns a nat, but one that returns the right nat
for the given argument.
TEXT. -/

-- QUOTE:
def pred' : nat → nat :=
begin
assume n,
cases n with n',  
exact 0,  -- when n is zero
exact n', -- when n is succ n'
end 

-- quick test
#eval pred' 6
example : pred' 6 = 5 := rfl
-- QUOTE.

/- TEXT:
Here's the same function just specified
using pattern matching notation (which,
as we've seen generalizes case analysis). 
TEXT. -/

-- QUOTE: 
def pred : nat → nat 
| nat.zero := nat.zero  -- loop at zero
| (nat.succ n') := n' 

#eval pred 5
example : pred 5 = 4 := rfl
-- QUOTE.

/- TEXT:
Pattern matching generalizes case analysis
by giving you a means to return different
results based on deeper analysis of argument
structures using pattern matching/unification.
This example implements subtract-two, looping
at zero. Notice how the third pattern matches
to the sub-natural-number object nested two
succ-levels deep.
TEXT. -/

-- QUOTE:
def sub2 : nat → nat 
| nat.zero := nat.zero
| (nat.succ nat.zero) := nat.zero
| (nat.succ (nat.succ n')) := n'
-- QUOTE.