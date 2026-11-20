# Member 4: Correctness and testing

## Your task
Prove the code is correct: random graphs, a certificate check, and agreement between Kruskal and Prim. Answer Q4.

## Files you commit (in COMMIT_THESE)
- tests/randomgraph.h
- tests/test_all.cpp
- tests/sample1.in, sample1.out, sample2.in, sample2.out
- test.ps1
- TOOLS.md

## Test your part
Copy COMMIT_THESE and NEEDED_TO_BUILD_DO_NOT_COMMIT into one scratch folder (not your repo). Make sure a folder called results exists, then:

    g++ -std=c++17 -O2 -o test_all tests/test_all.cpp
    .\test_all

You should see: cases: 1207, certified small graphs: 1050, disconnected: 152, failed: 0
It writes results/correctness.csv.

## Commit (small steps, own branch)

    git checkout -b feat/testing
    git add tests/randomgraph.h
    git commit -m "Add random graph generators for tests"
    git add tests/test_all.cpp
    git commit -m "Add differential test and certificate check"
    git add tests/sample1.in tests/sample1.out tests/sample2.in tests/sample2.out test.ps1
    git commit -m "Add sample files and test script"
    git add TOOLS.md
    git commit -m "Add TOOLS.md"
    git push -u origin feat/testing

Open a Pull Request into main. Set your git email to the one verified on your GitHub account first.
TOOLS.md: ask each teammate for their line (what Claude wrote, what they changed) and add it.

## Your question: Q4 (Prim is O(E log V), heap size)
- Each cable goes into the heap at most twice, once from each end, so the heap holds at most 2E entries.
- Each push or pop costs O(log of the heap size) = O(log E) = O(log V), because E <= V squared.
- At most 2E pushes and 2E pops, so the total is O(E log V).

## What to be able to explain in the viva
- The certificate check: every cable must cost at least as much as every cable on the tree path between its ends. Otherwise a swap would make the tree cheaper.
- Why the test uses a fixed seed.
- What kinds of graphs you tested (ties, disconnected, dense, trees, k=2 and k=n).

## Results already included
COMMIT_THESE/results/correctness.csv is the output of test_all from Rohith's run (1,207 cases, 0 failed). Commit it with:

    git add results/correctness.csv
    git commit -m "Add correctness results"
