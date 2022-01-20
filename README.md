# DPLL
Implementation of DPLL SAT-solver algorithm

Build with
> cmake .
> 
> make
 
Run example with
> ./DPLL cnf.DIMACS

Or generally
> ./DPLL [FilePath] ([HeuristicID = 3])

Where [HeuristicID] specifies which branching heuristic will be used while solving the problem:
0. Jeroslow - Wang
1. Hooker - Vinay
2. Van Gelder - Tsuji
3. CSAT