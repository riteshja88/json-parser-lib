#include <stdio.h>
#include <string.h>

typedef struct struct1_t {
	char x[10 + 1];
} struct1_t;

#define MIN(a,b) ((a<b)?(a):(b))



typedef struct json_string_in_serialized_json_t {
	const char *json_string_start;
	unsigned int json_string_len;
} json_string_in_serialized_json_t;


int parse_json_string(const char serialized_json[],
					  json_string_in_serialized_json_t *json_string_value)
{
	int bytes_parsed = 0;
	if('\"' != serialized_json[bytes_parsed] /* open quote */) {
		return -bytes_parsed;
	}
	bytes_parsed++;

	json_string_value->json_string_start = &serialized_json[bytes_parsed];
	while('\0' != serialized_json[bytes_parsed] &&
		  '\"' != serialized_json[bytes_parsed] /* while close quote is not found  */) {
		bytes_parsed++;
	}
	json_string_value->json_string_len = (&serialized_json[bytes_parsed] /* json_string_end */ -
										json_string_value->json_string_start);

	if('\"' != serialized_json[bytes_parsed] /* close quote */) {
		return -bytes_parsed;
	}
	bytes_parsed++;

	

	return bytes_parsed;
}

int parse_json_object_struct1_t(const char serialized_json[],
								struct1_t * const struct1)
{
	int bytes_parsed = 0;

	if('{' != serialized_json[bytes_parsed]) {
		return -bytes_parsed;
	}
	bytes_parsed++;


	json_string_in_serialized_json_t json_field_name;
	int rc = 0;
	rc = parse_json_string(&serialized_json[bytes_parsed],
						   &json_field_name);
	if(rc <= 0) {
		return -bytes_parsed + rc;
	}
	bytes_parsed += rc;

	if(':' != serialized_json[bytes_parsed] /* : seperator */) {
		return -bytes_parsed;
	}
	bytes_parsed++;

	if(json_field_name.json_string_len > 0 &&
	   0 == strncmp("x",
					json_field_name.json_string_start,
					json_field_name.json_string_len)) {
		json_string_in_serialized_json_t json_field_value;
		int rc = 0;
		rc = parse_json_string(&serialized_json[bytes_parsed],
							   &json_field_value);
		if(rc <= 0) {
			return -bytes_parsed + rc;
		}
		bytes_parsed += rc;


		int len = MIN(10,
					  json_field_value.json_string_len);
		strncpy(struct1->x,
				json_field_value.json_string_start,
				len);
		struct1->x[len] = '\0';
	}
			   

	if('}' != serialized_json[bytes_parsed]) {
		return -bytes_parsed;
	}
	bytes_parsed++;

	
	return bytes_parsed;
}

int main()
{
	//char serialized_json[] = "{}";
	//char serialized_json[] = "{\"qwertyuiop\":}";
	//char serialized_json[] = "{\"x\":}";
	char serialized_json[] = "{\"x\":\"poonam\"}";
	//char serialized_json[] = "{\"x\":1}";
	//char serialized_json[] = "{\"y\":1}";
	
	struct1_t struct1 = { "vinay" };
	int rc = parse_json_object_struct1_t(serialized_json,
										 &struct1);
	printf("%s\n",serialized_json);
	if(rc <= 0) {
		printf("parse failed. bytes_parsed=%d\n",-rc);
	}
	else {
		printf("parse success. bytes_parsed=%d\n",rc);
		printf("x:%s\n",struct1.x);
	}
}

