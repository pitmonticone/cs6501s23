
****************
Recursive Proofs
****************

This chapter you will teach you about proof by induction.

Proof by induction is a method for constructing proofs of
universal generalizations, of the form, *∀ (a : α), P a,*
where α is an arbitrary type and *P* is a predicate on (and
thus represents a property of) objects of type α.

The key idea is that such proofs are in general constructed
recursively, (1) with proofs of *P a* for *larger* values of 
*a* being constructible in some cases from proofs of *P a'* 
for  smaller values of *a*, and (2) starting from proofs of
*P a* for *smallest* values of *a*. 

The rest of this chapter will: 

- provide a concrete and pecific example of this reasoning and how we can automate it using tools we already have, concluding what is called the induction axiom for natural numbers (arguments to *P*); 
- see how the concept of an induction axiom generalizes to any inductively defined type, α; 
- introduce the concept of *inductive families* with recursive constructors; 
- introduce the idea of well founded recursion, meaning that a proof for every value of a type can be constructed starting with smallest values of the type. 
- recognize that some types have no smallest values, making proof by induction inapplicable in these cases


The Idea by Example
-------------------

In the last chapter we defined a *safe* version of *fold* 
by requiring that a proof be given as an argument: that the 
value returned for an empty list be a right identity for the 
binary operator argument. In many similar situations we will
require that a value be probably both a left and a right 
identity.

We found it easy to prove that *nat.zero* is indeed a right
identity for *nat.add*. So can't we just replicate the proof
that zero is a right identity to show that it's also a left
identity? The answer is actually no. 

The reason that it was easy to prove that zero is a right
identity is because it's already given as an *axiom,* by 
the very definition of *nat.add*. 

In particular, the first  rule in this definition states 
exactly that for any *a, nat.add a nat.zero = a*. Here's 
the definition of *nat.add* from Lean's core library. Look 
at the first case: if zero is the second (right) argument 
to add, we just return the first argument, which makes 
zero a right zero for any natural number first argument.:: 

  def add : nat → nat → nat
  | a  zero     := a
  | a  (succ b) := succ (add a b)

Here again is the proof we constructed. We assume *n* is
an arbitrary natural number. Then by the first rule of add,
*nat.add n zero* reduces to *n*, so all that remains to show
is that *n = n*. Lean automates construction of that proof
using *rfl*. 

.. code-block:: lean

  -- and a proof, zero on the right
  example : ∀ (a : ℕ), nat.add a nat.zero = a :=
  begin
  assume a,
  simp [nat.add],
  end

The *simp* tactict tries to find, and if found applies, 
rules/axioms from the definition of the listed functions: 
here from just nat.add. We could have used *rfl* instead
of *simp*, but writing the proof in this way emphasizes
that *simplification of expressions using already proven
or accepted lemmas* is a very important maneuver in many 
proofs. 

The Problem
~~~~~~~~~~~

What's *not* provided by the definition of *nat.add* is 
an axiom that stipulates that zero is a *left* identity
for nat.add. The problem is that if we try the same proof
technique to now prove *∀ n, 0 + a = a* (with zero now on
the left), it doesn't work! The definition of *nat.add* 
tells us nothing about the result when zero is added on 
the *left* to a value, *a*. Let's see what happens when
we try.

.. code-block:: lean

  example : ∀ n, nat.add nat.zero n = n :=
  begin
  assume n,
  simp [nat.add],
  -- that didn't help; we're stuck!
  end

We might instead consider proof by case analysis 
on *n*. That doesn't work either, as we see now.  

.. code-block:: lean

  example : ∀ n, nat.add nat.zero n = n :=
  begin
  assume n,
  cases n with n',
  -- first case: zero's also on the right
  simp [nat.add],
  -- second case, argument is succ of some n'
  -- how to show 0 + (succ n') = (succ n')
  -- but again we're stuck
  simp [nat.add],
  -- basically back where we started; stuck.
  end

A Solution
~~~~~~~~~~

From basic arithmetic we know it's true that
every natural number, *a*, has the *property*
that *0 + a = a,* but it's also now clear that 
we don't yet have the tools to prove that this 
is true. In this section we'll present a new
method, *proof by induction,* that will close
this gap.

To begin, let's formally state the *property*
that we want to prove is true for every *nat*.
As we've seen before, we'll formally state the
property as a predicate, and then we'll see a
way to prove that this predicate is true for
every natural number. 

Let's define *(P a)* to be the proposition that
*0 + a = a*. Our goal will then be to show that
*∀ (a : ℕ), P a.*

.. code-block:: lean

  
  -- The property we want to prove is universal
  def P (a : ℕ) : Prop := nat.add nat.zero a = a
  
  #check P      -- nat → Prop   -- property/predicate

Let's take a different approach, starting with 
a problem instance, with zero on the left, that 
we can easily prove: namely when zero is also on
the right, because in this special case we *can*
use the first axiom/rule of addition. (Yes, we
can use rfl instead, but we're interested to see
a general approach.)

.. code-block:: lean

  theorem p0 : P 0 := 
  begin
  unfold P,         -- expand definition of P
  simp [nat.add],        -- rfl to finish off proof
  
  end

That was easy but it doesn't get us very far. We
next ask the question, from the value, 0, and our
proof of (P 0), can we construct a proof of (P 1)? 
In fact we can.

.. code-block:: lean

  theorem p1 : P 1 := 
  begin
  unfold P,
  have ih := p0,
  unfold P at ih,
  show nat.succ (nat.add nat.zero nat.zero) = 1, -- first rule of add
  rw ih,
  end

Lean provides some automation here. First it
applies the second rule of nat.add to change
the goal to (in effect) 1 + (0 + 0) = 1; then 
it (in effect) uses p0 to rewrite 0 + 0 as 0, 
then it uses the first rule to rewrite 1 + 0
as 1 (zero on the right), and finally rfl to
polish off the proof. 

From a proof that 0 is a *left* identity for 
0 can we build a proof that 0 is a left identity 
for one! So from a proof of P 1, can we now build
a proof of P 2? Yes, we can!

.. code-block:: lean

  theorem p2 : P 2  :=
  begin
  unfold P,
  have ih := p1,
  show 1 + (0 + 1) = 2, -- second rule of add
  unfold P at ih,       -- use ih, Lean automation
  end 
  
  -- Wow, can we just keep doing this?
  
  theorem p3 : P 3  :=
  begin
  unfold P,
  have ih := p2,    -- just for clarity
  show 1 + (0 + 2) = 3,
  unfold P at ih,
  end 
  
  theorem p4 : P 4  :=
  begin
  have ih := p3,    -- just for clarity
  show 1 + (0 + 3) = 4,
  unfold P at ih,
  end 
  
  /- It looks like that from any nat, *a' : nat*, 
  and a proof of *P a'* we can prove *P (a' + 1)*.
  -/

Clearly we can't write such a proof for each value 
of *a'*. The next question is, *Can we generalize the
idea that we can *step up* from a proof of *P a'* to 
a proof of *P (a'+1)* for any value of *a'*?  That is,
can we show *∀ (a' : ℕ), P a' → P (a' + 1)? We can! 

.. code-block:: lean

  lemma step : ∀ (a' : ℕ), P a' → P (a'.succ) :=
  begin
  assume a' ih,
  unfold P at ih,
  unfold P,
  -- some tedious rewriting of notations is needed
  -- Lean confirms that these rewrites are valid
  show nat.add 0 a'.succ = a'.succ,
  -- now this simplification works
  simp [nat.add],
  -- same problem again
  show 0 + a' = a',
  /- 
  We've thus reduced the original goal to the
  goal of proving the hypothesis that we have
  already assumed (implication introduction). 
  -/
  apply ih,
  end

We don't yet have the proof of *∀ a, P a* that
we seek. What we do have are proofs, *p0 : P 0*
and *step: ∀ a', P a' → P (a'+1).* Moreover we've
just seen that if we start with *p0* and apply
*step a* times, we can construct a proof of *P a*
for any value, *a*. Of course now we can automate
that last step by writing a function that does 
just that: take any value, *a*, start with *p0*,
and apply *step a* times, and end up with a proof
of *P a*. The iterative application of *step* is
accomplished by recursion.

.. code-block:: lean

  -- formerly called pa (in class)
  def zero_left_ident_add : ∀ (a : ℕ), (nat.add 0 a = a) 
  | 0 := p0
  | (nat.succ a') := (step a' (zero_left_ident_add a'))
  
  #check zero_left_ident_add  
  -- ∀ (a : ℕ), 0.add a = a!
  

-- This function proves ∀ a, P a. It's a universal
generalization, so we can apply it to any specific value
of *a* to get a proof that zero is a left identity for that
particular *a*.  

.. code-block:: lean

  #reduce zero_left_ident_add 0
  #reduce zero_left_ident_add 1
  #reduce zero_left_ident_add 2
  #reduce zero_left_ident_add 3

Moreover, by inspecting the (semi-unreadable) proof terms, 
you can see that the proof term for each value, *a,* includes 
within it a proof term for the next smaller value, all the way 
down to the proof term for zero. Just as larger nat values 
are built from, and incorporate, smaller ones, down to zero,
so do proofs of *P a* for larger value of *a* build on and
incorporate proofs of *P a'* for smaller values of *a',* all 
the way down to a proof of *P 0*. We thus construct proofs of 
*P a* for any *a* inductively, just as we define the natural
numbers themselves inductively. This method is called proof
by induction.

Summary So Far
~~~~~~~~~~~~~~

Let's pull the pieces of this story together. We started by 
specifying a property, *P a := 0 + a = 0*, of natural numbers. 
Then we then proved that *every* natural number, *a*, has this 
property: *∀ (a : ℕ), P a*. The proof relied on two lemmas and
a procedure that uses both of them.

- First, we constructed a proof *refl : P 0* (0 is a left identity for 0);
- Second, we proved *step : ∀ a', P a' → P (a' + 1) (from any natural number, *a',if *we have a proof of *P a'*, then we can derive a proof of P (a' + 1); 
- Finally these facts prove that every natural number *a* has property *P* by giving a function that constructs a proof of *P a* for any *a*;
- Key idea: apply *step* to *refl a* times (by ordinary recursion) to produce a proof of *P a*.  

For our particular definition of *P a* at least, we've thus proved this::

  *∀ (a : ℕ), 
    P 0 → 
    (∀ (a' : ℕ), P a' → P (a' + 1)) →
    P a*

If *a* is an arbitrary natural number, and if we have a 
proof, *base : P 0,* and if we also have a proof, *step : 
∀ (a' : ℕ), P a' → P (a' + 1)*, then by iteratively applying
*step* to *base* we can derive a proof of *P a*. As *a* 
was arbitrary, we've proved *∀ a, P a.* Moreover, the proofs
constructed in this way have recursive structures. 

At this point we've proved that zero is both a left and a 
right identity for the natural numbers. We can thus say that
zero is an additive identity (on the left and right) for the
natural numbers.  

.. code-block:: lean

  -- 0 is a left and right identity for nat +
  theorem zero_ident_nat_add :
    ∀ (a : ℕ), 
      (nat.add 0 a = a) ∧
      (nat.add a 0 = a) :=
  begin
  assume a,
  split,
  apply zero_left_ident_add,  -- inductive case
  simp [nat.add],             -- base case is easyend
  end
  
  
  theorem zero_ident_nat_add' : ∀ (a : ℕ), (0:nat).add a = a ∧ a.add 0 = a :=
  begin
  assume a,
  split,
  apply zero_left_ident_add,
  apply rfl,
  end
  

Induction Axioms
~~~~~~~~~~~~~~~~

The principle we've developed is available as an axiom 
generated from the definition of the nat data type. The
name of the principle is *nat.rec_on*. Applying it to the
smaller lemmas yeilds a proof of the generalization. 

If you prove the lemmas first, in a bottom-up proof style,
you can just apply the induction principle to a value, *a*,
and to the two proofs, to get a proof of *P a*. Or you can
apply the axiom giving only nat value as an argument while
leaving the proof arguments to be provided as proofs of 
subgoals. 

.. code-block:: lean

  -- The induction principle for natural numbers.
  #check @nat.rec_on

A top-down approach is more typical, wherein we apply the 
induction axiom for natural numbers to construct the overall
proof we need, leaving the smaller lemmas to be proved as
subgoals. 

.. code-block:: lean

  example : ∀ a, P a :=
  begin
  assume a,
  apply nat.rec_on a,
  exact rfl,    -- base case
  exact step,   -- we use already proven lemma
  end
  
  -- You can also use Lean's *induction tactic*.
  example : ∀ a, P a :=
  begin
  assume a,
  induction a with a' ih, -- applies axiom
  exact rfl,              -- base case
  unfold P,               -- inductive case
  unfold P at ih,
  simp [nat.add],
  assumption,
  end

Exercises
~~~~~~~~~

Here from Lean's library is the definition
of natural number multiplication. Your job 
is to prove that 1 is an identity (left and
right identity) for nat multiplication. Fill
in the missing proof.

.. code-block:: lean

  #check nat.mul
  /-
  def mul : nat → nat → nat
  | a 0     := 0
  | a (b+1) := (mul a b) + a
  -/
  
  -- 
  def mul_one_left_ident_prop := ∀ a, nat.mul 1 a = a
  def mul_one_right_ident_prop := ∀ a, nat.mul a 1 = a
  def mul_one_ident_prop := mul_one_right_ident_prop ∧ mul_one_left_ident_prop
  
  theorem mul_one_ident : mul_one_ident_prop :=
  begin
  split,
  _         -- Replace this placeholder with your proof
  end

- Construct a proof, nat_add_assoc, that nat.add is associative.
- Construct a proof, nat_mul_assoc, that nat.mul is associative.


Monoids and Foldr
~~~~~~~~~~~~~~~~~

This proof is a significant accomplishment. It gives us a
proof we'll need to formalize the fundamental mathematical 
concept of a monoid: a structure comprising a collection of 
values (here of some type, α), an associative binary operator 
on such objects, and an identity element *for that operator*.  

We don't have a proof of associativity of addition, but we do
now have the tools to prove that nat.add is associative. We're
thus close to being able to formally define a monoid structure
on the natural numbers. 

In particular, we can now define a general structure that we
can instantiate to formally represent the additive monoid on 
the natural numbers.

.. code-block:: lean

  universe u
  
  -- general structure
  structure nat_monoid : Type := mk::
    (op : nat → nat → nat)
    (id : ℕ)
    (e : ∀ a, op id a = a ∧ op a id = a)
    (assoc: ∀ a b c, op a (op b c) = op (op a b) c)
  
  def nat_add_monoid := nat_monoid.mk   nat.add 0 zero_ident_nat_add' sorry  
  def nat_add_monoid' := nat_monoid.mk  nat.add 1 zero_ident_nat_add' sorry  -- yay caught error
  def nat_mul_monoid := nat_monoid.mk   nat.mul 1 sorry sorry                -- no checking here 
  
  -- EXERCISES: Construct proofs to fill in the *sorry*s.
  
  -- Monoid structure instances 
  #reduce foldr nat_add_monoid.op nat_add_monoid.id [1,2,3,4,5]
  #reduce foldr nat_mul_monoid.op nat_mul_monoid.id [1,2,3,4,5]
  
  
  -- A version of foldr that takes a monoid object and uses its op and e values
  def foldr' {α β : Type} : nat_monoid → list nat → nat
  | (nat_monoid.mk op e _ _) l := foldr op e l
  
  -- Safe use of monoid instances folds
  #reduce foldr' nat_add_monoid [1,2,3,4,5]
  #reduce foldr' nat_mul_monoid [1,2,3,4,5]

Induction Generalized 
---------------------

UNDER CONSTRUCTION FROM HERE ON DOWN.

Just as we will need a proof that 0 is not only a right
identity for nat.add (by the first axiom) but also a left
identity (a theorem proved by induction), so will need a
proof that nil is not only a right but also a left identity
for the list append operation.  

Here's the easy case first. From this proof you can infer
that the list.append operation (with infix notation ++) has
a rule/axiom that states that l ++ nil := l for any l. 

Here's the definition of list.append.
It asserts that [] is a left identity axiomatically. 

def append : list α → list α → list α
| []       l := l
| (h :: s) t := h :: (append s t)

.. code-block:: lean

  -- proving right identity is trivial just as for addition
  example (α : Type) : ∀ (l : list α), list.nil ++ l = l :=
  begin
  assume l,
  simp [list.append],
  end

We run into the same problem as we did before if we take a
naive approach to trying to prove that nil is also a left
identity for ++. And the solution is once again to define
a recursive function by case analysis on l that constructs
a proof of *nil ++ l = l* for any list l. If l = list.nil,
the proof of nil ++ nil is given by the first rule of list
append, otherwise l = (h::t), and we need to prove that
nil ++ h::t = h::t. By the second axiom of list append,
we can rewrite nil ++ h::t as h::(nil ++ t), where we can
obtain (and then us) a proof that nil ++ t = t by recursion,
terminating when t =nil. 

Fortunately, Lean's library already contains a proof that
nil is a right identity, and it's annotated as *[simp]*,
which means that the *simp* tactic will try to use it to
prove our goal. In other words, we can use [simp] to prove
the harder case precisely because someone else has already
done the work for us; and they did it recursively just as
we did to show that 0 is a right identity for addition. 

.. code-block:: lean

  def nil_left_ident_app (α : Type) : ∀ (l : list α), l ++ list.nil = l :=
  begin
  assume l,
  cases l with h t,
  -- base case
  simp [list.append],   -- uses first rule
  -- recursive case
  simp [list.append],   -- why does this work?
  end 
  
  -- Here's another formal demonstration of the same point
  variables (α : Type) (a : α) (l : list α) 
  example: list.nil ++ l = l := by simp    -- first rule
  example : l ++ list.nil  = l := by simp  -- by [simp] lemma in Lean library


Induction Axioms
----------------

YOU MAY STOP READING HERE. THE REMAINDER IS STILL *UNDER CONSTRUCTION.*

Inductive Families
------------------

Coming soon.

.. code-block:: lean

  inductive le (n : nat): nat → Prop 
  -- n is an implicit firt argument to each constructor
  | refl : le /-n-/ n     
  | step : ∀ m, le /-n-/ m → le /-n-/ m.succ
  
  -- you can see it in the types of the constructors
  #check @le.refl
  #check @le.step
  
  
  example : le 0 0 :=
  begin
  apply le.refl,
  end 
  
  example : le 3 3 :=
  begin
  apply le.refl,
  end 
  
  example : le 0 1 :=
  begin
  apply le.step,
  apply le.refl,
  end 
  
  example : le 0 3 :=
  begin
  apply le.step,
  apply le.step,
  apply le.step,
  apply le.refl,
  end 
  
  -- here's the same example using Lean's version of "le"
  -- it's called nat.less_than_or_equal
  example : 0 ≤ 3 :=
  begin
  apply nat.less_than_or_equal.step,
  apply nat.less_than_or_equal.step,
  apply nat.less_than_or_equal.step,
  -- apply nat.less_than_or_equal.step,
  apply nat.less_than_or_equal.refl,
  end 
  
  -- repeat tactical goes too far; use iterate instead
  example : 1 ≤ 4 :=
  begin
  -- repeat {apply nat.less_than_or_equal.step},
  iterate 3 {apply nat.less_than_or_equal.step},
  apply nat.less_than_or_equal.refl,
  end 
