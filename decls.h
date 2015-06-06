typedef struct state state;

state* new_state();
void free_state(state* state);

int pre_fork_setup(state* state);
int cleanup(state* state);

int child_post_fork_setup(state* state);
int child_warmup(int warmup_iters, state* state);
int child_loop(int iters, state* state);
int child_cleanup(state* state);

int parent_post_fork_setup(state* state);
int parent_warmup(int warmup_iters, state* state);
int parent_loop(int iters, state* state);
int parent_cleanup(state* state);

