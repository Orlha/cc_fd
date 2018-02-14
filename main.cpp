#include <iostream>
#include <fstream>
#include <string>

#include "inc/entity.h"
#include "inc/decoder.h"
#include "inc/reader.h"

using namespace std;

int main(int argc, char** argv) {
	
	if (argc != 3)
	{
		cout << "Arguments expected:" << endl;
		cout << "#inputfile #outputfile" << endl;
		return 10;
	}
	string filename_source = argv[1];
	string filename_target = argv[2];

	int ret_code;
	Reader reader;
	
	ret_code = reader.initFile(filename_source);
	if (ret_code != 0) {
		return ret_code;
	}
			
	ret_code = reader.readHeader();
	if (ret_code != 0) {
		return ret_code;
	}
	
	ret_code = reader.readEntCounter();
	if (ret_code != 0) {
		return ret_code;
	}

	ret_code = reader.readEntities();
	if (ret_code != 0) {
		return ret_code;
	}
	
	ret_code = reader.initDecoder();
	if (ret_code != 0) {
		return ret_code;
	}
	
	ret_code = reader.decompile();
	if (ret_code != 0) {
		//printf("FATAL: Something terrible has happened;\n");
	}
	return ret_code;
}
