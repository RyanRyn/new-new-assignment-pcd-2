#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include <stdbool.h>
#include<ctype.h>
#pragma warning (disable:4996)
#pragma warning (disable:4146)
#pragma warning(disable:6031)
#define coach_size 40

typedef struct {
	int day, month, year;
} date;
typedef struct {
	int hours, min;
} time_train;

typedef struct {
	char id[7], name[50], password[21], passwordRecovery[20], position[50];
	char gender;
} employee;

typedef struct {
	char id[6], name[50], password[21], passwordRecovery[20], ic[15], contactNumber[12];
	char gender;
}member;

typedef struct {
	char id[6], departureStation[31], arrivalStation[31];
	date departureDate, arrivalDate;
	time_train departureTime, arrivalTime;
	int coach;
	float price;
}trainScheduling;

typedef struct {
	char bookingId[6], memberId[6], trainId[6];
	char paymentInfo[20], ticketStatus[20];
	date bookingDate, departureDate;//dapartureData 可能不用只要有trainid就可以找回去 答案：又不是在做sql不能同时拿很多file 要也是可以 麻烦到要死
	int pax;
	float amount;
}ticketBooking;//还有ticketseat

void logintype(bool isAdmin);
bool userLogin(bool isAdmin, char loginId[7]);

void menu(bool isAdmin, char loggedId[7]);

employee findEmployeeData(char findId[7]);
member findMemberData(char findId[6]);
trainScheduling findTrainScheduleData(char findId[6]);
ticketBooking findTicketData(char findId[6]);
void findTrainSeat(char findId[6], char* strptr);
void findBookingSeat(char findId[6], char* strptr);

void searchEmployee(employee searchData);
void searchMember(member searchData);
void searchTrainSchedule(trainScheduling searchData);
void searchTicket(ticketBooking searchData);

void getEmployeeData(employee* getData, bool allowIdInput);
void getMemberData(member* getData, bool allowIdInput);
void getTrainScheduleData(trainScheduling* getData, bool allowIdInput);
void getTicketData(ticketBooking* getData, bool allowIdInput);
void getTicketDataByPurchaseMethod(ticketBooking* getTicketData, trainScheduling* getTrainData, char* strptr, FILE* ticketFile);

void addEmployee();
void addMember();
void addTrainSchedule();
bool addTicket(char memberId[6], char selectedTrainId[6]);

bool modifyEmployee(char modifyId[7]);
bool modifyMember(char modifyId[6]);
bool modifySchedule(char modifyId[6]);
bool modifyTicket(char modifyId[6]);
bool modifyTrainSeat(char modifyTrainId[6], char seat[]);
//应该只有booking id的edit需要在外面做

void displayTrainSeat(char* strptr);
void adjustSchedulePositionToSeat(FILE* adjustedFile, char destinationId[6]);
void adjustFilePositionToSelectedLine(FILE* adjustedFile, char destinationId[6]);//还有很多地方可以用
long int findFileSize(FILE* fileName);
void getCurrentDate(int* day, int* month, int* year);

int main() {
	char mode;
	do {
		printf("Welcome to Train ticket system\n");
		printf("Kindly type your number to choose the:\n");
		printf("1. Staff mode\n");
		printf("2. Member mode\n");
		printf("3. Exit\n>");
		rewind(stdin);
		scanf("%c", &mode);


		switch (mode) {
		case '1':
			system("cls");
			logintype(true);
			break;
		case '2':
			system("cls");
			logintype(false);
			break;
		case '3':
			printf(" \033[0;33m Exiting the program \033[0m");
			exit(0);
			break;
		default:
			printf("\033[0;31m Error Invalid input \033[0m\n\n");
		}


	} while (true);
	return 0;
}

void logintype(bool isAdmin) {
	char choice;
	char loginId[7];
	bool valid;
	do
	{

		printf("1. Existing User ID\n");
		printf("2. Register New User\n");
		printf("3. Return to previous page\n");
		printf("4. Exit\n>");
		rewind(stdin);
		scanf("%c", &choice);


		switch (choice)
		{
		case '1':
			do {
				printf(" Enter your id>");
				rewind(stdin);
				fgets(loginId, sizeof(loginId), stdin);
				loginId[strcspn(loginId, "\n")] = '\0';
				valid = userLogin(isAdmin, loginId);
			} while (valid == false);
			menu(isAdmin, loginId);
			break;
		case '2':
			if (isAdmin == true) {
				system("cls");
				addEmployee();
			}
			else {
				system("cls");
				addMember();
			}
			break;

		case '3':
			system("cls");
			return;

		case '4':
			printf(" \033[0;33m Exiting the program \033[0m");
			exit(0);
		default:
			printf("\033[0;31m Error Invalid input \033[0m\n\n");
			break;
		}

	} while (true);


}

bool userLogin(bool isAdmin, char loginId[7]) {
	char loginPassword[21], loggedId[7], loggedPassword[21], loggedPasswordRecovery[20];

	while (true) {
		if (isAdmin == true) {
			employee loginEmployee;
			loginEmployee = findEmployeeData(loginId);
			strcpy(loggedId, loginEmployee.id);
			strcpy(loggedPassword, loginEmployee.password);
			strcpy(loggedPasswordRecovery, loginEmployee.passwordRecovery);
		}
		else {
			member loginMember;
			loginMember = findMemberData(loginId);
			strcpy(loggedId, loginMember.id);
			strcpy(loggedPassword, loginMember.password);
			strcpy(loggedPasswordRecovery, loginMember.passwordRecovery);
		}

		if (strcmp(loginId, loggedId)) {
			printf("Error: User ID not found. Please check your ID and try again\n");
			return false;
		}
		else {
			break;
		}
	}

	for (int countError = 3; countError >= 0; countError--)
	{
		printf("enter your password>");
		rewind(stdin);
		fgets(loginPassword, sizeof(loginPassword), stdin);
		loginPassword[strcspn(loginPassword, "\n")] = '\0';
		system("cls");
		if (!strcmp(loginPassword, loggedPassword)) {
			printf("successful login\n");

			return true;
		}
		else {
			printf("error password\n");
			printf("password recovery:%s\n", loggedPasswordRecovery);
		}
		printf("change remain:%d\n", countError);
	}
	printf("unsuccessful login");
	exit(-1);
}

void menu(bool isAdmin, char loggedId[7]) {
	char choice;
	int choiceSearch;
	char id[7];
	do {
		member searchMemberData = { "*", "*", "*", "*", "*", "*", '*' };
		employee searchEmployeeData = { "*", "*", "*", "*", "*", '*' };
		trainScheduling searchTrainScheduleData = { "*","*","*",-1, -1, -1,-1, -1, -1,-1, -1,-1, -1,-1,-1.0 };
		ticketBooking searchTicketData = { "*", "*", "*","*","*", -1, -1, -1, -1, -1, -1, -1, -1.0 };
		if (isAdmin == true) {
			printf("1.add train schedule\n");
			printf("2.modify train schedule\n");
			printf("3.view member information\n");
			printf("4.view employee information\n");
			printf("5.modify member information\n");
			printf("6.modify employee information\n");
		}
		else {
			printf("booking ticket(B)\n");
		}
		printf("C.modify booking information\n");
		printf("D.view booking information \n");
		printf("E.view train schedule\n");
		printf("F.edit user information\n");
		printf("G.return to previous page\n");
		printf("H.exit\n>");
		scanf("%c", &choice);

		switch (choice) {
		case '1':
			addTrainSchedule();
			break;
		case '2':
			do {
				printf("which schedule need eit(input id)");
				rewind(stdin);
				fgets(id, sizeof(id), stdin);
				id[strcspn(id, "\n")] = '\0';
			} while (modifySchedule(id));
			break;
		case '3':
			printf("\n* = search all\n");
			getMemberData(&searchMemberData, true);
			searchMember(searchMemberData);
			break;
		case '4':
			printf("\n* = search all\n");
			getEmployeeData(&searchEmployeeData, true);
			searchEmployee(searchEmployeeData);
			break;
		case '5':
			do
			{
				printf("which member need eit(input id)");
				rewind(stdin);
				fgets(id, sizeof(id), stdin);
				id[strcspn(id, "\n")] = '\0';
			} while (modifyMember(id));
			break;
		case '6':
			do {
				printf("which employee need eit(input id)");
				rewind(stdin);
				fgets(id, sizeof(id), stdin);
				id[strcspn(id, "\n")] = '\0';
			} while (modifyEmployee(id));
			break;
		case '7':
			do {
				printf("which ticket need eit(input id)");
				rewind(stdin);
				fgets(id, sizeof(id), stdin);
				id[strcspn(id, "\n")] = '\0';
			} while (modifyTicket(id));
			break;
		case 'B':
			do {
				printf("which train need book(input id)");
				rewind(stdin);
				fgets(id, sizeof(id), stdin);
				id[strcspn(id, "\n")] = '\0';
			} while (addTicket(loggedId, id));
			break;
		case '8':
			printf("\n* = search all\n");
			printf("\n-1 = search all\n");
			getTicketData(&searchTicketData, true);
			searchTicket(searchTicketData);
			break;
		case '9':
			printf("\n* = search all\n");
			printf("\n-1 = search all\n");
			getTrainScheduleData(&searchTrainScheduleData, true);
			searchTrainSchedule(searchTrainScheduleData);
			break;
		case '10':
			if (isAdmin == true) {
				modifyEmployee(loggedId);
			}
			else {
				modifyMember(loggedId);
			}
		case '11':
			return;
		case '12':
			exit(0);

		default:
			printf("\033[0;31m Error Invalid input \033[0m\n\n");
			break;
		}
	} while (true);
}

void addEmployee() {
	FILE* employeeFile;

	employeeFile = fopen("employee.bin", "ab+");
	if (employeeFile == NULL) {
		printf("error open file");
		exit(-1);
	}
	employee newEmployee = { 0 };

	long int count_employee = findFileSize(employeeFile) / sizeof(employee);
	snprintf(newEmployee.id, sizeof(newEmployee.id), "ST");
	snprintf(newEmployee.id + strlen(newEmployee.id), sizeof(newEmployee.id) - strlen(newEmployee.id), "%04d", count_employee);

	getEmployeeData(&newEmployee, false);

	fwrite(&newEmployee, sizeof(newEmployee), 1, employeeFile);
	fclose(employeeFile);

}

void addMember() {
	FILE* memberFile;
	memberFile = fopen("member.bin", "ab+");
	if (memberFile == NULL) {
		printf("cannot open file");
		exit(-1);
	}

	member newMember;
	long int count_member = findFileSize(memberFile) / sizeof(member);
	snprintf(newMember.id, sizeof(newMember.id), "M");
	snprintf(newMember.id + strlen(newMember.id), sizeof(newMember.id) - strlen(newMember.id), "%04d", count_member);

	getMemberData(&newMember, false);
	fwrite(&newMember, sizeof(newMember), 1, memberFile);
	fclose(memberFile);
}

void addTrainSchedule() {
	FILE* scheduleFile;
	trainScheduling newSchedule = { "","","",{0, 0, 0},{0, 0, 0},{0, 0},{0, 0},0,0 };
	long int count_schedule = 0;
	int position, countAlphabet;
	scheduleFile = fopen("schedule.txt", "a+");
	if (scheduleFile == NULL) {
		printf("cannot open file");
		exit(-1);
	}

	while (fscanf(scheduleFile, "%*[^\n]\n") != EOF) {
		count_schedule++;
	}
	snprintf(newSchedule.id, sizeof(newSchedule.id), "T");
	snprintf(newSchedule.id + strlen(newSchedule.id), sizeof(newSchedule.id) - strlen(newSchedule.id), "%04d", count_schedule);
	getTrainScheduleData(&newSchedule, false);
	fprintf(scheduleFile, "%s|%s|%s|%d-%d-%d|%d-%d-%d|%02d:%02d|%02d:%02d|%d|%.2f|",
		newSchedule.id, newSchedule.departureStation, newSchedule.arrivalStation,
		newSchedule.departureDate.day, newSchedule.departureDate.month, newSchedule.departureDate.year,
		newSchedule.arrivalDate.day, newSchedule.arrivalDate.month, newSchedule.arrivalDate.year,
		newSchedule.departureTime.hours, newSchedule.departureTime.min,
		newSchedule.arrivalTime.hours, newSchedule.arrivalTime.min,
		newSchedule.coach, newSchedule.price);
	for (int i = 0; i < (newSchedule.coach * coach_size); i++) {
		fprintf(scheduleFile, "1");
	}
	fprintf(scheduleFile, "|\n");
	fclose(scheduleFile);
}

bool addTicket(char memberId[6], char selectedTrainId[6]) {//这个可以用gettraindata 之后试看看
	ticketBooking newTicket;
	trainScheduling selectedTrainData;
	FILE* bookingFile;
	int count_ticket = 0;

	bookingFile = fopen("booking.txt", "a+");
	if (bookingFile == NULL) {
		printf("cannot open file\n");
		exit(-1);
	}
	selectedTrainData = findTrainScheduleData(selectedTrainId);
	int lengthSeat = selectedTrainData.coach * coach_size + 1;
	char* strSeatPointer = (char*)calloc(lengthSeat, sizeof(char));
	if (strSeatPointer == NULL) {
		printf("memory allocation error\n");
		return -1;
	}
	if (strcmp(selectedTrainData.id, selectedTrainId) != 0) {
		printf("non-found train\n");
		return -1;
	}
	while (fscanf(bookingFile, "%*[^\n]\n") != EOF) {
		count_ticket++;
	}
	snprintf(newTicket.bookingId, sizeof(newTicket.bookingId), "B");
	snprintf(newTicket.bookingId + strlen(newTicket.bookingId), sizeof(newTicket.bookingId) - strlen(newTicket.bookingId), "%04d", count_ticket);
	strcpy(newTicket.memberId, memberId);
	strcpy(newTicket.trainId, selectedTrainData.id);
	newTicket.departureDate.day = selectedTrainData.departureDate.day;
	newTicket.departureDate.month = selectedTrainData.departureDate.month;
	newTicket.departureDate.year = selectedTrainData.departureDate.year;
	getCurrentDate(&newTicket.bookingDate.day, &newTicket.bookingDate.month, &newTicket.bookingDate.year);
	newTicket.pax = 0;

	getTicketDataByPurchaseMethod(&newTicket, &selectedTrainData, strSeatPointer, bookingFile);
	fprintf(bookingFile, "|%s|%s|%s|%d-%d-%d|%d-%d-%d|%d|%f|%s|%s|\n",
		newTicket.bookingId, newTicket.memberId, newTicket.trainId,
		newTicket.bookingDate.day, newTicket.bookingDate.month, newTicket.bookingDate.year,
		newTicket.departureDate.day, newTicket.departureDate.month, newTicket.departureDate.year,
		newTicket.pax, newTicket.amount, newTicket.paymentInfo, newTicket.ticketStatus);
	free(strSeatPointer);
	fclose(bookingFile);
}

employee findEmployeeData(char findId[7]) {
	employee foundData = { 0 };
	FILE* userfile;
	int countAlphabet, positionUser;

	userfile = fopen("employee.bin", "rb");
	if (userfile == NULL) {
		printf("error open file");
		exit(-1);
	}
	countAlphabet = 0;
	while (isalpha(findId[countAlphabet])) {
		countAlphabet++;
	}
	positionUser = atoi(findId + countAlphabet);
	fseek(userfile, positionUser * sizeof(employee), SEEK_SET);
	fread(&foundData, sizeof(employee), 1, userfile);
	fclose(userfile);
	return foundData;
}//这边本身不会cmp拿出来的对不对

member findMemberData(char findId[6]) {
	member foundData = { 0 };
	FILE* userfile;
	int countAlphabet, positionUser;

	userfile = fopen("member.bin", "rb");
	if (userfile == NULL) {
		printf("error open file");
		exit(-1);
	}
	countAlphabet = 0;
	while (isalpha(findId[countAlphabet])) {
		countAlphabet++;
	}
	positionUser = atoi(findId + countAlphabet);
	fseek(userfile, positionUser * sizeof(member), SEEK_SET);
	fread(&foundData, sizeof(member), 1, userfile);
	fclose(userfile);
	return foundData;
}

trainScheduling findTrainScheduleData(char findId[6]) {
	trainScheduling foundData = { 0 };
	FILE* scheduleFile;

	scheduleFile = fopen("schedule.txt", "r");
	if (scheduleFile == NULL) {
		printf("error open file");
		exit(-1);
	}

	adjustFilePositionToSelectedLine(scheduleFile, findId);
	fscanf(scheduleFile, "%[^|]|%[^|]|%[^|]|%d-%d-%d|%d-%d-%d|%d:%d|%d:%d|%d|%f|",
		foundData.id, foundData.departureStation, foundData.arrivalStation,
		&foundData.departureDate.day, &foundData.departureDate.month, &foundData.departureDate.year,
		&foundData.arrivalDate.day, &foundData.arrivalDate.month, &foundData.arrivalDate.year,
		&foundData.departureTime.hours, &foundData.departureTime.min,
		&foundData.arrivalTime.hours, &foundData.arrivalTime.min,
		&foundData.coach, &foundData.price);
	fclose(scheduleFile);
	return foundData;
}

ticketBooking findTicketData(char findId[6]) {
	ticketBooking foundData = { 0 };
	FILE* ticketFile;
	int countAlphabet, position;

	ticketFile = fopen("booking.txt", "r");
	if (ticketFile == NULL) {
		printf("error open file");
		exit(-1);
	}
	adjustFilePositionToSelectedLine(ticketFile, findId);
	fscanf(ticketFile, "%*[^|]|%[^|]|%[^|]|%[^|]|%d-%d-%d|%d-%d-%d|%d|%f|%[^|]|%[^|]|",
		foundData.bookingId, foundData.memberId, foundData.trainId,
		&foundData.bookingDate.day, &foundData.bookingDate.month, &foundData.bookingDate.year,
		&foundData.departureDate.day, &foundData.departureDate.month, &foundData.departureDate.year,
		&foundData.pax, &foundData.amount, foundData.paymentInfo, foundData.ticketStatus);
	fclose(ticketFile);
	return foundData;
}

void findTrainSeat(char findId[6], char* strptr) {
	FILE* scheduleFile;
	scheduleFile = fopen("schedule.txt", "r");
	if (scheduleFile == NULL) {
		printf("cannot open file");
		exit(-1);
	}
	adjustSchedulePositionToSeat(scheduleFile, findId);
	fscanf(scheduleFile, "%[^|]", strptr);
	fclose(scheduleFile);
};

void findBookingSeat(char findId[6], char* strptr) {
	FILE* ticketFile;
	ticketFile = fopen("booking.txt", "r");
	if (ticketFile == NULL) {
		printf("cannot open file");
		exit(-1);
	}
	adjustFilePositionToSelectedLine(ticketFile, findId);
	fscanf(ticketFile, "%[^|]", strptr);
	fclose(ticketFile);
}

void searchMember(member searchData) {
	FILE* memberFile;
	member searchedData;
	memberFile = fopen("member.bin", "rb");
	if (memberFile == NULL) {
		printf("cannot open file");
		exit(-1);
	}
	long int count_member;
	bool valid_id, valid_name, valid_password, valid_passwordRecovery, valid_ic, valid_contactNumber, valid_gender;
	count_member = findFileSize(memberFile) / sizeof(member);
	fclose(memberFile);
	char string_i[5];
	printf("| %-6s | %-50s | %-21s | %-20s | %-15s | %-12s | %-6s |\n", "ID", "Name", "Password", "Password Recovery", "IC", "Contact Number", "Gender");
	for (int i = 0; i < count_member; i++) {

		sprintf(string_i, "%d", i);
		searchedData = findMemberData(string_i);
		valid_id = (searchData.id[0] == '*') ? true : (strncmp(searchData.id, searchedData.id, strlen(searchData.id)) == 0);
		valid_name = (searchData.name[0] == '*') ? true : (strncmp(searchData.name, searchedData.name, strlen(searchData.name)) == 0);
		valid_password = (searchData.password[0] == '*') ? true : (strncmp(searchData.password, searchedData.password, strlen(searchData.password)) == 0);
		valid_passwordRecovery = (searchData.passwordRecovery[0] == '*') ? true : (strncmp(searchData.passwordRecovery, searchedData.passwordRecovery, strlen(searchData.passwordRecovery)) == 0);
		valid_ic = (searchData.ic[0] == '*') ? true : (strncmp(searchData.ic, searchedData.ic, strlen(searchData.ic)) == 0);
		valid_contactNumber = (searchData.contactNumber[0] == '*') ? true : (strncmp(searchData.contactNumber, searchedData.contactNumber, strlen(searchData.contactNumber)) == 0);
		valid_gender = (searchData.contactNumber[0] == '*') || (searchData.gender == searchedData.gender);
		if (valid_id == true && valid_name == true && valid_password == true && valid_passwordRecovery == true && valid_ic == true && valid_contactNumber == true && valid_gender == true) {
			printf("| %-6s | %-50s | %-21s | %-20s | %-15s | %-12s\t\b | %-6c |\n", searchedData.id, searchedData.name, searchedData.password, searchedData.passwordRecovery, searchedData.ic, searchedData.contactNumber, searchedData.gender);
		}
	}
}

void searchEmployee(employee searchData) {
	FILE* employeeFile;
	employee searchedData;
	employeeFile = fopen("employee.bin", "rb");
	long int count_employee;
	bool valid_id, valid_name, valid_password, valid_passwordRecovery, valid_position, valid_gender;
	count_employee = findFileSize(employeeFile) / sizeof(employee);
	fclose(employeeFile);
	char string_i[5];
	printf("| %-7s | %-50s | %-21s | %-20s | %-50s | %-6s |\n", "ID", "Name", "Password", "Password Recovery", "Position", "Gender");
	for (int i = 0; i < count_employee; i++) {
		sprintf(string_i, "%d", i);
		searchedData = findEmployeeData(string_i);
		valid_id = (searchData.id[0] == '*') ? true : (strncmp(searchedData.id, searchData.id, strlen(searchData.id)) == 0);
		valid_name = (searchData.name[0] == '*') ? true : (strncmp(searchedData.name, searchData.name, strlen(searchData.name)) == 0);
		valid_password = (searchData.password[0] == '*') ? true : (strncmp(searchedData.password, searchData.password, strlen(searchData.password)) == 0);
		valid_passwordRecovery = (searchData.passwordRecovery[0] == '*') ? true : (strncmp(searchedData.passwordRecovery, searchData.passwordRecovery, strlen(searchData.passwordRecovery)) == 0);
		valid_position = (searchData.position[0] == '*') ? true : (strncmp(searchedData.position, searchData.position, strlen(searchData.position)) == 0);
		valid_gender = (searchData.gender == '*') || (searchData.gender == searchedData.gender);
		if (valid_id == true && valid_name == true && valid_password == true && valid_passwordRecovery == true && valid_position == true && valid_gender == true) {
			printf("| %-7s | %-50s | %-21s | %-20s | %-50s | %-6c |\n", searchedData.id, searchedData.name, searchedData.password, searchedData.passwordRecovery, searchedData.position, searchedData.gender);
		}
	}
}

void searchTrainSchedule(trainScheduling searchData) {
	trainScheduling searchedData;
	bool valid_id, valid_departureStation, valid_arrivalStation, valid_departureDate, valid_arrivalDate, valid_departureTime, valid_arrivalTime, valid_coach, valid_price;;
	char string_i[5];
	FILE* scheduleFile;
	int count_schedule = 0;
	scheduleFile = fopen("schedule.txt", "r");
	if (scheduleFile == NULL) {
		printf("cannot open file");
		exit(-1);
	}
	while (fscanf(scheduleFile, "%*[^\n]\n") != EOF) {
		count_schedule++;
	}
	fclose(scheduleFile);
	printf("| %-6s | %-31s | %-31s | %-16s | %-13s | %-15s\b| %-15s\b| %-6s\t\b | %-8s\b\b |\n", "ID", "Departure Station", "Arrival Station", "Departure Date", "Arrival Date", "Departure Time", "Arrival Time", "Coach", "Price");
	for (int i = 0; i < count_schedule; i++) {
		sprintf(string_i, "%d", i);
		searchedData = findTrainScheduleData(string_i);
		valid_id = (searchData.id[0] == '*') ? true : (strncmp(searchedData.id, searchData.id, strlen(searchData.id)) == 0);
		valid_departureStation = (searchData.departureStation[0] == '*') ? true : (strncmp(searchedData.departureStation, searchData.departureStation, strlen(searchData.departureStation)) == 0);
		valid_arrivalStation = (searchData.arrivalStation[0] == '*') ? true : (strncmp(searchedData.arrivalStation, searchData.arrivalStation, strlen(searchData.arrivalStation)) == 0);
		valid_departureDate = (searchData.departureDate.day == -1 && searchData.departureDate.month == -1 && searchData.departureDate.year == -1) ? true : (searchData.departureDate.day == searchedData.departureDate.day && searchData.departureDate.month == searchedData.departureDate.month && searchData.departureDate.year == searchedData.departureDate.year);
		valid_arrivalDate = (searchData.arrivalDate.day == -1 && searchData.arrivalDate.month == -1 && searchData.arrivalDate.year == -1) ? true : (searchData.arrivalDate.day == searchedData.arrivalDate.day && searchData.arrivalDate.month == searchedData.arrivalDate.month && searchData.arrivalDate.year == searchedData.arrivalDate.year);
		valid_departureTime = (searchData.departureTime.hours == -1 && searchData.departureTime.min == -1) ? true : (searchData.departureTime.hours == searchedData.departureTime.hours && searchData.departureTime.min == searchedData.departureTime.min);
		valid_arrivalTime = (searchData.arrivalTime.hours == -1 && searchData.arrivalTime.min == -1) ? true : (searchData.arrivalTime.hours == searchedData.arrivalTime.hours && searchData.arrivalTime.min == searchedData.arrivalTime.min);
		valid_coach = (searchData.coach == -1) ? true : (searchData.coach == searchedData.coach);
		valid_price = (searchData.price == -1) ? true : (searchData.price == searchedData.price);

		if (valid_id && valid_departureStation && valid_arrivalStation && valid_departureDate && valid_arrivalDate && valid_departureTime && valid_arrivalTime && valid_coach && valid_price) {
			printf("| %-6s | %-31s | %-31s | %02d-%02d-%04d \t| %02d-%02d-%04d \t| %02d:%02d \t| %02d:%02d \t| %-5d \t| %-5.2f \t|\n",
				searchedData.id, searchedData.departureStation, searchedData.arrivalStation,
				searchedData.departureDate.day, searchedData.departureDate.month, searchedData.departureDate.year,
				searchedData.arrivalDate.day, searchedData.arrivalDate.month, searchedData.arrivalDate.year,
				searchedData.departureTime.hours, searchedData.departureTime.min,
				searchedData.arrivalTime.hours, searchedData.arrivalTime.min,
				searchedData.coach, searchedData.price);
		}
	}
}

void searchTicket(ticketBooking searchData) {
	ticketBooking searchedData;
	bool valid_bookingId, valid_memberId, valid_trainId, valid_paymentInfo, valid_ticketStatus, valid_bookingDate, valid_departureDate, valid_pax, valid_amount;
	char string_i[5];
	FILE* ticketFile;
	int count_tickets = 0;
	ticketFile = fopen("booking.txt", "r");
	if (ticketFile == NULL) {
		printf("cannot open file");
		exit(-1);
	}
	while (fscanf(ticketFile, "%*[^\n]\n") != EOF) {
		count_tickets++;
	}
	fclose(ticketFile);

	printf("| %-6s | %-6s | %-6s | %-20s | %-20s | %-16s | %-16s | %-3s | %-8s | %-50s |\n", "Booking ID", "Member ID", "Train ID", "Payment Info", "Ticket Status", "Booking Date", "Departure Date", "Pax", "Amount", "seat selected");
	for (int i = 0; i < count_tickets; i++) {
		sprintf(string_i, "%d", i);
		searchedData = findTicketData(string_i);
		char* strSeatIdPointer = (char*)calloc((searchedData.pax * 6), sizeof(char));//6= A01A and backspace
		if (strSeatIdPointer == NULL) {
			printf("memory allocation error\n");
			return -1;
		}
		valid_bookingId = (searchData.bookingId[0] == '*') ? true : (strncmp(searchedData.bookingId, searchData.bookingId, strlen(searchData.bookingId)) == 0);
		valid_memberId = (searchData.memberId[0] == '*') ? true : (strncmp(searchedData.memberId, searchData.memberId, strlen(searchData.memberId)) == 0);
		valid_trainId = (searchData.trainId[0] == '*') ? true : (strncmp(searchedData.trainId, searchData.trainId, strlen(searchData.trainId)) == 0);
		valid_paymentInfo = (searchData.paymentInfo[0] == '*') ? true : (strncmp(searchedData.paymentInfo, searchData.paymentInfo, strlen(searchData.paymentInfo)) == 0);
		valid_ticketStatus = (searchData.ticketStatus[0] == '*') ? true : (strncmp(searchedData.ticketStatus, searchData.ticketStatus, strlen(searchData.ticketStatus)) == 0);
		valid_bookingDate = (searchData.bookingDate.day == -1 && searchData.bookingDate.month == -1 && searchData.bookingDate.year == -1) ? true : (searchData.bookingDate.day == searchedData.bookingDate.day && searchData.bookingDate.month == searchedData.bookingDate.month && searchData.bookingDate.year == searchedData.bookingDate.year);
		valid_departureDate = (searchData.departureDate.day == -1 && searchData.departureDate.month == -1 && searchData.departureDate.year == -1) ? true : (searchData.departureDate.day == searchedData.departureDate.day && searchData.departureDate.month == searchedData.departureDate.month && searchData.departureDate.year == searchedData.departureDate.year);
		valid_pax = (searchData.pax == -1) ? true : (searchData.pax == searchedData.pax);
		valid_amount = (searchData.amount == -1) ? true : (searchData.amount == searchedData.amount);

		if (valid_bookingId && valid_memberId && valid_trainId && valid_paymentInfo && valid_ticketStatus && valid_bookingDate && valid_departureDate && valid_pax && valid_amount) {
			findBookingSeat(searchedData.bookingId, strSeatIdPointer);
			printf("| %-9s | %-9s | %-9s | %-20s | %-20s | %02d-%02d-%04d | %02d-%02d-%04d | %-3d | %-8.2f |",
				searchedData.bookingId, searchedData.memberId, searchedData.trainId,
				searchedData.paymentInfo, searchedData.ticketStatus,
				searchedData.bookingDate.day, searchedData.bookingDate.month, searchedData.bookingDate.year,
				searchedData.departureDate.day, searchedData.departureDate.month, searchedData.departureDate.year,
				searchedData.pax, searchedData.amount);
			printf("%-50s|\n", strSeatIdPointer);
		}
		free(strSeatIdPointer);
	}
}

void getMemberData(member* getData, bool allowIdInput) {
	int choice;
	do {
		printf("id\t\t>");
		puts(getData->id);
		printf("name\t\t>");
		puts(getData->name);
		printf("password\t>");
		puts(getData->password);
		printf("passwordRecovery>");
		puts(getData->passwordRecovery);
		printf("ic\t\t>");
		puts(getData->ic);
		printf("gender\t\t>%c\n", getData->gender);
		printf("contact number\t>");
		puts(getData->contactNumber);
		printf("-1.execute\n");
		printf("0.enter all\n");
		if (allowIdInput == true) {
			printf("1.enter id\n");
		}
		printf("2.enter name\n");
		printf("3.enter password\n");
		printf("4.enter password Recovery\n");
		printf("5.enter ic\n");
		printf("6.enter gender\n");
		printf("7.enter contact number\n");
		scanf("%d", &choice);

		switch (choice) {
		case 0:
		case 1:
			if (allowIdInput == true)
			{
				rewind(stdin);
				fgets(getData->id, sizeof(getData->id), stdin);
				getData->id[strcspn(getData->id, "\n")] = '\0';
				if (choice != 0) { break; }
			}
			else if (choice != 0) {
				printf("invalid choice");
				break;
			}

		case 2:
			rewind(stdin);
			fgets(getData->name, sizeof(getData->name), stdin);
			getData->name[strcspn(getData->name, "\n")] = '\0';
			if (choice != 0) { break; }

		case 3:
			rewind(stdin);
			fgets(getData->password, sizeof(getData->password), stdin);
			getData->password[strcspn(getData->password, "\n")] = '\0';
			if (choice != 0) { break; }

		case 4:
			rewind(stdin);
			fgets(getData->passwordRecovery, sizeof(getData->passwordRecovery), stdin);
			getData->passwordRecovery[strcspn(getData->passwordRecovery, "\n")] = '\0';
			if (choice != 0) { break; }

		case 5:
			rewind(stdin);
			fgets(getData->ic, sizeof(getData->ic), stdin);
			getData->ic[strcspn(getData->ic, "\n")] = '\0';
			if (choice != 0) { break; }

		case 6:
			rewind(stdin);
			scanf("%c", &getData->gender);
			if (choice != 0) { break; }

		case 7:
			rewind(stdin);
			fgets(getData->contactNumber, sizeof(getData->contactNumber), stdin);
			getData->contactNumber[strcspn(getData->contactNumber, "\n")] = '\0';
			if (choice != 0) { break; }

		case -1:
			break;
		default:
			printf("error input");
			break;
		}
	} while (choice != -1);
}

void getEmployeeData(employee* getData, bool allowIdInput) {
	int choice;
	do {
		printf("id\t\t>");
		puts(getData->id);
		printf("name\t\t>");
		puts(getData->name);
		printf("password\t>");
		puts(getData->password);
		printf("passwordRecovery>");
		puts(getData->passwordRecovery);
		printf("gender\t\t>%c\n", getData->gender);
		printf("position\t>");
		puts(getData->position);
		printf("-1.execute\n");
		printf("0.enter all\n");
		if (allowIdInput == true) {
			printf("1.enter id\n");
		}
		printf("2.enter name\n");
		printf("3.enter password\n");
		printf("4.enter password Recovery\n");
		printf("5.enter gender\n");
		printf("6.enter position\n");
		scanf("%d", &choice);

		switch (choice) {
		case 0:
		case 1:
			if (allowIdInput == true) {
				rewind(stdin);
				fgets(getData->id, sizeof(getData->id), stdin);
				getData->id[strcspn(getData->id, "\n")] = '\0';
				if (choice != 0) { break; }
			}
			else if (choice != 0) {
				printf("Invalid choice\n");
				break;
			}
		case 2:
			rewind(stdin);
			fgets(getData->name, sizeof(getData->name), stdin);
			getData->name[strcspn(getData->name, "\n")] = '\0';
			if (choice != 0) { break; }

		case 3:
			rewind(stdin);
			fgets(getData->password, sizeof(getData->password), stdin);
			getData->password[strcspn(getData->password, "\n")] = '\0';
			if (choice != 0) { break; }

		case 4:
			rewind(stdin);
			fgets(getData->passwordRecovery, sizeof(getData->passwordRecovery), stdin);
			getData->passwordRecovery[strcspn(getData->passwordRecovery, "\n")] = '\0';
			if (choice != 0) { break; }

		case 5:
			rewind(stdin);
			scanf("%c", &getData->gender);
			if (choice != 0) { break; }

		case 6:
			rewind(stdin);
			fgets(getData->position, sizeof(getData->position), stdin);
			getData->position[strcspn(getData->position, "\n")] = '\0';
			if (choice != 0) { break; }
		case -1:
			break;
			printf("Returning to Menu page");
		default:
			printf("error input");
			break;
		}
	} while (choice != -1);
}

void getTrainScheduleData(trainScheduling* getData, bool allowIdInput) {
	int choice;
	do {
		printf("id\t\t> %s\n", getData->id);
		printf("departureStation> %s\n", getData->departureStation);
		printf("arrivalStation\t> %s\n", getData->arrivalStation);
		printf("departureDate\t> %d-%d-%d\n", getData->departureDate.day, getData->departureDate.month, getData->departureDate.year);
		printf("arrivalDate\t> %d-%d-%d\n", getData->arrivalDate.day, getData->arrivalDate.month, getData->arrivalDate.year);
		printf("departureTime\t> %02d:%02d\n", getData->departureTime.hours, getData->departureTime.min);
		printf("arrivalTime\t> %02d:%02d\n", getData->arrivalTime.hours, getData->arrivalTime.min);
		printf("coach\t\t> %d\n", getData->coach);
		printf("price\t\t> %.2f\n", getData->price);
		printf("-1.execute\n");
		printf("0. enter all\n");
		if (allowIdInput == true) {
			printf("1. enter id\n");
		}
		printf("2. enter departure station\n");
		printf("3. enter arrival station\n");
		printf("4. enter departure date\n");
		printf("5. enter arrival date\n");
		printf("6. enter departure time\n");
		printf("7. enter arrival time\n");
		printf("8. enter coach\n");
		printf("9. enter price\n");
		scanf("%d", &choice);
		switch (choice) {
		case 0:
		case 1:
			if (allowIdInput == true) {
				rewind(stdin);
				fgets(getData->id, sizeof(getData->id), stdin);
				getData->id[strcspn(getData->id, "\n")] = '\0';
				if (choice != 0) { break; }
			}
			else if (choice != 0) {
				printf("Invalid choice\n");
				break;
			}
		case 2:
			rewind(stdin);
			fgets(getData->departureStation, sizeof(getData->departureStation), stdin);
			getData->departureStation[strcspn(getData->departureStation, "\n")] = '\0';
			if (choice != 0) { break; }
		case 3:
			rewind(stdin);
			fgets(getData->arrivalStation, sizeof(getData->arrivalStation), stdin);
			getData->arrivalStation[strcspn(getData->arrivalStation, "\n")] = '\0';
			if (choice != 0) { break; }
		case 4:
			scanf("%d-%d-%d", &getData->departureDate.day, &getData->departureDate.month, &getData->departureDate.year);
			if (choice != 0) { break; }
		case 5:
			scanf("%d-%d-%d", &getData->arrivalDate.day, &getData->arrivalDate.month, &getData->arrivalDate.year);
			if (choice != 0) { break; }
		case 6:
			scanf("%d:%d", &getData->departureTime.hours, &getData->departureTime.min);
			if (choice != 0) { break; }
		case 7:
			scanf("%d:%d", &getData->arrivalTime.hours, &getData->arrivalTime.min);
			if (choice != 0) { break; }
		case 8:
			scanf("%d", &getData->coach);
			if (choice != 0) { break; }
		case 9:
			scanf("%f", &getData->price);
			if (choice != 0) { break; }
		case -1:
			break;
		default:
			printf("Invalid choice\n");
			break;
		}
	} while (choice != -1);
}

void getTicketData(ticketBooking* getData, bool allowIdInput) {
	int choice;
	do {
		printf("bookingId\t> %s\n", getData->bookingId);
		printf("memberId\t> %s\n", getData->memberId);
		printf("trainId\t\t> %s\n", getData->trainId);
		printf("paymentInfo\t> %s\n", getData->paymentInfo);
		printf("ticketStatus\t> %s\n", getData->ticketStatus);
		printf("bookingDate\t> %d-%d-%d\n", getData->bookingDate.day, getData->bookingDate.month, getData->bookingDate.year);
		printf("departureDate\t> %d-%d-%d\n", getData->departureDate.day, getData->departureDate.month, getData->departureDate.year);
		printf("pax\t\t> %d\n", getData->pax);
		printf("amount\t\t> %.2f\n", getData->amount);
		printf("-1.execute\n");
		printf("0. enter all\n");
		if (allowIdInput) {
			printf("1. enter bookingId\n");
			printf("2. enter memberId\n");
			printf("3. enter trainId\n");
		}
		printf("4. enter paymentInfo\n");//这个要改
		printf("5. enter ticketStatus\n");
		printf("6. enter bookingDate\n");//应该只有4，5，6可以改7肯定不行 9跟着8 8的话需要重新买 
		printf("7. enter departureDate\n");
		printf("8. enter pax\n");
		printf("9. enter amount\n");
		scanf("%d", &choice);

		switch (choice) {
		case 0:
		case 1:
			if (allowIdInput) {
				printf("Enter bookingId: ");
				rewind(stdin);
				fgets(getData->bookingId, sizeof(getData->bookingId), stdin);
				getData->bookingId[strcspn(getData->bookingId, "\n")] = '\0';
				if (choice != 0) { break; }
			}
			else if (choice != 0) {
				printf("Invalid choice\n");
				break;
			}
		case 2:
			if (allowIdInput) {
				printf("Enter memberId: ");
				rewind(stdin);
				fgets(getData->memberId, sizeof(getData->memberId), stdin);
				getData->memberId[strcspn(getData->memberId, "\n")] = '\0';
				if (choice != 0) { break; }
			}
			else if (choice != 0) {
				printf("Invalid choice\n");
				break;
			}
		case 3:
			if (allowIdInput) {
				printf("Enter trainId: ");
				rewind(stdin);
				fgets(getData->trainId, sizeof(getData->trainId), stdin);
				getData->trainId[strcspn(getData->trainId, "\n")] = '\0';
			}
			else if (choice != 0) {
				printf("Invalid choice\n");
				break;
			}
		case 4:
			printf("Enter paymentInfo: ");
			rewind(stdin);
			fgets(getData->paymentInfo, sizeof(getData->paymentInfo), stdin);
			getData->paymentInfo[strcspn(getData->paymentInfo, "\n")] = '\0';
			if (choice != 0) { break; }
		case 5:
			printf("Enter ticketStatus: ");
			rewind(stdin);
			fgets(getData->ticketStatus, sizeof(getData->ticketStatus), stdin);
			getData->ticketStatus[strcspn(getData->ticketStatus, "\n")] = '\0';
			if (choice != 0) { break; }
		case 6:
			printf("Enter bookingDate (day-month-year): ");
			scanf("%d-%d-%d", &getData->bookingDate.day, &getData->bookingDate.month, &getData->bookingDate.year);
			if (choice != 0) { break; }
		case 7:
			printf("Enter departureDate (day-month-year): ");
			scanf("%d-%d-%d", &getData->departureDate.day, &getData->departureDate.month, &getData->departureDate.year);
			if (choice != 0) { break; }
		case 8:
			printf("Enter pax: ");
			scanf("%d", &getData->pax);
			if (choice != 0) { break; }
		case 9:
			printf("Enter amount: ");
			scanf("%f", &getData->amount);
			if (choice != 0) { break; }
		case -1:
			break;
		default:
			printf("Invalid choice\n");
			break;
		}
	} while (choice != -1);
}

void getTicketDataByPurchaseMethod(ticketBooking* getTicketData, trainScheduling* getTrainData, char* strptr, FILE* ticketFile) {
	char confirmBuyTicket, selectedSeatId[5], seatposition_column, seatPosition_coach;
	const int  alphebet = 64;
	int seatPosition_row, seatPosition, payMethod;
	do {
		printf("confirm want buy(Y=yes other =no)");
		rewind(stdin);
		scanf("%c", &confirmBuyTicket);
		if (confirmBuyTicket != 'Y') {
			break;
		}
		findTrainSeat(getTrainData->id, strptr);
		displayTrainSeat(strptr);
		printf("\n============================\n");
		printf("Which seat want buy\n");
		rewind(stdin);
		fgets(selectedSeatId, sizeof(selectedSeatId), stdin);
		selectedSeatId[strcspn(selectedSeatId, "\n")] = '\0';
		sscanf(selectedSeatId, "%c %d %c ", &seatPosition_coach, &seatPosition_row, &seatposition_column);
		seatPosition = ((seatPosition_coach - alphebet - 1) * 40) + ((seatPosition_row - 1) * 4) + (seatposition_column - alphebet);
		seatPosition--;
		if (*(strptr + seatPosition) == '1') {
			printf("select successful\n");
			*(strptr + seatPosition) = '0';
			modifyTrainSeat(getTrainData->id, strptr);
			getTicketData->pax++;
			fprintf(ticketFile, "%s ", selectedSeatId);
		}
		else { printf("non-found seat\n"); }

	} while (confirmBuyTicket == 'Y');

	getTicketData->amount = getTicketData->pax * getTrainData->price;

	printf("pay by ?\n");
	printf("1.Credit Card\n");
	printf("2.Cash\n");
	do {
		scanf("%d", &payMethod);
		switch (payMethod) {
		case 1:
			strcpy(getTicketData->paymentInfo, "Credit Card");
			break;
		case 2:
			strcpy(getTicketData->paymentInfo, "Cash");
			break;
		default:
			printf("error enter");
			break;
		}
	} while (payMethod != 1 && payMethod != 2);
	strcpy(getTicketData->ticketStatus, "Booked");
}

bool modifyEmployee(char modifyId[7]) {
	employee edit;
	int positionUser;
	edit = findEmployeeData(modifyId);
	if (strcmp(edit.id, modifyId) != 0) {
		printf("non-found user\n");
		return -1;
	}
	printf("edit\n");
	getEmployeeData(&edit, false);

	FILE* employeeFile;
	int countAlphabet = 0;
	while (isalpha(modifyId[countAlphabet])) {
		countAlphabet++;
	}
	positionUser = atoi(modifyId + countAlphabet);
	employeeFile = fopen("employee.bin", "rb+");
	if (employeeFile == NULL) {
		printf("error open file");
		exit(-1);
	}
	fseek(employeeFile, positionUser * sizeof(employee), SEEK_SET);
	fwrite(&edit, sizeof(employee), 1, employeeFile);
	fclose(employeeFile);
	return 0;
}

bool modifyMember(char modifyId[6]) {
	member edit;
	int positionUser;

	edit = findMemberData(modifyId);
	if (strcmp(edit.id, modifyId) != 0) {
		printf("non-found user\n");
		return -1;
	}
	printf("edit\n");
	getMemberData(&edit, false);

	FILE* memberFile;
	int countAlphabet = 0;
	while (isalpha(modifyId[countAlphabet])) {
		countAlphabet++;
	}
	positionUser = atoi(modifyId + countAlphabet);
	memberFile = fopen("member.bin", "rb+");
	if (memberFile == NULL) {
		printf("error open file");
		exit(-1);
	}
	fseek(memberFile, positionUser * sizeof(member), SEEK_SET);
	fwrite(&edit, sizeof(member), 1, memberFile);
	fclose(memberFile);
	return 0;
}

bool modifySchedule(char modifyId[6]) {
	trainScheduling edit;
	long int position;
	FILE* scheduleFile, * tempFile;
	char ch;

	edit = findTrainScheduleData(modifyId);
	if (strcmp(edit.id, modifyId) != 0) {
		printf("non-found user\n");
		return -1;
	}
	printf("edit\n");
	getTrainScheduleData(&edit, false);

	scheduleFile = fopen("schedule.txt", "r+");
	tempFile = fopen("temp.txt", "w+");
	if (scheduleFile == NULL || tempFile == NULL) {
		printf("cannot open file");
		exit(-1);
	}
	adjustFilePositionToSelectedLine(scheduleFile, edit.id);
	fscanf(scheduleFile, "%*[^\n]\n");
	while ((ch = fgetc(scheduleFile)) != EOF) {
		fputc(ch, tempFile);
	}
	fseek(scheduleFile, 0, SEEK_SET);
	adjustFilePositionToSelectedLine(scheduleFile, edit.id);

	position = ftell(scheduleFile);
	fseek(scheduleFile, position, SEEK_SET);
	fseek(tempFile, 0, SEEK_SET);
	fprintf(scheduleFile, "%s|%s|%s|%d-%d-%d|%d-%d-%d|%02d:%02d|%02d:%02d|%d|%.2f|",
		edit.id, edit.departureStation, edit.arrivalStation,
		edit.departureDate.day, edit.departureDate.month, edit.departureDate.year,
		edit.arrivalDate.day, edit.arrivalDate.month, edit.arrivalDate.year,
		edit.departureTime.hours, edit.departureTime.min,
		edit.arrivalTime.hours, edit.arrivalTime.min,
		edit.coach, edit.price);
	for (int i = 0; i < (edit.coach * coach_size); i++) {
		fprintf(scheduleFile, "1");
	}
	fprintf(scheduleFile, "|\n");
	while ((ch = fgetc(tempFile)) != EOF) {
		fputc(ch, scheduleFile);
	}
	fclose(scheduleFile);
	remove(tempFile);
	return 0;
}

bool modifyTicket(char modifyId[6]) {
	ticketBooking edit;
	trainScheduling selectedTrainData;
	FILE* ticketFile, * tempFile;
	long int position;
	char ch;

	edit = findTicketData(modifyId);//这边还要一个findbookingseat
	selectedTrainData = findTrainScheduleData(edit.trainId);
	int lengthSeat = selectedTrainData.coach * coach_size + 1;
	char* strSeatPointer = (char*)calloc(lengthSeat, sizeof(char));
	if (strcmp(edit.bookingId, modifyId) != 0) {
		printf("non-found user\n");
		return -1;
	}
	printf("edit\n");


	ticketFile = fopen("booking.txt", "r+");
	tempFile = fopen("temp.txt", "w+");
	if (ticketFile == NULL || tempFile == NULL) {
		printf("cannot open file");
		exit(-1);
	}
	adjustFilePositionToSelectedLine(ticketFile, modifyId);
	fscanf(ticketFile, "%*[^\n]\n");
	while ((ch = fgetc(ticketFile)) != EOF) {
		fputc(ch, tempFile);
	}
	fseek(ticketFile, 0, SEEK_SET);
	adjustFilePositionToSelectedLine(ticketFile, modifyId);

	position = ftell(ticketFile);
	fseek(ticketFile, position, SEEK_SET);
	fseek(tempFile, 0, SEEK_SET);
	edit.pax = 0;
	getTicketDataByPurchaseMethod(&edit, &selectedTrainData, strSeatPointer, ticketFile);

	fprintf(ticketFile, "|%s|%s|%s|%d-%d-%d|%d-%d-%d|%d|%f|%s|%s|\n",
		edit.bookingId, edit.memberId, edit.trainId,
		edit.bookingDate.day, edit.bookingDate.month, edit.bookingDate.year,
		edit.departureDate.day, edit.departureDate.month, edit.departureDate.year,
		edit.pax, edit.amount, edit.paymentInfo, edit.ticketStatus);
	position = ftell(ticketFile);
	long int fileSize = findFileSize(ticketFile);
	fileSize -= position;
	fseek(ticketFile, position, SEEK_SET);
	for (int i = position; i < fileSize; i++) {
		fputc(' ', ticketFile);
	}
	fseek(ticketFile, position, SEEK_SET);
	while ((ch = fgetc(tempFile)) != EOF) {
		fputc(ch, ticketFile);
	}
	free(strSeatPointer);
	fclose(ticketFile);
	fclose(tempFile);
	return 0;
}//还没试跑过

bool modifyTrainSeat(char modifyTrainId[6], char seat[]) {
	FILE* scheduleFile;
	scheduleFile = fopen("schedule.txt", "r+");
	if (scheduleFile == NULL) {
		printf("cannot open file");
		exit(-1);
	}
	adjustSchedulePositionToSeat(scheduleFile, modifyTrainId);
	long int position = ftell(scheduleFile);
	fseek(scheduleFile, position, SEEK_SET);
	fprintf(scheduleFile, "%s", seat);
	fclose(scheduleFile);
}

void displayTrainSeat(char* strptr) {
	int count_coach = (strlen(strptr)) / coach_size;
	int count_seat = 0;
	const int  alphebet = 64;
	char inputCoach;
	for (int i = 1; i <= count_coach; i++)
	{
		printf("%c COACH\n", i + alphebet);
	}
	rewind(stdin);
	scanf("%c", &inputCoach);
	printf("\t%c COACH\n", inputCoach);
	printf("============================\n");
	for (int seat_coach = 1; seat_coach <= count_coach; seat_coach++) {
		for (int seat_column = 1; seat_column <= 10; seat_column++) {
			for (int seat_row = 1; seat_row <= 4; seat_row++) {

				if (*(strptr + count_seat) == '1' && seat_coach == (inputCoach - alphebet)) {
					printf("%02d%c\t", seat_column, seat_row + alphebet);

				}
				else {
					printf("\t");
				}
				count_seat++;
			}
			if (seat_coach == (inputCoach - alphebet)) {
				printf("\n");
			}
		}
	}
}

void adjustSchedulePositionToSeat(FILE* adjustedFile, char destinationId[6]) {
	adjustFilePositionToSelectedLine(adjustedFile, destinationId);
	fscanf(adjustedFile, "%*[^|]|%*[^|]|%*[^|]|%*[^|]|%*[^|]|%*[^|]|%*[^|]|%*[^|]|%*[^|]|");

}

void adjustFilePositionToSelectedLine(FILE* adjustedFile, char destinationId[6]) {
	int countAlphabet, position;

	countAlphabet = 0;
	while (isalpha(destinationId[countAlphabet])) {
		countAlphabet++;
	}
	position = atoi(destinationId + countAlphabet);
	for (int i = 0; i < position; i++) {
		fscanf(adjustedFile, "%*[^\n]\n");
	}
}

long int findFileSize(FILE* fileName) {
	fseek(fileName, 0, SEEK_END);
	long int fileSize = ftell(fileName);
	return fileSize;
}

void getCurrentDate(int* day, int* month, int* year) {

	time_t currentTime;
	struct tm* localTime;

	time(&currentTime);
	localTime = localtime(&currentTime);

	*day = localTime->tm_mday;
	*month = localTime->tm_mon + 1;
	*year = localTime->tm_year + 1900;
}

//现在的问题是 当 modify schedule的时候我需要把所有买过座位的全部变成 cancel
//modify ticket 的话是有类似于 买新 ticket
//要做一个找 train 跟 seat就可以找到那张 ticket 的东西
//log book还没做
//权限还有很多问题

//可能可以多开一个getticdketdata for add 跟modify search的话就用原本的