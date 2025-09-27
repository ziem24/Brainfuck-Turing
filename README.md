# Brainfuck-Turing (BFT) v 1.0

*This is (the longest, most unemployed, and most importantly: 28 years too late) proof that Brainfuck is Turing-complete.*

---
### 1. What is BFT
As mentioned earlier, this is my proof of Brainfuck's Turing-completeness. It turns a simple Turing machine configuration file into a valid Brainfuck program.

---
### 2. Running the program
This is fairly simple. For Linux do
```bash
bash build.sh   # optionally: use no-tests to skip testing
bash run.sh path/to/file
```
There's currently no support for other operating systems, so non-Linux people will have to manually compile the files.

---
### 3. Turing machine configuration files
They are split into two parts: .section CONFIG and .section TRANSITIONS:
- CONFIG holds information about the file name and initial head position;
- TRANSITIONS tells the Turing machine what to do on each state. Each transition is defined as $q\text{ }:\text{ }q_0\text{ }s_0\text{ }m_0\text{ }q_1\text{ }s_1\text{ }m_1$, where
    - $q,q_0,q_1\in Q$ - Defined state, state if head encounters 0/1
    - $s_0,s_1\in\{0,1\}$ - New symbol if head encounters 0/1
    - $m_0,m_1\in\{L,R\}$ - Head movement direction if head encounters 0/1


Notes:
- Everything is case sensitive
- Everything (including the colons) is separated with at least 1 space
- Everything from the section CONFIG must be defined
- States that are not defined are treated as halting states
- State names are single words;
- Badly encoded sections will result in faulty compilation or an error

This is an example of how a configuration file might look like:
```c
.section CONFIG
    FILE        busy_beaver_4.bf
    HEADPOS     25

.section TRANSITIONS
    /*
    This is a multiline comment.
    Nothing in here will affect the configuration.
    */
    st_A : st_B 1 R st_B 1 L
    st_B : st_A 1 L st_C 0 L
    st_C : HALT 1 R st_D 1 L
    st_D : st_D 1 R st_A 0 R
```

---
### 4. Technical details
*I'm not doing it.* - Steve O

### 5. To do
- Some interpreter/visualizer for this thing
- Better docstrings (maybe)