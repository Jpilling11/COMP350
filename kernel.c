//Jon Pilling's OPSys Kernal
	//DECLARATIONS
void executeProgram(char*);
void deleteFile(char*);
void getDirectory();
void handleInterrupt21(int, int, int, int);
void handleTimerInterrupt(int, int);
void printChar(char); 		
void printString(char*);
void readFile(char*,char*,int*);
void readSector(char*, int);	
void readString(char*); 	
void terminate(char* char* int);
void writeFile(char* char* int);
void writeSector(char* int);

	//START
void main() {
	makeInterrupt21();
	interrupt(0x21,8,"this is a test message", "testmg",3);
	interrupt(0x21, 4, "shell\0", 0, 0);
	while(1);
}

void deleteFile(char* name) {
	char map[512];
	char dir[512];
	int *sectorsRead = 0;
	int e, j, k, x;

	readSector(map, 1);
	readSector(dir, 2);

	//move through sector
	for(e = 0; e < 512; e = e + 32){
		for (x = 0; x < 6; x++) {
			//check name
			if (name[x] != dir[e+x]) {
				//fail
				break;
			} else if (x == 5) { //if name matches
				dir[e] = 0x0;
				//set info in block to 0
				for(j = 6; j < 32; j++) {
					if(dir[e+j] != 0) {
						k=dir[e+j];
						map[k]=0;
					} else if (dir[e+j] == 0 && dir [e+j+1] == 0) {
						break;
					}
				}
				//write blank info to sector then free space in the map
				writeSector(dir,2);
				writeSector(map,1);
				break;

			}
		}
	}
}


void executeProgram(char* name) {
	int i;
	char buffer[13312];
	int segment = 0x2000;
	int sectorsRead = 0;	
	
	readFile(name, buffer, &sectorsRead);
	
	//if successfully wrote to sector
	if (sectorsRead>0) {
		for(i = 0; i < 13312; i++) {
			//load program into memory
			putInMemory(segment, i, buffer[i]);
		}
	} else {
		printString("No File Found");
	}	
	//run program from memory
	launchProgram(segment);
}

void getDirectory(){
	char buff[512];
	char dirBuff[512];
	int i,j;
	int index = 0;
	
	//sanitize arrays
	for(i=0;i<512;i++) {
		buff[i] = 0x0;
		dirBuff[i] = 0x0;
	}

	//Read dir
    readSector(dirBuff, 2); 

	//Look through the directory for files
	for(i=0;i<16;i++){
		 //check for file
		if (dirBuff[32*i] != 0x0){
			//check if string matches
			for (j=0; j < 6; j++){
				buff[index] = dirBuff[j+32*i];
				index++;
			}
			//line break
			buff[index] = '\r';
			index++;
			buff[index] = '\n';
			index++;
		}
	}
	for(i=0;i<512;i++) {
		printChar(buff[i]);
	}
	return;
}

void handleInterrupt21(int ax, int bx, int cx, int dx) {
	if (ax == 0) {
		printString(bx);
	} else if (ax == 1) {
		readString(bx);
	} else if (ax == 2) {
		readSector(bx, cx);
	} else if (ax == 3) {
		readFile(bx, cx, dx);
	} else if (ax == 4) {
		executeProgram(bx);
	} else if (ax == 5) {
		terminate();
	} else if (ax == 6) {
		writeSector(bx, cx);
	} else if (ax == 7) {
		deleteFile(bx);
	} else if (ax == 8) {
		writeFile(bx, cx, dx);
	} else if (ax == 9) {
		getDirectory();
	} else {
		printString("Error - Bad Interrupt");
	}
}

void handleTimerInterrupt(int segment, int sp) {
	printChar('T');
	printChar('i');
	printChar('c');
	returnFromTimer(segment,sp);	
}

void printChar(char c){
	char* string[2];
	string[0] = c;
	string[1] = '\0';
	printString(string);
}

void printString(char* string){
	int i = 0;
	while(string[i] != '\0'){
		int c = string[i];
		interrupt(0x10, 0xe*256+c, 0, 0, 0);
		i++;
	}
}

void readFile(char* name, char* buffer, int* sectorsRead) {
	int e = 0;
	int j = 0;
	int x = 0;
	char dir[512];
	*sectorsRead = 0;

	interrupt(0x21, 2, dir, 2, 0);
	//move through sector
	for(e = 0; e < 512; e = e + 32) {
		//check name
		for (x = 0; x < 6; x++) {
			if (name[x] != dir[e+x]) {
				//fail
				break;
			} else if (x == 5) { //if name matches
				for(j = e + 6; dir[j] != 0; j++){
					//increment sectors read
					*sectorsRead = *sectorsRead + 1;
					//read the sector into the buffer
					interrupt(0x21, 2, buffer, dir[j], 0);
					//increase the buffer for the next loop
					buffer = buffer + 512;
				}
				return;
			}
		}
	}
	*sectorsRead = 0;
}

void readSector(char* buffer, int sector) {
	interrupt(0x13, 2*256+1, buffer, 0*256+sector+1,0*256+0x80);
}

void readString(char* letters) {
	int x = 0;
	while (letters[x] != 0xd) {
		letters[x] = interrupt(0x16, 0, 0, 0, 0);
		//backspace handling
		if (letters[x] == 0x8 && x > 0) {
			printChar(0x8);
			printChar(' ');
			printChar(0x8);
			x--;
		//enter handling
		} else if (letters[x] == 0xd) {
			printChar(0xd);
			printChar(0xa);
			letters[x]=0xd;
			x++;
			letters[x]=0xa;
			x++;
			letters[x]=0x0;
			return;
		//write the char they typed
		} else {
			printChar(letters[x]);
			x++;
		}
	}
}

void terminate(){
	char shell[6];
	shell[0] = 's';
	shell[1] = 'h';
	shell[2] = 'e';
	shell[3] = 'l';
	shell[4] = 'l';
	shell[5] = '\0';
	interrupt(0x21,4,shell,0,0);
}

void writeFile(char* buffer,char* file,int sectors){

	char map[512];
	char dir[512];
	int i, j, k, address,sector;

	readSector(map,1);
	readSector(dir,2);
	//loop through dir
	for(i=0;i<512;i+=32){
		//look for empty space
		if(dir[i]=='\0'){
			//Write name to space
			for(j=0;j<6;j++){
				if(file[j]=='\0'){
					for(k=0;k<(6-j);k++){
						dir[i+j+k]='\0';
					}
					break;
				}else{
					dir[i+j]=file[j];
				}
			}

		}
	}
}

void writeSector(char* buffer, int sector) {
	interrupt(0x13, 3*256+1, buffer, 0*256+sector+1,0*256+0x80);
}

