//Jon Pilling's OPSys Kernal

void printString(char* letters);
void printChar(char c);
void readSector(char* buffer, int sector);
void readString(char* letters);
void handleInterrupt21(int ax, int bx, int cx, int dx);

void main() {
/*	char line[80];
	printString("Enter a line: ");
	readString(line);
	printString(line);
*/
/*
	char buffer[512];
	readSector(buffer, 30);
	printString(buffer);
*/
/*
	makeInterrupt21();
	interrupt(0x21,0,0,0,0);
*/
	char line[80];
	makeInterrupt21();
	interrupt(0x21,1,line,0,0);
	interrupt(0x21,0,line,0,0);
	while(1);
}

void printChar(char c){
	interrupt(0x10, 0xe*256+c, 0, 0, 0);
}

void printString(char* letters) {
	while (*letters != 0x0) {
		printChar(*letters);
		letters++;
        }
}

void readString(char* letters) {
	int x = 0;
	while (letters[x] != 0xd) {
		letters[x] = interrupt(0x16, 0, 0, 0, 0);
		if (letters[x] == 0x8 && x>0) {
			x--;
			letters[x] = 0x20;
			printChar(0x8);
		} else if (letters[x] == 0xd) {
			letters[x] = 0xa;
			x++;
			letters[x] = 0x0;
			printChar(0xd);
			printChar(0xa);
        		x++;
		        letters[x] = 0x0;
			return;
		} else {
			printChar(letters[x]);
			x++;
		}
	}
}

void readSector(char* buffer, int sector) {
	interrupt(0x13, 2*256+1, buffer, 0*256+sector+1,0*256+0x80);
}

void handleInterrupt21(int ax, int bx, int cx, int dx) {
	//printString("Hello World");
	if (ax = 0) {
		printString(&bx);
	} else if (ax = 1) {
		readString(&bx);
	} else if (ax = 2) {
		readSector(&bx, cx);
	} else {
		printString("Error");
	}
}
