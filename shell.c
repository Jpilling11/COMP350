//Jon Pilling's OPSys Shell
	//DECLARATIONS
void printf(char*);
void readLine(char*);
void inputSplit(char*, char*, char*);
int strComp(char*, char*);

	//START
void main() {
	char buff[180];
	char firstWord[7];
	char secondWord[7];
	char thirdWord[7];
	char fileBuff[13312];
	int* sectorsRead;
	int i = 0;
	int j = 0;
	int k = 0;
	int y = 0;
	int x = 0;

	for (x = 0; x < 180; x++) {
		buff[x] = 0x0;
	}

	while (1) {
		printf("Shell > ");
		readLine(buff);
		inputSplit(buff, &firstWord, &secondWord, &thirdWord);

		//INPUT CHECK
		if (strComp(firstWord, "type\0") == 1) {
			*sectorsRead = 0;
			syscall(3, secondWord, &sectorsRead);
			if (sectorsRead > 0) {
				printf(fileBuff);
				printf("\r\n");
				break;
			} else {
				printf("File not found.\n\r");
				break;
			}
		} else if (strComp(firstWord, "exec\0") == 1) {
			syscall(4, secondWord);
		} else if (strComp(firstWord, "quit\0") == 1) {
			syscall(5);
		} else if (strComp(firstWord, "del\0") == 1) {
            syscall(7, secondWord);
		} else if (strComp(firstWord, "copy\0") == 1) {
			syscall(3, secondWord, fileBuff, &sectorsRead);

			i = 0;
			while(fileBuff[i] != 0x0) {
				i++;
			}

			k = 0;
			while(k * 512 <= i) {
				k++;
			}
			syscall(8, fileBuff, thirdWord, sectorsRead);
		} else if (strComp(firstWord, "dir\0") == 1) {
			syscall(0,"\r\nDirectory\r\n");
            syscall(9);
			syscall(0,"\r");
		} else if (strComp(firstWord, "create\0") ==1) {
			
		} else {
			printf("Bad Command!\r\n");
			printf(firstWord);
			printf("\r\n");
			printf(secondWord);
			printf("\r\n");
		}
	}
}

void inputSplit(char* buff, char* firstWord, char* secondWord, char* thirdWord) {
	//Still only works if word is 6 or less letters
	int x = 0;
	int buffOffset = 0;
	
	//sanitize arrays
	for (x = 0; x < 7; x++) {
		firstWord[x] = '\0';
		secondWord[x] = '\0';
		thirdWord[x] = '\0';
	}
	x=0;

	//Get First Word
	while (buff[x] != ' ' && buff[x] != 0x0 && buff[x] != '\0' && buff[x] != 0x13 && buff[x] != 0xa) {
		//truncate
		if (x < 6) {
			firstWord[x] = buff[x];
			//printf(firstWord);
		}	
		x++;
	}
	x++; //continue past the first space
	buffOffset = x; //the difference between buffer and 0
	
	//Get Second Word
	while (buff[x] != ' ' && buff[x] != 0x0 && buff[x] != '\0' && buff[x] != 0x13 && buff[x] != 0xa) {
		//truncate
		if (x < 6 + buffOffset)
			secondWord[x-buffOffset] = buff[x];
		x++;
	}
	x++; //continue past the first space
	buffOffset = x; //the difference between buffer and 0
	
	//Get Third Word
	while (buff[x] != ' ' && buff[x] != 0x0 && buff[x] != '\0' && buff[x] != 0x13 && buff[x] != 0xa) {
		//truncate
		if (x < 6 + buffOffset)
			thirdWord[x-buffOffset] = buff[x];
		x++;
	}
}

void printf(char* buff){
	syscall(0, buff, 0, 0);
}

void readLine(char* buff) {
	syscall(1, buff, 0, 0);
}

int strComp(char* strOne, char* strTwo) {
	int x = 0;
	while (strOne[x] != '\0' ) {
		if (strOne[x] != strTwo[x])
			return 0;
		x++;
	}
	if (strTwo[x] != '\0') {
		return 0;
	}
	return 1;
}


