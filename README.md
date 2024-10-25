# Priority System (PR)

This project defines a **Priority System** that evaluates expressions and manages Lua code within a node pool. It offers a structured way to add and evaluate rules using a variety of data types and Lua-based code.

## Features
* **Lua Integration:** Supports executing Lua scripts within nodes to extend the system’s functionality.
* **Priority-based Rule Evaluation:** Allows adding rules with priorities for ordered evaluation.
* **Efficient Rule Sorting:** Implements a quicksort algorithm to sort rules by priority.
* **Boolean Expression Handling:** Supports a range of boolean and comparison operations (`AND`, `OR`, `NOT`, `GT`, `EQ`, etc.).
* **Error Handling:** Includes error tracking within node pools to detect and manage issues during node operations.
* **Streamlined Memory Management:** Allocates pools from which it picks the needed objects.

## Usage
**Initializing Pools:**
```c
pr_node_pool *node_pool = pr_node_pool_init(256);        // 256 is the number of nodes
pr_lua_str_pool *lua_pool = pr_lua_str_pool_init(1024);  // 1024 is the number of characters
```

**Adding nodes:**
* Adding a value node:
	```c
	pr_value value;
	value.i32 = 10;
	uint32_t node_index = pr_add_value_node_to_pool(node_pool, PR_VAL_I32, value);
	```
* Adding a Lua node:
	```c
	uint32_t lua_offset = pr_add_lua_code_from_file(lua_pool, "some.rule.lua");
	uint32_t lua_node_index = pr_add_lua_code_node_to_pool(node_pool, lua_pool, lua_offset);
	```
**Adding and Evaluating Rules:**
```c
pr_rule_pool *rule_pool = pr_rule_pool_init(10);
uint32_t rule_index = pr_add_rule(rule_pool, node_pool, 0, node_index);
bool result = pr_evaluate_rule(rule_pool, rule_index);
```

**Sorting Rules:**
```c
pr_sort_rules(rule_pool);
```

**Evaluating All Rules:**
```c
bool bool_buffer[10];
pr_evaluate_all_rules(rule_pool, bool_buffer, 10);
```

## Contributing
Contributions are welcome! If you'd like to report a bug or contribute to the codebase, please follow these steps:
1. Fork the repository.
2. Create a new branch (`git checkout -b feature-branch`).
3. Make your changes.
4. Commit your changes (`git commit -m 'Add new feature`).
5. Push to the branch (`git push origin feature-branch`).
6. Open a pull request.

## License
MIT License.
