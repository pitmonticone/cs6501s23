-- QUOTE: 
import .A_06_prop_logic_algebraic_axioms 
namespace cs6501
-- QUOTE.

/- TEXT:

**************************
Inference Rules Validation
**************************

This chapter pulls together in one place a formal
validation of the claim that our model of propositional
logic satisfies all of the =inference rules of that logic.

The first section of this chapter refactors the partial
solution we developed in class, grouping definitions of
the propositions that represent them, and separately a
proof that each rule expresses in our model is valid.
These sections also afford opportunities to introduce
a few more concepts in type theory and Lean.

To begin we import some definitions and declare a set
of variables available for use in this file.
TEXT. -/

-- QUOTE: 
section rule_validation
variables 
  (X Y Z: prop_expr) 
  (i : prop_var → bool)
-- QUOTE.


/- TEXT:
Inference Rule Statements
-------------------------

We start with a refactoring of the results of the last
chapter, into formal statements of the inference rules
and formal proofs that these rules are valid (truth-
preserving under all interpretations) in our model of
propositional logic.

Key idea: These are rules for reasoning about evidence.
What *evidence* do you need to derive a given conclusion?
These are the "introduction" rules. From a given piece of
evidence (and possibly with additional evidence) what new
forms of evidence can you derive? These are "elimination"
rules of the logic.
TEXT. -/

-- QUOTE:

-- remember, we can now use X, Y, Z, i

def true_intro_rule := ⟦ ⊤ ⟧ i = tt
def false_elim_rule := ⟦⊥⟧ i = tt → ⟦X⟧ i = tt  -- X is any proposition
def and_intro_rule := ⟦ X ⟧ i = tt → ⟦ Y ⟧ i = tt → ⟦(X ∧ Y)⟧ i = tt 
def and_elim_left_rule := (⟦(X ∧ Y)⟧ i = tt) → (⟦X⟧ i = tt)
def and_elim_right_rule := (⟦(X ∧ Y)⟧ i = tt) → (⟦Y⟧ i = tt)
def or_intro_left_rule := (⟦X⟧ i = tt) → (⟦(X ∨ Y)⟧ i = tt) 
def or_intro_right_rule := (⟦Y⟧ i = tt) → (⟦(X ∨ Y)⟧ i = tt) 
def or_elim_rule :=   (⟦(X ∨ Y)⟧ i = tt) → 
                      (⟦(X => Z)⟧ i = tt) → 
                      (⟦(Y => Z)⟧ i = tt) → 
                      (⟦(Z)⟧ i = tt)
-- formalize the rest
-- 9. ¬¬X ⊢ X                 -- negation elimination
-- 10. X → ⊥ ⊢ ¬X             -- negation introduction
-- 11. (X ⊢ Y) ⊢ (X → Y)      -- a little complicated
-- 12. X → Y, X ⊢ Y           -- arrow elimination
-- 13. X → Y, Y → X ⊢ X ↔ Y    -- iff introduction
-- 14. X ↔ Y ⊢ X → Y          -- iff elimination left
-- 15. X ↔ Y ⊢ Y → X          -- iff elimination right

-- QUOTE.


/- TITLE:
Our next task is to formalize statements of these
informally stated inference rules and to prove using
Lean that these rules are logically *valid* in our 
representation of propositional logic. Doing this
will also serve as a warmup for understanding how
essentially the same inference rules are the rules
of reasoning in predicate logic. 

We first present examples, and use them to introduce
and get some practice with key ideas in Lean. Then we
leave the rest for you to prove.

Examples from Class
-------------------

TEXT: -/

-- QUOTE:
open cs6501

-- note:
#reduce and_intro_rule e1 e2 i

-- prove it
theorem and_intro : and_intro_rule e1 e2 i :=
begin
assume X_true Y_true,
unfold pEval bin_op_sem, -- axioms of eq
rw X_true,
rw Y_true,
apply rfl,
end 

theorem and_elim_left : and_elim_left_rule X Y i :=
begin
unfold and_elim_left_rule pEval bin_op_sem,
-- case analysis
assume h_and,
cases ⟦ X ⟧ i,    -- cases analysis on X
{ -- case X (evaluates to) false
  cases ⟦ Y ⟧ i,  -- nested case analysis on Y 
    cases h_and,   -- contradiction
    cases h_and,   -- contradiction
},
{ -- case X (evaluates to) true
  cases ⟦ Y ⟧ i, -- nested case analysis on Y 
  cases h_and,  -- contradiction
  apply rfl,    -- ahh, equality
},
end 

theorem or_intro_left : or_intro_left_rule X Y i
:=
begin
unfold or_intro_left_rule pEval bin_op_sem,
assume X_true,
rw X_true,
apply rfl,
end

theorem or_elim : or_elim_rule X Y Z i :=
begin
-- expand definitions as assume premises
unfold or_elim_rule pEval bin_op_sem,
assume h_xory h_xz h_yz,
-- the rest is by nested case analysis
-- this script is refined from my original 
cases (⟦ X ⟧ i), -- case analysis on bool (⟦ X ⟧ i) 
repeat {
  repeat {      --  case analysis on bool (⟦ Y ⟧ i)
    cases ⟦ Y ⟧ i,
    repeat {    -- case analysis on bool (⟦ Z ⟧ i)
      cases ⟦ Z ⟧ i,
      /-
      If there's an outright contradiction in your
      context, this tactic will apply false elimination
      to ignore/dismiss this "case that cannot happen."
      -/
      contradiction, 
      apply rfl,
    },
  },
},
end  
-- QUOTE.



/- TEXT:
Proofs
------
In the style of the preceding examples, give formal proofs
of that the remaining inference rules are valid in our own
model of propositional logic. 

Identify any rules that fail to be provably valid in the 
presence of the bug we'd injected in bimp. Which rule
validation proofs break when you re-activate that bug? 

To get you started, the following proof shows that the
false elimination inference rule is valid in our logic.
TEXT. -/

/- TEXT:
For any proposition, e, and interpretation, i, in 
our logic, if ⊥ implies e, so from the truth of
⊥, the truth of any expression follows. But ⊥ 
can't ever be (evaluate to) true, because we've
defined the logic otherwise. But wait, is there
another way to formalize the axiom? If so, are
the two ways equivalent? 
TEXT. -/

-- QUOTE:
theorem false_elim : false_elim_rule X i :=
begin
unfold false_elim_rule pEval,
assume h,
cases h,  -- contradiction, can't happen, no cases!
          -- Lean determines tt = ff is impossible
end


-- Define the remaining propositions and proofs here: 


end rule_validation -- section
end cs6501          -- namepsace
-- QUOTE.
