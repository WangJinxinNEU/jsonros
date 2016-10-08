#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <json/json.h>
#include <boost/graph/graph_concepts.hpp>
using namespace std;
string int2str(int input)
{
	std::stringstream ss;
	std::string str;
	ss<<input;
	ss>>str;
	return str;
}

int main()
{
	 json_object *jobj__send = json_tokener_parse("{\"id\":\"1\",\"iface\":\"switch.detection\",\"body\":{\"reason\":\"FOUNG\",\"num\":\"1\"}}");
	 std::cout<<json_object_to_json_string ( jobj__send )<<std::endl;
	 int id=2;

	char client_id_str[1024];
	sprintf(client_id_str,"%d",id);
	
	 json_object *client_id=json_object_new_string(client_id_str);
	 json_object_object_add(jobj__send,"id",client_id);
	 std::cout<<json_object_to_json_string ( jobj__send )<<std::endl;
	 return 0;
}
