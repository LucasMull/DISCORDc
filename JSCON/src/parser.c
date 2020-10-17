/*
 * Copyright (c) 2020 Lucas Müller
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include <libjscon.h>

#include "hashtable_private.h"


struct jscon_utils_s {
  char *buffer;
  char *key; //holds key ptr to be received by item
  jscon_htwrap_st *last_accessed_htwrap; //holds last htwrap accessed
  jscon_callbacks_ft* parser_cb; //parser callback
};

/* function pointers used while building json items, 
  jscon_create_value_ft points to functions prefixed by "_jscon_value_set_"
  jscon_create_item_ft points to functions prefixed by "jscon_decode" */
typedef void (jscon_create_value_ft)(jscon_item_st *item, struct jscon_utils_s *utils);
typedef jscon_item_st* (jscon_create_item_ft)(jscon_item_st*, struct jscon_utils_s*, jscon_create_value_ft*);

/* create a new branch to current jscon object item, and return
  the new branch address */
static jscon_item_st*
_jscon_new_branch(jscon_item_st *item)
{
  ++item->comp->num_branch;

  item->comp->branch[jscon_size(item)-1] = calloc(1, sizeof(jscon_item_st));
  assert(NULL != item->comp->branch[jscon_size(item)-1]);

  item->comp->branch[jscon_size(item)-1]->parent = item;

  return item->comp->branch[jscon_size(item)-1];
}

static void
_jscon_destroy_composite(jscon_item_st *item)
{
  Jscon_htwrap_destroy(item->comp->htwrap);

  free(item->comp->branch);
  item->comp->branch = NULL;

  free(item->comp);
  item->comp = NULL;
}

static void
_jscon_destroy_preorder(jscon_item_st *item)
{
  switch (jscon_get_type(item)){
  case JSCON_OBJECT:
  case JSCON_ARRAY:
      for (size_t i=0; i < jscon_size(item); ++i){
        _jscon_destroy_preorder(item->comp->branch[i]);
      }
      _jscon_destroy_composite(item);
      break;
  case JSCON_STRING:
      free(item->string);
      item->string = NULL;
      break;
  default:
      break;
  }

  free(item->key);
  item->key = NULL;

  free(item);
  item = NULL;
}

/* destroy current item and all of its nested object/arrays */
void
jscon_destroy(jscon_item_st *item){
  _jscon_destroy_preorder(jscon_get_root(item));
}

static jscon_char_kt*
_jscon_utils_decode_string(struct jscon_utils_s *utils)
{
  char *start = utils->buffer;
  assert('\"' == *start); //makes sure a string is given

  char *end = ++start;
  while (('\0' != *end) && ('\"' != *end)){
    if ('\\' == *end++){ //skips escaped characters
      ++end;
    }
  }
  assert('\"' == *end); //makes sure end of string exists

  utils->buffer = end + 1; //skips double quotes buffer position

  jscon_char_kt* set_str = strndup(start, end - start);
  assert(NULL != set_str);

  return set_str;
}

/* fetch string type jscon and return allocated string */
static void
_jscon_value_set_string(jscon_item_st *item, struct jscon_utils_s *utils)
{
  item->type = JSCON_STRING;
  item->string = _jscon_utils_decode_string(utils);
}

static jscon_double_kt
_jscon_utils_decode_double(struct jscon_utils_s *utils)
{
  char *start = utils->buffer;
  char *end = start;

  /* 1st STEP: check for a minus sign and skip it */
  if ('-' == *end){
    ++end; //skips minus sign
  }

  /* 2nd STEP: skips until a non digit char found */
  assert(isdigit(*end)); //interrupt if char isn't digit
  while (isdigit(*++end)){
    continue; //skips while char is digit
  }

  /* 3rd STEP: if non-digit char is not a comma then it must be
      an integer*/
  if ('.' == *end){
    while (isdigit(*++end)){
      continue;
    }
  }

  /* 4th STEP: if exponent found skips its tokens */
  if (('e' == *end) || ('E' == *end)){
    ++end;
    if (('+' == *end) || ('-' == *end)){ 
      ++end;
    }
    assert(isdigit(*end));
    while (isdigit(*++end)){
      continue;
    }
  }

  /* 5th STEP: convert string to double and return its value */
  char numerical_string[MAX_DIGITS];
  strncpy(numerical_string, start, end-start);
  numerical_string[end-start] = '\0';

  jscon_double_kt set_double;
  sscanf(numerical_string,"%lf",&set_double);

  utils->buffer = end; //skips entire length of number

  return set_double;
}

/* fetch number jscon type by parsing string,
  find out whether its a integer or double and assign*/
static void
_jscon_value_set_number(jscon_item_st *item, struct jscon_utils_s *utils)
{
  double set_double = _jscon_utils_decode_double(utils);
  if (DOUBLE_IS_INTEGER(set_double)){
    item->type = JSCON_INTEGER;
    item->i_number = (jscon_integer_kt)set_double;
  } else {
    item->type = JSCON_DOUBLE;
    item->d_number = set_double;
  }
}

static jscon_boolean_kt
_jscon_utils_decode_boolean(struct jscon_utils_s *utils)
{
  if ('t' == *utils->buffer){
    utils->buffer += 4; //skips length of "true"
    return true;
  }
  utils->buffer += 5; //skips length of "false"
  return false;
}

static void
_jscon_value_set_boolean(jscon_item_st *item, struct jscon_utils_s *utils)
{
  item->type = JSCON_BOOLEAN;
  item->boolean = _jscon_utils_decode_boolean(utils);
}

static void
_jscon_utils_decode_null(struct jscon_utils_s *utils){
  utils->buffer += 4; //skips length of "null"
}

static void
_jscon_value_set_null(jscon_item_st *item, struct jscon_utils_s *utils)
{
  item->type = JSCON_NULL;
  _jscon_utils_decode_null(utils);
}

static jscon_composite_st*
_jscon_utils_decode_composite(struct jscon_utils_s *utils, size_t n_branch){
  jscon_composite_st *new_comp = calloc(1, sizeof *new_comp);
  assert(NULL != new_comp);

  new_comp->htwrap = Jscon_htwrap_init();

  new_comp->branch = malloc((1+n_branch) * sizeof(jscon_item_st*));
  assert(NULL != new_comp->branch);

  ++utils->buffer; //skips composite's '{' or '[' delim

  return new_comp;
}

inline static size_t
_jscon_count_property(char *buffer)
{
  /* skips the item and all of its nests, special care is taken for any
      inner string is found, as it might contain a delim character that
      if not treated as a string will incorrectly trigger 
      depth action*/
  size_t depth = 0;
  size_t num_branch = 0;
  do {
    switch (*buffer){
    case ':':
        num_branch += (depth == 1);
        break;
    case '{':
        ++depth;
        break;
    case '}':
        --depth;
        break;
    case '\"':
        /* loops until null terminator or end of string are found */
        do {
          /* skips escaped characters */
          if ('\\' == *buffer++){
            ++buffer;
          }
        } while ('\0' != *buffer && '\"' != *buffer);
        assert('\"' == *buffer);
        break;
    }

    ++buffer; //skips whatever char

    if (0 == depth) return num_branch; //entire item has been skipped, return

  } while ('\0' != *buffer);

  fprintf(stderr, "\n\nERROR: bad formatting\n");
  exit(EXIT_FAILURE);
}

static void
_jscon_value_set_object(jscon_item_st *item, struct jscon_utils_s *utils)
{
  item->type = JSCON_OBJECT;

  item->comp = _jscon_utils_decode_composite(utils, _jscon_count_property(utils->buffer));
  Jscon_htwrap_link_r(item, &utils->last_accessed_htwrap);
}

inline static size_t
_jscon_count_element(char *buffer)
{
  /* skips the item and all of its nests, special care is taken for any
      inner string is found, as it might contain a delim character that
      if not treated as a string will incorrectly trigger 
      depth action*/
  size_t depth = 0;
  size_t num_branch = 0;
  do {
    switch (*buffer){
    case ',':
        num_branch += (depth == 1);
        break;
    case '[':
        ++depth;
        break;
    case ']':
        --depth;
        break;
    case '\"':
        /* loops until null terminator or end of string are found */
        do {
          /* skips escaped characters */
          if ('\\' == *buffer++){
            ++buffer;
          }
        } while ('\0' != *buffer && '\"' != *buffer);
        assert('\"' == *buffer);
        break;
    }

    ++buffer; //skips whatever char

    if (0 == depth) return num_branch; //entire item has been skipped, return

  } while ('\0' != *buffer);

  fprintf(stderr, "\n\nERROR: bad formatting\n");
  exit(EXIT_FAILURE);
}

static void
_jscon_value_set_array(jscon_item_st *item, struct jscon_utils_s *utils)
{
  item->type = JSCON_ARRAY;

  item->comp = _jscon_utils_decode_composite(utils, _jscon_count_element(utils->buffer));
  Jscon_htwrap_link_r(item, &utils->last_accessed_htwrap);
}

/* create nested composite type (object/array) and return 
    the address. */
static jscon_item_st*
_jscon_new_composite(jscon_item_st *item, struct jscon_utils_s *utils, jscon_create_value_ft *value_setter)
{
  item = _jscon_new_branch(item);
  item->key = utils->key;
  utils->key = NULL;

  (*value_setter)(item, utils);
  item = (utils->parser_cb)(item);

  return item;
}

/* wrap array or object type jscon, which means
    all of its branches have been created */
static jscon_item_st*
_jscon_wrap_composite(jscon_item_st *item, struct jscon_utils_s *utils)
{
  ++utils->buffer; //skips '}' or ']'
  Jscon_htwrap_build(item);
  return jscon_get_parent(item);
}

/* create a primitive data type branch. */
static jscon_item_st*
_jscon_append_primitive(jscon_item_st *item, struct jscon_utils_s *utils, jscon_create_value_ft *value_setter)
{
  item = _jscon_new_branch(item);
  item->key = utils->key;
  utils->key = NULL;

  (*value_setter)(item, utils);
  item = (utils->parser_cb)(item);

  return jscon_get_parent(item);
}

/* this routine is called when setting a branch of a composite type
    (object and array) item. */
static jscon_item_st*
_jscon_build_branch(jscon_item_st *item, struct jscon_utils_s *utils)
{
  jscon_create_item_ft *item_setter;
  jscon_create_value_ft *value_setter;

  switch (*utils->buffer){
  case '{':/*OBJECT DETECTED*/
      //DEV fprintf(stderr, "NEW OBJECT\n");
      item_setter = &_jscon_new_composite;
      value_setter = &_jscon_value_set_object;
      break;
  case '[':/*ARRAY DETECTED*/
      //DEV fprintf(stderr, "NEW ARRAY\n");
      item_setter = &_jscon_new_composite;
      value_setter = &_jscon_value_set_array;
      break;
  case '\"':/*STRING DETECTED*/
      //DEV fprintf(stderr, "NEW STRING\n");
      item_setter = &_jscon_append_primitive;
      value_setter = &_jscon_value_set_string;
      break;
  case 't':/*CHECK FOR*/
  case 'f':/* BOOLEAN */
      if (!STRNEQ(utils->buffer,"true",4) && !STRNEQ(utils->buffer,"false",5)){
        goto error;
      }
      //DEV fprintf(stderr, "NEW BOOL\n");
      item_setter = &_jscon_append_primitive;
      value_setter = &_jscon_value_set_boolean;
      break;
  case 'n':/*CHECK FOR NULL*/
      if (!STRNEQ(utils->buffer,"null",4)){
        goto error; 
      }
      //DEV fprintf(stderr, "NEW NULL\n");
      item_setter = &_jscon_append_primitive;
      value_setter = &_jscon_value_set_null;
      break;
  default:
      /*CHECK FOR NUMBER*/
      if (!isdigit(*utils->buffer) && ('-' != *utils->buffer)){
        goto error;
      }
      //DEV fprintf(stderr, "NEW NUMBER\n");
      item_setter = &_jscon_append_primitive;
      value_setter = &_jscon_value_set_number;
      break;
  }

  return (*item_setter)(item, utils, value_setter);


  error:
    fprintf(stderr,"ERROR: invalid json token %c\n", *utils->buffer);
    exit(EXIT_FAILURE);
}

/* this will be active if the current item is of array type jscon,
  whatever item is created here will be this array's property.
  if a ']' token is found then the array is wrapped up */
static jscon_item_st*
_jscon_build_array(jscon_item_st *item, struct jscon_utils_s *utils)
{
  CONSUME_BLANK_CHARS(utils->buffer);
  //DEV fprintf(stderr, "arr{%s}: \'%c\' ", item->key, *utils->buffer);
  switch (*utils->buffer){
  case ']':/*ARRAY WRAPPER DETECTED*/
      //DEV fprintf(stderr, "WRAP {%s}\n", item->key);
      return _jscon_wrap_composite(item, utils);
  case ',': /*NEXT ELEMENT TOKEN*/
      //DEV fprintf(stderr, "NEXT ELEMENT\n");
      ++utils->buffer; //skips ','
      CONSUME_BLANK_CHARS(utils->buffer);

      return item;
  default:
   {
      //creates numerical key for the array element
      char numerical_key[MAX_DIGITS];
      snprintf(numerical_key, MAX_DIGITS-1, "%ld", jscon_size(item));
      assert(NULL == utils->key);
      utils->key = strdup(numerical_key);

      //DEV fprintf(stderr, "ARRTOKEN: \'%c\' ", *utils->buffer);
      //DEV fprintf(stderr, "CREATE {%s} ", utils->key);

      return _jscon_build_branch(item, utils);
   }
  }

  //token error checking done inside _jscon_build_branch
}

/* this will be active if the current item is of object type jscon,
  whatever item is created here will be this object's property.
  if a '}' token is found then the object is wrapped up */
static jscon_item_st*
_jscon_build_object(jscon_item_st *item, struct jscon_utils_s *utils)
{
  CONSUME_BLANK_CHARS(utils->buffer);
  //DEV fprintf(stderr, "obj{%s}: \'%c\' ", item->key, *utils->buffer);
  switch (*utils->buffer){
  case '}':/*OBJECT WRAPPER DETECTED*/
      //DEV fprintf(stderr, "WRAP {%s}\n", item->key);
      return _jscon_wrap_composite(item, utils);
  case '\"':/*KEY STRING DETECTED*/
      assert(NULL == utils->key);
      utils->key = _jscon_utils_decode_string(utils);
      assert(':' == *utils->buffer); //check for key's assign token 
      ++utils->buffer; //skips ':'
      CONSUME_BLANK_CHARS(utils->buffer);
      //DEV fprintf(stderr, "OBJTOKEN: \'%c\' ", *utils->buffer);
      //DEV fprintf(stderr, "CREATE {%s} ", utils->key);
      return _jscon_build_branch(item, utils);
  case ',': /*NEXT PROPERTY TOKEN*/
      //DEV fprintf(stderr, "NEXT PROPERTY\n");
      ++utils->buffer; //skips ','
      CONSUME_BLANK_CHARS(utils->buffer);

      return item;
  default:
      /*SKIPS IF CONTROL CHARACTER*/
      if (!(isspace(*utils->buffer) || iscntrl(*utils->buffer))){
        goto error;
      }
      return item;
  }


  error:
    fprintf(stderr,"ERROR: invalid json token %c\n", *utils->buffer);
    exit(EXIT_FAILURE);
}

/* this call will only be used once, at the first iteration,
  it also allows the creation of a jscon that's not part of an
  array or object. ex: jscon_item_parse("10") */
static jscon_item_st*
_jscon_build_entity(jscon_item_st *item, struct jscon_utils_s *utils)
{
  //DEV fprintf(stderr, "ent{%s}: \'%c\' ", item->key, *utils->buffer);
  switch (*utils->buffer){
  case '{':/*OBJECT DETECTED*/
      //DEV fprintf(stderr, "SET OBJECT\n");
      _jscon_value_set_object(item, utils);
      break;
  case '[':/*ARRAY DETECTED*/
      //DEV fprintf(stderr, "SET ARRAY\n");
      _jscon_value_set_array(item, utils);
      break;
  case '\"':/*STRING DETECTED*/
      //DEV fprintf(stderr, "SET STRING\n");
      _jscon_value_set_string(item, utils);
      break;
  case 't':/*CHECK FOR*/
  case 'f':/* BOOLEAN */
      if (!STRNEQ(utils->buffer,"true",4) && !STRNEQ(utils->buffer,"false",5)){
        goto error;
      }
      //DEV fprintf(stderr, "SET BOOL\n");
      _jscon_value_set_boolean(item, utils);
      break;
  case 'n':/*CHECK FOR NULL*/
      if (!STRNEQ(utils->buffer,"null",4)){
        goto error;
      }
      //DEV fprintf(stderr, "SET NULL\n");
      _jscon_value_set_null(item, utils);
      break;
  default:
      CONSUME_BLANK_CHARS(utils->buffer);
      /*CHECK FOR NUMBER*/
      if (!isdigit(*utils->buffer) && ('-' != *utils->buffer)){
        goto error;
      }
      //DEV fprintf(stderr, "SET NUMBER\n");
      _jscon_value_set_number(item, utils);
      break;
  }

  return item;


  error:
    fprintf(stderr,"ERROR: invalid json token %c\n", *utils->buffer);
    exit(EXIT_FAILURE);
}

static inline jscon_item_st*
_jscon_default_callback(jscon_item_st *item){
  return item;
}

jscon_callbacks_ft*
jscon_parser_callback(jscon_callbacks_ft *new_cb)
{
  jscon_callbacks_ft *parser_cb = &_jscon_default_callback;

  if (NULL != new_cb){
    parser_cb = new_cb;
  }

  return parser_cb;
}

/* parse contents from buffer into a jscon item object
  and return its root */
jscon_item_st*
jscon_parse(char *buffer)
{
  jscon_item_st *root = calloc(1, sizeof *root);
  assert(NULL != root);

  struct jscon_utils_s utils = {
    .buffer = buffer,
    .parser_cb = jscon_parser_callback(NULL),
  };
  
  //build while item and buffer aren't nulled
  jscon_item_st *item = root;
  while ((NULL != item) && ('\0' != *utils.buffer)){
    switch(item->type){
    case JSCON_OBJECT:
        item = _jscon_build_object(item, &utils);
        break;
    case JSCON_ARRAY:
        item = _jscon_build_array(item, &utils);
        break;
    case JSCON_UNDEFINED://this should be true only at the first call
        item = _jscon_build_entity(item, &utils);

        /* primitives can't have branches, skip the rest  */
        if (IS_PRIMITIVE(item)) return item;

        break;
    default: //nothing else to build, check buffer for potential error
        if (!(isspace(*utils.buffer) || iscntrl(*utils.buffer))){
          goto error;
        }
        ++utils.buffer; //moves if cntrl character found ('\n','\b',..)
        //DEV fprintf(stderr, "UNDEFINED \'%c\'\n", *utils.buffer);
        break;
    }
  }

  return root;


  error:
    fprintf(stderr,"ERROR: invalid json token %c\n",*utils.buffer);
    exit(EXIT_FAILURE);
}

inline static void
_jscon_scanf_skip_string(struct jscon_utils_s *utils)
{
  /* loops until null terminator or end of string are found */
  do {
    /* skips escaped characters */
    if ('\\' == *utils->buffer++){
      ++utils->buffer;
    }
  } while ('\0' != *utils->buffer && '\"' != *utils->buffer);
  assert('\"' == *utils->buffer);
  ++utils->buffer; //skip double quotes
}

inline static void
_jscon_scanf_skip_composite(int ldelim, int rdelim, struct jscon_utils_s *utils)
{
  /* skips the item and all of its nests, special care is taken for any
      inner string is found, as it might contain a delim character that
      if not treated as a string will incorrectly trigger 
      depth action*/
  size_t depth = 0;
  do {
    if (ldelim == *utils->buffer){
      ++depth;
      ++utils->buffer; //skips ldelim char
    } else if (rdelim == *utils->buffer) {
      --depth;
      ++utils->buffer; //skips rdelim char
    } else if ('\"' == *utils->buffer) { //treat string separetely
      _jscon_scanf_skip_string(utils);
    } else {
      ++utils->buffer; //skips whatever char
    }


    if (0 == depth) return; //entire item has been skipped, return

  } while ('\0' != *utils->buffer);
}

static void
_jscon_scanf_skip(struct jscon_utils_s *utils)
{
  switch (*utils->buffer){
  case '{':/*OBJECT DETECTED*/
      _jscon_scanf_skip_composite('{', '}', utils);
      return;
  case '[':/*ARRAY DETECTED*/
      _jscon_scanf_skip_composite('[', ']', utils);
      return;
  case '\"':/*STRING DETECTED*/
      _jscon_scanf_skip_string(utils);
      return;
  default:
      //consume characters while not end of string or not new key
      while ('\0' != *utils->buffer && ',' != *utils->buffer){
        ++utils->buffer;
      }
      return;
  }
}

static char*
_jscon_scanf_format_info(char *specifier, size_t *p_tmp)
{
  size_t *n_bytes;
  size_t discard; //throw values here if p_tmp is NULL
  if (NULL != p_tmp){
    n_bytes = p_tmp;
  } else {
    n_bytes = &discard;
  }

  if (STREQ(specifier, "js")){
    *n_bytes = sizeof(jscon_char_kt*);
    return "jscon_char_kt*";
  }
  if (STREQ(specifier, "jd")){
    *n_bytes = sizeof(jscon_integer_kt);
    return "jscon_integer_kt*";
  }
  if (STREQ(specifier, "jf")){
    *n_bytes = sizeof(jscon_double_kt);
    return "jscon_double_kt*";
  }
  if (STREQ(specifier, "jb")){
    *n_bytes = sizeof(jscon_boolean_kt);
    return "jscon_boolean_kt*";
  }
  if (STREQ(specifier, "ji")){
    /* this will never get validated at _jscon_scanf_apply(),
        but it doesn't matter, a JSCON_NULL item is created either way */
    *n_bytes = sizeof(jscon_item_st*);
    return "jscon_item_st**";
  }
  *n_bytes = 0;
  return "NaN";
}

static void
_jscon_scanf_apply(struct jscon_utils_s *utils, hashtable_entry_st *entry)
{
  char *specifier = &entry->key[strlen(entry->key)+1];

  /* if specifier is item, simply call jscon_parse at current buffer token */
  if (STREQ(specifier, "ji")){
    jscon_item_st **item = entry->value;
    *item = jscon_parse(utils->buffer);
    _jscon_scanf_skip(utils); //skip characters parsed by jscon_parse

    /* get key, but keep in mind that this item is an "entity". The key will
        be ignored when serializing with jscon_stringify(); */
    (*item)->key = strdup(entry->key);
    return;
  }

  /* specifier must be a primitive */
  char err_typeis[50];
  switch (*utils->buffer){
  case '\"':/*STRING DETECTED*/
   {
      if (!STREQ(specifier, "js")){
        strcpy(err_typeis, "jscon_char_kt* or jscon_item_st**");
        goto type_error;
      }
      
      jscon_char_kt *string = _jscon_utils_decode_string(utils);
      strcpy(entry->value, string);
      free(string);
      return;
   }
  case 't':/*CHECK FOR*/
  case 'f':/* BOOLEAN */
   {
      if (!STRNEQ(utils->buffer,"true",4) && !STRNEQ(utils->buffer,"false",5)){
        goto token_error;
      }
      if (!STREQ(specifier, "jb")){
        strcpy(err_typeis, "jscon_boolean_kt* or jscon_item_st**");
        goto type_error;
      }

      jscon_boolean_kt *boolean = entry->value;
      *boolean = _jscon_utils_decode_boolean(utils);
      return;
   }
  case 'n':/*CHECK FOR NULL*/
   {
      if (!STRNEQ(utils->buffer,"null",4)){
        goto token_error; 
      }

      _jscon_utils_decode_null(utils);

      /* null conversion */
      size_t n_bytes; //get amount of bytes that should be set to 0
      _jscon_scanf_format_info(specifier, &n_bytes);
      memset(entry->value, 0, n_bytes);
      return;
   }
  case '{':/*OBJECT DETECTED*/
  case '[':/*ARRAY DETECTED*/
      strcpy(err_typeis, "jscon_item_st**");
      goto type_error;
  default:
   { /*CHECK FOR NUMBER*/
      if (!isdigit(*utils->buffer) && ('-' != *utils->buffer)){
        goto token_error;
      }
      
      double tmp = _jscon_utils_decode_double(utils);
      if (DOUBLE_IS_INTEGER(tmp)){
        if (!STREQ(specifier, "jd")){
          strcpy(err_typeis, "jscon_integer_kt* or jscon_item_st**");
          goto type_error;
        }
        jscon_integer_kt *number_i = entry->value;
        *number_i = (jscon_integer_kt)tmp;
      } else {
        if (!STREQ(specifier, "jf")){
          strcpy(err_typeis, "jscon_double_kt* or jscon_item_st**");
          goto type_error;
        }
        jscon_double_kt *number_d = entry->value; 
        *number_d = tmp;
      }
      return;
   }
  }


  type_error:
    fprintf(stderr,"ERROR: expected specifier %s but specifier is %s\n",err_typeis, _jscon_scanf_format_info(specifier, NULL));
    exit(EXIT_FAILURE);

  token_error:
    fprintf(stderr,"ERROR: invalid json token %c\n",*utils->buffer);
    exit(EXIT_FAILURE);
}

/* count amount of keys and check for formatting errors */
static size_t
_jscon_scanf_format_analyze(char *format)
{
  /* count each "word" composed of allowed key characters */
  size_t num_keys = 0;
  char c;
  while (true) //run until end of string found
  {
    //consume any space or control characters sequence
    CONSUME_BLANK_CHARS(format);
    c = *format;

    /*1st STEP: check for key '#' prefix existence */
    if ('#' != c){
      fprintf(stderr, "\nERROR: missing key '#' prefix\n\n");
      exit(EXIT_FAILURE);
    }
    c = *++format;

    /* @todo check for escaped characters */
    /*2nd STEP: check if key matches a criteria for being a key */
    if (!ALLOWED_JSON_CHAR(c)){
      fprintf(stderr, "\nERROR: key char not allowed '%c'\n\n", c);
      exit(EXIT_FAILURE);
    }

    ++num_keys; //found key, increment num_keys
    do { //consume remaining key characters
      c = *++format;
    } while (ALLOWED_JSON_CHAR(c));

    /*3rd STEP: check if key's type is specified */
    if ('%' != c){
      fprintf(stderr, "\nERROR: missing type specifier\n\n");
      exit(EXIT_FAILURE);
    }

    /*@todo error check for available unknown characters */
    do { /* consume type formatting characters */
      c = *++format;
    } while (isalpha(c));

    if ('\0' == c) return num_keys; //return if found end of string

    ++format; //start next key
  }
}

/* this can be a little confusing, basically it stores the key and the
    type specifier characters in the same string, with a null
    terminator character inbetween, to make sure thata only the key is read
    yet the type can still be accessed by skipping the the first null terminator
    
    an approximate resulting format:
      input:    '#key%specifier\0'
      output:   'key\0specifier\0'   */
static char**
_jscon_scanf_format_decode(const size_t kNum_keys, char *format, hashtable_st *hashtable, va_list ap)
{
  assert('\0' != *format); //can't be empty string

  const size_t STR_LEN = 256;
  char str[STR_LEN];

  char **keys = malloc(kNum_keys * sizeof *keys);
  assert(NULL != keys);
  char *specifier;

  /* split individual keys from specifiers and set them
      to a hashtable */
  char c;
  size_t i = 0, j = 0; //str and keys index respectively
  while (true) //run until end of string found
  {
    CONSUME_BLANK_CHARS(format);
    c = *format;

    assert('#' == c); //make sure key prefix is there
    c = *++format; //skip '#'
    
    /* 1st STEP: build key specific characters */
    while ('%' != c){
      str[i] = c;
      ++i;
      assert(i <= STR_LEN);
      
      c = *++format;
    }
    str[i] = '\0'; //key is formed
    ++i;
    assert(i <= STR_LEN);

    /* 2nd STEP: key is formed, proceed to fetch the specifier */
    //skips '%' char and fetch successive specifier characters
    do { //isolate specifier characters
      c = *++format;
      str[i] = c;
      ++i;
      assert(i <= STR_LEN);
    } while(isalpha(c));

    str[i-1] = '\0';

    /* 3rd STEP: store '#key\0specifier\0' string and its variable
        address (given at ... parameter) in hashtable */
    keys[j] = malloc(i);
    assert(NULL != keys[j]);
    memcpy(keys[j], str, i); //get the string in its entirety

    specifier = &keys[j][strlen(keys[j])+1]; //get specifier string
    
    if ( STREQ("NaN", _jscon_scanf_format_info(specifier, NULL)) ){
      fprintf(stderr, "\n\nERROR: unknown type specifier %%%s\n", specifier);
      exit(EXIT_FAILURE);
    }
    hashtable_set(hashtable, keys[j], va_arg(ap, void*));

    if ('\0' == c) return keys; //end of string, return

    i = 0; //resets i to start next key from scratch
    ++format; //start of next key
    ++j; //next key index
    assert(j < kNum_keys);
  }
}

/* works like sscanf, will parse stuff only for the keys specified to the format string parameter. the variables assigned to ... must be in
the correct order, and type, as the requested keys.

  every key found that doesn't match any of the requested keys will be
  ignored along with all its contents. */
void
jscon_scanf(char *buffer, char *format, ...)
{
  assert(NULL != buffer);

  CONSUME_BLANK_CHARS(buffer);

  if ('{' != *buffer){
    fprintf(stderr, "\n\nERROR: json root item must be a JSONC_OBJECT\n");
    exit(EXIT_FAILURE);
  }

  struct jscon_utils_s utils = {
    .buffer = buffer,
  };

  va_list ap;
  va_start(ap, format);

  const size_t kNum_keys = _jscon_scanf_format_analyze(format);

  hashtable_st *hashtable = hashtable_init();
  hashtable_build(hashtable, kNum_keys);

  //return keys for freeing later
  char **keys = _jscon_scanf_format_decode(kNum_keys, format, hashtable, ap);

  while ('\0' != *utils.buffer)
  {
    if ('\"' == *utils.buffer){
      assert(NULL == utils.key);
      utils.key = _jscon_utils_decode_string(&utils);
      assert(':' == *utils.buffer);

      ++utils.buffer; //consume ':'
      CONSUME_BLANK_CHARS(utils.buffer);

      /* check wether key found is specified */
      hashtable_entry_st *entry = hashtable_get_entry(hashtable, utils.key);
      if (NULL != entry){
        _jscon_scanf_apply(&utils, entry);
      } else {
        _jscon_scanf_skip(&utils);
      }
      free(utils.key);
      utils.key = NULL;
    } else {
      ++utils.buffer;
    }
  }

  hashtable_destroy(hashtable);
  for (size_t i=0; i < kNum_keys; ++i){
    free(keys[i]);
  }
  free(keys);

  va_end(ap);
}
