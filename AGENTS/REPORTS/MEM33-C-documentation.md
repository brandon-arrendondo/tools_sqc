# MEM33-C: Allocate and copy structures containing a flexible array member dynamically

## Rule Summary
Structures containing a flexible array member should always be allocated dynamically to avoid undefined behavior.

## Key Requirements
1. Structures with flexible array members must:
   - Have dynamic storage duration (allocated via `malloc()`)
   - Be dynamically copied using `memcpy()` or similar function, not by assignment
   - Be passed by pointer when used as function arguments, not copied by value

## Violations to Detect

### 1. Automatic Storage for Flexible Array Structures
```c
// NONCOMPLIANT: Using automatic storage
struct flex_array_struct {
  size_t num;
  int data[];
};

void func() {
    struct flex_array_struct flex_struct;  // VIOLATION: automatic storage
    // Accessing flex_struct.data is undefined behavior
}
```

### 2. Assignment Copy of Flexible Array Structures
```c
// NONCOMPLIANT: Direct assignment
struct flex_array_struct *struct_a, *struct_b;
// ... allocate struct_a ...
*struct_b = *struct_a;  // VIOLATION: assignment copy
```

### 3. Passing Flexible Array Structures by Value
```c
// NONCOMPLIANT: Pass by value
void process_struct(struct flex_array_struct s) {  // VIOLATION: by value
    // ...
}
```

## Compliant Solutions

### 1. Dynamic Allocation
```c
struct flex_array_struct *flex_struct;
size_t array_size = 10;

flex_struct = malloc(
    sizeof(struct flex_array_struct) +
    sizeof(int) * array_size
);

if (flex_struct != NULL) {
    flex_struct->num = array_size;
    // Safe to use flex_struct->data[0] through flex_struct->data[9]
}
```

### 2. Dynamic Copying with memcpy
```c
memcpy(struct_b, struct_a,
    sizeof(struct flex_array_struct) +
    (sizeof(int) * struct_a->num)
);
```

### 3. Pass by Pointer
```c
void process_struct(struct flex_array_struct *s) {  // COMPLIANT: by pointer
    // ...
}
```

## Detection Patterns

1. **Flexible Array Member Detection**: Look for struct declarations with array members using `[]` syntax
2. **Automatic Storage**: Detect local variable declarations of flexible array structs
3. **Assignment Copy**: Detect direct assignment between flexible array struct instances
4. **Value Parameter**: Detect function parameters that are flexible array structs passed by value

## Risk Assessment
- Severity: Low
- Likelihood: Unlikely
- Remediation Cost: Medium

## Related Rules
- DCL38-C: Use the correct syntax when declaring a flexible array member
- MEM35-C: Allocate sufficient memory for an object

Source: https://wiki.sei.cmu.edu/confluence/display/c/MEM33-C