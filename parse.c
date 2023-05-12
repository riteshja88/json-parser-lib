#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct json_value_in_serialized_json_t {
	const unsigned char *json_value_start;
	unsigned int json_value_len;
} json_value_in_serialized_json_t;


/*
  string     => '"' characters '"'

  characters => ""
  characters => character characters

  character => '20' . 'FF' - '"' - '\'
            => '\' escape

  escape    => '"'
            => '\'
            => '/'
            => 'b'
            => 'f'
            => 'n'
            => 'r'
            => 't'
            => 'u' hex hex hex hex

   hex      => digit
            => 'A' . 'F'
            => 'a' . 'f'

  digit    => '0'
           => onenine

  onenine  => '1' . '9'
			
  
*/
#define IS_ONENINE(c) ((c) >= '1' && (c) <= '9')

#define IS_DIGIT(c) (('0' == (c)) ||			\
					 IS_ONENINE((c)))

#define IS_HEX(c) (IS_DIGIT((c))	||				\
				   ((c) >= 'a' && (c) <= 'f') ||	\
				   ((c) >= 'A' && (c) <= 'F'))

int parse_json_value_string(const unsigned char serialized_json[],
							json_value_in_serialized_json_t * const json_value_string)
{
	int bytes_parsed = 0;
	json_value_string->json_value_start = &serialized_json[bytes_parsed];
	if('\"' != serialized_json[bytes_parsed] /* open quote */) {
		return -bytes_parsed;
	}
	bytes_parsed++;

	while('\0' != serialized_json[bytes_parsed]) {
		if(serialized_json[bytes_parsed] < 0x20) { /* '00' . '1F' */
			return -bytes_parsed;
		}
		else if('\\' == serialized_json[bytes_parsed]) { /* '\' escape */
			bytes_parsed++;
			switch(serialized_json[bytes_parsed]) {
				case '\"':
				case '\\':
				case '/':
				case 'b':
				case 'f':
				case 'n':
				case 'r':
				case 't':
					bytes_parsed++;
					break;

				case 'u':
					bytes_parsed++;
					if(IS_HEX(serialized_json[bytes_parsed])) {
					}
					else {
						return -bytes_parsed;
					}
					bytes_parsed++;

					if(IS_HEX(serialized_json[bytes_parsed])) {
					}
					else {
						return -bytes_parsed;
					}
					bytes_parsed++;

					if(IS_HEX(serialized_json[bytes_parsed])) {
					}
					else {
						return -bytes_parsed;
					}
					bytes_parsed++;

					if(IS_HEX(serialized_json[bytes_parsed])) {
					}
					else {
						return -bytes_parsed;
					}
					bytes_parsed++;
					break;

				default:
					return -bytes_parsed;
			}
		}
		else if('\"' != serialized_json[bytes_parsed]) { /* '20' . 'FF' - '"' - '\' */
			bytes_parsed++;
		}
		else { /* '"' */
			break;
		}
	}
	
	if('\"' != serialized_json[bytes_parsed] /* close quote */) {
		return -bytes_parsed;
	}
	bytes_parsed++;

	json_value_string->json_value_len = (&serialized_json[bytes_parsed] /* json_string_end */ -
										 json_value_string->json_value_start);

	return bytes_parsed;
}


/*
  number    => integer fraction exponent

  integer    => digit
             => onenine digits
             => '-' digit
             => '-' onenine digits

  digits    => digit
            => digit digits

  digit     => '0'
            => onenine

  onenine   => '1' . '9'

  fraction  => ""
            => '.' digits

  exponent  => ""
            => 'E' sign digits
            => 'e' sign digits

  sign      => ''
            => '+'
            => '-'

*/
int parse_json_value_number(const unsigned char serialized_json[],
							json_value_in_serialized_json_t * const json_value_number)
{
	int bytes_parsed = 0;
	json_value_number->json_value_start = &serialized_json[bytes_parsed];

	/* integer */
	if('-' == serialized_json[bytes_parsed] /* '-' */) {
		bytes_parsed++;
	}
	
	if(IS_DIGIT(serialized_json[bytes_parsed]) &&
	   !IS_DIGIT(serialized_json[bytes_parsed + 1])) { /* digit */
		bytes_parsed++;
	}
	else if(IS_ONENINE(serialized_json[bytes_parsed]) &&
			IS_DIGIT(serialized_json[bytes_parsed + 1])) { /* onenine digits */
		bytes_parsed++;
		bytes_parsed++;
		while('\0' != serialized_json[bytes_parsed]) {
			if(IS_DIGIT(serialized_json[bytes_parsed])) {
				bytes_parsed++;
			}
			else {
				break;
			}
		}
	}
	else {
		return -bytes_parsed;
	}

	/* fraction */
	if('.' == serialized_json[bytes_parsed]) { /* '.' digits */
		bytes_parsed++;
		if(IS_DIGIT(serialized_json[bytes_parsed])) {
			bytes_parsed++;
		}
		else {
			return -bytes_parsed;
		}
		while('\0' != serialized_json[bytes_parsed]) {
			if(IS_DIGIT(serialized_json[bytes_parsed])) {
				bytes_parsed++;
			}
			else {
				break;
			}
		}
	}

	/* exponent */
	if('E' == serialized_json[bytes_parsed] ||
	   'e' == serialized_json[bytes_parsed]) {
		/*
		  'E' sign digits
		  'e' sign digits
		*/
		bytes_parsed++;

		/* sign */
		if('-' == serialized_json[bytes_parsed] ||
		   '+' == serialized_json[bytes_parsed]) {
			bytes_parsed++;
		}

		if(IS_DIGIT(serialized_json[bytes_parsed])) {
			bytes_parsed++;
		}
		else {
			return -bytes_parsed;
		}

		while('\0' != serialized_json[bytes_parsed]) {
			if(IS_DIGIT(serialized_json[bytes_parsed])) {
				bytes_parsed++;
			}
			else {
				break;
			}
		}
	}

	json_value_number->json_value_len = (&serialized_json[bytes_parsed] /* json_number_end */ -
										 json_value_number->json_value_start);
	return bytes_parsed;
}


/*
  "true"
*/
int parse_json_value_boolean_true(const unsigned char serialized_json[],
								  json_value_in_serialized_json_t * const json_value_boolean_true)
{
	int bytes_parsed = 0;
	json_value_boolean_true->json_value_start = &serialized_json[bytes_parsed];

	if('t' == serialized_json[bytes_parsed]) {
		bytes_parsed++;
	}
	else {
		return -bytes_parsed;
	}

	if('r' == serialized_json[bytes_parsed]) {
		bytes_parsed++;
	}
	else {
		return -bytes_parsed;
	}

	if('u' == serialized_json[bytes_parsed]) {
		bytes_parsed++;
	}
	else {
		return -bytes_parsed;
	}

	if('e' == serialized_json[bytes_parsed]) {
		bytes_parsed++;
	}
	else {
		return -bytes_parsed;
	}

	json_value_boolean_true->json_value_len = (&serialized_json[bytes_parsed] /* json_boolea_true_end */ -
											   json_value_boolean_true->json_value_start);
	return bytes_parsed;
}

/*
  "false"
*/
int parse_json_value_boolean_false(const unsigned char serialized_json[],
								   json_value_in_serialized_json_t * const json_value_boolean_false)
{
	int bytes_parsed = 0;
	json_value_boolean_false->json_value_start = &serialized_json[bytes_parsed];

	if('f' == serialized_json[bytes_parsed]) {
		bytes_parsed++;
	}
	else {
		return -bytes_parsed;
	}

	if('a' == serialized_json[bytes_parsed]) {
		bytes_parsed++;
	}
	else {
		return -bytes_parsed;
	}

	if('l' == serialized_json[bytes_parsed]) {
		bytes_parsed++;
	}
	else {
		return -bytes_parsed;
	}

	if('s' == serialized_json[bytes_parsed]) {
		bytes_parsed++;
	}
	else {
		return -bytes_parsed;
	}

	if('e' == serialized_json[bytes_parsed]) {
		bytes_parsed++;
	}
	else {
		return -bytes_parsed;
	}

	json_value_boolean_false->json_value_len = (&serialized_json[bytes_parsed] /* json_boolean_false_end */ -
												json_value_boolean_false->json_value_start);
	return bytes_parsed;
}



/*
  boolean    => "true"
             => "false"
*/
int parse_json_value_boolean(const unsigned char serialized_json[],
							 json_value_in_serialized_json_t * const json_value_boolean)
{
	int bytes_parsed = 0;

	if('t' == serialized_json[0]) {
		return parse_json_value_boolean_true(serialized_json,
											 json_value_boolean);
	}
	else if('f' == serialized_json[0]) {
		return parse_json_value_boolean_false(serialized_json,
											  json_value_boolean);
	}
	return bytes_parsed;
}

/*
  "null"
*/
int parse_json_value_null(const unsigned char serialized_json[],
						  json_value_in_serialized_json_t * const json_value_null)
{
	int bytes_parsed = 0;
	json_value_null->json_value_start = &serialized_json[bytes_parsed];

	if('n' == serialized_json[bytes_parsed]) {
		bytes_parsed++;
	}
	else {
		return -bytes_parsed;
	}

	if('u' == serialized_json[bytes_parsed]) {
		bytes_parsed++;
	}
	else {
		return -bytes_parsed;
	}

	if('l' == serialized_json[bytes_parsed]) {
		bytes_parsed++;
	}
	else {
		return -bytes_parsed;
	}

	if('l' == serialized_json[bytes_parsed]) {
		bytes_parsed++;
	}
	else {
		return -bytes_parsed;
	}

	json_value_null->json_value_len = (&serialized_json[bytes_parsed] /* json_null_end */ -
									   json_value_null->json_value_start);
	return bytes_parsed;
}


/*
  value  => object
         => array
		 => string
		 => number
		 => boolean
		 => null
*/
int parse_json_value_object(const unsigned char serialized_json[],
							json_value_in_serialized_json_t * const json_value_object,
							void * const trie,
							void * const obj);
int parse_json_value_array(const unsigned char serialized_json[],
						   json_value_in_serialized_json_t * const json_value_array);
int parse_json_value(const unsigned char serialized_json[],
					 json_value_in_serialized_json_t * const json_value,
					 void * const trie,
					 void * const obj)
{
	if('{' == serialized_json[0]) { //object
		return parse_json_value_object(serialized_json,
									   json_value,
									   trie,
									   obj);
	}
	else if('[' == serialized_json[0]) { //array
		return parse_json_value_array(serialized_json,
									   json_value);
	}
	else if('\"' == serialized_json[0]) { //string
		return parse_json_value_string(serialized_json,
									   json_value);
	}
	else if('-' == serialized_json[0] ||
			IS_DIGIT(serialized_json[0])) { //number
		return parse_json_value_number(serialized_json,
									   json_value);
	}
	else if('t' == serialized_json[0] /* true */ ||
			'f' == serialized_json[0] /* false */) { //boolean
		return parse_json_value_boolean(serialized_json,
										json_value);
	}
	else if('n' == serialized_json[0] /* null */) { //null
		return parse_json_value_null(serialized_json,
									 json_value);
	}
	return 0; /* parse failure */
}

/*
  ws  => ""
      => '0020' ws
	  => '000A' ws
	  => '000D' ws
	  => '0009' ws
*/

#define IS_WS(c) (0x20 == (c) ||				\
				  0x0a == (c) ||				\
				  0x0d == (c) ||				\
				  0x09 == (c))

int ignore_json_ws(const unsigned char serialized_json[],
				  json_value_in_serialized_json_t * const json_ws)
{
	int bytes_parsed = 0;
	json_ws->json_value_start = &serialized_json[bytes_parsed];
	while('\0' != serialized_json[bytes_parsed]) {
		if(IS_WS(serialized_json[bytes_parsed])) {
			bytes_parsed++;
		}
		else {
			break;
		}
	}
	json_ws->json_value_len = (&serialized_json[bytes_parsed] /* json_ws_end */ -
							   json_ws->json_value_start);
	return bytes_parsed;
}

/*
  element    => ws value ws
*/
int parse_json_element(const unsigned char serialized_json[],
					   json_value_in_serialized_json_t * const json_value,
					   void *trie,
					   void *obj)
{
	int bytes_parsed = 0;
	json_value_in_serialized_json_t json_ws = { NULL, 0};
	/* ws */
	ignore_json_ws(&serialized_json[bytes_parsed],
				   &json_ws);
	bytes_parsed += json_ws.json_value_len;

	int rc = 0;
	rc = parse_json_value(&serialized_json[bytes_parsed],
						  json_value,
						  trie,
						  obj);
	if(rc <= 0) {
		return -bytes_parsed + rc;
	}
	bytes_parsed += rc;

	/* ws */
	ignore_json_ws(&serialized_json[bytes_parsed],
				   &json_ws);
	bytes_parsed += json_ws.json_value_len;

	return bytes_parsed;
}



/*
  member  => ws string ws ':' element
*/
int parse_json_member(const unsigned char serialized_json[],
					  json_value_in_serialized_json_t * const json_string,
					  json_value_in_serialized_json_t * const json_value,
					  void *trie,
					  void *obj)
					  
{
	int bytes_parsed = 0;
	json_value_in_serialized_json_t json_ws = { NULL, 0};
	/* ws */
	ignore_json_ws(&serialized_json[bytes_parsed],
				   &json_ws);
	bytes_parsed += json_ws.json_value_len;

	/* string */
	int rc = 0;
	rc = parse_json_value_string(&serialized_json[bytes_parsed],
								 json_string);
	if(rc <= 0) {
		return -bytes_parsed + rc;
	}
	bytes_parsed += rc;

	/* ws */
	ignore_json_ws(&serialized_json[bytes_parsed],
				   &json_ws);
	bytes_parsed += json_ws.json_value_len;

	/* ':' */
	if(':' == serialized_json[bytes_parsed]) {
		bytes_parsed++;
	}
	else {
		return -bytes_parsed;
	}

	/* element */
	trie_t *result = NULL;
	void *parse_json_element_fn_pointer = NULL;
	if(0 == trie_search(trie,
						json_string->json_value_start,
						json_string->json_value_len,
						result,
						parse_json_element_fn_pointer)) {
		// found in trie
		rc = parse_json_element_fn_pointer(&serialized_json[bytes_parsed],
										   json_value,
										   trie /* NULL if not json_object */,
										   obj);
	}
	else {
		// validate and ignore json element
		rc = parse_json_element(&serialized_json[bytes_parsed],
								json_value,
								trie,
								obj);
	}
	if(rc <= 0) {
		return -bytes_parsed + rc;
	}
	bytes_parsed += rc;

	return bytes_parsed;
}

/*
  object     => '{' ws '}'
             => '{' members '}'  // implemented as '{' ws members '}'

  ws         => ""
             => '0020' ws
			 => '000A' ws
			 => '000D' ws
			 => '0009' ws

  members    => member
             => member ',' members

  member     => ws string ws ':' element

  element    => ws value ws
*/
int parse_json_value_object(const unsigned char serialized_json[],
							json_value_in_serialized_json_t * const json_value_object,
							void *trie,
							void *obj)
{
	int bytes_parsed = 0;
	json_value_object->json_value_start = &serialized_json[bytes_parsed];
	/* '{' */
	if('{' != serialized_json[bytes_parsed] /* open curly bracket */) {
		return -bytes_parsed;
	}
	bytes_parsed++;


	json_value_in_serialized_json_t json_ws = { NULL, 0};
	/* ws */
	ignore_json_ws(&serialized_json[bytes_parsed],
				  &json_ws);
	bytes_parsed += json_ws.json_value_len;

	/* } */
	if('}' == serialized_json[bytes_parsed]) {
		bytes_parsed++;
		json_value_object->json_value_len = (&serialized_json[bytes_parsed] /* json_object_end */ -
											 json_value_object->json_value_start);
		return bytes_parsed; /* '{' ws '}' */
	}

	/* members */
	while('\0' != serialized_json[bytes_parsed]) {
		/* member */
		json_value_in_serialized_json_t json_string = { NULL, 0};
		json_value_in_serialized_json_t json_value = { NULL, 0};
		int rc = 0;
		rc = parse_json_member(&serialized_json[bytes_parsed],
							   &json_string,
							   &json_value,
							   trie,
							   obj);
		if(rc <= 0) {
			return -bytes_parsed + rc;
		}

		bytes_parsed += rc;

	
		if(',' == serialized_json[bytes_parsed] /* ',' */) {
			bytes_parsed++;
			continue;
		}
		else {
			break;
		}
	}
	
	if('}' != serialized_json[bytes_parsed] /* close curly bracket */) {
		return -bytes_parsed;
	}
	bytes_parsed++;

	json_value_object->json_value_len = (&serialized_json[bytes_parsed] /* json_object_end */ -
										 json_value_object->json_value_start);

	return bytes_parsed;
}

/*
  array   =>   '[' ws ']'
          =>   '[' elements ']'


  elements =>  element
               element, elements

*/
int parse_json_value_array(const unsigned char serialized_json[],
						   json_value_in_serialized_json_t * const json_value_array)
{
	int bytes_parsed = 0;
	json_value_array->json_value_start = &serialized_json[bytes_parsed];
	/* '[' */
	if('[' != serialized_json[bytes_parsed] /* open square bracket */) {
		return -bytes_parsed;
	}
	bytes_parsed++;


	json_value_in_serialized_json_t json_ws = { NULL, 0};
	/* ws */
	ignore_json_ws(&serialized_json[bytes_parsed],
				  &json_ws);
	bytes_parsed += json_ws.json_value_len;

	/* ] */
	if(']' == serialized_json[bytes_parsed]) {
		bytes_parsed++;
		json_value_array->json_value_len = (&serialized_json[bytes_parsed] /* json_array_end */ -
											json_value_array->json_value_start);
		return bytes_parsed; /* '[' ws ']' */
	}

	/* elements */
	while('\0' != serialized_json[bytes_parsed]) {
		/* element */
		json_value_in_serialized_json_t json_value = { NULL, 0};
		int rc = 0;
		rc = parse_json_element(&serialized_json[bytes_parsed],
								&json_value,
								NULL,
								NULL);
		if(rc <= 0) {
			return -bytes_parsed + rc;
		}

		bytes_parsed += rc;
	
		if(',' == serialized_json[bytes_parsed] /* ',' */) {
			bytes_parsed++;
			continue;
		}
		else {
			break;
		}
	}
	
	if(']' != serialized_json[bytes_parsed] /* close square bracket */) {
		return -bytes_parsed;
	}
	bytes_parsed++;

	json_value_array->json_value_len = (&serialized_json[bytes_parsed] /* json_array_end */ -
										json_value_array->json_value_start);

	return bytes_parsed;
}



int parse_json_element_number(const unsigned char serialized_json[],
							  double * const json_element_number_out)
{
	*json_element_number_out = 0.0; //init
	json_value_in_serialized_json_t json_value = { NULL, 0};
	int rc = parse_json_element(serialized_json,
								&json_value,
								NULL,
								NULL);
	if(rc <= 0) {
		return rc;
	}

	if(IS_DIGIT(json_value.json_value_start[0]) ||
	   '-' == json_value.json_value_start[0]) { /* number type match */
		*json_element_number_out = atof((const char *)json_value.json_value_start);
	}

	return rc;
}


#define SUFFIX "jjjj"
int main()
{
	unsigned char serialized_json[][1024] =
		{
			"\"ritesh\"" SUFFIX,
			"\"ritesh"SUFFIX,
			"\"ritesh\x01\"" SUFFIX,
			"\"ritesh\x1f\"" SUFFIX,
			"\"ritesh\x20\"" SUFFIX,
			"\"ritesh\xff\"" SUFFIX,
			"\"ritesh\\\"\\\\/\\/\\b\\f\\n\\r\\t/\\u0189agarwal\"" SUFFIX,
			"\"ritesh\\a\"" SUFFIX,
			"\"ritesh\\u0\"" SUFFIX,
			"\"ritesh\\u00\"" SUFFIX,
			"\"ritesh\\u000\"" SUFFIX,
			"\"ritesh\\u0000\"" SUFFIX,
			"\"ritesh\\u000a\"" SUFFIX,
			"\"ritesh\\u000f\"" SUFFIX,
			"\"ritesh\\u000A\"" SUFFIX,
			"\"ritesh\\u000F\"" SUFFIX,
			"\"ritesh\\u000g\"" SUFFIX,
			"\"ritesh\\u000G\"" SUFFIX,
			"\"ritesh\\u000`\"" SUFFIX,
			"\"ritesh\\u000@\"" SUFFIX,
			"-------------------",

			"0" SUFFIX,
			"-0" SUFFIX,
			"1" SUFFIX,
			"-1" SUFFIX,
			"9" SUFFIX,
			"-9" SUFFIX,
			"/" SUFFIX,
			":" SUFFIX,
			"01" SUFFIX,
			"-01" SUFFIX,
			"11" SUFFIX,
			"-11" SUFFIX,
			"91" SUFFIX,
			"-91" SUFFIX,
			"11234567890" SUFFIX,
			"-11234567890" SUFFIX,
			"91234567890" SUFFIX,
			"-91234567890" SUFFIX,
			"1234567890.1" SUFFIX,
			"1234567890.a" SUFFIX,
			"1234567890.0123456789" SUFFIX,
			"1234567890E1" SUFFIX,
			"1234567890E+1" SUFFIX,
			"1234567890E-1" SUFFIX,
			"1234567890e1" SUFFIX,
			"1234567890e+1" SUFFIX,
			"1234567890e-1" SUFFIX,
			"1234567890E" SUFFIX,
			"1234567890e" SUFFIX,
			"1234567890E-" SUFFIX,
			"1234567890e-" SUFFIX,
			"1234567890E+" SUFFIX,
			"1234567890e+" SUFFIX,
			"1234567890E1234567890" SUFFIX,
			"1234567890.0123456789E1234567890" SUFFIX,
			"-------------------",
			
			"true" SUFFIX,
			"truE" SUFFIX,
			"false" SUFFIX,
			"falsE" SUFFIX,
			"-------------------",

			"null" SUFFIX,
			"nulL" SUFFIX,
			"-------------------",

			"zzz" SUFFIX,
			"-------------------",

			"{}" SUFFIX,
			"{ }" SUFFIX,
			"{\t}" SUFFIX,
			"{\r}" SUFFIX,
			"{\n}" SUFFIX,
			"{ \t }" SUFFIX,
			"-------------------",

			"{\"x\":2}" SUFFIX,
			"{ \"x\":2}" SUFFIX,
			"{\"x\" :2}" SUFFIX,
			"{\"x\": 2}" SUFFIX,
			"{\"x\":2 }" SUFFIX,
			"{\"x\":2,\"y\":3}" SUFFIX,
			"{\"x\":2 ,\"y\":3}" SUFFIX,
			"{\"x\":2 , \"y\":3}" SUFFIX,
			"{\"x\":2,\"y\" :3}" SUFFIX,
			"{\"x\":2,\"y\": 3}" SUFFIX,
			"{\"x\":2,\"y\":3 }" SUFFIX,
			"{" SUFFIX,
			"{ " SUFFIX,
			"{ \"string\"" SUFFIX,
			"{ \"string\":" SUFFIX,
			"{ \"string\":2" SUFFIX,
			"{ \"string\":2}" SUFFIX,
			"{ \"string\":2,}" SUFFIX,
			"{ \"string\":2,  \"string\"}" SUFFIX,
			"{ \"string\":2,  \"string\":}" SUFFIX,
			"{ \"string\":2,  \"string\":2}" SUFFIX,
			"{\"x\":2, \"obj_in\":{\"y\":3}}" SUFFIX,
			"{\"x\":2, \"obj_in\":{\"obj_in_in\":{\"x1\":2},\"y\":3}}" SUFFIX,
			"-------------------",

			"             2            "SUFFIX
			"-------------------",
			"[]" SUFFIX,
			"{}" SUFFIX,
			"[ ]" SUFFIX,
			"{ }" SUFFIX,
			"[1]" SUFFIX,
			"[ 1]" SUFFIX,
			"[1 ]" SUFFIX,
			"[ 1 ]" SUFFIX,
			"[1,2]" SUFFIX,
			"[1 ,2]" SUFFIX,
			"[1, 2]" SUFFIX,
			"[1,2 ]" SUFFIX,
			" [1,2] " SUFFIX,
			"-------------------",
		};

	for(unsigned int i = 0;i<(sizeof(serialized_json) / sizeof(char[1024]));i++) {
		json_value_in_serialized_json_t json_value;
		int rc = parse_json_element(serialized_json[i],
									&json_value,
									NULL,
									NULL);
		unsigned char *na =  (unsigned char*)"*N/A*";
		printf("%50s | %50.*s | %5d\n",
			   serialized_json[i],
			   ((rc>0)?(json_value.json_value_len):(5)),
			   ((rc>0)?(json_value.json_value_start):(na)),
			   rc);
			   
		
	}

	double json_element_number = 0;
	int rc = parse_json_element_number((const unsigned char *)" -3.1e2 ",
									   &json_element_number);
	if(rc > 0) {
		printf("json_element_number = %lf (%ld)\n",
			   json_element_number,
			   (long) json_element_number);
	}
	else {
		printf("json_element_number = parse fail\n");
	}

	return 0;
}
