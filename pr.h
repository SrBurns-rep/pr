/*-------------------------------------*\
| Priority system (PR)				 	|
| This module defines structures and    |
| functions for a priority system       |
| that evaluates expressions and        |
| manages Lua code within a node pool.  |
\*-------------------------------------*/

#ifndef _PR_
#define _PR_

#include <stdint.h>
#include <stdbool.h>

#define PR_NODE_POOL_START_SIZE (256)
#define PR_LUA_STR_POOL_START_SIZE (256)
#define PR_RULE_POOL_START_SIZE	(256)

typedef enum pr_ops {
	PR_NIL,			// node is undefined
	PR_GT,			// greather than
	PR_GE,			// greather or equals
	PR_LT,			// less than
	PR_LE,			// less or equals
	PR_EQ,			// equals
	PR_DF,			// different from
	PR_AND,			// logical and
	PR_OR,			// logical or
	PR_NOT,			// logical not
	PR_VAL_U32,		// value end unsigend integer
	PR_VAL_I32,		// value end signed integer
	PR_VAL_F32,		// value end float
	PR_LUA,			// lua code end leaf
}pr_ops;

typedef union pr_value {
    uint32_t u32;
    int32_t i32;
    float f32;
} pr_value;

typedef struct pr_node {
	pr_ops type;							// type of operation
	union { 
		struct {
			uint32_t left_index;			// index of the left operand in the node pool
			uint32_t right_index;			// index of the right operand in the node pool
		}op;								// binary branching operations
		pr_value value;						// numeric value end
		struct {
			uint32_t id;					// Unique identifier for lua code
			uint32_t lua_str_pool_index;	// Index of lua code in the string pool
		}lua;								// lua code end-nodes (leaf)
	}data;									// data associated with the node
} pr_node;

typedef struct pr_node_pool {
	uint32_t current_index;
	int error;
	pr_node pool[];
} pr_node_pool;

typedef struct pr_lua_str_pool {
	uint32_t size;
	uint32_t current_offset;
	char buffer[];
} pr_lua_str_pool;

typedef struct pr_rule {
	uint8_t priority;
	pr_node_pool *pool;
	uint32_t node_root_index;
	int error;
} pr_rule;

typedef struct pr_rule_pool {
	uint32_t size;
	uint32_t current_offset;
	pr_rule pool[];
} pr_rule_pool;

// Initialize a Lua string pool with the specified size.
pr_lua_str_pool *pr_lua_str_pool_init(uint32_t size);

// Double the size of the lua code pool when it's getting full.
pr_lua_str_pool *pr_lua_str_pool_resize(pr_lua_str_pool *pool);

// Frees the memory allocated for the string pool.
void pr_lua_str_pool_deinit(pr_lua_str_pool* pool);

// Initialize a node pool with the specified number of nodes;
pr_node_pool *pr_node_pool_init(uint32_t n_nodes);

// Double the size of the node pool when it's getting full.
pr_node_pool *pr_node_pool_resize(pr_node_pool *pool);

// Frees the memory allocated for the node pool.
void pr_node_pool_deinit(pr_node_pool *pool);

// Add lua code to the string pool and returns its offset.
uint32_t pr_add_lua_code_from_file(pr_lua_str_pool *pool, char *file);

// Add a value node to pool and returns its index.
uint32_t pr_add_value_node_to_pool(pr_node_pool *pool, pr_ops op, pr_value value);

// Add a branch node to pool and returns its index.
uint32_t pr_add_branch_node_to_pool(pr_node_pool *pool, pr_ops op, uint32_t left_index, uint32_t right_index);

// Add a lua code node to pool and returns its index.
uint32_t pr_add_lua_code_node_to_pool(pr_node_pool *pool, pr_lua_str_pool *lua_pool, uint32_t lua_pool_offset);

// Bind two nodes to a branching new base node and returns its index.
uint32_t pr_bind_nodes(pr_node_pool *pool, pr_node *new_base_node, uint32_t left_node_index, uint32_t right_node_index);

// Evaluates the branch node under two leafs, if one or more of its ends is not a leaf
// error in the node pool will be set
bool pr_evaluate_branch_node(pr_node_pool *pool, uint32_t left_node_index, uint32_t right_node_index);

// Evaluates a lua node in the node pool and returns its value as a boolean.
// error in the node pool will be set if lua returns with an error.
bool pr_evaluate_lua_node(pr_node_pool *pool, uint32_t lua_node_index);

// Evaluates a node in the node pool and returns a boolean value.
bool pr_evaluate_node(pr_node_pool *pool, uint32_t node_index);

// Calculates every node in the tree and returns true or false.
bool pr_evaluate_tree(pr_node_pool *pool, uint32_t root_node_index);

// Initialize a rule pool.
pr_rule_pool *pr_rule_pool_init(uint32_t n_rules);

// Double the size of the rule pool when it's getting full.
pr_rule_pool *pr_rule_pool_resize(pr_rule_pool *pool);

// Frees the memory associated with the rule pool.
pr_rule_pool *pr_rule_pool_deinit(pr_rule_pool *pool);

// Adds a rule to the pool.
uint32_t pr_add_rule(pr_rule_pool *rule_pool, uint8_t priority, pr_node_pool *node_pool, uint32_t node_root_index);

// Evaluates a rule and return its bolean value
bool pr_evaluate_rule(pr_rule_pool *pool, uint32_t rule_index);

// Sort the rules for evaluation in priority order 0 is higher, 255 is lower
pr_rule_pool *pr_sort_rules(pr_rule_pool *pool);

// Evaluates all the rulles and returns a boolean array.
bool *pr_evaluate_all_rules(pr_rule_pool *pool, bool *bool_buffer, uint32_t bool_buffer_size);

#endif // _PR_
