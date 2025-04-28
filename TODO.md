# TODO

This is not a list of what I will implement, it is a list of what I would like to implement. If it turns out that the newly added "improvement" is detremental to the engine's playing strength then I will discard it. However, I think that this list will not change. 

### Other

- [ ] Add piece lists for faster get_piece_at() lookups

### Search Algorithm

- [x] Negamax Search
- [x] Alpha-Beta (fail-hard negamax framework)
- [x] Triangular PV Table
- [x] Iterative Deepening
- [ ] Repetition detection
- [ ] 3 fold rep and 50 move detection
- [x] Principle Variation Search
- [x] Aspiration Windows

#### Move ordering

- [x] Done

In order of importance:
- Hash move
- PV move
- Promotions
- MVA/LVA
- Killer Heuristic
- History Heuristic

#### Transposition Tables

- [x] Done

Info stored:
- Best move (hash, refutation)
- Score
- Type of node:
    - PV-node, score is exact
    - All-node, score is upper bound
    - Cut-node, score is lower bound

#### Selectivity

- [x] Quiescence search

##### Extensions

- [ ] Check extensions

##### Reductions

- [x] Null move reductions (with Dynamic depth reduction)
- [x] Late move reductions
- [ ] Razoring

##### Pruning

- [ ] Delta pruning (for quiescence)
- [x] Futility pruning


### Evaluation Function

- [ ] Tapered Eval for game phases
- [x] Piece square tables
- [ ] Space
- [ ] Outposts
- [ ] Fianchetto

##### Pawn Structure

- [x] Doubled & Tripled
- [ ] Backward
- [ ] Isolated
- [ ] Passed
- [ ] Phalanx
- [ ] Connected

##### King Safety

Most important in middle game.
None existent in endgame.
- [ ] Pawn Shelter
- [ ] Attacking king zone
- [ ] Pawn storm

##### Mobility

- [ ] Trapped pieces
- [ ] Rooks on (semi) open files

##### Material score

- [ ] Point values, each for middle and endgame
- [ ] Bishop pair