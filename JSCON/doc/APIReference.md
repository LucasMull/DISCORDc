# JSCON API Reference

This document describes the public C API.

## Datatypes

### Primitives

| Primitive | Alias | Flag |
| :--- | :--- | :--- |
|**`jscon_char_kt *`**|`char *`|`JSCON_STRING`|
|**`jscon_double_kt`**|`double`|`JSCON_NUMBER_DOUBLE`|
|**`jscon_integer_kt`**|`long long`|`JSCON_NUMBER_INTEGER`|
|**`jscon_boolean_kt`**|`bool`|`JSCON_BOOLEAN`|
|`---`|`---`|`JSCON_NULL`|
|`---`|`---`|`JSCON_UNDEFINED`|

### Structs

| Struct | Flag |
| :--- | :--- |
|[**`jscon_composite_st`**](api/jscon_composite_st.md)|`JSCON_OBJECT or JSCON_ARRAY`|
|[**`jscon_item_st;`**](api/jscon_item_st.md)|`---`|
|[**`jscon_list_st;`**](api/jscon_list_st.md)|`---`|

### Enums

* [`jscon_type_et;`](api/jscon_type_et.md)

### Callbacks

* [`jscon_callbacks_ft;`](api/jscon_callbacks_ft.md)

## Functions

### Decoding Functions

* [`jscon_parse(buffer);`](api/jscon_parse.md)
* [`jscon_parser_callback(new_cb);`](api/jscon_parser_callback.md)
* [`jscon_scanf(buffer, format, ...);`](api/jscon_scanf.md)

### Encoding Functions

* [`jscon_stringify(item, type);`](api/jscon_stringify.md)

### Initialization Functions

* [`jscon_null(key);`](api/jscon_null.md)
* [`jscon_boolean(boolean, key);`](api/jscon_boolean.md)
* [`jscon_integer(i_number, key);`](api/jscon_integer.md)
* [`jscon_double(d_number, key);`](api/jscon_double.md)
* [`jscon_number(d_number, key);`](api/jscon_number.md)
* [`jscon_string(string, key);`](api/jscon_string.md)

* [`jscon_list_init();`](api/jscon_list_init.md)
* [`jscon_object(list, key);`](api/jscon_object.md)
* [`jscon_array(list, key);`](api/jscon_array.md)

### Destructor Functions

* [`jscon_delete(item, key);`](api/jscon_delete.md)
* [`jscon_destroy(item);`](api/jscon_destroy.md)
* [`jscon_list_destroy(list);`](api/jscon_list_destroy.md)

### Manipulation Functions

#### Movement Functions

* [`jscon_iter_next(item);`](api/jscon_iter_next.md)
* [`jscon_iter_composite_r(item, p_current_item);`](api/jscon_iter_composite_r.md)

#### Utility Functions

* [`jscon_list_append(list, item);`](api/jscon_list_append.md)
* [`jscon_size(item);`](api/jscon_size.md)
* [`jscon_append(item, new_branch);`](api/jscon_append.md)
* [`jscon_dettach(item);`](api/jscon_dettach.md)
* [`jscon_clone(item);`](api/jscon_clone.md)
* [`jscon_typeof(item);`](api/jscon_typeof.md)
* [`jscon_strdup(item);`](api/jscon_strdup.md)
* [`jscon_strcpy(dest, item);`](api/jscon_strcpy.md)

#### Comparison Functions

* [`jscon_typecmp(item, type);`](api/jscon_typecmp.md)
* [`jscon_keycmp(item, key);`](api/jscon_keycmp.md)
* [`jscon_doublecmp(item, double);`](api/jscon_doublecmp.md)
* [`jscon_intcmp(item, int);`](api/jscon_intcmp.md)

#### Getter Functions

* [`jscon_get_depth(item);`](api/jscon_get_depth.md)
* [`jscon_get_root(item);`](api/jscon_get_root.md)
* [`jscon_get_branch(item, key);`](api/jscon_get_branch.md)
* [`jscon_get_sibling(origin, relative_index);`](api/jscon_get_sibling.md)
* [`jscon_get_parent(item);`](api/jscon_get_parent.md)
* [`jscon_get_byindex(item, index);`](api/jscon_get_byindex.md)
* [`jscon_get_index(item, key);`](api/jscon_get_key_index.md)
* [`jscon_get_type(item);`](api/jscon_get_type.md)
* [`jscon_get_boolean(item);`](api/jscon_get_boolean.md)
* [`jscon_get_string(item);`](api/jscon_get_string.md)
* [`jscon_get_double(item);`](api/jscon_get_double.md)
* [`jscon_get_integer(item);`](api/jscon_get_integer.md)
